#pragma once

typedef struct {
    uint32_t    time;
    float       give;           // 每股送
    float       allocate;       // 每股配多少股
    float       allocatePrice;  // 每股配价
    float       earnings;       // 每股红利
} SplitInfo;

