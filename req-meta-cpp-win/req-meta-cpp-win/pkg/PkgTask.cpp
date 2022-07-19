#include "PkgTask.h"
#include <chrono>

PkgTask::PkgTask(std::shared_ptr<PkgData> pkg, ResolveFunc resolve, RejectFunc reject)
{
    _pkg = std::move(pkg);
    _resolve = resolve;
    _reject = reject;
    _state = STATE_IDLE;
    _startTs = 0;
    _lastTs = 0;
    _fullfilled = false;
}

// NOTE: this function may called from another thread, so _resolve _reject do
// not dispatched here, and waiting later from main thread using fullfill to dispatch.
PkgTask::RecvRet PkgTask::onRecvPkg(std::shared_ptr<PkgData> pkg)
{
    if (!canRecv()) {
        return RECV_NONE;
    }

    if (pkg->header().msgId != _pkg->header().msgId) {
        return RECV_NONE;
    }

    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    _lastTs = (uint32_t)millis;

    // 400 and 500
    if (pkg->header().msgType >= 400) {
        // 通用错误包
        fprintf(stderr, "PkgCommonError msgType %d, pkg dump:\n", pkg->header().msgType);
        dump();
        // TODO: save this error package?
        _changeState(STATE_COMMON_ERR);
        return RECV_DONE;
    }

    fprintf(stdout, "------> msgId %d\n", pkg->header().msgId);

    _onePkg = pkg;
    _changeState(STATE_DONE);
    return RECV_DONE;
}

void PkgTask::onOneWay()
{
    _changeState(STATE_DONE);
    if (_resolve) {
        _resolve(this);
    }
}

bool PkgTask::checkTimeout(uint32_t currentTs)
{
    if (_lastTs == 0) return false;
    if (currentTs - _lastTs < PKG_TASK_TIMEOUT) {
        return false;
    }
    _changeState(STATE_TIMEOUT);
    return true;
}

void PkgTask::_changeState(State state)
{
    _state = state;
}

void PkgTask::destroy()
{
    _changeState(STATE_DESTROY);
    if (_reject) {
        _reject(this);
    }
}

void PkgTask::dump() const
{
    fprintf(stdout, "msgId:%d, msgType:%d, state:%d\n", _pkg->header().msgId,
            _pkg->header().msgType, _state);
}

// return true means fullfilled, and can be removed from queue
bool PkgTask::fullfill()
{
    if (_fullfilled) {
        return true;
    }
    if (_state == STATE_DONE) {
        if (_resolve) {
            _resolve(this);
        }
        _fullfilled = true;
        return true;
    }
    if (_state == STATE_COMMON_ERR || _state == STATE_TIMEOUT || _state == STATE_DESTROY) {
        if (_reject) {
            _reject(this);
        }
        _fullfilled = true;
        return true;
    }
    return false;
}

bool PkgTask::canBeFullfill()
{
    return _state != STATE_IDLE && _state != STATE_DOING;
}

bool PkgTask::canRecv()
{
    return _state == STATE_DOING;
}
