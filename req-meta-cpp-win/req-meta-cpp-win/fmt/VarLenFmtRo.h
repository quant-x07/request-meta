#pragma once
#include <stdint.h>
#include <vector>
#include "Common.h"

typedef struct {
    uint16_t magic;
    uint16_t version;
    uint16_t maxCnt;
    uint16_t cnt;
} BinHead;

typedef struct {
    char label[12];
    uint32_t idx;
} LabelId;

int labelId_comp(const LabelId * a, const LabelId * b);
DEC_BIN_SEARCH(labelId_bin_search, const LabelId, labelId_comp)

typedef struct DataPos {
    uint32_t offset;
    uint32_t size;
} DataPos;

class VarLenFmtRo {
public:
    VarLenFmtRo(const uint8_t *buf, size_t bufSz);
    bool searchLabelId(LabelId &search);

    const BinHead& head() const { return _head; }
    const LabelId* labelIdVec() const { return _labelIdVec; }
    const DataPos* dataPosVec() const { return _dataPosVec; }
    const uint8_t* data() const { return _data; }

private:
    void _parse();

private:
    const uint8_t* _buf;
    size_t _bufSz;

    BinHead _head;
    const LabelId* _labelIdVec;
    const DataPos* _dataPosVec;
    const uint8_t* _data;
};

