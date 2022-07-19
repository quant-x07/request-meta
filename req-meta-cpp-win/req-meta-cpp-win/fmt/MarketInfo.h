#pragma once

#define STK_NAME_LEN 32
#define STK_CODE_LEN 10

#pragma pack(push, 1)   //表示以字节对齐

struct MarketInfo {
    char        marketId[2];                // 市场代码, "SH", "SZ"
    char        marketName[STK_NAME_LEN];   // 市场名称(英文简称，如 SHSE 表示上海交易所), \0结尾
    int         localTime;                  // 时区
    short       marketKind;                 // 市场属性(1 股票；3 期货)
    short       preserved0;
    short       periodCount;                // 交易时段个数
    short       openTime[8];                // 开市时间 1,2,3,4,5 (开盘分钟数，比如 570 表示 9:30)
    short       closeTime[8];               // 收市时间 1,2,3,4,5
    uint32_t    preserved1;
    uint32_t    count;                      // 该市场的证券个数
};

struct StockInfo {
    short   seq;                        // 序号
    char    stockId[STK_CODE_LEN];      // 股票代码, \0结尾
    char    stockName[STK_NAME_LEN];    // 股票名称, \0结尾
    char    block;                      // 品种属性，0指数，1 A、B股，2 基金, 3 债券，4 质押 ，7 期货，8 期货指数
    char    pointCount;                 // 小数点个数 0、1、2、3
    short   hand;
};

#pragma pack(pop)   //表示以字节对齐
