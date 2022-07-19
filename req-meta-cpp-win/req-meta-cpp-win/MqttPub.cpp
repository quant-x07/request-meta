#include "MqttPub.h"
#include <vector>
#include <map>
#include "pkg/PkgData.h"
#include "pkg/PkgTaskLooper.h"
#include "fmt/MarketInfo.h"
#include "fmt/VarLenFmtRo.h"
#include "fmt/SplitInfo.h"

#include <zlib.h>

#include "pkg/PkgTaskLooper.h"

std::mutex g_display_mutex;

void ThreadProc(void* lpParameter)
{
    std::thread::id this_id = std::this_thread::get_id();
    g_display_mutex.lock();
    std::cout << "ThreadProc tid " << this_id << "\n";
    g_display_mutex.unlock();
    MqttPub *pub = (MqttPub*)lpParameter;
    pub->loop();
}

//static
MqttPub* MqttPub::instance()
{
    static MqttPub pub;
    return &pub;
}

static bool decompressZlib(const uint8_t* data, size_t dataSz, std::vector<uint8_t> &outData)
{
    outData.resize(dataSz * 7);
    size_t decompressSz = outData.size();
    int ret = Z_BUF_ERROR;

    while (Z_BUF_ERROR == (ret = uncompress(&outData.at(0), (uLongf*)&decompressSz, data, dataSz))) {
        decompressSz = outData.size() * 2;
        outData.resize(decompressSz);
    }

    if (ret == Z_OK) {
        outData.resize(decompressSz);
        return true;
    } else {
        outData.clear();
        fprintf(stderr, "zlib decompress error %d\n", ret);
        return false;
    }
}

// static
void MqttPub::on_message(struct mosquitto *m, void *obj, const struct mosquitto_message *msg)
{
	MqttPub *c = static_cast<MqttPub*>(obj);
	if (msg->topic[0] == 'c' && msg->topic[1] == '/') {
		// server response
		c->onResponse(m, obj, msg);
		return;
	}
}

class PkgTaskLooperImpl : public PkgTaskLooper {
protected:
	virtual void onSendPkgBuf(PkgData* pkg, uint8_t* buf, size_t length)
	{
		MqttPub::instance()->client_publish(pkg->topic().c_str(), buf, length, 0, false);
	}
};

static PkgTaskLooperImpl taskLooper;
static std::vector<uint8_t> marketInfoBin;
static std::vector<uint8_t> splitInfoBin;

void dumpMarketInfo()
{
	if (marketInfoBin.size() == 0) {
		printf("marketInfoBin size is 0\n");
		return;
	}

	VarLenFmtRo fmt(marketInfoBin.data(), marketInfoBin.size());
	const BinHead& head = fmt.head();
	const LabelId* labelIdVec = fmt.labelIdVec();
	const DataPos* dataPosVec = fmt.dataPosVec();
	const uint8_t* data = fmt.data();
	uint32_t idx, itemIdx;
	for (idx = 0; idx < head.cnt; idx++) {
		const LabelId *labelId = (labelIdVec + idx);
		const DataPos *dataPos = (dataPosVec + idx);
		const uint8_t* dataItem = data + dataPos->offset;

		const MarketInfo *mkt = (const MarketInfo*)(dataItem);
		std::string mktId(mkt->marketId, mkt->marketId + 2);
		printf("market: %s %s\n", mktId.c_str(), mkt->marketName);

		const StockInfo *stkInfoVec = (const StockInfo*)(dataItem + sizeof(MarketInfo));
		uint32_t vecSz = (dataPos->size - sizeof(MarketInfo)) / sizeof(StockInfo);
		for (itemIdx = 0; itemIdx < vecSz; itemIdx++) {
			const StockInfo *stk = stkInfoVec + itemIdx;
			// 注意这里 stockName 编码格式为 GBK
			printf("\t%s %s\n", stk->stockId, stk->stockName);
			if (itemIdx == 9) {
				printf("more...\n");
				break;  // 仅打印10条
			}
		}
	}
}

void dumpSplitInfo()
{
	if (splitInfoBin.size() == 0) {
		printf("splitInfoBin size is 0\n");
		return;
	}

	VarLenFmtRo fmt(splitInfoBin.data(), splitInfoBin.size());
	const BinHead& head = fmt.head();
	const LabelId* labelIdVec = fmt.labelIdVec();
	const DataPos* dataPosVec = fmt.dataPosVec();
	const uint8_t* data = fmt.data();
	uint32_t idx, itemIdx;
	for (idx = 0; idx < head.cnt; idx++) {
		const LabelId *labelId = (labelIdVec + idx);
		const DataPos *dataPos = (dataPosVec + idx);
		const uint8_t* dataItem = data + dataPos->offset;

		const SplitInfo *splitInfoVec = (const SplitInfo*)(dataItem);
		uint32_t vecSz = dataPos->size / sizeof(SplitInfo);
		printf("label:%s, id:%d, cnt: %d\n", labelId->label, labelId->idx, vecSz);
		for (itemIdx = 0; itemIdx < vecSz; itemIdx++) {
			const SplitInfo *s = splitInfoVec + itemIdx;
			// uint32_t    time;
			// float       give;           // 每股送
			// float       allocate;       // 每股配多少股
			// float       allocatePrice;  // 每股配价
			// float       earnings;       // 每股红利
			printf("\t%d, %f, %f, %f, %f\n", s->time, s->give, s->allocate, s->allocatePrice, s->earnings);
		}
		if (idx == 2) {
			printf("more...\n");
			break;  // 仅打印3条
		}
	}
}

void MqttPub::onResponse(struct mosquitto *m, void *obj, const struct mosquitto_message *msg)
{
	std::shared_ptr<PkgData> pkg = std::shared_ptr<PkgData>(new PkgData);
	if (!PkgData::Parse((const uint8_t*)msg->payload, msg->payloadlen, *pkg)) {
		fprintf(stderr, "parse pkg failed\n");
		return;
	}
	taskLooper.onRecvPkg(std::move(pkg));
}

void MqttPub::testGetMarketInfo()
{
	std::shared_ptr<PkgData> pkg = std::shared_ptr<PkgData>(new PkgData);
	std::string topic;
	topic += "s/" + clientid + "/meta/MarketInfo";
	pkg->setTopic(topic);

	marketInfoBin.clear();
	taskLooper.sendPkg(pkg,
		[](PkgTask *task) {
		const std::vector<uint8_t>& payload = task->onePkg()->payload().data();
		fprintf(stdout, "on resolve length %ld\n", payload.size());
		if (decompressZlib(payload.data(), payload.size(), marketInfoBin)) {
			fprintf(stdout, "on resolve decompress success %ld\n", marketInfoBin.size());
		}
		else {
			fprintf(stdout, "on resolve decompress failed %ld\n", marketInfoBin.size());
		}
	},
		[](PkgTask *task) {
		fprintf(stderr, "on reject\n");
	}
	);

	taskLooper.loop();
	dumpMarketInfo();
}

void MqttPub::testGetSplitInfo()
{
	std::shared_ptr<PkgData> pkg = std::shared_ptr<PkgData>(new PkgData);
	std::string topic;
	topic += "s/" + clientid + "/meta/SplitInfo";
	pkg->setTopic(topic);

	splitInfoBin.clear();
	taskLooper.sendPkg(pkg,
		[](PkgTask *task) {
		const std::vector<uint8_t>& payload = task->onePkg()->payload().data();
		fprintf(stdout, "on resolve length %ld\n", payload.size());
		if (decompressZlib(payload.data(), payload.size(), splitInfoBin)) {
			fprintf(stdout, "on resolve decompress success %ld\n", splitInfoBin.size());
		}
		else {
			fprintf(stdout, "on resolve decompress failed %ld\n", splitInfoBin.size());
		}
	},
		[](PkgTask *task) {
		fprintf(stderr, "on reject\n");
	}
	);

	taskLooper.loop();
	dumpSplitInfo();
}
