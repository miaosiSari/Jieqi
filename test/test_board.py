from board import board
import time 
import traceback
from multiprocessing import Process


#for cnt in range(1000000000):
#	if cnt % 2 == 0:
#		pass

B = board.Board()
B.print_initial_state()
#B.stupid_print_all_legal_moves()
stupid_way = set(B.stupid_generate_all_legal_moves())
smart_way = set(B.get_legal_moves())
print(len(smart_way), len(stupid_way))
print(smart_way - stupid_way)
print(stupid_way - smart_way)

S = 0.0
for cnt in range(10000000):
    start = time.time()
    board, mapping, shuaijiang, chessdict = B.random_board()
    start1 = time.time()
    print(start1 - start, 'random_board')
    #setA = set(B.get_legal_moves(board=board, shuaijiang=shuaijiang, chessdict=chessdict, turn=True))
    setA = set(B.get_legal_moves_speedup(board=board, shuaijiang=shuaijiang, chessdict=chessdict, turn=True))
    start2 = time.time()
    print(start2 - start1, 'get_legal_moves')
    setB = set(B.stupid_generate_all_legal_moves(board=board, shuaijiang=shuaijiang, turn=True))
    start3 = time.time()
    print(start3 - start2, 'stupid')
    B.print_board(board)
    print(setA - setB)
    print(setB - setA)
print(S)



#x = time.time()
#for cnt in range(10000000):
#    b, m = B.generate_and_check()
#print(time.time() - x)

'''
def random_generate(pid, num):
    x = time.time()
    BOARD = board.Board()
    for cnt in range(10000000//num):
        b, m, shuaijiang = B.random_board()
        if cnt % 10000 == 0:
            print("pid = %s, cnt = %s"%(pid, cnt), time.time() - x)

l = []
for i in range(5):
    p = Process(target=random_generate, args=(i, 5, ))
    p.start()
    l.append(p)

for i in range(5):
    l[i].join()


#board, mapping = B.random_board()

B.print_board(board)
B.uncover_board(board, mapping)
B.copy_board(board=board)
print(B.search_kings())
b = B.initialize_another_board()
b[0][5] = 13
b[9][5] = 5
print(B.search_kings(b))
'''

'''
result = B.check_legal_and_jiangjun((0, 1), (2, 0))
print(result)
result = B.check_legal_and_jiangjun((2, 1), (8, 1))
print(result)
B.stupid_print_all_legal_moves()
board, mapping = B.random_board()
B.copy_board(board=board, mapping=mapping)
B.stupid_print_all_legal_moves()
B.turn = not B.turn
B.stupid_print_all_legal_moves()
board = B.initialize_another_board()
board[0][2] = 2
board[1][2] = 9
board[2][4] = 13
board[7][3] = 5
B.copy_board(board=board)
B.print_board()
B.stupid_print_all_legal_moves()
'''


