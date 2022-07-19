#pragma once
#include <stddef.h>
#include <stdint.h>
#include <vector>

class PkgPayload {
public:
    void setData(const uint8_t *data, size_t offset, size_t length);
    const std::vector<uint8_t>& data() { return _data; }

    void packBuf(std::vector<uint8_t> &outBuf);

public:
    std::vector<uint8_t> _data;
};
