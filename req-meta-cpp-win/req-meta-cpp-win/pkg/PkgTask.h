#pragma once
#include "PkgData.h"
#include <memory>
#include <functional>

// 10s
#define PKG_TASK_TIMEOUT 10000

class PkgTask {
public:
    typedef enum {
        STATE_IDLE = 0,
        STATE_DOING = 1,
        STATE_DONE = 2,
        STATE_COMMON_ERR = 3,
        STATE_TIMEOUT = 4,
        STATE_DESTROY = 5
    } State;

    typedef enum {
        RECV_NONE = 0,
        RECV_DONE = 1
    } RecvRet;

    typedef std::function<void(PkgTask*)> ResolveFunc;
    typedef std::function<void(PkgTask*)> RejectFunc;

public:
    PkgTask(std::shared_ptr<PkgData> pkg, ResolveFunc resolve, RejectFunc reject);
    State state() { return _state; }
    PkgData* pkg() { return _pkg.get(); }

    RecvRet onRecvPkg(std::shared_ptr<PkgData> pkg);

    void onOneWay();

    bool checkTimeout(uint32_t currentTs);

    void destroy();

    void dump() const;

    bool fullfill();
    bool canBeFullfill();

    bool canRecv();

	std::shared_ptr<PkgData> onePkg() { return _onePkg;  }
private:
    void _changeState(State state);

private:
    friend class PkgTaskLooper;
    State _state;
    std::shared_ptr<PkgData> _pkg;
    std::shared_ptr<PkgData> _onePkg;
    ResolveFunc _resolve;
    RejectFunc _reject;
    uint32_t _startTs;
    uint32_t _lastTs;
    std::vector<std::shared_ptr<PkgData>> _recvPkgs;
    bool _fullfilled;
};
