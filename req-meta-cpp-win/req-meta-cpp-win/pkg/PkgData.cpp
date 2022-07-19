#include "PkgData.h"
#include <assert.h>
#include <stdio.h>

PkgData::PkgData()
{
}

void PkgData::packBuf(std::vector<uint8_t> &outBuf)
{
    _header.packBuf(outBuf);
    _payload.packBuf(outBuf);
}

// static
size_t PkgData::Parse(const uint8_t* data, size_t dataSz, PkgData &outPkg)
{
    PkgHeader tmpHeader;
    if (dataSz < sizeof(PkgHeader)) {
        fprintf(stderr, "package data length should large than header size %lu\n", sizeof(PkgHeader));
        return 0;
    }
    tmpHeader.parse(data, 0);
    if (tmpHeader.magic != PKG_HEADER_MAGIC || tmpHeader.version > PKG_HEADER_VERSION) {
        fprintf(stderr, "package support magic version: %04X %d, got %04X %d", PKG_HEADER_MAGIC, PKG_HEADER_VERSION,
                tmpHeader.magic, tmpHeader.version);
        return 0;
    }

    uint32_t offset = sizeof(PkgHeader);
    uint32_t payloadLen = tmpHeader.frameLength;
    if (dataSz < offset + payloadLen) {
        fprintf(stderr, "package length %u, but got data length %lu", offset + payloadLen, dataSz);
        return 0;
    }
    outPkg._payload.setData(data, offset, payloadLen);
    outPkg._header = tmpHeader;
    return offset + payloadLen;
}
