#include "aiboard.h"
   
const int board::AIBoard::_chess_board_size = CHESS_BOARD_SIZE;
const char board::AIBoard::_initial_state[MAX] = 
                    "                "
                    "                "
                    "                "
                    "   defgkgfed    "
                    "   .........    "
                    "   .h.....h.    "
                    "   i.i.i.i.i    "
                    "   .........    "
                    "   P........    "
                    "   I.I.I.I.I    "
                    "   .H.....H.    "
                    "   .........    "
                    "   DEFGKGFED    "
                    "                "
                    "                "
                    "                ";

char board::AIBoard::_dir[91][8] = {{0}};
std::unordered_map<std::string, SCORE> score_bean;
std::unordered_map<std::string, KONGTOUPAO_SCORE> kongtoupao_score_bean;

board::AIBoard::AIBoard() noexcept: num_of_legal_moves(0),
                      version(0),
					  turn(true),
                      _has_initialized(false),
                      _round(0),
                      _score_func(NULL),
                      _kongtoupao_score_func(NULL){
    memset(_state_red, 0, sizeof(_state_red));
    memset(_state_black, 0, sizeof(_state_black));
    strncpy(_state_red, _initial_state, _chess_board_size);
    strncpy(_state_black, _initial_state, _chess_board_size);
    _initialize_dir();
    Scan();
    _has_initialized = true;
}

board::AIBoard::AIBoard(const char another_state[MAX], bool turn, int round) noexcept: version(0) {
    this -> turn = turn;
    _round = round;
    strncpy(_state_red, another_state, _chess_board_size);
    strncpy(_state_black, another_state, _chess_board_size);
    _state_red[_chess_board_size] = '\0';
    _state_black[_chess_board_size] = '\0';
    if(turn){
        rotate(_state_black);
    }else{
        rotate(_state_red);
    }
    _initialize_dir();
    Scan();
    _has_initialized = true;
}

board::AIBoard::AIBoard(const board::AIBoard& another_board){
    this -> turn = another_board.turn;
    this -> _round = another_board._round;
    this -> version = another_board.version;
    strncpy(this -> _state_red, another_board._state_red, _chess_board_size);
    strncpy(this -> _state_black, another_board._state_black, _chess_board_size);
    _initialize_dir();
    Scan();
    _has_initialized = true;
}

void board::AIBoard::Reset() noexcept{
    turn = true;
    _round = 0;
    _score_func = NULL;
    num_of_legal_moves = 0;
    memset(_state_red, 0, sizeof(_state_red));
    memset(_state_black, 0, sizeof(_state_black));
    strncpy(_state_black, _initial_state, _chess_board_size);
    _initialize_dir();
}

void board::AIBoard::_initialize_dir(){
    memset(_dir, 0, sizeof(_dir));
    _dir[(int)'P'][0] = NORTH;
    _dir[(int)'P'][1] = WEST;
    _dir[(int)'P'][2] = EAST;
    
    _dir[(int)'I'][0] = NORTH;
    
    _dir[(int)'N'][0] = NORTH + NORTH + EAST; //N+N+E, E+N+E, E+S+E, S+S+E, S+S+W, W+S+W, W+N+W, N+N+W
    _dir[(int)'N'][1] = EAST + NORTH + EAST;
    _dir[(int)'N'][2] = EAST + SOUTH + EAST;
    _dir[(int)'N'][3] = SOUTH + SOUTH + EAST;
    _dir[(int)'N'][4] = SOUTH + SOUTH + WEST;
    _dir[(int)'N'][5] = WEST + SOUTH + WEST;
    _dir[(int)'N'][6] = WEST + NORTH + WEST;
    _dir[(int)'N'][7] = NORTH + NORTH + WEST;

    _dir[(int)'E'][0] = NORTH + NORTH + EAST; //N+N+E, E+N+E, E+S+E, S+S+E, S+S+W, W+S+W, W+N+W, N+N+W
    _dir[(int)'E'][1] = EAST + NORTH + EAST;
    _dir[(int)'E'][2] = WEST + NORTH + WEST;
    _dir[(int)'E'][3] = NORTH + NORTH + WEST;
    
    _dir[(int)'B'][0] = 2 * NORTH + 2 * EAST;//2 * N + 2 * E, 2 * S + 2 * E, 2 * N + 2 * W, 2 * S + 2 * W
    _dir[(int)'B'][1] = 2 * SOUTH + 2 * EAST;
    _dir[(int)'B'][2] = 2 * NORTH + 2 * WEST;
    _dir[(int)'B'][3] = 2 * SOUTH + 2 * WEST;
    
    _dir[(int)'F'][0] = 2 * NORTH + 2 * EAST;
    _dir[(int)'F'][1] = 2 * NORTH + 2 * WEST;
    
    _dir[(int)'R'][0] = NORTH;
    _dir[(int)'R'][1] = EAST;
    _dir[(int)'R'][2] = SOUTH;
    _dir[(int)'R'][3] = WEST;

    _dir[(int)'D'][0] = NORTH;
    _dir[(int)(int)'D'][1] = EAST;
    _dir[(int)'D'][2] = WEST;

    _dir[(int)'C'][0] = NORTH;
    _dir[(int)'C'][1] = EAST;
    _dir[(int)'C'][2] = SOUTH;
    _dir[(int)'C'][3] = WEST;

    _dir[(int)'H'][0] = NORTH;
    _dir[(int)'H'][1] = EAST;
    _dir[(int)'H'][2] = SOUTH;
    _dir[(int)(int)'H'][3] = WEST;

    _dir[(int)'A'][0] = NORTH + EAST;
    _dir[(int)'A'][1] = SOUTH + EAST;
    _dir[(int)'A'][2] = NORTH + WEST;
    _dir[(int)'A'][3] = SOUTH + WEST;

    _dir[(int)'G'][0] = NORTH + EAST;
    _dir[(int)'G'][1] = NORTH + WEST;

    _dir[(int)'K'][0] = NORTH;
    _dir[(int)'K'][1] = EAST;
    _dir[(int)'K'][2] = SOUTH;
    _dir[(int)'K'][3] = WEST;
}

void board::AIBoard::SetScoreFunction(std::string function_name, int type){
    if(type == 0){
        _score_func = GetWithDefUnordered<std::string, SCORE>(score_bean, function_name, trivial_score_function);
    }else if(type == 1){
        _kongtoupao_score_func = GetWithDefUnordered<std::string, KONGTOUPAO_SCORE>(kongtoupao_score_bean, function_name, trivial_kongtoupao_score_function);
    }
}

std::vector<std::string> board::AIBoard::GetStateString() const{
    std::string tmp_red(_state_red);
    tmp_red = tmp_red.substr(0, board::AIBoard::_chess_board_size);
    std::string tmp_black(_state_black);
    tmp_black = tmp_black.substr(0, board::AIBoard::_chess_board_size);
    return (std::vector<std::string>){tmp_red, tmp_black};
}

bool board::AIBoard::GetRound() const{
    return _round;
}

void board::AIBoard::Move(const std::pair<int, int> start, const std::pair<int, int> end){
    Move(start.first, start.second, end.first, end.second);
}

void board::AIBoard::Move(const std::string ucci){
    //the ucci string is in "a0a1“ format.
    //Please check https://www.xqbase.com/protocol/cchess_ucci.htm
    assert(ucci.size() == 4);
    const int y1 = (int)(ucci[0] - 'a');
    const int x1 = (int)(ucci[1] - '0');
    const int y2 = (int)(ucci[2] - 'a');
    const int x2 = (int)(ucci[3] - '0');
    Move(x1, y1, x2, y2);
}

void board::AIBoard::Move(const char* ucci){
    //the ucci string is in "a0a1“ format.
    //Please check https://www.xqbase.com/protocol/cchess_ucci.htm
    assert(strlen(ucci) == 4);
    const int y1 = (int)(ucci[0] - 'a');
    const int x1 = (int)(ucci[1] - '0');
    const int y2 = (int)(ucci[2] - 'a');
    const int x2 = (int)(ucci[3] - '0');
    Move(x1, y1, x2, y2);
}

void board::AIBoard::Move(const int x1, const int y1, const int x2, const int y2){
    int encode_from = translate_x_y(x1, y1);
    int encode_to = translate_x_y(x2, y2);
    int reverse_encode_from = reverse(encode_from);
    int reverse_encode_to = reverse(encode_to);
    if(turn){
        _state_red[encode_to] = _state_red[encode_from];
        _state_red[encode_from] = '.';
        _state_black[reverse_encode_to] = _state_black[reverse_encode_from];
        _state_black[reverse_encode_from] = '.';
    } else{
        _state_black[encode_to] = _state_black[encode_from];
        _state_black[encode_from] = '.';
        _state_red[reverse_encode_to] = _state_red[reverse_encode_from];
        _state_red[reverse_encode_from] = '.';
    }
    turn = !turn;
    if(turn){
       ++_round;
    }
}

void board::AIBoard::GenMovesWithScore(){
    //To make it more efficient, this implementation is rather dirty
    num_of_legal_moves = 0;
    memset(legal_moves, 0, sizeof(legal_moves));
    const char *_state_pointer = turn?_state_red:_state_black;
    if(!_score_func) {
        SetScoreFunction(std::string(""), 0);
    }
    for(unsigned char i = 51; i <= 203; ++i){
        const char p = _state_pointer[i];
        int intp = (int)p;
        if(!isupper(p) || p == 'U') {
            continue;
        }

        else if(p == 'C' || p == 'H') {
            for(unsigned char cnt = 0; cnt < 8; ++cnt){
                if(_dir[intp][cnt] == 0) {
                    break;
                }
                char cfoot = 0;
                for(unsigned char j = i + _dir[intp][cnt];; j += _dir[intp][cnt]) {
                    const char q = _state_pointer[j];
                    if(q == ' '){
                        break;
                    }
                    if(cfoot == 0){
                        if(q == '.'){
                            legal_moves[num_of_legal_moves] = std::make_tuple(_score_func(this, _state_pointer, i, j), i, j);
                            ++num_of_legal_moves;
                        } else{
                            ++cfoot;
                        }
                    }else{
                        if(islower(q)) {
                            legal_moves[num_of_legal_moves] = std::make_tuple(_score_func(this, _state_pointer, i, j), i, j);
                            ++num_of_legal_moves;
                            break;
                        } else if(isupper(q)) {
                            break;
                        }
                    } //cfoot
                }//j
            }//dir
            continue;
        } //C, H

        else if(p == 'K'){
            for(unsigned char scanpos = i - 16; scanpos > A9; scanpos -= 16){
                if(_state_pointer[scanpos] == 'k'){
                    legal_moves[num_of_legal_moves] = std::make_tuple(_score_func(this, _state_pointer, i, scanpos), i, scanpos);
                    ++num_of_legal_moves;
                } else if(_state_pointer[scanpos] != '.'){
                    break;
                }
            }
        }

        //For other kinds of chesses
        for(unsigned char cnt = 0; cnt < 8; ++cnt){
            if(_dir[intp][cnt] == 0) {
                break;
            }
            const char d = _dir[intp][cnt];
            for(unsigned char j = i + d ;; j += d) {
                const char q = _state_pointer[j];
                if(q == ' ' || isupper(q)){
                    break;
                }
                if(i > 128 && p == 'P' && (d == EAST || d == WEST)) {
                    break;
                }
                else if(p == 'K' && (j < 160 || (j & 15) > 8 || (j & 15) < 6)) {
                    break;
                }
                else if(p == 'G' && j != 183) {
                    break;
                }
                else if(p == 'E' || p == 'N'){
                    int n_diff_x = ((int)(j - i)) & 15;
                    if(n_diff_x == 2 || n_diff_x == 14){
                        if(_state_pointer[i + (n_diff_x == 2?1:-1)] != '.'){
                           break;
                        }
                    } else{
                        if(j > i && _state_pointer[i + 16] != '.') {
                            break;
                        }
                        if(j < i && _state_pointer[i - 16] != '.') {
                            break;
                        }
                    }
                }
                else if((p == 'B' || p == 'F') && _state_pointer[i + d/2] != '.') {
                    break;
                }
                legal_moves[num_of_legal_moves] = std::make_tuple(_score_func(this, _state_pointer, i, j), i, j);
                ++num_of_legal_moves;
                if((p != 'D' && p != 'H' && p != 'C' && p != 'R') || islower(q)){
                    break;
                }
            } //j
        } //dir
    } //for
    std::sort(legal_moves, legal_moves + num_of_legal_moves, GreaterTuple<unsigned short, unsigned char, unsigned char>);
}//GenMovesWithScore()

void board::AIBoard::Scan(){
     const char *_state_pointer = turn?_state_red:_state_black;
     if(!_kongtoupao_score_func){
         SetScoreFunction(std::string(""), 1);
     }
     for(int i = 51; i <= 203; ++i){
         char p = _state_pointer[i];
         if((i >> 4) == 3 && (p == 'd' || p == 'e' || p == 'f' || p == 'g' || p == 'r' || p == 'n' || p == 'c')){
             ++endline;
         }
         else if(p == 'R' || p == 'N' || p == 'B' || p == 'A' || p == 'K' || p == 'C' || p == 'P'){
            score_rough += pst[(int)p][i];
            if(p == 'R'){
               ++che;
            }else if(p == 'P'){
               ++zu;
            }
         }
         else if(p >= 'D' && p <= 'I'){
            ++covered;
         }
         else if(p == 'U'){
            score_rough += average[version][turn?1:0][1][i];
            ++covered;
         }
         else if(p == 'r' || p == 'n' || p == 'b' || p == 'a' || p == 'k' || p == 'c' || p == 'p'){
            score_rough -= pst[((int)p) ^ 32][254 - i];
            if(p == 'r'){
               ++che_opponent;
            }
         }
         else if(p >= 'd' && p <= 'i'){
            ++covered_opponent;
         } 
         else if(p == 'u'){
            score_rough -= average[version][turn?0:1][1][254 - i];
            ++covered_opponent;
         }
         else if(p == 'C' && ((i & 15) == 7)){
            KongTouPao(_state_pointer, i, true);
         }
         else if(p == 'c' && ((i & 15) == 7)){
            KongTouPao(_state_pointer, i, false);
         }
     }
     std::cout << _kongtoupao_score_func;
     _kongtoupao_score_func(this, &kongtoupao_score, &kongtoupao_score_opponent);
}

void board::AIBoard::KongTouPao(const char* _state_pointer, int pos, bool myself){
     char cannon = myself?'C':'c';
     char king = myself?'k':'K';
     if(myself){
          if(kongtoupao != 0){
              return;
          } 
          for(int scanpos = pos - 16; scanpos > A9; scanpos -= 16){
              char p = _state_pointer[scanpos];
              if(p == cannon){
                 continue;
              }
              else if(p != '.'){
                   if(p == king){
                      ++kongtoupao;
                   }else{
                      kongtoupao = 0;
                   }
                   return;
              }else{
                   ++kongtoupao; //Python版本 (musesfish_pvs_20210604_fixed.py)没有这一行, Python BUG!
              } 
          }
     }else{
          if(kongtoupao_opponent != 0){
              return;
          } 
          for(int scanpos = pos + 16; scanpos < I0; scanpos += 16){
              char p = _state_pointer[scanpos];
              if(p == cannon){
                 continue;
              }
              else if(p != '.'){
                   if(p == king){
                      ++kongtoupao_opponent;
                   }else{
                      kongtoupao_opponent = 0;
                   }
                   return;
              }else{
                   ++kongtoupao_opponent; //Python (musesfish_pvs_20210604_fixed.py)版本没有这一行, BUG!
              } 
          }
     } //else
} //KongTouPao

inline short trivial_score_function(void* self, const char* state_pointer, unsigned char src, unsigned char dst){
    board::AIBoard* bp = reinterpret_cast<board::AIBoard*>(self);
    return 0;
}

inline short complicated_score_function(void* self, const char* state_pointer, unsigned char src, unsigned char dst){
	#define LOWER_BOUND -32768
	#define UPPER_BOUND 32767
	constexpr int version = bp -> version;
	constexpr int turn = bp -> turn ? 1 : 0;
	constexpr int che_char = bp -> turn ? (int)'R': (int)'r';
	constexpr int che_opponent_char = che ^ 32;
	constexpr int zu_char = bp -> turn ? (int)'P': (int)'p';
	const char p = state_pointer[src], q = state_pointer[dst];
	const int intp = (int)p, intq = (int)q;
	float score = 0.0;
    board::AIBoard* bp = reinterpret_cast<board::AIBoard*>(self);
	float possible_che = 0.0;
	if(sumall[version][turn]){
	    possible_che = (float)((bp -> covered) * di[version][turn][che_char])/sumall[version][turn];
	}
	float possible_che_opponent = 0.0;
	if(sumall[version][1 - turn]){
		possible_che_opponent = (float)((bp -> covered) * di[version][1 - turn][che_opponent_char])/sumall[version][1 - turn];
	}
	float bing_possibility = 0.0;
	if(sumall[version][turn]){
		bing_possibility = (float)(di[version][turn][zu_char])/sumall[version][turn];
	}
	if(q == 'K'){
		return MATE_UPPER;
	}
	if(p == 'R' || p == 'N' || p == 'B' || p == 'A' || p == 'K' || p == 'C' || p == 'P'){
		score = pst[intp][dst] - pst[intp][src];
		//沉底炮逻辑
		if(p == 'C'){
			if(((src >> 4) != 3) && ((dst >> 4) == 3) && bp -> endline <= 2){
				do{
					if((dst == 51 || dst == 52) && state_pointer[53] == 'f' &&  state_pointer[54] == 'g') { break; }
					if((dst == 59 || dst == 58) && state_pointer[57] == 'f' &&  state_pointer[56] == 'g') { break; }
					score -= (bp -> endline >= 1 ? 30 : 55); 			
				}while(false);
			}
			if(((src >> 4) == 3) && ((dst >> 4) != 3) && bp -> endline <= 2){
				do{
					if((dst == 51 || dst == 52) && state_pointer[53] == 'f' &&  state_pointer[54] == 'g') { break; }
					if((dst == 59 || dst == 58) && state_pointer[57] == 'f' &&  state_pointer[56] == 'g') { break; }
					score += (bp -> endline >= 1 ? 30 : 55); 			
				}while(false);
			}
		}//if(p == 'C')
		
		else if(p == 'R'){
			if(state_pointer[51] != 'd' && state_pointer[51] != 'r' && state_pointer[54] != 'a' && state_pointer[71] != 'a' && (state_pointer[71] == 'p' || state_pointer[87] != 'n')){
				if( (dst & 15) == 6 && (src & 15) != 6 ) { score += 30; }
				else if((src & 15) == 6 && (dst & 15) != 6) { score -= 30; }
			}
			
			if(state_pointer[59] != 'd' && state_pointer[59] != 'r' && state_pointer[56] != 'a' && state_pointer[71] != 'a' && (state_pointer[71] == 'p' || state_pointer[87] != 'n')){
				if( (dst & 15) == 8 && (src & 15) != 8 ) { score += 30; }
				else if((src & 15) == 8 && (dst & 15) != 8) { score -= 30; }
			}
			
			if((src >> 4) == 3 && (dst >> 4) != 3){
				if(bp -> endline <= 1) { score += 30; }
				else if(bp -> score_rough < -150) { score += 40; }
			}
			
			if((src >> 4) != 3 && (dst >> 4) == 3){
				if(bp -> endline <= 1) { score -= 30; }
				else if(bp -> score_rough < -150) { score -= 40; }
			}
		}
	}
	
	else{
		score = average[version][turn][1][dst] - average[version][turn][0][0] + ENCOURAGE;
		if(p == 'D'){
			if(bp -> score_rough < -150){
			    score -= (45 * (possible_che_opponent/2 + bp -> che_opponent));
			}else{
				score -= (30 * (possible_che_opponent/2 + bp -> che_opponent));
			}
		}
	}
	if(score < LOWER_BOUND) return LOWER_BOUND;
	else if(score > UPPER_BOUND) return UPPER_BOUND;
	return (short)round(score);
}

inline void trivial_kongtoupao_score_function(void* self, short* kongtoupao_score, short* kongtoupao_score_opponent){

}

void register_score_functions(){
    score_bean.insert({"trivial_score_function", trivial_score_function});
    kongtoupao_score_bean.insert({"trivial_kongtoupao_score_function", trivial_kongtoupao_score_function});
}


