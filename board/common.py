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

DIRECTION_DICT = {
  #红明
  (0<<4)+(1<<3)+2: {(2, 1), (1, 2), (-1, 2), (-2, 1), (-2, -1), (-1, -2), (1, -2), (2, -1)}, #马走日
  (0<<4)+(1<<3)+3: {(2, 0), (0, 2), (-2, 0), (0, -2)}, #相走田
  (0<<4)+(1<<3)+4: {(1, 1), (-1, 1), (-1, -1), (1, -1)}, #红士
  (0<<4)+(1<<3)+5: {(1, 0), (0, 1), (-1, 0), (-1, -1)},#红帅
  (0<<4)+(1<<3)+7: {(1, 0), (0, -1), (0, 1)}, #红兵向前， x轴增加
  #红暗
  (1<<4)+(1<<3)+2: {(2, -1), (2, 1), (1, -2), (2, 1)}, #红暗马，不能往棋盘外跳
  (1<<4)+(1<<3)+3: {(2, 2), (2, -2)}, #红暗相，不能往棋盘外飞
  (1<<4)+(1<<3)+4: {(1, 1), (1, -1)}, #红暗士，不能往棋盘外撑
  (1<<4)+(1<<3)+7: {(1, 0)}, #红暗兵，只能向前拱, x坐标提升
  #黑明
  (0<<4)+(0<<3)+2: {(2, 1), (1, 2), (-1, 2), (-2, 1), (-2, -1), (-1, -2), (1, -2), (2, -1)}, #马走日
  (0<<4)+(0<<3)+3: {(2, 0), (0, 2), (-2, 0), (0, -2)}, #相走田
  (0<<4)+(0<<3)+4: {(1, 1), (-1, 1), (-1, -1), (1, -1)}, #黑士
  (0<<4)+(0<<3)+5: {(1, 0), (0, 1), (-1, 0), (-1, -1)},#黑将
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
COVERED_COUNTER = {(1<<4)+(1<<3)+1: 2, (1<<4)+(1<<3)+2: 2, (1<<4)+(1<<3)+3: 2, (1<<4)+(1<<3)+4: 2, (1<<4)+(1<<3)+6: 2, (1<<4)+(1<<3)+7: 5,
(1<<4)+(0<<3)+1: 2, (1<<4)+(0<<3)+2: 2, (1<<4)+(0<<3)+3: 2, (1<<4)+(0<<3)+4: 2, (1<<4)+(0<<3)+6: 2, (1<<4)+(0<<3)+7: 5}
UNCOVERED_COUNTER = {(0<<4)+(1<<3)+1: 2, (0<<4)+(1<<3)+2: 2, (0<<4)+(1<<3)+3: 2, (0<<4)+(1<<3)+4: 2, (0<<4)+(1<<3)+5: 1, (0<<4)+(1<<3)+6: 2, (0<<4)+(1<<3)+7: 5,
(0<<4)+(0<<3)+1: 2, (0<<4)+(0<<3)+2: 2, (0<<4)+(0<<3)+3: 2, (0<<4)+(0<<3)+4: 2, (0<<4)+(0<<3)+5: 1, (0<<4)+(0<<3)+6: 2, (0<<4)+(0<<3)+7: 5}

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
    if not num:
	    num = random.randint(0, lens)
    sampled = random.sample(s, num)
    if return_type:
        sampled = return_type(sampled)
    return num, sampled 


def addsub(A, B, op):
    if op == '+':
        return (A[0] + B[0], A[1] + B[1])
    elif op == '-':
	    return (A[0] - B[0], A[1] - B[1])
    elif op == '==':
        return A == B