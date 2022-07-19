#include "VarLenFmtRo.h"
#include <string.h>
#include <assert.h>

int labelId_comp(const LabelId * a, const LabelId * b)
{
    return memcmp(a->label, b->label, sizeof(LabelId::label));
}

VarLenFmtRo::VarLenFmtRo(const uint8_t *buf, size_t bufSz)
{
    _buf = buf;
    _bufSz = bufSz;
    _parse();
}

void VarLenFmtRo::_parse()
{
    uint32_t offset = 0;
    assert(_bufSz > sizeof(BinHead));
    memcpy(&_head, _buf, sizeof(BinHead));
    offset += sizeof(BinHead);

    _labelIdVec = (const LabelId*)(_buf + offset);
    offset += _head.maxCnt * sizeof(LabelId);
    assert(offset <= _bufSz);

    _dataPosVec = (const DataPos*)(_buf + offset);
    offset += _head.maxCnt * sizeof(DataPos);
    assert(offset <= _bufSz);
    _data = _buf + offset;
    // TODO: check data length
}

bool VarLenFmtRo::searchLabelId(LabelId &search)
{
    int outIdx;
    if (1 == labelId_bin_search(_labelIdVec, _head.cnt, &search, &outIdx)) {
        // true
        search = _labelIdVec[outIdx];
        return true;
    } else {
        return false;
    }
}
