import random
#参考代码
#https://github.com/NeymarL/ChineseChess-AlphaZero/
#https://github.com/bupticybee/icyChessZero
#https://zhuanlan.zhihu.com/p/34433581

#Notations:
#棋盘的每一个位置是一个五位二进制数
#0: 无子
#1: 车
#2: 马
#3: 相
#4: 士
#5: 帅
#6: 炮
#7: 兵
#1 << 3位: 红1/黑0
#1 << 4位: 明0/暗1
#1 << 5位: Uncertain
RED_SHUAI = 13 #(0 << 4 + 1 << 3 + 5)
BLACK_JIANG = 5 #(0 << 4 + 0 << 3 + 5)


#棋盘的数字编码:
'''
9
8
7
6
5
4
3
2
1
012345678
'''

#棋盘的UCCI编码(https://www.xqbase.com/protocol/cchess_ucci.htm):
'''
9
8
7
6
5
4
3
2
1
0
abcdefghi
'''

#第一个下标是纵向, 维度为10, 贯穿双方底线
#第二个下标是横向, 维度为9, 贯穿双方底线

MASK_CHESS = 7 #计算子的类型
MASK_COLOR = 8 #计算子的颜色
MASK_CHESS_COLOR = 15 #计算子的类型+颜色
MASK_CHESS_ISCOVERED = 16 #计算子的明暗
MASK_CHESS_UNCERTAIN = 32 #此子是否不确定

DIRECTION_DICT = {
  #红明
  (0<<4)+(1<<3)+2: {(2, 1), (1, 2), (-1, 2), (-2, 1), (-2, -1), (-1, -2), (1, -2), (2, -1)}, #马走日
  (0<<4)+(1<<3)+3: {(2, 2), (-2, 2), (-2, -2), (2, -2)}, #相走田
  (0<<4)+(1<<3)+4: {(1, 1), (-1, 1), (-1, -1), (1, -1)}, #红士
  (0<<4)+(1<<3)+5: {(1, 0), (0, 1), (-1, 0), (0, -1)},#红帅
  (0<<4)+(1<<3)+7: {(1, 0), (0, -1), (0, 1)}, #红兵向前， x轴增加
  #红暗
  (1<<4)+(1<<3)+2: {(2, -1), (2, 1), (1, -2), (2, 1)}, #红暗马，不能往棋盘外跳
  (1<<4)+(1<<3)+3: {(2, 2), (2, -2)}, #红暗相，不能往棋盘外飞
  (1<<4)+(1<<3)+4: {(1, 1), (1, -1)}, #红暗士，不能往棋盘外撑
  (1<<4)+(1<<3)+7: {(1, 0)}, #红暗兵，只能向前拱, x坐标提升
  #黑明
  (0<<4)+(0<<3)+2: {(2, 1), (1, 2), (-1, 2), (-2, 1), (-2, -1), (-1, -2), (1, -2), (2, -1)}, #马走日
  (0<<4)+(0<<3)+3: {(2, 2), (-2, 2), (-2, -2), (2, -2)}, #相走田
  (0<<4)+(0<<3)+4: {(1, 1), (-1, 1), (-1, -1), (1, -1)}, #黑士
  (0<<4)+(0<<3)+5: {(1, 0), (0, 1), (-1, 0), (0, -1)},#黑将
  (0<<4)+(0<<3)+7: {(-1, 0), (0, -1), (0, 1)}, #黑卒向后， x轴减小
  #黑暗
  (1<<4)+(0<<3)+2: {(-2, -1), (-2, 1), (-1, -2), (-1, 2)}, #黑暗马，不能往棋盘外跳
  (1<<4)+(0<<3)+3: {(-2, 2), (-2, -2)}, #黑暗相，不能往棋盘外飞
  (1<<4)+(0<<3)+4: {(-1, 1), (-1, -1)}, #黑暗士，不能往棋盘外撑
  (1<<4)+(0<<3)+7: {(-1, 0)} #黑暗兵，只能向前拱, x坐标降低
}
DIRECTION_DICT_KEYS = DIRECTION_DICT.keys()


DESC_DICT = {
   0: '  ',
   1:'车', 9: '车',
   2:'馬', 10: '傌',
   3:'象', 11: '相',
   4:'士', 12: '仕',
   5:'将', 13: '帅',
   6:'炮', 14: '炮',
   7:'卒', 15: '兵',
}


CHESSES = [1, 1, 2, 2, 3, 3, 4, 4, 6, 6, 7, 7, 7, 7, 7] #不能映射老将
RED_POSITIONS = [(0, 0), (0, 1), (0, 2), (0, 3), (0, 5), (0, 6), (0, 7), (0, 8), (2, 1), (2, 7), (3, 0), (3, 2), (3, 4), (3, 6), (3, 8)] #红暗子所在地
BLACK_POSITIONS = [(9, 0), (9, 1), (9, 2), (9, 3), (9, 5), (9, 6), (9, 7), (9, 8), (7, 1), (7, 7), (6, 0), (6, 2), (6, 4), (6, 6), (6, 8)] #黑暗子所在地
COVERED_POSITIONS = set(RED_POSITIONS + BLACK_POSITIONS)
LEN_COVERED_POSITIONS = len(COVERED_POSITIONS)
ALL_POSITIONS = set()
for i in range(10):
    for j in range(9):
        ALL_POSITIONS.add((i, j))
RED_JIUGONG = {(0, 3), (0, 4), (0, 5), (1, 3), (1, 4), (1, 5), (2, 3), (2, 4), (2, 5)} #红方九宫格
BLACK_JIUGONG = {(9, 3), (9, 4), (9, 5), (8, 3), (8, 4), (8, 5), (7, 3), (7, 4), (7, 5)} #黑方九宫格
COVERED_COUNTER = {(1 << 4)+(1 << 3)+1: 2, (1 << 4)+(1 << 3)+2: 2, (1 << 4)+(1 << 3)+3: 2, (1 << 4)+(1 << 3)+4: 2, (1 << 4)+(1 << 3)+6: 2, (1 << 4)+(1 << 3)+7: 5,
(1 << 4)+(0 << 3)+1: 2, (1 << 4)+(0 << 3)+2: 2, (1 << 4)+(0 << 3)+3: 2, (1 << 4)+(0 << 3)+4: 2, (1 << 4)+(0 << 3)+6: 2, (1 << 4)+(0 << 3)+7: 5}
UNCOVERED_COUNTER = {(0 << 4)+(1 << 3)+1: 2, (0 << 4)+(1 << 3)+2: 2, (0 << 4)+(1 << 3)+3: 2, (0 << 4)+(1 << 3)+4: 2, (0 << 4)+(1 << 3)+5: 1, (0 << 4)+(1 << 3)+6: 2, (0 << 4)+(1 << 3)+7: 5,
(0 << 4)+(0 << 3)+1: 2, (0 << 4)+(0 << 3)+2: 2, (0 << 4)+(0 << 3)+3: 2, (0 << 4)+(0 << 3)+4: 2, (0 << 4)+(0 << 3)+5: 1, (0 << 4)+(0 << 3)+6: 2, (0 << 4)+(0 << 3)+7: 5}
RED_BING_FORBIDDEN = {(0, 0), (0, 8), (1, 2), (1, 6)}
BLACK_ZU_FORBIDDEN = {(9, 0), (9, 8), (8, 2), (8, 6)}

pst = {
    "P": ( #兵
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0, 75, 84, 85, 87, 87, 87, 85, 84, 75,  0,  0,  0,  0,
      0,  0,  0, 75, 84, 89, 101, 113, 101, 89, 84, 75,  0,  0,  0,  0,
      0,  0,  0, 60, 74, 74, 79, 97, 79, 74, 74, 60,  0,  0,  0,  0,
      0,  0,  0, 47, 57, 47, 49, 60, 49, 47, 57, 47,  0,  0,  0,  0,
      0,  0,  0, 34, 55, 37, 37, 39, 37, 37, 55, 34,  0,  0,  0,  0,
      0,  0,  0, 24, 35, 20, 27, 29, 27, 20, 35, 24,  0,  0,  0,  0,
      0,  0,  0, 13, 13, 14, 14, 15, 14, 14, 13, 13,  0,  0,  0,  0,
      0,  0,  0,  5,  5,  6,  4,  5,  4,  6,  5,  5,  0,  0,  0,  0,
      0,  0,  0,  4,  4,  5, -6, -10, -6, 5,  4,  4,  0,  0,  0,  0,
      0,  0,  0,  2,  2,  3, -9, -12, -9, 3,  2,  2,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    ),
    "B": ( #相
    # *   *   *   a   b   c   d   e   f   g   h   i   *   *   *   *
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0, 70, 39, 12, 35, 72, 35, 12, 39, 70,  0,  0,  0,  0,  #  对方底线
      0,  0,  0, 65, 55, 15, 20, 68, 20, 15, 55, 65,  0,  0,  0,  0,
      0,  0,  0, 12, 41, 75, 41,  6, 41, 75, 41, 12,  0,  0,  0,  0,
      0,  0,  0, 27, 31, 70, 30, 25, 30, 70, 31, 27,  0,  0,  0,  0,  #  a6/i6相可以阻挡暗车
      0,  0,  0, 65, 35, 12, 35, 70, 35, 12, 35, 65,  0,  0,  0,  0,
      0,  0,  0, 60, 30, 31, 30, 65, 30, 31, 30, 60,  0,  0,  0,  0,
      0,  0,  0, 10, 35, 65, 35, 10, 35, 65, 35, 10,  0,  0,  0,  0,
      0,  0,  0, 35, 30, 49, 30, 43, 30, 49, 30, 35,  0,  0,  0,  0,
      0,  0,  0, 55, 27,  6, 27, 30, 27,  6, 27, 55,  0,  0,  0,  0,
      0,  0,  0, 35, 16, 40, 16, 20, 16, 40, 16, 35,  0,  0,  0,  0,  #  本方底线
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    ),
    "A": ( #士
    # *   *   *   a   b   c   d   e   f   g   h   i   *   *   *   *
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0, 98, 99, 90, 95, 90, 95, 90, 99, 98,  0,  0,  0,  0,
      0,  0,  0, 88, 103, 92, 115, 100, 115, 92, 103, 88,  0,  0,  0,  0,  #  对方底线
      0,  0,  0, 88, 88, 99, 109, 106, 109, 99, 88, 88,  0,  0,  0,  0,
      0,  0,  0, 85, 109, 88, 91, 88, 91, 88, 109, 85,  0,  0,  0,  0,
      0,  0,  0, 83, 85, 88, 86, 88, 86, 88, 85, 83,  0,  0,  0,  0,
      0,  0,  0, 81, 82, 85, 82, 85, 82, 85, 82, 81,  0,  0,  0,  0,
      0,  0,  0, 73, 85, 75, 85, 75, 85, 75, 85, 73,  0,  0,  0,  0,
      0,  0,  0, 82, 69, 89, 69, 93, 69, 89, 69, 82,  0,  0,  0,  0,
      0,  0,  0, 69, 90, 65, 100, 61, 100, 65, 90, 69,  0,  0,  0,  0,
      0,  0,  0, 65, 57, 95, 69, 105, 69, 95, 57, 65,  0,  0,  0,  0,
      0,  0,  0, 65, 92, 57, 104, 55, 104, 57, 92, 65,  0,  0,  0,  0,  #  本方底线
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    ),
    "N": ( #马
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0, 138, 120, 120, 126, 120, 126, 120, 120, 138,  0,  0,  0,  0,
      0,  0,  0, 120, 126, 153, 117, 124, 117, 153, 126, 120,  0,  0,  0,  0,
      0,  0,  0, 117, 113, 114, 135, 129, 135, 114, 113, 117,  0,  0,  0,  0,
      0,  0,  0, 113, 138, 122, 135, 120, 135, 122, 138, 113,  0,  0,  0,  0,
      0,  0,  0, 110, 120, 119, 133, 134, 133, 119, 120, 110,  0,  0,  0,  0,
      0,  0,  0, 110, 118, 121, 122, 122, 122, 121, 118, 110,  0,  0,  0,  0,
      0,  0,  0, 112, 114, 118, 115, 118, 115, 118, 114, 112,  0,  0,  0,  0,
      0,  0,  0, 113, 113, 114, 115, 125, 115, 114, 113, 113,  0,  0,  0,  0,
      0,  0,  0, 99, 105, 99, 99, 99, 99, 99, 105, 99,  0,  0,  0,  0,
      0,  0,  0, 85, 85, 90, 105, 95, 105, 90, 85, 85,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    ),
    "R": (#车
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,316,318,318,326,324,326,318,318,316,  0,  0,  0,  0, #增加底线车的分数。和明棋不同的是，在揭棋中底线车可以横扫暗子
      0,  0,  0,280,282,282,289,303,289,282,282,280,  0,  0,  0,  0,
      0,  0,  0,280,282,282,289,296,289,282,282,280,  0,  0,  0,  0,
      0,  0,  0,280,282,282,289,286,289,282,282,280,  0,  0,  0,  0,
      0,  0,  0,278,281,281,287,285,287,281,281,278,  0,  0,  0,  0,
      0,  0,  0,278,272,282,287,285,287,282,272,278,  0,  0,  0,  0,
      0,  0,  0,274,274,274,287,284,287,274,274,274,  0,  0,  0,  0,
      0,  0,  0,268,274,274,289,272,289,274,274,268,  0,  0,  0,  0,
      0,  0,  0,270,278,276,289,260,289,276,278,270,  0,  0,  0,  0,
      0,  0,  0,274,276,274,282,250,282,274,276,274,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    ),
    "C": ( #炮
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,195,195,192,182,170,182,192,195,195,  0,  0,  0,  0,
      0,  0,  0,128,138,126,122,129,122,126,138,128,  0,  0,  0,  0,
      0,  0,  0,127,127,126,121,122,121,126,127,127,  0,  0,  0,  0,
      0,  0,  0,126,129,129,128,130,128,129,129,126,  0,  0,  0,  0,
      0,  0,  0,126,126,126,126,130,126,126,126,126,  0,  0,  0,  0,
      0,  0,  0,125,126,129,126,130,126,129,126,125,  0,  0,  0,  0,
      0,  0,  0,126,126,126,126,140,126,126,126,126,  0,  0,  0,  0,
      0,  0,  0,127,126,100,129,140,129,100,126,127,  0,  0,  0,  0,
      0,  0,  0,126,127,128,128,135,128,128,127,126,  0,  0,  0,  0,
      0,  0,  0,126,126,127,129,129,129,127,126,126,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    ),
    "K": ( #帅
    # *   *   *   a   b   c   d   e   f   g   h   i   *   *   *   *
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  2460,  2470,  2460,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  2470,  2480,  2470,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  2490,  2500,  2490,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    ),
    "0": ( #Bonus
    # *   *   *   a   b   c   d   e   f   g   h   i   *   *   *   *
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  100,  0,  0,  0,  0,  0,  0,  0,  100,  0,  0,  0,  0, #  对方底线
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  30,  0,  30,  0, 30,  0, 30,  0, 30,  0,  0,  0,  0, #翻动暗兵有奖励
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  -80,  0,  0,  0,  0,  0,  0,  0,  -80,  0,  0,  0,  0, #  本方底线
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    )
}

usefulkeys = 'RBNAKCP'
discount_factor = 1.5
pst["1"] = {}
s = 0
for key in usefulkeys:
    pst["1"][key] = sum(pst[key])/90
    s += pst["1"][key]
pst["1"]["a"] = s/len(usefulkeys)
pst["1"]["d"] = pst["1"]["a"]//discount_factor

piece = {'P': 44, 'N': 108, 'B': 23, 'R': 233, 'A': 23, 'C': 101, 'K': 2500}
'''
#https://zhuanlan.zhihu.com/p/34433581
def create_uci_labels2():
    labels_array = []
    letters = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i']
    numbers = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']

    for l1 in range(9):
        for n1 in range(10):
            destinations = [(t, n1) for t in range(9)] + \
                           [(l1, t) for t in range(10)] + \
                           [(l1 + a, n1 + b) for (a, b) in
                            [(-2, -1), (-1, -2), (-2, 1), (1, -2), (2, -1), (-1, 2), (2, 1), (1, 2), (1, 1), (-1, 1), (-1, -1), (1, -1), (2, 2), (-2, 2), (-2, -2), (2, -2)]]  # 马走日, 相走田, 士走对角
            for (l2, n2) in destinations:
                if (l1, n1) != (l2, n2) and l2 in range(9) and n2 in range(10):
                    move = letters[l1] + numbers[n1] + letters[l2] + numbers[n2]
                    labels_array.append(move)

    return labels_array
'''

def create_uci_labels():
    labels_array = []
    letters = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i']
    numbers = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']

    for l1 in range(10):
        for n1 in range(9):
            destinations = [(t, n1) for t in range(10)] + \
                           [(l1, t) for t in range(9)] + \
                           [(l1 + a, n1 + b) for (a, b) in
                            [(-2, -1), (-1, -2), (-2, 1), (1, -2), (2, -1), (-1, 2), (2, 1), (1, 2), (1, 1), (-1, 1), (-1, -1), (1, -1), (2, 2), (-2, 2), (-2, -2), (2, -2)]]  # 马走日, 相走田, 士走对角
            for (l2, n2) in destinations:
                if (l1, n1) != (l2, n2) and (l2 >= 0 and l2 <= 9) and (n2 >= 0 and n2 <= 8):
                    move = letters[n1] + numbers[l1] + letters[n2] + numbers[l2]
                    labels_array.append(move)

    return labels_array


labels_array = create_uci_labels()
#print(labels_array, len(labels_array))


def random_select(s, return_type=None, num=None):
    #s is a (multi-)set. This function selects one element from the power set of s.
    lens = len(s)
    if num is None:
       num = random.randint(0, lens)
    sampled = random.sample(s, num)
    if return_type:
        sampled = return_type(sampled)
    return num, sampled 


def addsub(A, B, op):
    if op == '+':
        return A[0] + B[0], A[1] + B[1]
    elif op == '-':
        return A[0] - B[0], A[1] - B[1]
    elif op == '==':
        return A == B