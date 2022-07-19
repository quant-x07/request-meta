#pragma once
#include <vector>
#include <stdint.h>

#define PKG_HEADER_MAGIC 0xAA0A
#define PKG_HEADER_VERSION 1

class PkgHeader {
    static uint32_t gMsgId;
public:
    typedef enum {
        TWO_WAY = 0,
        ONE_WAY = 1,
    } ConnType;

    typedef enum {
        ENC_TYPE_NONE = 0,
        ENC_TYPE_ZLIB = 1,
        ENC_TYPE_XZ = 2,
    } PayloadEncType;
public:
    // parse function
    void parse(const uint8_t *data, size_t offset);

    // prepare package function
    void packBuf(std::vector<uint8_t> &outBuf);

    PkgHeader clone();

    void updateMsgId();

public:
    uint16_t magic = PKG_HEADER_MAGIC;
    uint16_t version = PKG_HEADER_VERSION;

    uint32_t msgId = 0;  // communication sequence id
    uint16_t msgType = 0;  // how to interpret payload, may business logic
    uint8_t payloadEncType = ENC_TYPE_NONE;  // plain zlib xz
    uint8_t connType = TWO_WAY;  // one way, two way

    // package split
    uint16_t totalFrame = 0;
    uint16_t frameSeq = 0;
    uint32_t frameLength = 0;
};
