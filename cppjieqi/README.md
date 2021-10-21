# CPP README

## Quick Start:

On Ubuntu:

`mkdir build && cd build && cmake .. && make`

On Windows:

`mkdir build && cd build && cmake .. -G "MinGW Makefiles" && make`

Windows的终端字符集要设为UTF-8, 否则会出现乱码

On Mac:

To Be Done.



注: 如果您不希望显示电脑吃您的暗子, 请注释CMakeLists.txt中的add_definitions(-DSHOWDARK)

下棋: 输入4位UCCI表示。例如, 兵一进一就是i3i4。

## Players.conf

第一行表示红方, 第二行表示黑方

0表示人，1, 2, ...表示电脑, eg:

1

0

表示AIBoard1是玩家1, AIBoard2是玩家2。

2

1

表示AIBoard2是玩家1,  AIBoard1是玩家2。

第三行表示胜利阈值。玩家1, 2交替执红黑行棋, 谁的胜利盘数先到阈值谁赢。第一局玩家一执红棋。

第四行表示对局日志文件路径。可以调用God::Play函数回放对局。如果该行以@开头, 代表初始时会清空日志文件。

## 双递归&&不确定子的明子化

To Be Done



