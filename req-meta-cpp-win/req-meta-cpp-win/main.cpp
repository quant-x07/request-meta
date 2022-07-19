//#define TEST_QUOTE
#define TEST_META
#include "MqttPub.h"

int main()
{
    MqttPub *pub = MqttPub::instance();
    pub->client_init();
	pub->testGetMarketInfo();
	pub->testGetSplitInfo();
    pub->loopForever();
    pub->stopAndJoin();
    return 0;
}
