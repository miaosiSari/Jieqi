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
      0,  0,  0,  9, 12, 15, 17, 22, 17, 15, 12,  9,  0,  0,  0,  0,
      0,  0,  0, 19, 24, 69, 81, 93, 81, 69, 24, 19,  0,  0,  0,  0,
      0,  0,  0, 19, 24, 42, 69, 87, 69, 42, 24, 19,  0,  0,  0,  0,
      0,  0,  0, 19, 23, 27, 29, 30, 29, 27, 23, 19,  0,  0,  0,  0,
      0,  0,  0, 14, 18, 20, 27, 29, 27, 20, 18, 14,  0,  0,  0,  0,
      0,  0,  0, 57,  9, 63, 14, 67, 14, 63,  9, 57,  0,  0,  0,  0,
      0,  0,  0,  7,  7,  7, 11, 15, 11,  7,  7,  7,  0,  0,  0,  0,
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
      0,  0,  0, 65, 15, 15, 20, 68, 20, 15, 15, 65,  0,  0,  0,  0,
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
      0,  0,  0, 58, 59, 50, 55, 50, 55, 50, 59, 58,  0,  0,  0,  0,
      0,  0,  0, 58, 63, 62, 85, 70, 85, 62, 63, 58,  0,  0,  0,  0,  #  对方底线
      0,  0,  0, 58, 58, 69, 79, 76, 79, 69, 58, 58,  0,  0,  0,  0,
      0,  0,  0, 55, 59, 58, 61, 58, 61, 58, 59, 55,  0,  0,  0,  0,
      0,  0,  0, 53, 55, 58, 56, 58, 56, 58, 55, 53,  0,  0,  0,  0,
      0,  0,  0, 51, 52, 55, 52, 55, 52, 55, 52, 51,  0,  0,  0,  0,
      0,  0,  0, 43, 55, 45, 55, 45, 55, 45, 55, 43,  0,  0,  0,  0,
      0,  0,  0, 52, 39, 59, 39, 63, 39, 59, 39, 52,  0,  0,  0,  0,
      0,  0,  0, 29, 60, 35, 70, 31, 70, 35, 60, 29,  0,  0,  0,  0,
      0,  0,  0, 35, 27, 65, 39, 75, 39, 65, 27, 35,  0,  0,  0,  0,
      0,  0,  0, 25, 54, 27, 74, 25, 74, 27, 54, 25,  0,  0,  0,  0,  #  本方底线
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    ),
    "N": ( #马
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0, 90, 90, 90, 96, 90, 96, 90, 90, 90,  0,  0,  0,  0,
      0,  0,  0, 80, 86,113, 87, 94, 87,113, 86, 80,  0,  0,  0,  0,
      0,  0,  0, 87, 83, 84, 105, 99, 105, 84, 83, 87,  0,  0,  0,  0,
      0,  0,  0, 83, 98, 92, 97, 90, 97, 92, 98, 83,  0,  0,  0,  0,
      0,  0,  0, 80, 90, 89,103,104,103, 89, 90, 80,  0,  0,  0,  0,
      0,  0,  0, 80, 88, 91, 92, 93, 92, 91, 88, 80,  0,  0,  0,  0,
      0,  0,  0, 82, 84, 88, 85, 88, 85, 88, 84, 82,  0,  0,  0,  0,
      0,  0,  0, 83, 83, 84, 85, 95, 85, 84, 83, 83,  0,  0,  0,  0,
      0,  0,  0, 69, 75, 69, 69, 78, 69, 69, 75, 69,  0,  0,  0,  0,
      0,  0,  0, 55, 55, 60, 75, 65, 75, 60, 55, 55,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    ),
    "R": (#车
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,246,248,247,256,254,256,247,248,246,  0,  0,  0,  0,
      0,  0,  0,246,252,252,259,273,259,252,252,246,  0,  0,  0,  0,
      0,  0,  0,246,252,252,259,266,259,252,252,246,  0,  0,  0,  0,
      0,  0,  0,246,253,253,259,256,259,253,253,246,  0,  0,  0,  0,
      0,  0,  0,248,251,251,257,255,257,251,251,248,  0,  0,  0,  0,
      0,  0,  0,248,242,252,257,255,257,252,242,248,  0,  0,  0,  0,
      0,  0,  0,244,244,244,257,254,257,244,244,244,  0,  0,  0,  0,
      0,  0,  0,238,244,244,259,242,259,244,244,238,  0,  0,  0,  0,
      0,  0,  0,240,248,246,259,230,259,246,248,240,  0,  0,  0,  0,
      0,  0,  0,244,246,244,252,220,252,244,246,244,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    ),
    "C": (
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,145,145,132,132,120,132,132,145,145,  0,  0,  0,  0,
      0,  0,  0,128,128,126,122,129,122,126,128,128,  0,  0,  0,  0,
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
discount_factor = 1.3
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
