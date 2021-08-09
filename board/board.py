from . import common
import random
import json
from copy import deepcopy
from collections import Counter


#参考代码
#https://github.com/NeymarL/ChineseChess-AlphaZero/
#https://github.com/bupticybee/icyChessZero
#https://zhuanlan.zhihu.com/p/34433581


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

   A0, I0, A9, I9 = 12 * 16 + 3, 12 * 16 + 11, 3 * 16 + 3,  3 * 16 + 11
   ALL = ord('a') + ord('i')

   def __init__(self, H=10, W=9, num=16):
       '''
       Initialize a chessboard
       '''
       self.H = H
       self.W = W
       self.num = num #每方16个棋子
       self.board = []
       self.mapping = {} #记录暗子到明子的映射
       self.chessdict = dict()
       self.initialize()
       self.original_board = deepcopy(self.board)
       self.pst = deepcopy(common.pst)
       self.turn = True #turn == True: 红方行棋, turn == False: 黑方行棋
       self.history = []
       self.shuai = (0, 4)
       self.jiang = (9, 4)

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
          shuai, jiang = self.search_kings()
          self.shuai, self.jiang = shuai, jiang
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

   def is_legal_board(self, board=None):
       if board is None:
          board = self.board
       shuai = None
       jiang = None
       shuaicount = 0
       jiangcount = 0

       uncovered_counter = deepcopy(common.UNCOVERED_COUNTER)
       if board[1][2] == 15 or board[1][6] == 15 or board[0][0] == 15 or board[0][8] == 15 or board[8][2] == 7 or board[8][6] == 7 or board[9][0] == 7 or board[9][8] == 7:
          '''
          红方兵不可能出现在以下a位置:
          1**a***a**
          0a*******a
          0012345678
          '''
          #print(i, j, "ILLEGAL BING/ZU")
          return False

       for element in (common.ALL_POSITIONS - common.COVERED_POSITIONS):
          i = element[0]
          j = element[1]
          if board[i][j] & common.MASK_CHESS_ISCOVERED != 0:
             #print(i, j, "NON COVERED AREA FINDS COVERED CHESSES!")
             return False

       for element in common.RED_POSITIONS:
          i = element[0]
          j = element[1]
          if board[i][j] & common.MASK_CHESS_ISCOVERED != 0 and board[i][j] & common.MASK_COLOR == 0:
             #print(i, j, "COVERED RED AREA FINDS COVERED BLACK CHESSES!")
             return False

       for element in common.BLACK_POSITIONS:
          i = element[0]
          j = element[1]
          if board[i][j] & common.MASK_COLOR != 0 and  board[i][j] & common.MASK_CHESS_ISCOVERED != 0:
             #print(i, j, "COVERED BLACK AREA FINDS COVERED RED CHESSES!")
             return False

       for i in range(self.H):
          for j in range(self.W):
              if board[i][j] > 0 and board[i][j] & common.MASK_CHESS_ISCOVERED == 0:
                  if uncovered_counter[board[i][j]] < 1:
                      return False
                  uncovered_counter[board[i][j]] -= 1
              if board[i][j] == common.RED_SHUAI:
                  if i > 2 or j < 3 or j > 5:
                      #print("RED_SHUAI OUT OF BOUND!")
                      return False
                  shuaicount += 1
                  shuai = (i, j)
                  if shuaicount >= 2:
                      #print("SHUAICOUNT ILLEGAL")
                      return False

              if board[i][j] == common.BLACK_JIANG:
                  if i < 7 or j < 3 or j > 5:
                      #print("BLACK_JIANG OUT OF BOUND!")
                      return False
                  jiangcount += 1
                  jiang = (i, j)
                  if jiangcount >= 2:
                      #print("JIANGCOUNT ILLEGAL")
                      return False
       if shuaicount == 1 and jiangcount == 1:
           return (shuai, jiang)
       else:
           return False

   def initialize_chessdict(self, board=None):
       chessdict = self.chessdict if board is None else {}
       if board is None:
           board = self.board
       for i in range(self.H):
           for j in range(self.W):
               if board[i][j] != 0:
                  chessdict[(i, j)] = board[i][j]
       return chessdict

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
       self.chessdict = deepcopy(self.initialize_chessdict())

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
       调试函数: 打印棋盘 Print the board
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

   def print_board_icybee(self, board=None):
   	  #https://github.com/bupticybee/elephantfish/blob/master/elephantfish.py
   	  #print_pos(pos) function
      chessstr = ''
      uni_pieces = {
       '.':'．',
       'R':'\033[31m俥\033[0m',
       'N': '\033[31m傌\033[0m', 
       'B': '\033[31m相\033[0m', 
       'A': '\033[31m仕\033[0m', 
       'K': '\033[31m帅\033[0m', 
       'P': '\033[31m兵\033[0m', 
       'C': '\033[31m炮\033[0m', 
       'U': '\033[31m不\033[0m', 
       'D': '\033[31m暗\033[0m', 
       'E': '\033[31m暗\033[0m',
       'F': '\033[31m暗\033[0m',
       'G': '\033[31m暗\033[0m',
       'H': '\033[31m暗\033[0m', 
       'I': '\033[31m暗\033[0m',
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
      for i, row in enumerate(board.split()):
          joinstr = ''.join(uni_pieces.get(p, p) for p in row)
          print(' ', 9-i, joinstr)
          chessstr += (' ' + str(9-i) + joinstr)
      print('    ａｂｃｄｅｆｇｈｉ\n\n')
      chessstr += '    ａｂｃｄｅｆｇｈｉ\n\n\n'
      return chessstr

   def print_initial_state(self):
       print("Initial state:")
       self.print_board()
       print("Uncovering... Do not tell others!")
       virtual_board = self.uncover_board(self.board, self.mapping, verbose=True)

   def random_board(self):
       #This function generates a random board
       new_board = self.initialize_another_board()
       mapping = self.initialize_another_mapping()
       chessdict = {}
       #处理固定的暗子
       covered_number, fixed_covered_chesses = common.random_select(common.COVERED_POSITIONS, set)
       for pos in fixed_covered_chesses:
           chessdict[pos] = new_board[pos[0]][pos[1]] = self.original_board[pos[0]][pos[1]]
       #安置帅将
       RED_SHUAI_POSSIBLE_POSITIONS = common.RED_JIUGONG - fixed_covered_chesses #红帅不能和红暗士重叠
       RED_SHUAI_POS = random.sample(RED_SHUAI_POSSIBLE_POSITIONS, 1)[0]
       new_board[RED_SHUAI_POS[0]][RED_SHUAI_POS[1]] = chessdict[RED_SHUAI_POS] = common.RED_SHUAI
       BLACK_JIANG_POSSIBLE_POSITIONS = common.BLACK_JIUGONG - fixed_covered_chesses #黑将不能和黑暗士重叠
       BLACK_JIANG_POS = random.sample(BLACK_JIANG_POSSIBLE_POSITIONS, 1)[0]
       new_board[BLACK_JIANG_POS[0]][BLACK_JIANG_POS[1]] = chessdict[BLACK_JIANG_POS] = common.BLACK_JIANG
       other_possible_positions = common.ALL_POSITIONS- {RED_SHUAI_POS} - {BLACK_JIANG_POS} - fixed_covered_chesses
       uncovered_counter = deepcopy(common.UNCOVERED_COUNTER)
       #将帅已安置，不能重复考虑
       uncovered_counter[common.RED_SHUAI] = uncovered_counter[common.BLACK_JIANG] = 0
       #去除没被揭开的部分
       for pos in fixed_covered_chesses:
           uncovered_counter[mapping[pos]] -= 1
       #将dict展开为multi-set: {'k':2} --> ['k', 'k']
       red_bing_positions = common.random_select(other_possible_positions - common.RED_BING_FORBIDDEN, return_type=set, num=uncovered_counter.get(15, 0))[1]
       for pos in red_bing_positions:
           chessdict[pos] = new_board[pos[0]][pos[1]] = 15
       black_zu_positions = common.random_select(other_possible_positions - common.BLACK_ZU_FORBIDDEN - red_bing_positions, return_type=set, num=uncovered_counter.get(7, 0))[1]
       for pos in black_zu_positions:
           chessdict[pos] = new_board[pos[0]][pos[1]] = 7
       uncovered_counter[15] = uncovered_counter[7] = 0
       other_possible_positions = other_possible_positions - red_bing_positions - black_zu_positions

       all_uncovered_chesses = []
       for k in uncovered_counter:
           all_uncovered_chesses += [k]*uncovered_counter[k]
       #选取存活的明子
       number_of_alived_uncovered_chesses, alive_uncovered_chesses = common.random_select(all_uncovered_chesses)
       positions_of_alived_uncovered_chesses = random.sample(other_possible_positions, number_of_alived_uncovered_chesses)
       for i, element in enumerate(alive_uncovered_chesses):
           pos = positions_of_alived_uncovered_chesses[i]
           new_board[pos[0]][pos[1]] = element
           chessdict[(pos[0], pos[1])] = element
       assert self.initialize_chessdict(new_board) == chessdict
       return new_board, mapping, (RED_SHUAI_POS, BLACK_JIANG_POS), chessdict

   def random_board_uncertainty(self):
       #This function generates a random board
       #Consider uncertainty 2021/05/22
       new_board = self.initialize_another_board()
       mapping = self.initialize_another_mapping()
       chessdict = {}
       red = black = 0
       #处理固定的暗子
       covered_number, fixed_covered_chesses = common.random_select(common.COVERED_POSITIONS, set)
       for pos in fixed_covered_chesses:
           chessdict[pos] = new_board[pos[0]][pos[1]] = self.original_board[pos[0]][pos[1]]
       #安置帅将
       RED_SHUAI_POSSIBLE_POSITIONS = common.RED_JIUGONG - fixed_covered_chesses #红帅不能和红暗士重叠
       RED_SHUAI_POS = random.sample(RED_SHUAI_POSSIBLE_POSITIONS, 1)[0]
       new_board[RED_SHUAI_POS[0]][RED_SHUAI_POS[1]] = chessdict[RED_SHUAI_POS] = common.RED_SHUAI
       BLACK_JIANG_POSSIBLE_POSITIONS = common.BLACK_JIUGONG - fixed_covered_chesses #黑将不能和黑暗士重叠
       BLACK_JIANG_POS = random.sample(BLACK_JIANG_POSSIBLE_POSITIONS, 1)[0]
       new_board[BLACK_JIANG_POS[0]][BLACK_JIANG_POS[1]] = chessdict[BLACK_JIANG_POS] = common.BLACK_JIANG
       other_possible_positions = common.ALL_POSITIONS- {RED_SHUAI_POS} - {BLACK_JIANG_POS} - fixed_covered_chesses
       uncovered_counter = deepcopy(common.UNCOVERED_COUNTER)
       #将帅已安置，不能重复考虑
       uncovered_counter[common.RED_SHUAI] = uncovered_counter[common.BLACK_JIANG] = 0
       #去除没被揭开的部分
       for pos in fixed_covered_chesses:
           uncovered_counter[mapping[pos]] -= 1
       #将dict展开为multi-set: {'k':2} --> ['k', 'k']
       red_bing_positions = common.random_select(other_possible_positions - common.RED_BING_FORBIDDEN, return_type=set, num=uncovered_counter.get(15, 0))[1]
       for pos in red_bing_positions:
           chessdict[pos] = new_board[pos[0]][pos[1]] = 15
       black_zu_positions = common.random_select(other_possible_positions - common.BLACK_ZU_FORBIDDEN - red_bing_positions, return_type=set, num=uncovered_counter.get(7, 0))[1]
       for pos in black_zu_positions:
           chessdict[pos] = new_board[pos[0]][pos[1]] = 7
       uncovered_counter[15] = uncovered_counter[7] = 0
       other_possible_positions = other_possible_positions - red_bing_positions - black_zu_positions

       all_uncovered_chesses = []
       for k in uncovered_counter:
           all_uncovered_chesses += [k]*uncovered_counter[k]
       #选取存活的明子
       number_of_alived_uncovered_chesses, alive_uncovered_chesses = common.random_select(all_uncovered_chesses)
       positions_of_alived_uncovered_chesses = random.sample(other_possible_positions, number_of_alived_uncovered_chesses)
       for i, element in enumerate(alive_uncovered_chesses):
           pos = positions_of_alived_uncovered_chesses[i]
           new_board[pos[0]][pos[1]] = element
           chessdict[(pos[0], pos[1])] = element

       #计算剩余位置, 2021/05/22
       possible_uncertainties_positions = other_possible_positions - set(positions_of_alived_uncovered_chesses)

       for key in chessdict:
       	   if chessdict[key] == 0:
       	   	  continue
       	   if chessdict[key] & common.MASK_COLOR == common.MASK_COLOR:
       	   	  red += 1
       	   else:
       	   	  black += 1
       
       red, black = 16-red, 16-black
       
       set_red_uncertainty_positions = set()
       if red > 0:
          num_of_red_uncertainties = random.randint(1, red)
          _, set_red_uncertainty_positions = common.random_select(possible_uncertainties_positions, return_type=set, num=num_of_red_uncertainties)
          for pos in set_red_uncertainty_positions:
              chessdict[pos] = new_board[pos[0]][pos[1]] = 40

       set_black_uncertainty_positions = set()
       if black > 0:
          possible_uncertainties_positions -= set_red_uncertainty_positions
          num_of_black_uncertainties = random.randint(1, black)
          _, set_black_uncertainty_positions = common.random_select(possible_uncertainties_positions, return_type=set, num=num_of_black_uncertainties)
          for pos in set_black_uncertainty_positions:
              chessdict[pos] = new_board[pos[0]][pos[1]] = 32

       return new_board, mapping, (RED_SHUAI_POS, BLACK_JIANG_POS), chessdict

   def generate_and_check(self):
       new_board, mapping = self.random_board()
       assert self.is_legal_board(new_board)
       return new_board, mapping

   def uncover_board(self, board, mapping, verbose=True):
       virtual_board = deepcopy(board)
       for element in common.COVERED_POSITIONS:
           if board[element[0]][element[1]] & common.MASK_CHESS_ISCOVERED != 0: #找到暗子
                virtual_board[element[0]][element[1]] = mapping[element]
       if verbose:
           self.print_board(virtual_board)
       return virtual_board

   def uncover_board_icybee(self, board, mapping):
   	   covered_set = {'d', 'D', 'e', 'E', 'f', 'F', 'g', 'G', 'h', 'H', 'i', 'I'}
   	   newboard = list(board)
   	   for key in mapping:
   	   	   if board[key] in covered_set:
   	   	      newboard[key] = mapping[key]
   	   return ''.join(newboard)
       
   def dict(self):
       return {'board': self.board, 'mapping': self.mapping, 'history': self.history, 'turn': self.turn}

   def __str__(self):
       return str(self.dict())

   def inchessboard(self, x, y):
       return (x >= 0) and (x < self.H) and (y >= 0) and (y < self.W)

   def inchessboard_tuple(self, xy):
       return (xy[0] >= 0) and (xy[0] < self.H) and (xy[1] >= 0) and (xy[1] < self.W)
  
   def drink(self, board=None, shuaijiang=None):
       if not board:
          board = self.board
       if shuaijiang is None:
          shuai = self.shuai
          jiang = self.jiang
       else:
          shuai, jiang = shuaijiang
       '''
       将帅在同一条直线上时, 中间必须隔着其他子力, 否则饮酒
       '''
       if shuai[1] != jiang[1]:
           return False
       else:
           for i in range(shuai[0]+1, jiang[0]):
               if board[i][shuai[1]] != 0:
                   return False
           return True

   def check_color(self, place, xor=True, board=None, turn=None):
       '''
       检查(x, y)处的子力颜色
       为了加快运算速度，这里并没有对x, y做越界判断!
       xor = Exclusive or
       如果xor == False, 则返回坐标(x, y)处子力的颜色, 红色为True。
       如果xor == True, 则返回坐标(x, y)处子力的颜色与当前行棋的一方是否吻合。
       '''
       red_or_black = None
       if board is None:
          board = self.board
       if turn is None:
          turn = self.turn
       if board[place[0]][place[1]] == 0:
          return None
       else:
          red_or_black = (board[place[0]][place[1]] & common.MASK_COLOR != 0)
          if xor:
            return not (red_or_black ^ turn)
          else:
            return red_or_black

   def search_kings(self, board=None):
       #Where are the kings?
       #搜寻将帅位置
       shuai = None
       jiang = None
       if not board:
           board = self.board
       for i in (0, 1, 2, 7, 8, 9):
           for j in (3, 4, 5):
               if board[i][j] == common.RED_SHUAI:
                    assert i <= 2
                    shuai = (i, j)
               elif board[i][j] == common.BLACK_JIANG:
                    assert i >= 7
                    jiang = (i, j)
               if shuai and jiang:
                    return shuai, jiang
       if shuai is None or jiang is None:
           raise ValueError("Where are the kings?")
       return None, None

   def check_legal(self, src, dst, board=None, turn=None, shuaijiang=None):
       #20210510 morning not tested!
       '''
       src: (x, y), dst: (z, w) 为二元组
       return: islegal
       islegal: 是否为合法步骤
       '''

       #检查是否超出棋盘范围
       '''
       jiangjun = False
       if (self.turn and dst == self.jiang) or ((not self.turn) and dst == self.shuai):
           jiangjun = True
           if ((self.turn and src == self.shuai) or ((not self.turn) and src == self.jiang)) and self.drink():
               return True, True
       '''
       if board is None:
          board = self.board
       if turn is None:
          turn = self.turn
       if shuaijiang is None:
          shuai = self.shuai
          jiang = self.jiang
       else:
          shuai, jiang = shuaijiang
       if src == dst or not self.inchessboard_tuple(src) or not self.inchessboard_tuple(dst):
          return False
       #检查棋子颜色，不能吃自己的子
       check_src_color = self.check_color(src, xor=True, board=board, turn=turn)
       check_dst_color = self.check_color(dst, xor=False, board=board, turn=turn)
       if not check_src_color or (check_dst_color == turn):
           return False
       chess = board[src[0]][src[1]]
       chess_type = chess & common.MASK_CHESS
       #print("chess = %s, chess_type = %s, self.drink = %s"%(chess, chess_type, self.drink()))
       if chess_type not in (1, 6): #车炮比较特殊
          if chess_type == 2:#马
              absx = abs(src[0] - dst[0])
              absy = abs(src[1] - dst[1])
              #计算马腿位置
              biematui = None
              if absx == 2 and absy == 1:
                 biematui = ((src[0] + dst[0]) // 2, src[1])
              elif absx == 1 and absy == 2:
                 biematui = (src[0], (src[1] + dst[1])//2)
              else:
                 return False
              if board[biematui[0]][biematui[1]] != 0:
                 return False
              else:
                 return True

          elif chess_type == 3:#相/象, 判断塞象眼
              if board[(src[0] + dst[0])//2][(src[1] + dst[1])//2] != 0: #象眼位置: ((src[0] + dst[0])//2, (src[1] + dst[1])//2)
                 return False  
          
          elif chess_type == 4:#士, 暗士只能往中间走
              if board[src[0]][src[1]] & common.MASK_CHESS_ISCOVERED != 0 and dst[1] != 4:
                 return False

          elif chess_type == 5:#帅
              if ((turn and dst == jiang) or (not turn and dst == shuai)) and self.drink(board=board, shuaijiang=shuaijiang): #判断饮酒
                 return True
              if (turn and dst not in common.RED_JIUGONG) or ((not turn) and dst not in common.BLACK_JIUGONG):
                 return False

          #20210513, 发现兵的逻辑不对，遗漏了未过河兵的判断
          elif chess_type == 7:#兵
              if turn and src[0] <= 4 and (dst[1] != src[1]): #未过河兵只能竖着走
                 return False
              if (not turn) and src[0] >= 5 and (dst[1] != src[1]):
                 return False

          sub_vector = common.addsub(dst, src, '-')
          #print("sub_vector: ", sub_vector)
          return (sub_vector in common.DIRECTION_DICT[chess])

       elif chess_type == 1: #车
          if src[0] == dst[0]:
               #检查障碍物
               #Check obstacles in the open interval (src[1], dst[1])
               for potential_obstacle in range(min(src[1], dst[1])+1, max(src[1], dst[1])):
                   if board[src[0]][potential_obstacle] != 0:
                        return False
               return True
          elif src[1] == dst[1]:
               for potential_obstacle in range(min(src[0], dst[0])+1, max(src[0], dst[0])):
                   if board[potential_obstacle][src[1]] != 0:
                        return False
               return True
          else:
               return False

       elif chess_type == 6: #炮
          if src[0] == dst[0]:
             obstacles = 0
             for potential_obstacle in range(min(src[1], dst[1])+1, max(src[1], dst[1])):
                if board[src[0]][potential_obstacle] != 0:
                    if check_dst_color is None:
                        return False
                    obstacles += 1
                if obstacles == 2:
                    return False
             return obstacles == 1 or (check_dst_color is None)
          elif src[1] == dst[1]:
             obstacles = 0
             for potential_obstacle in range(min(src[0], dst[0])+1, max(src[0], dst[0])):
                if board[potential_obstacle][src[1]] != 0:
                   if check_dst_color is None:
                        return False
                   obstacles += 1
                if obstacles == 2:
                   return False
             return obstacles == 1 or (check_dst_color is None)
          else:
             return False

   def check_legal_and_jiangjun(self, src, dst, board=None, turn=None, shuaijiang=None):
       if not self.check_legal(src, dst, board=board, turn=turn, shuaijiang=shuaijiang):
           return False, False
       if board is None:
          board = self.board
       if turn is None:
          turn = self.turn
       if shuaijiang is None:
          shuai = self.shuai
          jiang = self.jiang
       else:
          shuai, jiang = shuaijiang
       if (turn and dst == jiang) or ((not turn) and dst == shuai):
           return True, True
       else:
           return True, False

   def stupid_print_all_legal_moves(self, only_legal=True, board=None, turn=None, shuaijiang=None):
       counter = 0
       # A very stupid way to test check_legal_and_jiangjun
       for x1 in range(self.H):
           for x2 in range(self.H):
               for y1 in range(self.W):
                   for y2 in range(self.W):
                       islegal, isjiangjun = self.check_legal_and_jiangjun((x1, y1), (x2, y2), board=None, turn=None, shuaijiang=None)
                       drink = self.drink()
                       if counter % 10 == 0:
                           if not only_legal or islegal:
                               self.print_board()
                       if not only_legal:
                           print("(%s, %s) --> (%s, %s): 是否合法:%s, 是否将军:%s, 是否对饮:%s"%(x1, y1, x2, y2, islegal, isjiangjun, drink))
                       elif islegal:
                           print("(%s, %s) --> (%s, %s): 是否合法:%s, 是否将军:%s, 是否对饮:%s"%(x1, y1, x2, y2, islegal, isjiangjun, drink))
                           counter += 1
                       if not only_legal:
                           counter += 1
       print(counter)

   def stupid_generate_all_legal_moves(self, board=None, turn=None, shuaijiang=None):
       counter = 0
       legal_moves = []
       # A very stupid way to test check_legal_and_jiangjun
       for x1 in range(self.H):
           for x2 in range(self.H):
               for y1 in range(self.W):
                   for y2 in range(self.W):
                       islegal = self.check_legal((x1, y1), (x2, y2), board=board, turn=turn, shuaijiang=shuaijiang)
                       if islegal:
                           legal_moves.append((x1, y1, x2, y2))
       return legal_moves

   def move(self, src, dst, need_check_legal=True):
        #20210513
        #Warning: Not tested! May have many bugs!
        #We will test it after building the UI
        translate = "" #UCCI Representation/UCCI 表示
        if need_check_legal:
           if not self.check_legal(src, dst):
             return False
        if self.board[dst[0]][dst[1]] == 0: #目标位置没有子力
          #Update the board & the chessdict
           self.chessdict.pop(src)
           if self.board[src[0]][src[1]] & common.MASK_CHESS_ISCOVERED == 0:
             self.chessdict[dst] = self.board[dst[0]][dst[1]] = self.board[src[0]][src[1]]
           else:
             self.chessdict[dst] = self.board[dst[0]][dst[1]] = self.mapping[src]
             if self.board[dst[0]][dst[1]] == common.RED_SHUAI:
                self.shuai = (dst[0], dst[1])
             if self.board[dst[0]][dst[1]] == common.BLACK_JIANG:
                self.jiang = (dst[0], dst[1])
          #Update the history
           self.history.append((src[0], src[1], dst[0], dst[1], None))
        else:
          #Update the board & the chessdict
           if self.board[src[0]][src[1]] & common.MASK_CHESS_ISCOVERED == 0:
             self.chessdict[dst] = self.board[dst[0]][dst[1]] = self.chessdict[src]
           else:
             self.chessdict[dst] = self.board[dst[0]][dst[1]] = self.mapping[src]
             if self.board[dst[0]][dst[1]] == common.RED_SHUAI:
                self.shuai = (dst[0], dst[1])
             if self.board[dst[0]][dst[1]] == common.BLACK_JIANG:
                self.jiang = (dst[0], dst[1])
           self.chessdict.pop(src)
           #Update the history
           self.history.append((src[0], src[1], dst[0], dst[1], self.board[dst[0]][dst[1]]))
        self.board[src[0]][src[1]] = 0
        self.turn = not self.turn
        return (src, dst, translate, self.turn)

   def translate_board(self, board=None):
      new_board = [' ']*256
      t = 15
      d = {
          (False, 1): 'R', 
          (True, 1): 'D', 
          (False, 2): 'N', 
          (True, 2): 'E', 
          (False, 3): 'B', 
          (True, 3): 'F', 
          (False, 4): 'A', 
          (True, 4): 'G', 
          (False, 5): 'K', 
          (False, 6): 'C', 
          (True, 6): 'H', 
          (False, 7): 'P', 
          (True, 7): 'I'
      }
      while t < 256:
   	  	 new_board[t] = '\n'
   	  	 t += 16
      if board is None:
         board = self.board
      for i in range(self.H):
         for j in range(self.W):
            x, y = 12 - i, 3 + j
            pos = x * 16 + y
            chess = board[i][j]
            ######################################################
            #Handle the Uncertainty Case   
            ######################################################
            if chess & common.MASK_CHESS_UNCERTAIN != 0:
               if chess & common.MASK_COLOR != 0:
               	  new_board[pos] = 'U'
               else:
               	  new_board[pos] = 'u'
               continue
            covered = ((chess & common.MASK_CHESS_ISCOVERED) != 0)
            color = chess & common.MASK_COLOR
            chess_type = chess & common.MASK_CHESS
            if chess_type < 1:
                new_board[pos] = '.'
                continue
            T = d[(covered, chess_type)]
            if not color:
                T = T.swapcase()
            new_board[pos] = T

      return ''.join(new_board)

   def translate_chess(self, chess, return_type=1):
      assert return_type in {1, 2}
      d1 = {
          (False, 1): 'R', 
          (True, 1): 'D', 
          (False, 2): 'N', 
          (True, 2): 'E', 
          (False, 3): 'B', 
          (True, 3): 'F', 
          (False, 4): 'A', 
          (True, 4): 'G', 
          (False, 5): 'K', 
          (False, 6): 'C', 
          (True, 6): 'H', 
          (False, 7): 'P', 
          (True, 7): 'I'
      }
      d2 = {
          (False, 1): '车', 
          (True, 1): '暗车', 
          (False, 2): '马', 
          (True, 2): '暗马', 
          (False, 3): '相', 
          (True, 3): '暗相', 
          (False, 4): '士', 
          (True, 4): '暗士', 
          (False, 5): '皇', 
          (False, 6): '炮', 
          (True, 6): '暗炮', 
          (False, 7): '兵', 
          (True, 7): '暗兵'
      }
      if chess & common.MASK_CHESS_UNCERTAIN != 0:
         if chess & common.MASK_COLOR != 0:
            if return_type == 1:
              return 'U'
            else:
              return '不'
         else:
            if return_type == 1:
               return 'u'
            else:
               return '不'
      covered = ((chess & common.MASK_CHESS_ISCOVERED) != 0)
      color = chess & common.MASK_COLOR
      chess_type = chess & common.MASK_CHESS
      if chess_type < 1:
         if return_type == 1:
            return '.'
         else:
            return '无'
      if return_type == 1:
         T = d1[(covered, chess_type)]
         if not color:
            T = T.swapcase()
         return T
      else:
      	 return d2[(covered, chess_type)]

   def translate_move(self, t):
      orda = ord('a')
      def _num2char(num):
   	  	  return chr(orda + num)
      ucci = _num2char(t[1]) + str(t[0]) + _num2char(t[3]) + str(t[2])
      return ucci

   def reverse_move(self, t):
   	  return chr(Board.ALL - ord(t[0])) + str(9 - int(t[1])) + chr(Board.ALL - ord(t[2])) + str(9 - int(t[3]))

   def translate_mapping(self, mapping):
      d = {
        1: 'R', 
        2: 'N', 
        3: 'B', 
        4: 'A', 
        5: 'K',  
        6: 'C', 
        7: 'P', 
      }
      newmapping = {}
      for key in mapping.keys():
          x, y = 12 - key[0], 3 + key[1]
          pos = x * 16 + y
          chess = mapping[key]
          covered = ((chess & common.MASK_CHESS_ISCOVERED) != 0)
          color = chess & common.MASK_COLOR
          chess_type = chess & common.MASK_CHESS
          T = d[chess_type]
          if not color:
          	 T = T.swapcase()
          newmapping[pos] = T
      return newmapping

   def generate(self, turn, check=False, file=False, verbose=False, uncertainty=False):
      board, mapping, shuaijiang, chessdict = None, None, None, None
      if uncertainty:
         board, mapping, shuaijiang, chessdict = self.random_board_uncertainty()
      else:
         board, mapping, shuaijiang, chessdict = self.random_board()
      original_board = deepcopy(board)
      original_mapping = deepcopy(mapping)
      legal_moves = list(self.get_legal_moves_speedup(board=board, turn=turn, shuaijiang=shuaijiang, chessdict=chessdict))
      if check:
         legal_moves_2 = self.stupid_generate_all_legal_moves(board=board, turn=turn, shuaijiang=shuaijiang)
         uncovered_board = self.uncover_board(board=board, mapping=mapping, verbose=False)
         translate_uncovered_board2 = self.translate_board(uncovered_board)
         try:
             assert set(legal_moves) == set(legal_moves_2)
         except:
             print(set(legal_moves), 'self.get_legal_moves_speedup')
             print(set(legal_moves_2), 'self.stupid_generate_all_legal_moves')
             print(set(legal_moves) - set(legal_moves_2))
             print(set(legal_moves_2) - set(legal_moves))
             self.print_board_icybee(self.translate_board(board))
             assert False
      newmapping = self.translate_mapping(mapping)
      legal_moves = list(map(self.translate_move, legal_moves))
      board = self.translate_board(board)
      if verbose:
         self.print_board_icybee(board)
      if check:
         translate_uncovered_board = self.uncover_board_icybee(board, newmapping)
         print('translate_uncovered_board: mapping --translate_mapping-> newmapping --uncover_board_icybee-> translate_uncovered_board')
         self.print_board_icybee(translate_uncovered_board)
         print('translate_uncovered_board: board --uncover_board-> uncovered_board --> translate_uncovered_board2')
         self.print_board_icybee(translate_uncovered_board2)
         assert translate_uncovered_board2 == translate_uncovered_board
         self.print_board_icybee(translate_uncovered_board)
      if file:
         filedict = {
           'turn': turn,
           'board': board,
           'mapping': newmapping,
           'legal_moves': legal_moves
         }
         with open(file, 'w') as f:
            json.dump(filedict, f)
      return board, newmapping, legal_moves, shuaijiang, chessdict, original_board, original_mapping

   def scan(self, board):
      red, black = 0, 0
      red_possible_list, black_possible_list = [], []
      uncovered_counter = deepcopy(common.UNCOVERED_COUNTER)
      if board is None:
        board = self.board
      for i in range(self.H):
        for j in range(self.W):
          if board[i][j] == 0:
            continue
          if (board[i][j] & common.MASK_CHESS_UNCERTAIN > 0) or (board[i][j] & common.MASK_CHESS_ISCOVERED > 0):
            if board[i][j] & common.MASK_COLOR > 0:
              red += 1
            else:
              black += 1
          else:
            uncovered_counter[board[i][j]] -= 1
      for chess in uncovered_counter:
        if chess & common.MASK_COLOR > 0:
          red_possible_list += [chess] * uncovered_counter[chess]
        else:
          black_possible_list += [chess] * uncovered_counter[chess]
      numred = random.randint(red, len(red_possible_list))
      numblack = random.randint(black, len(black_possible_list))
      finalredlist = random.sample(red_possible_list, numred)
      finalblacklist = random.sample(black_possible_list, numblack)
      #print("board.py --> scan: RED: choose %s from [%s, %s], BLACK: choose %s from [%s, %s]"%(numred, red, len(red_possible_list), numblack, black, len(black_possible_list)))
      return dict(Counter(finalredlist)), dict(Counter(finalblacklist))

   def scan_translate(self, board):
      finalreddict, finalblackdict = self.scan(board)
      reditems, blackitems = list(finalreddict.items()), list(finalblackdict.items())
      f1 = lambda x: (self.translate_chess(x[0], 1), x[1])
      f2 = lambda x: (self.translate_chess(x[0], 2), x[1])
      l1_red = list(map(f1, reditems))
      l2_red = list(map(f2, reditems))
      l1_black = list(map(f1, blackitems))
      l2_black = list(map(f2, blackitems))
      d_red = {'1': dict(l1_red), '2': dict(l2_red)}
      d_black = {'1': dict(l1_black), '2': dict(l2_black)}
      d_dict = {'r': finalreddict, 'b': finalblackdict}
      return d_red, d_black, d_dict

   def evaluate(self, board, dict_red, dict_black, verbose=False):
       '''
       核心! 揭棋局面评分函数
       board: icybee 字符串表示
       dict_red: 红方剩余暗子的可能集合
       dict_black: 黑方剩余暗子
       '''

       def _helper(pos):
          return chr(ord('a') + pos % 16 - 3) + str(12 - pos//16)

       bonus_anche = 130
       discount_factor = 3

       sum_red, sum_black = 0, 0
       average_coveredr, average_coveredb = 0, 0
       averager, averageb = {}, {}
       lenkeyr, lenkeyb = sum(dict_red[key] for key in dict_red), sum(dict_black[key] for key in dict_black)
       
       if lenkeyr == 0:
          average_coveredr = 0
       else:
          for key in dict_red:
              sum_red += self.pst["1"][key] * dict_red[key] // discount_factor
          average_coveredr = sum_red / lenkeyr


       if lenkeyb == 0:
          average_coveredb = 0
       else:
          for key in dict_black:
              sum_black += self.pst["1"][key.swapcase()] * dict_black[key] // discount_factor
          average_coveredb = sum_black / lenkeyb

       for cnt in range(51, 204):
          sum_red = 0
          sum_black = 0
          if lenkeyr == 0:
              averager[cnt] = 0
          else:
              for key in dict_red:
                  sum_red += self.pst[key][cnt] * dict_red[key]
              averager[cnt] = sum_red / lenkeyr
          if lenkeyb == 0:
              averageb[cnt] = 0
          else:
              for key in dict_black:
                  sum_black += self.pst[key.swapcase()][cnt] * dict_black[key]
              averageb[cnt] = sum_black / lenkeyb
       #print(averager, averageb)

       #评分函数
       score_red = 0
       score_black = 0
       for cnt in range(51, 204):
          if board[cnt] in 'DEFGHI':
             if verbose:
                print("\033[31m 红暗: (%s): %s + %s --> %s \033[0m"%(_helper(cnt), score_red, average_coveredr, score_red+average_coveredr))
             score_red += average_coveredr
          elif board[cnt] in 'RNBAKCP':
             if verbose:
                print("\033[31m 红明: (%s): %s + %s --> %s \033[0m"%(_helper(cnt), score_red, self.pst[board[cnt]][cnt], score_red+self.pst[board[cnt]][cnt]))
             score_red += self.pst[board[cnt]][cnt]
          elif board[cnt] == 'U':
             if verbose:
                print("\033[31m 红不: (%s): %s + %s --> %s \033[0m"%(_helper(cnt), score_red, averager[cnt], score_red+averager[cnt]))
             score_red += averager[cnt]
          elif board[cnt] in 'defghi':
             if verbose:
                print(" 黑暗: (%s): %s + %s --> %s"%(_helper(cnt), score_black, average_coveredb, score_black+average_coveredb))  
             score_black += average_coveredb
          elif board[cnt] in 'rnbakcp':
             if verbose:
                print(" 黑明: (%s): %s + %s --> %s"%(_helper(cnt), score_black, self.pst[board[cnt].swapcase()][254-cnt], score_black+self.pst[board[cnt].swapcase()][254-cnt]))  
             score_black += self.pst[board[cnt].swapcase()][254-cnt]
          elif board[cnt] == 'u':
             if verbose:
                print(" 黑不: (%s): %s + %s --> %s"%(_helper(cnt), score_black, averageb[254-cnt], score_black+averageb[254-cnt]))  
             score_black += averageb[254-cnt]

       #暗车开出，扣分!
       if board[195] != 'D':
          score_red -= bonus_anche
       if board[203] != 'D':
          score_red -= bonus_anche
       if board[51] != 'd':
       	  score_black -= bonus_anche
       if board[59] != 'd':
       	  score_black -= bonus_anche

       return score_red, score_black

   def render(self, i):
      #num to ucci representation
      rank, fil = divmod(i - Board.A0, 16)
      return chr(fil + ord('a')) + str(-rank)

   def render_move(self, move):
   	  #move is a tuple
   	  return self.render(move[0]) + self.render(move[1])

   def get_legal_moves_speedup(self, board=None, turn=None, shuaijiang=None, chessdict=None):
      #0.00013s
      if board is None or chessdict is None:
         board = self.board
         chessdict = self.chessdict
      if turn is None:
         turn = self.turn
      if shuaijiang is None:
         shuai = self.shuai
         jiang = self.jiang
      else:
         shuai, jiang = shuaijiang

      def _helper(pos):
         if board[pos[0]][pos[1]] == 0:
            return None
         return (board[pos[0]][pos[1]] & common.MASK_COLOR != 0)

      legal_moves = []
      for pos in chessdict:
        chess = chessdict[pos]
        if (turn and (chess & common.MASK_COLOR == 0)) or ((not turn) and (chess & common.MASK_COLOR == common.MASK_COLOR)):
          continue
        chess_type = chess & common.MASK_CHESS
        if chess_type == 1: #车
          #We are at pos = (pos[0], pos[1])
           for i in range(pos[0]+1, self.H):
               result = _helper((i, pos[1]))
               if result is None:
                  yield (pos[0], pos[1], i, pos[1])
               elif result == turn:
                  break
               else:
                  yield (pos[0], pos[1], i, pos[1])
                  break

           for i in range(pos[0]-1, -1, -1):
               result = _helper((i, pos[1]))
               if result is None:
                  yield  (pos[0], pos[1], i, pos[1])
               elif result == turn:
                  break
               else:
                  yield (pos[0], pos[1], i, pos[1])
                  break

           for j in range(pos[1]+1, self.W):
               result = _helper((pos[0], j))
               if result is None:
                  yield (pos[0], pos[1], pos[0], j)
               elif result == turn:
                  break
               else:
                  yield (pos[0], pos[1], pos[0], j)
                  break

           for j in range(pos[1]-1, -1, -1):
              result = _helper((pos[0], j))
              if result is None:
                  yield (pos[0], pos[1], pos[0], j)
              elif result == turn:
                  break
              else:
                  yield (pos[0], pos[1], pos[0], j)
                  break

        if chess_type == 2: #马
          if pos[0] < self.H - 2 and board[pos[0]+1][pos[1]] == 0:
             if pos[1] > 0 and _helper((pos[0]+2, pos[1]-1)) != turn:
                yield (pos[0], pos[1], pos[0]+2, pos[1]-1)
             if pos[1] < self.W - 1 and _helper((pos[0]+2, pos[1]+1)) != turn:
                yield (pos[0], pos[1], pos[0]+2, pos[1]+1)

          if pos[0] >= 2 and board[pos[0]-1][pos[1]] == 0:
             if pos[1] > 0 and _helper((pos[0]-2, pos[1]-1)) != turn:
                yield (pos[0], pos[1], pos[0]-2, pos[1]-1)
             if pos[1] < self.W - 1 and _helper((pos[0]-2, pos[1]+1)) != turn:
                yield (pos[0], pos[1], pos[0]-2, pos[1]+1)

          if pos[1] < self.W - 2 and board[pos[0]][pos[1]+1] == 0:
             if pos[0] > 0 and _helper((pos[0]-1, pos[1]+2)) != turn:
                yield (pos[0], pos[1], pos[0]-1, pos[1]+2)
             if pos[0] < self.H-1 and _helper((pos[0]+1, pos[1]+2)) != turn:
                yield (pos[0], pos[1], pos[0]+1, pos[1]+2)

          if pos[1] >= 2 and board[pos[0]][pos[1]-1] == 0:
             if pos[0] > 0 and _helper((pos[0]-1, pos[1]-2)) != turn:
                yield (pos[0], pos[1], pos[0]-1, pos[1]-2)
             if pos[0] < self.H-1 and _helper((pos[0]+1, pos[1]-2)) != turn:
                yield (pos[0], pos[1], pos[0]+1, pos[1]-2)

        if chess_type == 3: #相
           if pos[0] < self.H - 2:
              if pos[1] < self.W - 2 and board[pos[0]+1][pos[1]+1] == 0 and _helper((pos[0]+2, pos[1]+2)) != turn:
                 yield (pos[0], pos[1], pos[0]+2, pos[1]+2)
              if pos[1] >= 2 and board[pos[0]+1][pos[1]-1] == 0 and _helper((pos[0]+2, pos[1]-2)) != turn:
                 yield (pos[0], pos[1], pos[0]+2, pos[1]-2)
           if pos[0] >= 2:
              if pos[1] < self.W - 2 and board[pos[0]-1][pos[1]+1] == 0 and _helper((pos[0]-2, pos[1]+2)) != turn:
                 yield (pos[0], pos[1], pos[0]-2, pos[1]+2)
              if pos[1] >= 2 and board[pos[0]-1][pos[1]-1] == 0 and _helper((pos[0]-2, pos[1]-2)) != turn:
                 yield (pos[0], pos[1], pos[0]-2, pos[1]-2)

        if chess_type == 4: #仕
           if board[pos[0]][pos[1]] & common.MASK_CHESS_ISCOVERED == common.MASK_CHESS_ISCOVERED: #暗士
              if turn and (pos[1] == 3 or pos[1] == 5) and _helper((1, 4)) != turn:
                 yield (pos[0], pos[1], 1, 4)
              elif (not turn) and (pos[1] == 3 or pos[1] == 5) and _helper((8, 4)) != turn:
                 yield (pos[0], pos[1], 8, 4)
           else:
              if pos[0] > 0 and pos[1] > 0 and _helper((pos[0]-1, pos[1]-1)) != turn:
                 yield (pos[0], pos[1], pos[0]-1, pos[1]-1)
              if pos[0] > 0 and pos[1] < self.W-1 and _helper((pos[0]-1, pos[1]+1)) != turn:
                 yield(pos[0], pos[1], pos[0]-1, pos[1]+1)
              if pos[0] < self.H-1 and pos[1] > 0 and _helper((pos[0]+1, pos[1]-1)) != turn:
                 yield (pos[0], pos[1], pos[0]+1, pos[1]-1)
              if pos[0] < self.H-1 and pos[1] < self.W-1 and _helper((pos[0]+1, pos[1]+1)) != turn:
                 yield (pos[0], pos[1], pos[0]+1, pos[1]+1)

        if chess_type == 5: #帅
           isdrink = self.drink(board=board, shuaijiang=shuaijiang)
           if turn:
              if pos[0] <= 1 and _helper((pos[0]+1, pos[1])) != turn:
                 yield (pos[0], pos[1], pos[0]+1, pos[1])
              if pos[0] >= 1 and _helper((pos[0]-1, pos[1])) != turn:
                 yield (pos[0], pos[1], pos[0]-1, pos[1])
              if pos[1] >= 4 and _helper((pos[0], pos[1]-1)) != turn:
                 yield (pos[0], pos[1], pos[0], pos[1]-1)
              if pos[1] <= 4 and _helper((pos[0], pos[1]+1)) != turn:
                 yield (pos[0], pos[1], pos[0], pos[1]+1)
              if isdrink:
                 yield (shuai[0], shuai[1], jiang[0], jiang[1])
           else:
              if pos[0] <= 8 and _helper((pos[0]+1, pos[1])) != turn:
                 yield (pos[0], pos[1], pos[0]+1, pos[1])
              if pos[0] >= 8 and _helper((pos[0]-1, pos[1])) != turn:
                 yield (pos[0], pos[1], pos[0]-1, pos[1])
              if pos[1] >= 4 and _helper((pos[0], pos[1]-1)) != turn:
                 yield (pos[0], pos[1], pos[0], pos[1]-1)
              if pos[1] <= 4 and _helper((pos[0], pos[1]+1)) != turn:
                 yield (pos[0], pos[1], pos[0], pos[1]+1)
              if isdrink:
                 yield (jiang[0], jiang[1], shuai[0], shuai[1])

        if chess_type == 6:#炮
            obstacles = 0
            for i in range(pos[0]+1, self.H):
               result = _helper((i, pos[1]))
               if result is None:
                  if obstacles == 0:
                     yield (pos[0], pos[1], i, pos[1])
               elif result == turn:
                  if obstacles == 1:
                     break
                  obstacles += 1
               else:
                  if obstacles == 1:
                     yield (pos[0], pos[1], i, pos[1])
                     break
                  obstacles += 1

            obstacles = 0
            for i in range(pos[0]-1, -1, -1):
               result = _helper((i, pos[1]))
               if result is None:
                  if obstacles == 0:
                     yield (pos[0], pos[1], i, pos[1])
               elif result == turn:
                  if obstacles == 1:
                     break
                  obstacles += 1
               else:
                  if obstacles == 1:
                     yield (pos[0], pos[1], i, pos[1])
                     break
                  obstacles += 1

            obstacles = 0
            for j in range(pos[1]+1, self.W):
               result = _helper((pos[0], j))
               if result is None:
                  if obstacles == 0:
                     yield (pos[0], pos[1], pos[0], j)
               elif result == turn:
                  if obstacles == 1:
                     break
                  obstacles += 1
               else:
                  if obstacles == 1:
                     yield (pos[0], pos[1], pos[0], j)
                     break
                  obstacles += 1

            obstacles = 0
            for j in range(pos[1]-1, -1, -1):
               result = _helper((pos[0], j))
               if result is None:
                  if obstacles == 0:
                     yield (pos[0], pos[1], pos[0], j)
               elif result == turn:
                  if obstacles == 1:
                     break
                  obstacles += 1
               else:
                  if obstacles == 1:
                     yield (pos[0], pos[1], pos[0], j)
                     break
                  obstacles += 1

        if chess_type == 7: #兵/卒
           if turn:
              if pos[0] <= 4:
                 if _helper((pos[0]+1, pos[1])) != turn:
                    yield (pos[0], pos[1], pos[0]+1, pos[1])
              else:
                 if pos[0] <= 8 and _helper((pos[0]+1, pos[1])) != turn:
                    yield (pos[0], pos[1], pos[0]+1, pos[1])
                 if pos[1] < self.W-1 and _helper((pos[0], pos[1]+1)) != turn:
                    yield (pos[0], pos[1], pos[0], pos[1]+1)
                 if pos[1] >= 1 and _helper((pos[0], pos[1]-1)) != turn:
                    yield (pos[0], pos[1], pos[0], pos[1]-1)
           else:
              if pos[0] >= 5:
                 if _helper((pos[0]-1, pos[1])) != turn:
                    yield (pos[0], pos[1], pos[0]-1, pos[1])
              else:
                 if pos[0] >= 1 and _helper((pos[0]-1, pos[1])) != turn:
                    yield (pos[0], pos[1], pos[0]-1, pos[1])
                 if pos[1] < self.W-1 and _helper((pos[0], pos[1]+1)) != turn:
                    yield (pos[0], pos[1], pos[0], pos[1]+1)
                 if pos[1] >= 1 and _helper((pos[0], pos[1]-1)) != turn:
                    yield (pos[0], pos[1], pos[0], pos[1]-1)



