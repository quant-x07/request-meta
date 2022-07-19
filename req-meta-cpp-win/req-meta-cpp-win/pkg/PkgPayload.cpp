#include "PkgPayload.h"
#include <math.h>
#include <string.h>

void PkgPayload::setData(const uint8_t *data, size_t offset, size_t length)
{
    _data.resize(length);
    memcpy(_data.data(), data+offset, length);
}

void PkgPayload::packBuf(std::vector<uint8_t> &outBuf)
{
    if (_data.empty()) return;
    uint32_t offset = outBuf.size();
    outBuf.resize(offset + _data.size());
    memcpy(outBuf.data() + offset, _data.data(), _data.size());
}
