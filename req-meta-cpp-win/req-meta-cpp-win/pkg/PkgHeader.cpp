#include "PkgHeader.h"
#include <string.h>

// static
uint32_t PkgHeader::gMsgId = 0;

void PkgHeader::parse(const uint8_t *data, size_t offset)
{
    memcpy(this, data + offset, sizeof(PkgHeader));
}

PkgHeader PkgHeader::clone()
{
    return *this;
}

void PkgHeader::packBuf(std::vector<uint8_t> &outBuf)
{
    uint32_t offset = outBuf.size();
    outBuf.resize(offset + sizeof(PkgHeader));
    memcpy(outBuf.data() + offset, this, sizeof(PkgHeader));
}

void PkgHeader::updateMsgId()
{
    msgId = ++PkgHeader::gMsgId;
}
