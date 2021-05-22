from board import board
import time 
import traceback
import elephantfish_pvs

B = board.Board()
for cnt in range(2000000):
    rb, mapping, legal_moves, shuaijiang, chessdict, original_board, original_mapping = B.generate(turn=True, check=False, file=None, verbose=True, uncertainty=True)
    #print(rb)
    #print(original_board)
    #continue
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
