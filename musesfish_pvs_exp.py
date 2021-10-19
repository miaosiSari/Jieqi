#!/usr/bin/env pypy
# -*- coding: utf-8 -*-

#Updated by Si Miao 2021/05/20
from __future__ import print_function
import re, sys, time
from itertools import count
from collections import namedtuple
import random
from board import board, common_20210604_fixed as common, library
from copy import deepcopy
import readline
import json

NULLMOVE = True
QS = True

debug_var = ''
B = board.Board()
piece = {'P': 44, 'N': 108, 'B': 23, 'R': 233, 'A': 23, 'C': 101, 'K': 2500}
put = lambda board, i, p: board[:i] + p + board[i+1:]
r = {'R': 2, 'N': 2, 'B': 2, 'A': 2, 'C': 2, 'P': 5}
b = {'r': 2, 'n': 2, 'b': 2, 'a': 2, 'c': 2, 'p': 5}
di = {0: {True: deepcopy(r), False: deepcopy(b)}}
sumall = {
    0: {True: sum(di[0][True][key] for key in di[0][True]), False: sum(di[0][False][key] for key in di[0][False])}}
# 子力价值表参考“象眼”
cache = {}
forbidden_moves = set()
kaijuku = deepcopy(library.kaijuku)


def setcache(bo):
    if bo not in cache:
        cache[bo] = 1
    else:
        cache[bo] += 1


def resetrbdict():
    global r, b, di, sumall
    r = {'R': 2, 'N': 2, 'B': 2, 'A': 2, 'C': 2, 'P': 5}
    b = {'r': 2, 'n': 2, 'b': 2, 'a': 2, 'c': 2, 'p': 5}
    di = {0: {True: deepcopy(r), False: deepcopy(b)}}
    sumall = {
        0: {True: sum(di[0][True][key] for key in di[0][True]), False: sum(di[0][False][key] for key in di[0][False])}}


pst = deepcopy(common.pst)
discount_factor = common.discount_factor  # 1.6

A0, I0, A9, I9 = 12 * 16 + 3, 12 * 16 + 11, 3 * 16 + 3,  3 * 16 + 11

'''
D: 暗车
E: 暗马
F: 暗相
G: 暗士
H: 暗炮
I: 暗车
'''

initial_covered = (
    '               \n'  # 0
    '               \n'  # 1
    '               \n'  # 2
    '   defg.kfed   \n'  # 3
    '   ....a....   \n'  # 4
    '   .h.....h.   \n'  # 5
    '   i.i...i.i   \n'  # 6
    '   .....r...   \n'  # 7
    '   .........   \n'  # 8
    '   I.I.I.I.I   \n'  # 9
    '   .H..B..H.   \n'  # 10
    '   .........   \n'  # 11
    '   DEFGKG.ED   \n'  # 12
    '               \n'  # 13
    '               \n'  # 14
    '                '  # 15
)

bug = (
    '               \n'  # 0
    '               \n'  # 1
    '               \n'  # 2
    '   d..gk....   \n'  # 3
    '   ....p....   \n'  # 4
    '   .C.......   \n'  # 5
    '   ..C......   \n'  # 6
    '   .........   \n'  # 7
    '   .........   \n'  # 8
    '   r........   \n'  # 9
    '   .........   \n'  # 10
    '   ....A....   \n'  # 11
    '   ...AK....   \n'  # 12
    '               \n'  # 13
    '               \n'  # 14
    '                '  # 15
)

# Lists of possible moves for each piece type.
N, E, S, W = -16, 1, 16, -1
directions = {
    'P': (N, W, E),
    'I': (N, ), #暗兵
    'N': (N+N+E, E+N+E, E+S+E, S+S+E, S+S+W, W+S+W, W+N+W, N+N+W),
    'E': (N+N+E, E+N+E, W+N+W, N+N+W), #暗马
    'B': (2 * N + 2 * E, 2 * S + 2 * E, 2 * S + 2 * W, 2 * N + 2 * W),
    'F': (2 * N + 2 * E, 2 * N + 2 * W), #暗相
    'R': (N, E, S, W),
    'D': (N, E, W), #暗车
    'C': (N, E, S, W),
    'H': (N, E, S, W), #暗炮
    'A': (N+E, S+E, S+W, N+W),
    'G': (N+E, N+W), #暗士
    'K': (N, E, S, W)
}

uni_pieces = {
    '.': '．',
    'R': '\033[31m俥\033[0m',
    'N': '\033[31m傌\033[0m',
    'B': '\033[31m相\033[0m',
    'A': '\033[31m仕\033[0m',
    'K': '\033[31m帅\033[0m',
    'P': '\033[31m兵\033[0m',
    'C': '\033[31m炮\033[0m',
    'D': '\033[31m暗\033[0m',
    'E': '\033[31m暗\033[0m',
    'F': '\033[31m暗\033[0m',
    'G': '\033[31m暗\033[0m',
    'H': '\033[31m暗\033[0m',
    'I': '\033[31m暗\033[0m',
    'U': '\033[31m不\033[0m',
    'r': '车',
    'n': '马',
    'b': '象',
    'a': '士',
    'k': '将',
    'p': '卒',
    'c': '炮',
    'd': '暗',
    'e': '暗',
    'f': '暗',
    'g': '暗',
    'h': '暗',
    'i': '暗',
    'u': '不'
}

MATE_LOWER = piece['K'] - (2*piece['R'] + 2*piece['N'] + 2*piece['B'] + 2*piece['A'] + 2*piece['C'] + 5*piece['P'])
MATE_UPPER = piece['K'] + (2*piece['R'] + 2*piece['N'] + 2*piece['B'] + 2*piece['A'] + 2*piece['C'] + 5*piece['P'])

# The table size is the maximum number of elements in the transposition table.
TABLE_SIZE = 1e7

# Constants for tuning search
QS_LIMIT = 219
EVAL_ROUGHNESS = 13
DRAW_TEST = True
THINK_TIME = 1
THRESHOLD = 150


###############################################################################
# Mapping
# To be more convenient, we initialize the mapping as a global const dictionary
###############################################################################
mapping = {}
average = {0: {}}
depths = (2, 2); len_depths = len(depths)

# mapping: 暗子到明子映射
# di:
#    version: 版本号，用于将来可能的递归(还没实现)
#         True: 红方
#              'R', 'N', ... 'P': 暗子中可能的数目
#         False: 黑方
#              'r', 'n', ..., 'p': 暗子中可能的数目
# sumall:
#    version: 版本号，用于将来可能的递归(还没实现)
#         True: 红方可能暗子数量之和 = 红方所有暗子+不确定子+黑方吃掉红方的子。
#         False: 黑方可能暗子数量之和 = 黑方所有暗子+不确定子+红方吃掉黑方的子。
#         //注意，sumall[version][True] >= 红方所有暗子+不确定子, 因为黑方可能不知道红方吃了黑方什么子
# average:
#    version: 版本号，用于将来可能的递归(还没实现)
#         True: 红方
#               Ture: 不确定子
#                     pos: 棋盘位置
#               False: 正在睡觉的暗子
#         False: 黑方
#               Ture: 不确定子
#                     pos: 棋盘位置
#               False: 正在睡觉的暗子

###############################################################################
# Chess logic
###############################################################################

class Position(namedtuple('Position', 'board score turn version')):
    """ A state of a chess game
    board -- a 256 char representation of the board
    score -- the board evaluation
    """

    def set(self):

        self.che = 0
        self.che_opponent = 0
        self.zu = 0
        self.covered = 0
        self.covered_opponent = 0
        self.endline = 0
        self.score_rough = 0
        self.kongtoupao = 0
        self.kongtoupao_opponent = 0
        self.kongtou_score = 0
        self.kongtou_score_opponent=0

        for i in range(51, 204):
            if i >> 4 == 3:
                if self.board[i] in 'defgrnc':
                    self.endline += 1

            p = self.board[i]

            if p in 'RNBAKCP':
                self.score_rough += pst[p][i]

            elif p in 'DEFGHI':
                self.covered += 1

            elif p in 'U':
                self.score_rough += average[self.version][self.turn][True][i]
                self.covered += 1

            elif p in 'rnbakcp':
                self.score_rough -= pst[p.upper()][254 - i]

            elif p in 'defghi':
                self.covered_opponent += 1

            elif p in 'u':
                self.score_rough -= average[self.version][not self.turn][True][254 - i]
                self.covered_opponent += 1

            if p == 'R':
                self.che += 1

            if p == 'r':
                self.che_opponent += 1

            if p == 'P':
                self.zu += 1

            if p == 'C' and i & 15 == 7:
                self.check_kongtoupao(i, True)

            if p == 'c' and i & 15 == 7:
                self.check_kongtoupao(i, False)
       
        if (self.kongtoupao > 0 and self.kongtoupao_opponent <= 0) or (self.kongtoupao > self.kongtoupao_opponent > 0):
            if (self.che >= self.che_opponent and self.che > 0) or self.kongtoupao >= 3:
                self.kongtou_score += 100
            else:
                self.kongtou_score += 70

        elif (self.kongtoupao <= 0 and self.kongtoupao_opponent > 0) or (self.kongtoupao_opponent > self.kongtoupao > 0):
            if (self.che_opponent >= self.che and self.che_opponent > 0) or self.kongtoupao_opponent >= 3:
                self.kongtou_score_opponent += 100
            else:
                self.kongtou_score_opponent += 70

        return self

    def check_kongtoupao(self, pos, t):
        cannon = 'C' if t else 'c'
        king = 'k' if t else 'K'
        if t:
            if self.kongtoupao:
                return
            for scanpos in range(pos-16, 51, -16):
                if self.board[scanpos] == cannon:
                   continue
                elif self.board[scanpos] != '.':
                   if self.board[scanpos] == king:
                       self.kongtoupao += 1                 
                       return
                   self.kongtoupao = 0
                   return     
            self.kongtoupao = 0
            return
        else: 
            if self.kongtoupao_opponent:
                return
            for scanpos in range(pos+16, 204, 16):
                if self.board[scanpos] == cannon:
                   continue
                elif self.board[scanpos] != '.':
                   if self.board[scanpos] == king: 
                       self.kongtoupao_opponent += 1           
                       return
                   self.kongtoupao_opponent = 0  
                   return     
            self.kongtoupao_opponent = 0
            return

    def gen_moves(self):
        # For each of our pieces, iterate through each possible 'ray' of moves,
        # as defined in the 'directions' map. The rays are broken e.g. by
        # captures or immediately in case of pieces such as knights.
        for i in range(51, 204):

            p = self.board[i]

            if not p.isupper() or p == 'U': continue

            if p == 'K': 
                for scanpos in range(i - 16, A9, -16):
                    if self.board[scanpos] == 'k':
                        yield (i, scanpos)
                    elif self.board[scanpos] != '.':
                        break

            if p in ('C', 'H'): #明暗炮
                for d in directions[p]:
                    cfoot = 0
                    for j in count(i+d, d):
                        q = self.board[j]
                        if q.isspace(): break
                        if cfoot == 0 and q == '.': yield (i, j)
                        elif cfoot == 0 and q != '.': cfoot += 1
                        elif cfoot == 1 and q.islower(): yield (i, j); break
                        elif cfoot == 1 and q.isupper(): break;
                continue

            for d in directions[p]:
                for j in count(i+d, d):
                    q = self.board[j]
                    # Stay inside the board, and off friendly pieces
                    if q.isspace() or q.isupper(): break
                    # 过河的卒/兵才能横着走
                    if p == 'P' and d in (E, W) and i > 128: break
                    # j & 15 等价于 j % 16但是更快
                    elif p == 'K' and (j < 160 or j & 15 > 8 or j & 15 < 6): break
                    elif p == 'G' and j != 183: break # 暗士, 花心坐标: (11, 7), 11 * 16 + 7 = 183
                    elif p in ('N', 'E'): # 暗马
                        n_diff_x = (j - i) & 15
                        if n_diff_x == 14 or n_diff_x == 2:
                            if self.board[i + (1 if n_diff_x == 2 else -1)] != '.': break
                        else:
                            if j > i and self.board[i + 16] != '.': break
                            elif j < i and self.board[i - 16] != '.': break
                    elif p in ('B', 'F') and self.board[i + d // 2] != '.': break
                    # Move it
                    yield (i, j)
                    # Stop crawlers from sliding, and sliding after captures
                    if p in 'PNBAKIEFG' or q.islower(): break

    def rooted(self):
        '''
        计算有根子
        '''
        rooted_chesses = set()
        for i in range(51, 204):

            p = self.board[i]

            if not p.isupper() or p == 'U': continue

            if p in ('C', 'H'): #明暗炮
                for d in directions[p]:
                    cfoot = 0
                    for j in count(i+d, d):
                        q = self.board[j]
                        if q.isspace(): break
                        if cfoot == 0 and q == '.': continue
                        elif cfoot == 0 and q != '.': cfoot += 1
                        elif cfoot == 1 and q.islower(): break
                        elif cfoot == 1 and q.isupper(): rooted_chesses.add(j); break
                    continue

            else:
                for d in directions[p]:
                    for j in count(i+d, d):
                        q = self.board[j]
                        # Stay inside the board, and off friendly pieces
                        # 过河的卒/兵才能横着走
                        if q.isspace() or q.islower(): break
                        if p == 'P' and d in (E, W) and i > 128: break
                        # j & 15 等价于 j % 16但是更快
                        elif p == 'K' and (j < 160 or j & 15 > 8 or j & 15 < 6): break
                        elif p == 'G' and j != 183: break # 暗士, 花心坐标: (11, 7), 11 * 16 + 7 = 183
                        elif p in ('N', 'E'): # 暗马
                            n_diff_x = (j - i) & 15
                            if n_diff_x == 14 or n_diff_x == 2:
                                if self.board[i + (1 if n_diff_x == 2 else -1)] != '.': break
                            else:
                                if j > i and self.board[i + 16] != '.': break
                                elif j < i and self.board[i - 16] != '.': break
                        elif p in ('B', 'F') and self.board[i + d // 2] != '.': break
                        # Move it
                        if q.isupper(): rooted_chesses.add(j); break
                        # Stop crawlers from sliding, and sliding after captures
                        if p in 'PNBAKIEFG': break
        return rooted_chesses

    def rotate(self):
        ''' Rotates the board, preserving enpassant '''
        p = Position(
            self.board[-2::-1].swapcase() + " ", -self.score, not self.turn, self.version)
        p.set()
        return p

    @staticmethod
    def rotate_new(board, score, turn, version):
        p = Position(
            board[-2::-1].swapcase() + " ", -score, not turn, version)
        p.set()
        return p

    def nullmove(self):
        ''' Like rotate, but clears ep and kp '''
        return self.rotate()

    def move(self, move):
        i, j = move
        # Copy variables and reset ep and kp
        movevalue = self.value(move)
        score = self.score + movevalue if movevalue < MATE_UPPER else MATE_UPPER
        # Actual move
        if self.board[i] in 'RNBAKCP':
            board = put(self.board, j, self.board[i])
        else:
            board = put(self.board, j, 'U')
        board = put(board, i, '.')
        return Position.rotate_new(board, score, self.turn, self.version)

    def mymove_check(self, move, discount_red=True, discount_black=False):
        if move is None:
           return self.rotate().set(), None, None, None
        i, j = move
        # Copy variables and reset ep and kp
        ############################################################################
        # TODO: Evaluate the score of each move!
        # The following line is NOT implemented. However, it is extremely important!
        # score = self.score + self.value(move)
        ############################################################################
        # Actual move
        # put = lambda board, i, p: board[:i] + p + board[i + 1:]
        eat = self.board[j]
        dst = None

        checkmate = False
        if self.board[j] == 'k':
            checkmate = True

        if self.board[j] in "defghi":
            dst = None
            if self.turn:
                dst = mapping[j]
            else:
                dst = mapping[254-j]
            # 这里是吃暗子的逻辑
            # 在这个简易程序中，假设AI执黑，玩家执红。
            # 那么AI其实是不知道玩家吃了自己什么暗子的
            # 因此在玩家执红吃黑暗子时，黑方的暗子集合并不会更新。
            # 本程序设置了discount_red/black开关处理这一逻辑。
            if self.turn:
               if discount_black:
                   dst2 = dst.lower()
                   di[self.version][False][dst2] -= 1
            else:
               if discount_red:
                   dst2 = dst.upper()
                   di[self.version][True][dst2] -= 1

        if self.board[i] in "RNBAKCP":
            board = put(self.board, j, self.board[i])
        else:
            if self.turn:
                board = put(self.board, j, mapping[i])
                dst2 = mapping[i].upper()
                di[self.version][True][dst2] -= 1
            else:
                board = put(self.board, j, mapping[254 - i].upper())
                dst2 = mapping[254 - i].lower()
                di[self.version][False][dst2] -= 1
        board = put(board, i, '.')
        sumall[self.version][True] = sum(di[self.version][True][key] for key in di[self.version][True])
        sumall[self.version][False] = sum(di[self.version][False][key] for key in di[self.version][False])
        return Position.rotate_new(board, self.score, self.turn, self.version), checkmate, eat, dst

    def calc(self):
         shi_possibility = 0 if sumall[self.version][not self.turn] == 0 else di[self.version][not self.turn]['a' if self.turn else 'A']/sumall[self.version][not self.turn]
         base_possibility = 1
         if self.board[54] == 'g':
             base_possibility *= (1 - shi_possibility)
         if self.board[56] == 'g':
             base_possibility *= (1 - shi_possibility)
         return base_possibility

    def value(self, move):
        i, j = move
        p, q = self.board[i], self.board[j].upper()
        possible_che = 0 if sumall[self.version][self.turn] == 0 else self.covered * di[self.version][self.turn][
            'R' if self.turn else 'r']/sumall[self.version][self.turn]
        possible_che_opponent = 0 if sumall[self.version][not self.turn] == 0 else self.covered_opponent * di[self.version][not self.turn][
            'r' if self.turn else 'R'] / sumall[self.version][not self.turn]
        bing_possibility = 0 if sumall[self.version][self.turn] == 0 else di[self.version][self.turn]['P' if self.turn else 'p']/sumall[self.version][self.turn]
        # Actual move
        # 这里有一个隐藏的很深的BUG。如果对手走出将帅对饮的一步棋，score应该很高(因为直接赢棋)。但由于减了pst[p][i], 减了自己的皇上，所以代码中的score是接近0的。
        # 因此，当对方是老将时应直接返回最大值，不能考虑己方。
        if q == 'K':
            return MATE_UPPER
        if p in 'RNBAKCP':
            score = pst[p][j] - pst[p][i]
            if p == 'C':
                # 以下为沉底炮逻辑
                if (i >> 4) != 3 and (j >> 4) == 3 and self.endline <= 2:
                    if (j == 51 or j == 52) and self.board[53] == 'f' and self.board[54] == 'g':
                        pass
                    elif (j == 59 or j == 58) and self.board[57] == 'f' and self.board[56] == 'g':
                        pass
                    else:
                        if self.endline >= 1:
                            score -= 30
                        else:
                            score -= 55
                    ######################################################################################################################
                    #  2021/05/28  关于score -= 30的解释
                    #  揭棋的沉底炮比象棋的沉底炮威胁更大，因为揭棋通常没有连环象的防守，容易闷宫，也没有连环士的防守，一旦对手出车，可以瞄准底暗士叫杀，或捉底暗象。
                    #  此外，揭棋中沉底炮可以牵制暗子，而明棋中只能牵制子力价值较弱的士象。
                    #  因此，我在board/common.py中，给沉底炮赋予了很高的分数。
                    #  但是，如果对方底线暗子已出，则牵制力大大减弱。盲目下底炮就不是一个好事情。
                    #  2021/05/29 改为score -= 55
                    ######################################################################################################################

                if (i >> 4) == 3 and (j >> 4) != 3 and self.endline <= 2:
                    if (i == 51 or i == 52) and self.board[53] == 'f' and self.board[54] == 'g':
                        pass
                    elif (i == 59 or i == 58) and self.board[57] == 'f' and self.board[56] == 'g':
                        pass
                    else:
                        if self.endline >= 1:
                            score += 30
                        else:
                            score += 55

            elif p == 'R':
                if self.board[51] not in 'dr' and self.board[54] != 'a' and self.board[71] != 'a' and (self.board[71] == 'p' or self.board[87] != 'n'):
                    if j & 15 == 6 and i & 15 != 6:
                        score += 30  # 如果对方暗车出动，相应侧又没有士或者中马的防守，抓紧抢占肋道
                    if j & 15 != 6 and i & 15 == 6:
                        score -= 30

                if self.board[59] not in 'dr' and self.board[56] != 'a' and self.board[71] != 'a' and (self.board[71] == 'p' or self.board[87] != 'n'):
                    if j & 15 == 8 and i & 15 != 8:
                        score += 30  # 如果对方暗车出动，相应侧又没有士或者中马的防守，抓紧抢占肋道
                    if j & 15 != 8 and i & 15 == 8:
                        score -= 30

                if (i >> 4) == 3 and (j >> 4) != 3 and (self.endline <= 1 or self.score_rough < -150):
                    if self.endline <= 1:
                        score += 30
                    elif self.score_rough < -150:
                        score += 40

                if (i >> 4) != 3 and (j >> 4) == 3 and (self.endline <= 1 or self.score_rough < -150):
                    if self.endline <= 1:
                        score -= 30
                    elif self.score_rough < -150:
                        score -= 40

        else:
            # 不确定明子的平均价值计算算法:
            # 假设某一方可能的暗子是 两车一炮。
            # 则在某位置处不确定明子的价值为 (车在该处的价值*2 + 炮在该处的价值)/(2+1)。
            # 为了加速计算，这一数值已经被封装到了average这一字典中并预先计算(Pre-compute)。
            score = average[self.version][self.turn][True][j] - average[self.version][self.turn][False] + 20  # 相应位置不确定明子的平均价值 - 暗子

            if p == 'D':
                minus = 30*(possible_che_opponent / 2 + self.che_opponent)
                score -= minus  # 暗车溜出，扣分! 扣的分数和对方剩余车的个数有关
                if self.score_rough < -150:
                    score -= minus//2

            elif p == 'E':
                if i == 196 and j == 165 and self.board[149] == 'I':  # 对方车从3,7线杀出，翻动暗马保住暗兵
                    for scanpos in range(133, 51, -16):
                        if self.board[scanpos] == 'r':
                            score += average[self.version][self.turn][False]//2
                        elif self.board[scanpos] != '.':
                            break

                if i == 202 and j == 169 and self.board[153] == 'I':
                    for scanpos in range(137, 51, -16):
                        if self.board[scanpos] == 'r':
                            score += average[self.version][self.turn][False]//2
                        elif self.board[scanpos] != '.':
                            break

            elif p == 'F':
                if (i == 197 or i == 201) and j == 167 and self.board[151] == 'I':
                    findche = False
                    for scanpos in range(135, 51, -16):
                        if self.board[scanpos] == 'r':
                            score += average[self.version][self.turn][False]//2
                            findche = True
                        elif self.board[scanpos] != '.':
                            break
                    if not findche:
                        for scanpos in range(135, 130, -1):
                            if self.board[scanpos] == 'r':
                                score += average[self.version][self.turn][False] // 2
                                findche = True
                            elif self.board[scanpos] != '.':
                                break
                    if not findche:
                        for scanpos in range(135, 140, 1):
                            if self.board[scanpos] == 'r':
                                score += average[self.version][self.turn][False] // 2
                            elif self.board[scanpos] != '.':
                                break

            elif p == 'G':
                # 1 2 3 4 5 6 7 8 9
                # i
                # h
                # g
                # f
                # e
                # d
                # c
                # b
                # a
                # 9 8 7 6 5 4 3 2 1
                # 195 - 16x + y

                # 对手9路暗车出动， 己方可以考虑出将/出帅助攻。翻开四路暗士， 查看四路肋道车的数量。如果己方车数量大于对方车，鼓励翻动士助攻
                if i == 200 and self.board[59] not in 'dr' and self.board[56] != 'a' and self.board[71] != 'a' and (
                        self.board[71] == 'p' or self.board[87] != 'n'):
                    cheonleidao = 0
                    che_opponent_onleidao = 0
                    for scanpos in range(184, 51, -16):
                        if self.board[scanpos] == 'R':
                            cheonleidao += 1
                        elif self.board[scanpos] == 'r':
                            che_opponent_onleidao += 1
                    if cheonleidao > che_opponent_onleidao and possible_che >= possible_che_opponent:
                        score += (40 * self.calc())

                # 对手1路暗车出动， 己方可以考虑出将/出帅助攻。翻开六路暗士， 查看六路肋道车的数量。如果己方车数量大于对方车，鼓励翻动士助攻
                if i == 198 and self.board[51] not in 'dr' and self.board[54] != 'a' and self.board[71] != 'a' and (
                        self.board[71] == 'p' or self.board[87] != 'n'):
                    cheonleidao = 0
                    che_opponent_onleidao = 0
                    for scanpos in range(182, 51, -16):
                        if self.board[scanpos] == 'R':
                            cheonleidao += 1
                        elif self.board[scanpos] == 'r':
                            che_opponent_onleidao += 1
                    if cheonleidao > che_opponent_onleidao and possible_che >= possible_che_opponent:
                        score += (40 * self.calc())

                elif sumall[self.version][self.turn] > 0 and \
                        (di[self.version][self.turn]['P' if self.turn else 'p'] * self.covered /
                         sumall[self.version][self.turn] <= 2):
                            score -= 20  # 如果当前暗子中兵过多，翻士容易翻出窝心兵，不利于防守!

            elif p == 'H':
                if i == 164 and j == 68 and self.board[52] == 'e':
                    prob = 0 if sumall[self.version][self.turn] == 0 else (di[self.version][self.turn]['P' if self.turn else 'p']/sumall[self.version][self.turn])  # 平均可能卒的个数
                    coeff = average[self.version][not self.turn][False]
                    bonus = round(prob * coeff/2)
                    score += bonus//2
                    if self.board[53] != '':
                        score += bonus
                    if self.board[51] == 'd':
                        score += bonus

                if i == 170 and j == 74 and self.board[58] == 'e':
                    prob = 0 if sumall[self.version][self.turn] == 0 else (di[self.version][self.turn]['P' if self.turn else 'p'] / sumall[self.version][self.turn])  # 平均可能卒的个数
                    coeff = average[self.version][not self.turn][False]
                    bonus = round(prob * coeff / 2)
                    score += bonus // 2
                    if self.board[57] != '':
                        score += bonus
                    if self.board[59] == 'd':
                        score += bonus

                # 暗炮搏马
                if ((i == 164 and j == 52 and self.board[52] == 'e' and self.board[51] in 'dr') or (
                        i == 170 and j == 58 and self.board[58] == 'e' and self.board[59] in 'dr')):  # TODO: 使用更智能的方式处理博子
                    if (i == 164 and self.board[148] == 'p') or (i == 170 and self.board[154] == 'p'):
                        pass
                    if self.che < self.che_opponent or self.che == 0 or self.score_rough < 150:
                        score -= 100
                    elif self.che == self.che_opponent:
                        if self.score_rough > 200:
                            pass
                        elif (i == 164 and (self.board[131] != 'N' or self.board[115] != 'p')) and (i == 170 and (self.board[139] != 'N' or self.board[123] != 'p')):
                            score -= 30

            elif p == 'I':
                if self.board[i - 32] in 'rp':  # 原先是RP, 这是个BUG!现解决
                    score -= average[self.version][self.turn][False]//2
                elif self.board[i - 32] == 'nc':  # 之前是N, 不正确，已更正!
                    score += 30
                elif self.board[i - 48] == 'i':
                    score += 30
                else:
                    score += 20

        # Capture
        if q.isupper():
            k = 254 - j
            if q in 'RNBAKCP':
                score += pst[q][k]
                if q == 'P' and self.board[j+32] == 'I':
                    score += 30
            else:
                if q != 'U':
                    score += average[self.version][not self.turn][False]
                    if q == 'I':
                       score += 10
                else:
                    score += average[self.version][not self.turn][True][k]
                    if j >> 4 == 7 and j & 1 == 1: 
                       score += 10 #吃由暗兵翻出来的不确定子加分，鼓励控制暗兵
                if q == 'D':
                    addition = 30*(possible_che/2 + self.che)
                    score += addition  # 吃对方暗车，加分! 加的分数和己方剩余车的个数相关，如果本方没有车了，那吃个暗车不算太大的收益
                    if self.score_rough > 150:
                        score += addition//2

        return score

###############################################################################
# Search logic
###############################################################################

# lower <= s(pos) <= upper
Entry = namedtuple('Entry', 'lower upper')


class Searcher:
    def __init__(self):
        self.tp_score = {}
        self.tp_move = {}
        self.result_dict = {}
        self.history = set()
        self.nodes = 0

    def quiescence(self, pos, moves, oppo):
        score = 0
        maxscore = 0
        oppo_rooted_set = oppo.rooted()
        oppo_rooted_set = set(map(lambda x: 254-x, oppo_rooted_set)) #对方有根子
        argmax = None
        for move in moves:
            p = pos.board[move[0]]
            q = pos.board[move[1]]
            if q == '.':
                continue
            if p == 'D':
                if q == 'r':
                    if 240 > maxscore:
                        argmax = move
                        maxscore = 240
                else:
                    continue
            j = move[1]
            k = 254 - j
            if q in 'rcnabpk':
                score += pst[q.upper()][k]
            elif q in 'defghi':
                score += average[oppo.version][oppo.turn][False]
            elif q == 'u':
                score += average[oppo.version][oppo.turn][True][k]
            if move[1] in oppo_rooted_set:
                if p in 'RCNABPK':
                    score -= pst[p][j]
                if p in 'EFGHI':
                    score -= average[pos.version][pos.turn][False]
            if score > maxscore:
                argmax = move
                maxscore = score
        if argmax and pos.board[argmax[1]] == 'c' and argmax[1] & 15 == 7 and pos.kongtou_score_opponent > 0 and pos.kongtoupao_opponent > 0:
            pos.kongtou_score_opponent = 0
        return maxscore, argmax

    def alphabeta(self, pos, alpha, beta, depth, root=True, nullmove=False, nullmove_now=False):
        """ returns r where
                s(pos) <= r < gamma    if gamma > s(pos)
                gamma <= r <= s(pos)   if gamma <= s(pos)"""
        global debug_var

        oppo = pos.rotate()

        if root:
            self.tp_score = {}
            self.tp_move = {}
        self.nodes += 1

        # Depth <= 0 is QSearch. Here any position is searched as deeply as is needed for
        # calmness, and from this point on there is no difference in behaviour depending on
        # depth, so so there is no reason to keep different depths in the transposition table.
        depth = max(depth, 0)

        # Sunfish is a king-capture engine, so we should always check if we
        # still have a king. Notice since this is the only termination check,
        # the remaining code has to be comfortable with being mated, stalemated
        # or able to capture the opponent king.
        if pos.score <= -MATE_LOWER:
            return -MATE_UPPER

        moves = sorted(pos.gen_moves(), key=pos.value, reverse=True)
        killer = self.tp_move.get(pos)
        for move in [killer] + moves:
            if (move is not None) and pos.board[move[1]] == 'k':
                self.tp_move[pos] = move
                return MATE_UPPER

        # Look in the table if we have already searched this position before.
        # We also need to be sure, that the stored search was over the same
        # nodes as the current search.
        entry = self.tp_score.get((pos, depth, root), Entry(-MATE_UPPER, MATE_UPPER))
        if entry.lower >= beta and (not root or self.tp_move.get(pos) is not None):
            return entry.lower
        if entry.upper < alpha:
            return entry.upper

        if nullmove_now and depth > 3 and not root and any(c in pos.board for c in 'RNCI'):
            if all(oppo.board[m[1]] != 'k' for m in oppo.gen_moves()):
               val = -self.alphabeta(pos.rotate(), -beta, 1-beta, depth-3, root=False, nullmove=nullmove, nullmove_now=False)
               if val >= beta and self.alphabeta(pos, alpha, beta, depth-3, root=False, nullmove=nullmove, nullmove_now=False):
                  return val

        nullmove_now = nullmove

        # For QSearch we have a different kind of null-move, namely we can just stop
        # and not capture anything else.
        if depth == 0:
            if QS:
                score = self.quiescence(pos, moves, oppo)
                return pos.score + pos.kongtou_score - pos.kongtou_score_opponent + score[0]
            else:
                return pos.score + pos.kongtou_score - pos.kongtou_score_opponent
        # Then killer move. We search it twice, but the tp will fix things for us.
        # Note, we don't have to check for legality, since we've already done it
        # before. Also note that in QS the killer must be a capture, otherwise we
        # will be non deterministic.
        best = -MATE_UPPER

        # Then all the other moves
        mvBest = None

        for move in [killer] + moves:
            if root and move in forbidden_moves:
                continue
            if (move is not None) and (depth > 0):
                if best == -MATE_UPPER:
                    val = -self.alphabeta(pos.move(move), -beta, -alpha, depth - 1, root=False, nullmove=nullmove, nullmove_now=nullmove_now)
                else:
                    val = -self.alphabeta(pos.move(move), -alpha - 1, -alpha, depth - 1, root=False, nullmove=nullmove, nullmove_now=nullmove_now)
                    if val > alpha and val < beta:
                        val = -self.alphabeta(pos.move(move), -beta, -alpha, depth - 1, root=False, nullmove=nullmove, nullmove_now=nullmove_now)
                if val >= MATE_UPPER:
                    updated = pos.move(move).nullmove()
                    if any(updated.board[m[1]] == 'k' for m in updated.gen_moves()):
                        mvBest = move
                        best = val
                        break
                if val > best and val > -MATE_UPPER:
                    best = val
                    mvBest = move
                    if val > beta:
                        break
                    if val > alpha:
                        alpha = val
                        
        if not mvBest and moves:
            mvBest = moves[0]

        if mvBest is not None:
            # Clear before setting, so we always have a value
            # Save the move for pv construction and killer heuristic
            if len(self.tp_move) > TABLE_SIZE: self.tp_move.clear()
            self.tp_move[pos] = mvBest

        # Stalemate checking is a bit tricky: Say we failed low, because
        # we can't (legally) move and so the (real) score is -infty.
        # At the next depth we are allowed to just return r, -infty <= r < gamma,
        # which is normally fine.
        # However, what if gamma = -10 and we don't have any legal moves?
        # Then the score is actaully a draw and we should fail high!
        # Thus, if best < gamma and best < 0 we need to double check what we are doing.
        # This doesn't prevent sunfish from making a move that results in stalemate,
        # but only if depth == 1, so that's probably fair enough.
        # (Btw, at depth 1 we can also mate without realizing.)
        if best < alpha and best < 0 and depth > 0:
            is_dead = lambda pos: any(pos.value(m) >= MATE_LOWER for m in pos.gen_moves())
            if all(is_dead(pos.move(m)) for m in pos.gen_moves()):
                in_check = is_dead(pos.nullmove())
                best = -MATE_UPPER if in_check else 0

        # Clear before setting, so we always have a value
        if len(self.tp_score) > TABLE_SIZE: self.tp_score.clear()
        # Table part 2
        if best >= beta:
            self.tp_score[pos, depth, root] = Entry(best, entry.upper)
        if best < alpha:
            self.tp_score[pos, depth, root] = Entry(entry.lower, best)

        return best

    def alphabeta_double_recursive(self, pos, alpha, beta, depth, root=True, nullmove=False, nullmove_now=False, version=0, pruning=False):
        """ returns r where
                s(pos) <= r < gamma    if gamma > s(pos)
                gamma <= r <= s(pos)   if gamma <= s(pos)"""
        global debug_var
        if pruning is True:
            pruning = 5

        oppo = pos.rotate()

        if root:
            self.tp_score = {}
            self.tp_move = {}
        self.nodes += 1

        # Depth <= 0 is QSearch. Here any position is searched as deeply as is needed for
        # calmness, and from this point on there is no difference in behaviour depending on
        # depth, so so there is no reason to keep different depths in the transposition table.
        depth = max(depth, 0)

        # Sunfish is a king-capture engine, so we should always check if we
        # still have a king. Notice since this is the only termination check,
        # the remaining code has to be comfortable with being mated, stalemated
        # or able to capture the opponent king.
        if pos.score <= -MATE_LOWER:
            return -MATE_UPPER

        moves = sorted(pos.gen_moves(), key=pos.value, reverse=True)
        killer = self.tp_move.get(pos)
        for move in [killer] + moves:
            if (move is not None) and pos.board[move[1]] == 'k':
                self.tp_move[pos] = move
                return MATE_UPPER

        # Look in the table if we have already searched this position before.
        # We also need to be sure, that the stored search was over the same
        # nodes as the current search.
        entry = self.tp_score.get((pos, depth, root), Entry(-MATE_UPPER, MATE_UPPER))
        if entry.lower >= beta and (not root or self.tp_move.get(pos) is not None):
            return entry.lower
        if entry.upper < alpha:
            return entry.upper

        if nullmove_now and depth > 3 and not root and any(c in pos.board for c in 'RNCI'):
            if all(oppo.board[m[1]] != 'k' for m in oppo.gen_moves()):
                val = -self.alphabeta_double_recursive(pos.rotate(), -beta, 1 - beta, depth - 3, root=False, nullmove=nullmove,
                                      nullmove_now=False, version=version, pruning=pruning)
                if val >= beta and self.alphabeta_double_recursive(pos, alpha, beta, depth - 3, root=False, nullmove=nullmove,
                                                  nullmove_now=False, version=version, pruning=pruning):
                    return val

        nullmove_now = nullmove

        # For QSearch we have a different kind of null-move, namely we can just stop
        # and not capture anything else.
        if depth == 0:
            return self.evaluate(pos, oppo, version+1, pruning=pruning)
        # Then killer move. We search it twice, but the tp will fix things for us.
        # Note, we don't have to check for legality, since we've already done it
        # before. Also note that in QS the killer must be a capture, otherwise we
        # will be non deterministic.
        best = -MATE_UPPER

        # Then all the other moves
        mvBest = None

        counter = 0
        for move in [killer] + moves:
            if root and move in forbidden_moves:
                continue
            if root and counter >= pruning > 0:
                break
            if (move is not None) and (depth > 0):
                if best == -MATE_UPPER:
                    val = -self.alphabeta_double_recursive(pos.move(move), -beta, -alpha, depth - 1, root=False, nullmove=nullmove,
                                          nullmove_now=nullmove_now, version=version, pruning=pruning)
                else:
                    val = -self.alphabeta_double_recursive(pos.move(move), -alpha - 1, -alpha, depth - 1, root=False, nullmove=nullmove,
                                          nullmove_now=nullmove_now, version=version, pruning=pruning)
                    if alpha < val < beta:
                        val = -self.alphabeta_double_recursive(pos.move(move), -beta, -alpha, depth - 1, root=False, nullmove=nullmove,
                                              nullmove_now=nullmove_now, version=version, pruning=pruning)
                if val >= MATE_UPPER:
                    updated = pos.move(move).nullmove()
                    if any(updated.board[m[1]] == 'k' for m in updated.gen_moves()):
                        mvBest = move
                        best = val
                        break
                if val > best and val > -MATE_UPPER:
                    best = val
                    mvBest = move
                    if val > beta:
                        break
                    if val > alpha:
                        alpha = val
            counter += 1

        if not mvBest and moves:
            mvBest = moves[0]

        if mvBest is not None:
            # Clear before setting, so we always have a value
            # Save the move for pv construction and killer heuristic
            if len(self.tp_move) > TABLE_SIZE: self.tp_move.clear()
            self.tp_move[pos] = mvBest

        # Stalemate checking is a bit tricky: Say we failed low, because
        # we can't (legally) move and so the (real) score is -infty.
        # At the next depth we are allowed to just return r, -infty <= r < gamma,
        # which is normally fine.
        # However, what if gamma = -10 and we don't have any legal moves?
        # Then the score is actaully a draw and we should fail high!
        # Thus, if best < gamma and best < 0 we need to double check what we are doing.
        # This doesn't prevent sunfish from making a move that results in stalemate,
        # but only if depth == 1, so that's probably fair enough.
        # (Btw, at depth 1 we can also mate without realizing.)
        if best < alpha and best < 0 and depth > 0:
            is_dead = lambda pos: any(pos.value(m) >= MATE_LOWER for m in pos.gen_moves())
            if all(is_dead(pos.move(m)) for m in pos.gen_moves()):
                in_check = is_dead(pos.nullmove())
                best = -MATE_UPPER if in_check else 0

        # Clear before setting, so we always have a value
        if len(self.tp_score) > TABLE_SIZE: self.tp_score.clear()
        # Table part 2
        if best >= beta:
            self.tp_score[pos, depth, root] = Entry(best, entry.upper)
        if best < alpha:
            self.tp_score[pos, depth, root] = Entry(entry.lower, best)

        return best

    def _inner_recur(self, version, uncertainty_dict, uncertainty_keys, len_uncertainty_keys, i, board, score, turn, me=1, op=1, pruning=False):
        '''
        uncertainty_dict 记录位置到U/u的映射
        例如board[2] = 'U', 则uncertainty_dict[2] = 'U'。
        mul_black 黑色子力组合计数
        mul_red 红色子力组合计数
        '''
        notturn = not turn
        if i >= len_uncertainty_keys:
            pos = Position(board, score, turn, version)
            pos.set()
            self.calc_average(version)
            sumall[version] = {}
            sumall[version][True] = sum(di[version][True][key] for key in di[version][True])
            sumall[version][False] = sum(di[version][False][key] for key in di[version][False])
            ############################################
            # 这里我们需要new一个pos
            # 在new一个pos的时候，需要四个参数:
            # board, score, turn, version
            # 最后别忘了set, 以及重新计算version版本的均值数组
            ############################################
            result = self.alphabeta_double_recursive(
                pos, -MATE_UPPER, MATE_UPPER, depth=depths[version], root=True, nullmove=NULLMOVE, nullmove_now=NULLMOVE, version=version, pruning=pruning)
            self.result_dict[version][board] = (result, me, op)
            return

        key = uncertainty_keys[i]
        if uncertainty_dict[key] == 'U':
            for di_key in di[version][turn]:
                if di[version][turn][di_key] > 0 and di_key in 'RPCrpc':
                    di[version][turn][di_key] -= 1
                    newboard = put(board, key, di_key.upper())
                    score_diff = pst[di_key.upper()][key] - average[version - 1][turn][True][key]
                    self._inner_recur(version, uncertainty_dict, uncertainty_keys, len_uncertainty_keys, i+1, newboard,
                                      score+score_diff, turn, me=me*(di[version][turn][di_key]+1), op=op, pruning=pruning)
                    di[version][turn][di_key] += 1

        elif uncertainty_dict[key] == 'u':
            for di_key in di[version][notturn]:
                if di[version][notturn][di_key] > 0 and di_key in 'RPCrpc':
                    di[version][notturn][di_key] -= 1
                    newboard = put(board, key, di_key.lower())
                    rkey = 254 - key
                    score_diff = pst[di_key.upper()][rkey] - average[version - 1][notturn][True][rkey]
                    ###################################################################################
                    # score_diff 这一行逻辑特别复杂，指的是
                    # 如果将不确定子变成明子，带来的分数变化。
                    # 1. pst[di_key.upper()][254 - key]
                    # 首先, di_key不一定是大写，先变为大写, 这样才能在分值字典中访问到相关的子力
                    # 其次，这里key是本方坐标，而我们需要计算对手子力在key处的分值，所以我们需要使用逆转函数:254-key
                    # 2. average[version - 1][notturn][True][di_key][254 - rkey]
                    # version - 1是版本号，访问的是上一版本的average数据
                    # notturn = not turn, 表明访问的是对手的average字典
                    # True, 表示访问的是不确定子的数据。如访问暗子的数据，应该是False。
                    # rkey: 同上。
                    ###################################################################################
                    self._inner_recur(version, uncertainty_dict, uncertainty_keys, len_uncertainty_keys, i+1, newboard,
                                      score-score_diff, turn, me=me, op=op*(di[version][notturn][di_key]+1), pruning=pruning)
                    di[version][notturn][di_key] += 1

    def evaluate(self, pos, oppo, version, pruning=False):
        if version >= len_depths:
            moves = pos.gen_moves()
            if QS:
                score = self.quiescence(pos, moves, oppo)
                return pos.score + pos.kongtou_score - pos.kongtou_score_opponent + score[0]
            else:
                return pos.score + pos.kongtou_score - pos.kongtou_score_opponent

        elif version == 0:
            self.calc_average(0)
            return self.alphabeta_double_recursive(
                pos, -MATE_UPPER, MATE_UPPER, depth=depths[version], root=True, nullmove=NULLMOVE, nullmove_now=NULLMOVE, version=version, pruning=pruning)

        else:
            di[version] = deepcopy(di[version-1])
            self.result_dict[version] = {}
            uncertainty_dict = {}
            for i, x in enumerate(pos.board):
                if x == 'U':
                    uncertainty_dict[i] = x
                elif x == 'u':
                    uncertainty_dict[i] = x
            uncertainty_keys = list(uncertainty_dict.keys())
            len_uncertainty_keys = len(uncertainty_keys)
            self._inner_recur(version, uncertainty_dict, uncertainty_keys, len_uncertainty_keys, 0, pos.board, pos.score,
                              pos.turn, me=1, op=1, pruning=pruning)
            S = 0  # Sum of Situations
            Sscore = 0  # Sum of Scores
            for key in self.result_dict[version]:
                item = self.result_dict[version][key]
                combinations = item[1] * item[2]
                Sscore += item[0] * combinations
                S += combinations
            if S == 0:
                return 0
            else:
                return Sscore/S

    def call_evaluate(self, pos, version, pruning=False, printf=True):
        result = self.evaluate(pos, pos.rotate(), version, pruning=pruning)
        if printf:
            S = 0
            for key in self.result_dict[1]:
                #print_pos(key)
                item = self.result_dict[1][key]
                score, me, op = item[0], item[1], item[2]
                S += item[1] * item[2]
                print("score = %s, me = %d, op = %d, S = %s" % (score, me, op, S))
        print(render_tuple(self.tp_move[pos]))
        return result

    def search(self, pos, history=()):
        """ Iterative deepening MTD-bi search """
        self.nodes = 0
        self.calc_average()
        print("AI: I think my score is %d" % pos.score_rough)
        pos.set()
        if DRAW_TEST:
            self.history = set(history)
            # print('# Clearing table due to new history')
            self.tp_score.clear()

        # In finished games, we could potentially go far enough to cause a recursion
        # limit exception. Hence we bound the ply.
        for depth in range(5, 8):
            # The inner loop is a binary search on the score of the position.
            # Inv: lower <= score <= upper
            # 'while lower != upper' would work, but play tests show a margin of 20 plays
            # better.
            lower, upper = -MATE_UPPER, MATE_UPPER
            val = self.alphabeta(pos, lower, upper, depth, nullmove=NULLMOVE, nullmove_now=NULLMOVE)
            yield depth, self.tp_move.get(pos), self.tp_score.get((pos, depth, True), Entry(-MATE_UPPER, MATE_UPPER)).lower

    @staticmethod
    def setdepth(new_depths):
        global depths, len_depths
        depths = new_depths
        len_depths = len(depths)
        return depths, len_depths

    def calc_average(self, version=0):
        numr, numb = sum(di[version][True][key] for key in di[version][True]), sum(di[version][False][key] for key in di[version][False])
        averagecoveredr, averagecoveredb = 0, 0
        averager, averageb = {}, {}

        if numr == 0:
            averagecoveredr = 0
            for i in range(51, 204):
                averager[i] = 0

        else:
            sumr = 0
            for key in di[version][True].keys():
                sumr += pst["1"][key] * di[version][True][key] / discount_factor
            averagecoveredr = int(sumr//numr)

            for i in range(51, 204):
                sumr = 0
                for key in di[version][True].keys():
                    sumr += pst[key][i] * di[version][True][key]
                averager[i] = sumr//numr

        if numb == 0:
            averagecoveredb = 0
            for i in range(51, 204):
                averageb[i] = 0

        else:
            sumb = 0
            for key in di[version][False].keys():
                sumb += pst["1"][key.swapcase()] * di[version][False][key] / discount_factor
            averagecoveredb = int(sumb//numb)

            for i in range(51, 204):
                sumb = 0
                for key in di[version][False].keys():
                    sumb += pst[key.swapcase()][i] * di[version][False][key]
                averageb[i] = sumb//numb

        self.average = {True: {False: averagecoveredr, True: averager}, False: {False: averagecoveredb, True: averageb}}
        average[version] = deepcopy(self.average)
        return self.average

###############################################################################
# User interface
###############################################################################


def parse(c):
    fil, rank = ord(c[0]) - ord('a'), int(c[1])
    return A0 + fil - 16*rank


def render(i, reverse=False):
    if reverse:
        i = 254-i
    rank, fil = divmod(i - A0, 16)
    return chr(fil + ord('a')) + str(-rank)


def render_tuple(t, reverse=False):
    if (not isinstance(t, tuple)) or len(t) < 2:
        return ''
    return render(t[0], reverse) + render(t[1], reverse)


def print_pos(pos):
    if isinstance(pos, str):
        board = pos
    elif isinstance(pos, Position):
        board = pos.board
    else:
        assert False
    chessstr = ''
    for i, row in enumerate(board.split()):
        joinstr = ''.join(uni_pieces.get(p, p) for p in row)
        print(' ', 9 - i, joinstr)
        chessstr += (' ' + str(9 - i) + joinstr)
    print('    ａｂｃｄｅｆｇｈｉ\n\n')
    chessstr += '    ａｂｃｄｅｆｇｈｉ\n\n\n'
    return chessstr


def random_policy(pos):
    '''
    A test function that generates a random policy
    '''
    all_moves = list(pos.gen_moves())
    stupid_AI_move = random.choice(all_moves)
    return stupid_AI_move


def translate_eat(eat, dst, turn, type):
    assert turn in {'RED', 'BLACK'} and type in {'CLEARMODE', 'DARKMODE'}
    if eat is None or eat == '.':
        return None
    if turn == 'BLACK':
        eat = eat.swapcase()
    if type == 'DARKMODE':
        return uni_pieces[eat]
    else:
        if dst is None:  # 吃明子
            return uni_pieces[eat]
        else:  # 吃暗子
            dst = uni_pieces[dst]
            if turn == 'RED':
                dst += "(暗)"
            else:
                dst += "\033[31m(暗)\033[0m"
            return dst


def generate_forbiddenmoves(pos, check_bozi=True, step=0):
    # 生成禁着
    # 这里禁着判断比较简单，如果走了这步棋以后形成的局面在过往局面中超过3次，不允许电脑走。
    # 这里的pos是电脑视角
    global forbidden_moves
    forbidden_moves = set()
    pos.set()
    moves = pos.gen_moves()
    for move in moves:
        posnew = pos.move(move)
        if cache.get(posnew.board, 0) > 0:
            forbidden_moves.add(move)
        if check_bozi:
            i, j = move
            p, q = pos.board[i], pos.board[j].upper()
            # Actual move
            if p == 'H' and ((i == 164 and j == 52 and pos.board[51] in 'dr') or (
                    i == 170 and j == 58 and pos.board[59] in 'dr')):  # TODO: 使用更智能的方式处理博子
                if (i == 164 and pos.board[131] == 'N' and pos.board[115] == 'p') or (
                        j == 170 and pos.board[139] == 'N' and pos.board[123] == 'p'):  # 兵顶马
                    continue
                if (i == 164 and pos.board[148] == 'p') or (i == 170 and pos.board[154] == 'p'):
                    continue
                if pos.score_rough < 150 or pos.che == 0:
                    forbidden_moves.add(move)
            if p == 'H' and ((i == 164 and j == 100) or (i == 170 and j == 106)):
                if pos.score_rough < 160 or pos.che == 0:
                    forbidden_moves.add(move)
            if step < 5 and ((i == 164 and j == 116) or (i == 170 and j == 122)):
                forbidden_moves.add(move) 
    pos.set()
    return forbidden_moves


def print_cache():
    # 内部调试函数，打印cache
    print("print_cache starts!")
    for cnt, key in enumerate(cache):
        print("Cache " + str(cnt) + ":")
        print_pos(Position(key, 0, True, 0).set())
    print("print_cache ends!")


def printmapping():
    for k, v in mapping.items():
        print(render(k), ':', v)


def translate_rooted(rooted_chesses, rotated=False):
    def _translate(i):
        if rotated:
            return render(254-i)
        else:
            return render(i)
    return list(map(_translate, rooted_chesses))


def rotate_board_only(board):
    return board[-2::-1].swapcase() + " "


def main(random_move=False, AI=True, debug=False):
    global mapping
    resetrbdict()
    mapping = B.translate_mapping(B.mapping)
    searcher = Searcher()
    searcher.calc_average()
    with open("debug.json", "w") as f:
         json.dump(mapping, f)
    if debug:
        hist = [Position(initial_covered, 0, False, 0).set()]
        for i, x in enumerate(hist[-1].board):
            if x in 'RNBACP':
                di[0][hist[-1].turn][x if hist[-1].turn else x.lower()] -= 1
            elif x in 'rnbacp':
                di[0][not hist[-1].turn][x if hist[-1].turn else x.upper()] -= 1

    else:
        hist = [Position(initial_covered, 0, True, 0).set()]
    setcache(hist[-1].board)
    searcher.alphabeta(hist[-1], -MATE_UPPER, MATE_UPPER, 4, nullmove=False, nullmove_now=False)
    print_pos(hist[-1])
    print(render_tuple(searcher.tp_move.get(hist[-1])))
    searcher.call_evaluate(hist[-1], 0, False, False)
    assert di[0] == di[1]
    exit(0)
    myeatlist = []
    AIeatlist = []
    step = 0

    while True:
        print("\033[31m玩家吃子\033[0m: " + " ".join(myeatlist))
        print("电脑吃子:" + " " + " ".join(AIeatlist))

        print_pos(hist[-1])

        if hist[-1].score <= -MATE_LOWER:
            print("You lost")
            break

        # We query the user until she enters a (pseudo) legal move.
        move = None
        genmoves = set(hist[-1].gen_moves())
        while not debug and move not in genmoves:
            inp = input('Your move: ').strip()
            if inp.upper() == 'R':
                print("You resign!")
                exit(0)
            match = re.match('([a-i][0-9])'*2, inp)
            if match:
                move = parse(match.group(1)), parse(match.group(2))
                if inp.upper() == 'R':
                    print("You RESIGNED!")
                    break

            else:
                # Inform the user when invalid input (e.g. "help") is entered
                print("Please enter a move like h2e2")

        pos, win, eat, dst = hist[-1].mymove_check(move)

        if win:
            print("You win!")
            break

        rendered_eat = translate_eat(eat, dst, "RED", "CLEARMODE")
        if rendered_eat and move:
            myeatlist.append(rendered_eat)

        hist.append(pos)  # move的过程Rotate了一次, 这里pos是电脑视角
        rotated = hist[-1].rotate()  # 玩家视角
        setcache(rotated.board)

        # After our move we rotate the board and print it again.
        # This allows us to see the effect of our move.
        print("\033[31m玩家吃子\033[0m:" + " ".join(myeatlist))
        print("电脑吃子:" + " " + " ".join(AIeatlist))

        print_pos(rotated)

        # Fire up the engine to look for a move.
        _depth = 0

        move, score = None, 0
        if AI:
            if random_move:
                move = random_policy(hist[-1])
            else:
                if hist[-1].board in kaijuku:
                    move = kaijuku[hist[-1].board]
                else:
                    start = time.time()
                    generate_forbiddenmoves(hist[-1], check_bozi=True, step=step)
                    for _depth, move, score in searcher.search(hist[-1], hist):
                        if time.time() - start > THINK_TIME:
                            break
        else:
            genmoves = set(hist[-1].gen_moves())
            while move not in genmoves:
                match = re.match('([a-i][0-9])' * 2, input('Your move: '))
                if match:
                    move = parse(match.group(1)), parse(match.group(2))
                    move = (254 - move[0], 254 - move[1])
                else:
                    # Inform the user when invalid input (e.g. "help") is entered
                    print("Please enter a move like h2e2")

        if score == MATE_UPPER:
            print("Checkmate!")

        # The black player moves from a rotated position, so we have to
        # 'back rotate' the move before printing it.
        if move is None:
            print("You win!")
            break

        print("Think depth: {} My move: {} (score {})".format(_depth, render(254 - move[0]) + render(254 - move[1]), score))
        pos, win, eat, dst = hist[-1].mymove_check(move)

        if win:
            print("You lose, HAHAHAHAHAHAHAHAHAHA!")
            break

        rendered_eat = translate_eat(eat, dst, "BLACK", "CLEARMODE")
        if rendered_eat and move:
            AIeatlist.append(rendered_eat)

        hist.append(pos)
        setcache(hist[-1].board)
        
        if debug:
           print("RETURN FROM DEBUG MODE!")
           print_pos(hist[-1])
           break
        
        step += 1

    histdict = {}
    for i, history in enumerate(hist):
        histdict[i] = history.board
    with open("history.json", "w") as f:
        json.dump(histdict, f)


if __name__ == '__main__':
    main(random_move=False, AI=True, debug=True)

