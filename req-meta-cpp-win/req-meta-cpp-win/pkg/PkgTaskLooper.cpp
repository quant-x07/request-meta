#include "PkgTaskLooper.h"
#include <stdio.h>
#include <thread>
#include <chrono>

bool PkgTaskLooper::onRecvPkg(std::shared_ptr<PkgData> pkg)
{
    const std::lock_guard<std::mutex> lock(_mutex);

    if (_reqTaskQueue.size() == 0) {
        fprintf(stderr, "omit, reqTaskQueue length == 0\n");
        return false;
    }

    uint32_t idx = 0;
    while (idx < _reqTaskQueue.size()) {
        PkgTask &task = *_reqTaskQueue.at(idx);
        PkgTask::RecvRet recvRet = task.onRecvPkg(std::move(pkg));
        if (recvRet == PkgTask::RECV_DONE) {
            _triggerTaskQueue();
            return true;
        } else {
            // goon
        }

        idx++;
    }

    return false;
}

void PkgTaskLooper::sendPkg(std::shared_ptr<PkgData> pkg, PkgTask::ResolveFunc resolve, PkgTask::RejectFunc reject)
{
    const std::lock_guard<std::mutex> lock(_mutex);

    pkg->header().updateMsgId();
    std::shared_ptr<PkgTask> task = std::shared_ptr<PkgTask>(new PkgTask(std::move(pkg), std::move(resolve), std::move(reject)));
    _reqTaskQueue.emplace_back(std::move(task));
    _triggerTaskQueue();
}

bool PkgTaskLooper::_triggerTaskQueue()
{
    if (_reqTaskQueue.size() == 0) {
        return false;
    }
    PkgTask &task = *_reqTaskQueue.at(0);
    if (task.state() != PkgTask::STATE_IDLE) {
        return false;
    }
    _sendTask(&task);
    if (task._pkg->header().connType == PkgHeader::ONE_WAY) {
        task.onOneWay();
        _reqTaskQueue.erase(_reqTaskQueue.begin());  // remove first element
        _triggerTaskQueue();
    }
    return true;
}

bool PkgTaskLooper::_sendTask(PkgTask *task)
{
    if (task->_state != PkgTask::STATE_IDLE) {
        return false;
    }

    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    task->_startTs = (uint32_t)millis;
    task->_lastTs = (uint32_t)millis;

    task->_changeState(PkgTask::STATE_DOING);

    std::vector<uint8_t> buf;
    task->_pkg->packBuf(buf);
    onSendPkgBuf(task->_pkg.get(), buf.data(), buf.size());

    return true;
}

int PkgTaskLooper::poll()
{
    const std::lock_guard<std::mutex> lock(_mutex);

    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    uint32_t currentTs = (uint32_t)millis;
    _checkTaskQueue(currentTs);
    return _reqTaskQueue.size();
}

void PkgTaskLooper::loop()
{
    while (poll() > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void PkgTaskLooper::_checkTaskQueue(uint32_t currentTs)
{
    bool changed = false;
    size_t idx = 0;
    while (idx < _reqTaskQueue.size()) {
        if (_reqTaskQueue.at(idx)->canBeFullfill()) {
            changed = true;
            _reqTaskQueue.at(idx)->fullfill();
            _reqTaskQueue.erase(_reqTaskQueue.begin() + idx);
        } else if (_reqTaskQueue.at(idx)->checkTimeout(currentTs)) {
            changed = true;
            _reqTaskQueue.at(idx)->fullfill();
            _reqTaskQueue.erase(_reqTaskQueue.begin() + idx);
        } else {
            idx++;
        }
    }
    if (changed) {
        // update _reqTaskQueue
        _triggerTaskQueue();
    }
    if (_reqTaskQueue.size() > 0 && _reqTaskQueue.at(0)->state() == PkgTask::STATE_IDLE) {
        // robust
        _triggerTaskQueue();
    }
    _dumpTaskQueue();
}

void PkgTaskLooper::destroy()
{
    const std::lock_guard<std::mutex> lock(_mutex);

    for (auto &task: _reqTaskQueue) {
        task->destroy();
    }
    _reqTaskQueue.clear();
}

void PkgTaskLooper::_dumpTaskQueue()
{
    if (_reqTaskQueue.size() == 0) {
        fprintf(stdout, "### dumpTaskQueue size 0\n");
        return;
    }
    fprintf(stdout, "### dumpTaskQueue begin len %lu\n", _reqTaskQueue.size());
    for (auto &task: _reqTaskQueue) {
        task->dump();
    }
    fprintf(stdout, "### dumpTaskQueue end\n");
}
