from board import board, common
import time 
import traceback
import elephantfish_pvs
from copy import deepcopy

########################################
#Check the symmetry of the scoring table
########################################
CHECK = False

pst = deepcopy(common.pst)
for key in pst:
    if key == "1":
        continue
    for i in range(10):
        for j in range(4):
            k = 8 - j
            x, y, z = 12 - i, 3 + j, 3 + k
            pos1, pos2 = x * 16 + y, x * 16 + z
            try:
                assert pst[key][pos1] == pst[key][pos2]
            except:
                print("check i = %d, j = %d, key=%s, pos1=%s, pos2=%s, value1 = %s, value2 = %s"%(i, j, key, pos1, pos2, pst[key][pos1], pst[key][pos2]))
                assert False
print("CHECKED!")                

########################################
#Finish Checking
########################################

B = board.Board()
for cnt in range(2000000):
    rb, mapping, legal_moves, shuaijiang, chessdict, original_board, original_mapping = B.generate(turn=True, check=False, file=None, verbose=True, uncertainty=True)
    #print(rb)
    #print(original_board)
    #continue

    ########################################
    #Check translate_chess function
    ########################################
    if CHECK:
        for i in range(10):
            for j in range(9):
                print("i = %d, j = %d"%(i, j), original_board[i][j])
                print(B.translate_chess(original_board[i][j], 1))
                print(B.translate_chess(original_board[i][j], 2))
                print("\n\n")
        input("Input anything to continue...")
    ########################################
    #Finish Checking
    ########################################
    rdict, bdict, d = B.scan_translate(original_board)
    print("\033[31m 红: %s \033[0m"%str(rdict))
    print("黑: %s"%str(bdict))
    score_red, score_black = B.evaluate(rb, rdict['1'], bdict['1'], True)
    print("\033[31m 红方得分: %s \033[0m, 黑方得分: %s"%(score_red, score_black))
    print("cnt=%d FINISHED!\n\n"%cnt)
    continue
    p = elephantfish_pvs.Position(rb, 0)
    start = time.time()
    results = p.gen_moves()
    print(time.time() - start)
    results = list(map(B.render_move, p.gen_moves()))
    print(set(results), 'icybee')
    print(set(legal_moves), 'board')
    try:
        assert set(results) == set(legal_moves)
    except:
    	print(set(results), 'elephantfish_pvs')
    	print(set(legal_moves), 'board')
    	print(set(results) - set(legal_moves), 'elephantfish_pvs - board')
    	print(set(legal_moves) - set(results), 'board - elephantfish_pvs')
    	assert False
    #print(results, 'results')
    blackturn = list(B.get_legal_moves_speedup(board=original_board, turn=False, shuaijiang=shuaijiang, chessdict=chessdict))
    blackturn = list(map(B.translate_move, blackturn))
    q = p.rotate()
    results = list(map(B.render_move, q.gen_moves()))
    reverse_results = list(map(B.reverse_move, results))
    try:
        assert set(blackturn) == set(reverse_results)
    except:
    	print(set(reverse_results), 'elephantfish_pvs')
    	print(set(blackturn), 'board')
    	print(set(reverse_results) - set(blackturn), 'elephantfish_pvs - board')
    	print(set(blackturn) - set(reverse_results), 'board - elephantfish_pvs')
    	assert False
#print(legal_moves)
