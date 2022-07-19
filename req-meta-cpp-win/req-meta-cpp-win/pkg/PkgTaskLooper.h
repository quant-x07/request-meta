#pragma once
#include "PkgData.h"
#include "PkgTask.h"
#include <vector>
#include <mutex>

class PkgTaskLooper {
public:
    void sendPkg(std::shared_ptr<PkgData> pkg, PkgTask::ResolveFunc resolve, PkgTask::RejectFunc reject);
    // may called from other thread
    bool onRecvPkg(std::shared_ptr<PkgData> pkg);

    // check once
    int poll();

    // loop to empty task queue.
    void loop();

    void destroy();

protected:
    virtual void onSendPkgBuf(PkgData* pkg, uint8_t* buf, size_t length) {}

private:
    bool _triggerTaskQueue();
    bool _sendTask(PkgTask *task);
    // to check timeout and finish it.
    void _checkTaskQueue(uint32_t currentTs);
    void _dumpTaskQueue();

private:
    std::vector<std::shared_ptr<PkgTask>> _reqTaskQueue;
    std::mutex _mutex;
};
