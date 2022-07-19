#pragma once
#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <string>
#include "PkgHeader.h"
#include "PkgPayload.h"

class PkgData {
public:
    // 256KB bytes
    PkgData();

    PkgHeader& header() { return _header; }
    PkgPayload& payload() { return _payload; }
    void updateMsgId();

    // parse function
    static size_t Parse(const uint8_t* data, size_t length, PkgData &outPkg);

    void packBuf(std::vector<uint8_t> &outBuf);

    void setTopic(const std::string &topic) { _topic = topic;  }
    std::string& topic() { return _topic;  }

private:
    PkgHeader _header;
    PkgPayload _payload;
    std::string _topic;
};
