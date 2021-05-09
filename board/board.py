from . import common
import random
from copy import deepcopy

#Notations
#棋盘的每一个位置是一个五位二进制数
#0: 无子
#1: 车
#2: 马
#3: 相
#4: 士
#5: 帅
#6: 炮
#7: 兵
#1 << 3位: 明0/暗1
#1 << 4位: 红1/黑0


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

class Board:
   def __init__(self, H=10, W=9, num=16):
       '''
       Initialize a chessboard
       '''
       self.H = H
       self.W = W
       self.num = num #每方16个棋子
       self.board = []
       self.mapping = {} #记录暗子到名字的映射
       self.initialize()
       self.original_board = deepcopy(self.board)
       self.turn = True #turn == True: 红方行棋, turn == False: 黑方行棋
       self.history = []


   def initialize_board(self):
       self.board = []
       for i in range(self.H):
           self.board.append([0]*self.W)


   def initialize_another_board(self):
       other_board = []
       for i in range(self.H):
           other_board.append([0]*self.W)
       return other_board

   def initialize_mapping(self):
       CHESSES = deepcopy(common.CHESSES)
       random.shuffle(CHESSES)
       for i, red_dark_chess in enumerate(common.RED_POSITIONS): #映射红暗子
           self.mapping[red_dark_chess] = common.MASK_COLOR + CHESSES[i]
       random.shuffle(CHESSES)
       for i, black_dark_chess in enumerate(common.BLACK_POSITIONS): #映射红暗子
           self.mapping[black_dark_chess] = CHESSES[i]

   def initialize_another_mapping(self):
       CHESSES = deepcopy(common.CHESSES)
       mapping = dict()
       random.shuffle(CHESSES)
       for i, red_dark_chess in enumerate(common.RED_POSITIONS): #映射红暗子
           mapping[red_dark_chess] = common.MASK_COLOR + CHESSES[i]
       random.shuffle(CHESSES)
       for i, black_dark_chess in enumerate(common.BLACK_POSITIONS): #映射红暗子
           mapping[black_dark_chess] = CHESSES[i]
       return mapping

   def copy_board(self, board=None, mapping=None, history=None, turn=None):
       copied = []
       if isinstance(board, list):
          self.board = deepcopy(board)
          copied.append("board")
       if isinstance(mapping, dict):
          self.mapping = deepcopy(mapping)
          copied.append("mapping")
       if isinstance(history, list):
          self.history = deepcopy(history)
          copied.append("history")
       if isinstance(turn, bool):
          self.turn = turn
          copied.append("turn")
       return copied

   def initialize_soldiers(self):
       #红
       self.board[0][0] = (1<<4) + (1<<3) + 1 #红左暗车
       self.board[0][1] = (1<<4) + (1<<3) + 2 #红左暗马
       self.board[0][2] = (1<<4) + (1<<3) + 3 #红左暗像
       self.board[0][3] = (1<<4) + (1<<3) + 4 #红左暗士
       self.board[0][4] = (1<<3) + 5 #红帅
       self.board[0][5] = (1<<4) + (1<<3) + 4 #红右暗士
       self.board[0][6] = (1<<4) + (1<<3) + 3 #红右暗相
       self.board[0][7] = (1<<4) + (1<<3) + 2 #红右暗马
       self.board[0][8] = (1<<4) + (1<<3) + 1 #红右暗车
       #红炮
       self.board[2][1] = (1<<4) + (1<<3) + 6 #红左暗炮
       self.board[2][7] = (1<<4) + (1<<3) + 6 #红右暗炮
       #红兵
       self.board[3][0] = (1<<4) + (1<<3) + 7 #红九路暗边兵
       self.board[3][2] = (1<<4) + (1<<3) + 7 #红七路暗边兵
       self.board[3][4] = (1<<4) + (1<<3) + 7 #红五路暗边兵
       self.board[3][6] = (1<<4) + (1<<3) + 7 #红三路暗边兵
       self.board[3][8] = (1<<4) + (1<<3) + 7 #红一路暗边兵
       #黑
       self.board[9][0] = (1<<4) + 1 #黑右暗车
       self.board[9][1] = (1<<4) + 2 #黑右暗马
       self.board[9][2] = (1<<4) + 3 #黑右暗像
       self.board[9][3] = (1<<4) + 4 #黑右暗士
       self.board[9][4] = 5 #黑将
       self.board[9][5] = (1<<4) + 4 #黑左暗士
       self.board[9][6] = (1<<4) + 3 #黑左暗相
       self.board[9][7] = (1<<4) + 2 #黑左暗马
       self.board[9][8] = (1<<4) + 1 #黑左暗车
       #黑炮
       self.board[7][1] = (1<<4) + 6 #黑右暗炮
       self.board[7][7] = (1<<4) + 6 #黑左暗炮
       #黑兵
       self.board[6][0] = (1<<4) + 7 #黑一路暗边兵
       self.board[6][2] = (1<<4) + 7 #黑三路暗边兵
       self.board[6][4] = (1<<4) + 7 #黑五路暗边兵
       self.board[6][6] = (1<<4) + 7 #黑七路暗边兵
       self.board[6][8] = (1<<4) + 7 #黑九路暗边兵

   def initialize(self):
       self.initialize_mapping()
       self.initialize_board()
       self.initialize_soldiers()
       self.turn = True


   def reset(self):
       self.mapping = {}
       self.initialize_board()
       self.turn = False


   def return_turn(self, verbose=True):
       if self.turn:
          if verbose:
             print('红')
          return '红'
       else:
          if verbose:
             print('黑')
          return '黑'

   def print_board(self, board=None, with_number=True):
       '''
       调试函数: 打印棋盘
       '''
       if board is None:
            board = self.board
       def _helper(color, desc):
            #color in ['red', 'black']
            if color == common.MASK_COLOR:
                 print("\033[31m" + desc + "\033[0m", end="")
            else:
                 print(desc, end="")
            
       for i in range(self.H-1, -1, -1):
            if with_number:
                 print(i, end="")
            for j in range(0, self.W):
                 color = board[i][j] & common.MASK_COLOR
                 if board[i][j] & common.MASK_CHESS_ISCOVERED == common.MASK_CHESS_ISCOVERED:
                      desc = '暗'
                 else:
                      desc = common.DESC_DICT[board[i][j] & common.MASK_CHESS_COLOR]
                 _helper(color, desc)
            print("")
            if i == self.H//2:
                 for k in range(2*self.W+1):
                     print("*", end="")
                 print("")
       if with_number:
            print("*", end="")
            for i in range(0, self.W):
                print(i, end=" ")#一个中文字符占用两个空格
            print("")

   def uncover(self):
       '''
       Uncover a chess
       将暗子转为对应的明子
       '''
       virtual_board = self.initialize_another_board()
       for element in self.mapping:
           virtual_board[element[0]][element[1]] = self.mapping[element]  
       virtual_board[0][4] = common.RED_SHUAI
       virtual_board[9][4] = common.BLACK_JIANG
       self.print_board(virtual_board)

   def print_initial_state(self):
       print("Initial state:")
       self.print_board()
       print("Uncovering... Do not tell others!")
       self.uncover()

   def random_board(self):
       #This function generates a random board
       new_board = self.initialize_another_board()
       mapping = self.initialize_another_mapping()
       #处理固定的暗子
       covered_number, fixed_covered_chesses = common.random_select(common.COVERED_POSITIONS, set)
       for element in fixed_covered_chesses:
           new_board[element[0]][element[1]] = self.original_board[element[0]][element[1]]
       #安置帅将
       RED_SHUAI_POSSIBLE_POSITIONS = common.RED_JIUGONG - fixed_covered_chesses #红帅不能和红暗士重叠
       RED_SHUAI_POS = random.sample(RED_SHUAI_POSSIBLE_POSITIONS, 1)[0]
       new_board[RED_SHUAI_POS[0]][RED_SHUAI_POS[1]] = common.RED_SHUAI
       BLACK_JIANG_POSSIBLE_POSITIONS = common.BLACK_JIUGONG - fixed_covered_chesses #黑将不能和黑暗士重叠
       BLACK_JIANG_POS = random.sample(BLACK_JIANG_POSSIBLE_POSITIONS, 1)[0]
       new_board[BLACK_JIANG_POS[0]][BLACK_JIANG_POS[1]] = common.BLACK_JIANG
       other_possible_positions = common.ALL_POSITIONS- {RED_SHUAI_POS} - {BLACK_JIANG_POS} - fixed_covered_chesses
       uncovered_counter = deepcopy(common.UNCOVERED_COUNTER)
       #将帅已安置，不能重复考虑
       uncovered_counter[common.RED_SHUAI] = 0
       uncovered_counter[common.BLACK_JIANG] = 0
       #去除没被揭开的部分
       for element in fixed_covered_chesses:
           uncovered_counter[mapping[element]] -= 1
       #将dict展开为multi-set: {'k':2} --> ['k', 'k']
       all_uncovered_chesses = []
       for k in uncovered_counter:
           all_uncovered_chesses += [k]*uncovered_counter[k]
       #选取存活的明子
       number_of_alived_uncovered_chesses, alive_uncovered_chesses = common.random_select(all_uncovered_chesses)
       positions_of_alived_uncovered_chesses = random.sample(other_possible_positions, number_of_alived_uncovered_chesses)
       for i, element in enumerate(alive_uncovered_chesses):
           pos = positions_of_alived_uncovered_chesses[i]
           new_board[pos[0]][pos[1]] = element
       return new_board, mapping

   def dict(self):
       return {'board': self.board, 'mapping': self.mapping, 'history': self.history, 'turn': self.turn}

   def __str__(self):
       return str(self.dict())