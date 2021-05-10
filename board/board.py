from . import common
import random
from copy import deepcopy


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

   def is_legal_board(self):
       shuai = None
       jiang = None
       if self.shuai not in common.RED_JIUGONG or self.jiang not in common.BLACK_JIUGONG:
          return False
       for i in range(self.H):
          for j in range(self.W):
              if self.board[i][j] == common.RED_SHUAI and (i > 2 or j < 3 or j > 5):
                  return False
              if self.board[i][j] == common.BLACK_JIANG and (i, j) not in common.BLACK_JIUGONG:
                  return False
       return shuai, jiang


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

   def print_initial_state(self):
       print("Initial state:")
       self.print_board()
       print("Uncovering... Do not tell others!")
       virtual_board = self.uncover_board(self.board, self.mapping, verbose=True)

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

   def uncover_board(self, board, mapping, verbose=True):
       virtual_board = deepcopy(board)
       for element in common.COVERED_POSITIONS:
           if board[element[0]][element[1]] & common.MASK_CHESS_ISCOVERED != 0: #找到暗子
                virtual_board[element[0]][element[1]] = mapping[element]
       if verbose:
           self.print_board(virtual_board)
       return virtual_board
       
   def dict(self):
       return {'board': self.board, 'mapping': self.mapping, 'history': self.history, 'turn': self.turn}

   def __str__(self):
       return str(self.dict())

   def inchessboard(self, x, y):
       return (x >= 0) and (x < self.H) and (y >= 0) and (y < self.W)

   def inchessboard_tuple(self, xy):
       return (xy[0] >= 0) and (xy[0] < self.H) and (xy[1] >= 0) and (xy[1] < self.W)
  
   def drink(self):
       '''
       将帅在同一条直线上时, 中间必须隔着其他子力, 否则饮酒
       '''
       if self.shuai[1] != self.jiang[1]:
           return False
       else:
           for i in range(self.shuai[0]+1, self.jiang[0]):
               if self.board[i][self.shuai[1]] != 0:
                   return False
           return True

   def check_color(self, place, xor=True):
       '''
       检查(x, y)处的子力颜色
       为了加快运算速度，这里并没有对x, y做越界判断!
       xor = Exclusive or
       如果xor == False, 则返回坐标(x, y)处子力的颜色, 红色为True。
       如果xor == True, 则返回坐标(x, y)处子力的颜色与当前行棋的一方是否吻合。
       '''
       red_or_black = None
       if self.board[place[0]][place[1]] == 0:
           return None
       else:
           red_or_black = (self.board[place[0]][place[1]] & common.MASK_COLOR != 0)
           if xor:
                return not (red_or_black ^ self.turn)
           else:
                return red_or_black

   def check_legal(self, src, dst):
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
       if src == dst or not self.inchessboard_tuple(src) or not self.inchessboard_tuple(dst):
           return False
       #检查棋子颜色，不能吃自己的子
       check_src_color = self.check_color(src, xor=True)
       check_dst_color = self.check_color(dst, xor=False)
       if not check_src_color or (check_dst_color == self.turn):
           return False
       chess = self.board[src[0]][src[1]]
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
              if self.board[biematui[0]][biematui[1]] != 0:
                 return False
              else:
                 return True

          if chess_type == 3:#相/象, 判断塞象眼
              if self.board[(src[0] + dst[0])//2][(src[1] + dst[1])//2] != 0: #象眼位置: ((src[0] + dst[0])//2, (src[1] + dst[1])//2)
                 return False  

          elif chess_type == 5:
              if ((self.turn and dst == self.jiang) or (not self.turn and dst == self.shuai)) and self.drink(): #判断饮酒
                 return True
              if (self.turn and dst not in common.RED_JIUGONG) or ((not self.turn) and dst not in common.BLACK_JIUGONG):
                 return False

          sub_vector = common.addsub(dst, src, '-')
          #print("sub_vector: ", sub_vector)
          return (sub_vector in common.DIRECTION_DICT[chess])

       elif chess_type == 1: #车
          if src[0] == dst[0]:
               #检查障碍物
               #Check obstacles in the open interval (src[1], dst[1])
               for potential_obstacle in range(min(src[1], dst[1])+1, max(src[1], dst[1])):
                   if self.board[src[0]][potential_obstacle] != 0:
                        return False
               return True
          elif src[1] == dst[1]:
               for potential_obstacle in range(min(src[0], dst[0])+1, max(src[0], dst[0])):
                   if self.board[potential_obstacle][src[1]] != 0:
                        return False
               return True
          else:
               return False

       elif chess_type == 6: #炮
          if src[0] == dst[0]:
             obstacles = 0
             for potential_obstacle in range(min(src[1], dst[1])+1, max(src[1], dst[1])):
                if self.board[src[0]][potential_obstacle] != 0:
                    if check_dst_color is None:
                        return False
                    obstacles += 1
                if obstacles == 2:
                    return False
             return obstacles == 1 or (check_dst_color is None)
          elif src[1] == dst[1]:
             obstacles = 0
             for potential_obstacle in range(min(src[0], dst[0])+1, max(src[0], dst[0])):
                if self.board[potential_obstacle][src[1]] != 0:
                   if check_dst_color is None:
                        return False
                   obstacles += 1
                if obstacles == 2:
                   return False
             return obstacles == 1 or (check_dst_color is None)
          else:
             return False

   def check_legal_and_jiangjun(self, src, dst):
       if not self.check_legal(src, dst):
           return False, False
       if (self.turn and dst == self.jiang) or ((not self.turn) and dst == self.shuai):
           return True, True
       else:
           return True, False

   def stupid_print_all_legal_moves(self, only_legal=True):
       counter = 0
       # A very stupid way to test check_legal_and_jiangjun
       for x1 in range(self.H):
           for x2 in range(self.H):
               for y1 in range(self.W):
                   for y2 in range(self.W):
                       islegal, isjiangjun = self.check_legal_and_jiangjun((x1, y1), (x2, y2))
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