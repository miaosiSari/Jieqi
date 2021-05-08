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
       self.turn = True


   def initialize_board(self):
       self.board = []
       for i in range(self.H):
           self.board.append([0]*self.W)

   def initialize_mapping(self):
       CHESSES = deepcopy(common.CHESSES)
       random.shuffle(CHESSES)
       for i, red_dark_chess in enumerate(common.RED_POSITION): #映射红暗子
           self.mapping[red_dark_chess] = CHESSES[i]
       random.shuffle(CHESSES)
       for i, black_dark_chess in enumerate(common.BLACK_POSITION): #映射红暗子
           self.mapping[black_dark_chess] = CHESSES[i]
        
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

   def print_board(self, board=None):
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
            for j in range(0, self.W):
                 color = board[i][j] & common.MASK_COLOR
                 if board[i][j] & common.MASK_CHESS_ISCOVERED == common.MASK_CHESS_ISCOVERED:
                      desc = '暗'
                 else:
                      desc = common.desc_dict[board[i][j] & common.MASK_CHESS_COLOR]
                 _helper(color, desc)
            print("")


   def uncover(self):
       '''
       Uncover a chess
       将暗子转为对应的明子
       '''
       virtual_board = []
       for i in range(self.H):
           virtual_board.append([0]*self.W)
       for element in self.mapping:
           virtual_board[element[0]][element[1]] = (self.board[element[0]][element[1]] & common.MASK_COLOR) + self.mapping[element]  
       virtual_board[0][4] = 13
       virtual_board[9][4] = 5
       self.print_board(virtual_board)


   def print_initial_state(self):
       print("Initial state:")
       self.print_board()
       print("Uncovering... Do not tell others!")
       self.uncover()
                 
                        
   

