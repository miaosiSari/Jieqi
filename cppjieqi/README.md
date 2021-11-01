# CPP README

## Quick Start:

On Ubuntu:

`mkdir build && cd build && cmake .. && make`

On Windows:

`mkdir build && cd build && cmake .. -G "MinGW Makefiles" && make`

Windows的终端字符集要设为UTF-8, 否则会出现乱码

On Mac:

同Ubuntu


注: 如果您不希望显示电脑吃您的暗子, 请注释CMakeLists.txt中的add_definitions(-DSHOWDARK)

下棋: 输入4位UCCI表示。例如, 兵一进一就是i3i4。

## Players.conf:

第一行表示红方, 第二行表示黑方

0表示人，1, 2, ...表示电脑, eg:

3

0

表示AIBoard1是玩家3, AIBoard2是玩家0(人类)。

3

5

表示AIBoard2是玩家3,  AIBoard1是玩家5。

第三行表示胜利阈值。玩家1, 2交替执红黑行棋, 谁的胜利盘数先到阈值谁赢。第一局玩家一执红棋。

第四行表示对局日志文件路径。可以调用God::Play函数回放对局。如果该行以@开头, 代表初始时会清空日志文件。

## 双递归&&不确定子的明子化:

AI4还在调试状态, 请先尝试AI3,5。

## AI描述

|  AI  |                             描述                             |
| :--: | :----------------------------------------------------------: |
|  1   |                        最原始AI(淘汰)                        |
|  2   | 在AI1的基础上增添了保护子和过河兵的逻辑(淘汰)。保护子: 暗车, 暗士, 中明士。如果保护子小于4, 则过河兵的价值将有一定程度增加, 增加幅度取决于过河兵到花心的L1距离(ScanProtectors) |
|  3   |                  在AI2的基础上做了细节调优                   |
|  4   |                           双递归AI                           |
|  5   |     在AI3的基础上增加了全局置换表, 每步走完置换表不清空      |



