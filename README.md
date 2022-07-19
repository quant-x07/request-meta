# request-meta
在mqtt之上封装req rep模式，获取market股票列表、除权除息等meta信息

## 如何运行
* 准备 visual studio 环境（在vs2017验证通过）

* 打开 sln 项目，注意选择 x86 编译模式，mosquitto 以及 zlib 库已经附带，编译即可

* 运行如下
```
ThreadProc tid 32448### dumpTaskQueue begin len 1

msgId:1, msgType:0, state:1
### dumpTaskQueue end
on_connect: Connection Accepted.
sub rc 0
Packet ID 2: The topic has been subscribed.
------> msgId 1
on resolve length 206663
on resolve decompress success 1117616
### dumpTaskQueue size 0
market: SH SHSE
        000001 上证指数
        000002 Ａ股指数
        000003 Ｂ股指数
        000004 工业指数
        000005 商业指数
        000006 地产指数
        000007 公用指数
        000008 综合指数
        000009 上证380
        000010 上证180
more...
market: SZ SZSE
        000001 平安银行
        000002 万 科Ａ
        000003 PT金田A
        000004 ST国华
        000005 ST星源
        000006 深振业Ａ
        000007 全新好
        000008 神州高铁
        000009 中国宝安
        000010 美丽生态
more...
### dumpTaskQueue begin len 1
msgId:2, msgType:0, state:1
### dumpTaskQueue end
------> msgId 2
on resolve length 301835
on resolve decompress success 1141908
### dumpTaskQueue size 0
label:SH500001, id:0, cnt: 10
        923356800, 0.000000, 0.000000, 0.000000, 0.049000
        955411200, 0.000000, 0.000000, 0.000000, 0.190000
        986774400, 0.000000, 0.000000, 0.000000, 0.425000
        1017964800, 0.000000, 0.000000, 0.000000, 0.016000
        1112832000, 0.000000, 0.000000, 0.000000, 0.035000
        1174521600, 0.000000, 0.000000, 0.000000, 0.290000
        1206662400, 0.000000, 0.000000, 0.000000, 0.500000
        1209340800, 0.000000, 0.000000, 0.000000, 1.208000
        1269388800, 0.000000, 0.000000, 0.000000, 0.110000
        1303430400, 0.000000, 0.000000, 0.000000, 0.209000
label:SH500002, id:1, cnt: 11
        954115200, 0.000000, 0.000000, 0.000000, 0.022000
        968025600, 0.000000, 0.000000, 0.000000, 0.130000
        986860800, 0.000000, 0.000000, 0.000000, 0.147000
        999820800, 0.000000, 0.000000, 0.000000, 0.226000
        1157068800, 0.000000, 0.000000, 0.000000, 0.130000
        1175817600, 0.000000, 0.000000, 0.000000, 0.480000
        1185926400, 0.000000, 0.000000, 0.000000, 0.655000
        1207180800, 0.000000, 0.000000, 0.000000, 1.640000
        1270512000, 0.000000, 0.000000, 0.000000, 0.047000
        1301616000, 0.000000, 0.000000, 0.000000, 0.172000
        1395705600, 0.000000, 0.000000, 0.000000, 0.248600
label:SH500003, id:2, cnt: 16
        923356800, 0.000000, 0.000000, 0.000000, 0.042000
        953596800, 0.000000, 0.000000, 0.000000, 0.320000
        986515200, 0.000000, 0.000000, 0.000000, 0.575000
        1017964800, 0.000000, 0.000000, 0.000000, 0.230000
        1080864000, 0.000000, 0.000000, 0.000000, 0.010000
        1088035200, 0.000000, 0.000000, 0.000000, 0.050000
        1114473600, 0.000000, 0.000000, 0.000000, 0.006000
        1145404800, 0.000000, 0.000000, 0.000000, 0.029000
        1175817600, 0.000000, 0.000000, 0.000000, 0.420000
        1182384000, 0.000000, 0.000000, 0.000000, 0.200000
        1191801600, 0.000000, 0.000000, 0.000000, 0.200000
        1201219200, 0.000000, 0.000000, 0.000000, 0.300000
        1207785600, 0.000000, 0.000000, 0.000000, 0.770000
        1238457600, 0.000000, 0.000000, 0.000000, 0.260000
        1270080000, 0.000000, 0.000000, 0.000000, 0.400000
        1300924800, 0.000000, 0.000000, 0.000000, 0.120000
more...
loopForever tid 19380
```

注意 默认体验密码和服务器地址都已经在程序中固定，但可能会过期，如果有需要可以加qq群：444206515 获取

