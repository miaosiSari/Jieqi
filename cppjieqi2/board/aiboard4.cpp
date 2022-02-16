#include "aiboard4.h"


char LUT4[256];

bool board::AIBoard4::has_initialized = false;
const int board::AIBoard4::_chess_board_size = CHESS_BOARD_SIZE;
const char board::AIBoard4::_initial_state[MAX] = 
                    "                "
                    "                "
                    "                "
                    "   defgkgfed    "
                    "   .........    "
                    "   .h.....h.    "
                    "   i.i.i.i.i    "
                    "   .........    "
                    "   .........    "
                    "   I.I.I.I.I    "
                    "   .H.....H.    "
                    "   .........    "
                    "   DEFGKGFED    "
                    "                "
                    "                "
                    "                ";



const std::unordered_map<std::string, std::string> board::AIBoard4::_uni_pieces = {
    {".", "．"},
    {"R", "\033[31m俥\033[0m"},
    {"N", "\033[31m傌\033[0m"},
    {"B", "\033[31m相\033[0m"},
    {"A", "\033[31m仕\033[0m"},
    {"K", "\033[31m帅\033[0m"},
    {"P", "\033[31m兵\033[0m"},
    {"C", "\033[31m炮\033[0m"},
    {"D", "\033[31m暗\033[0m"},
    {"E", "\033[31m暗\033[0m"},
    {"F", "\033[31m暗\033[0m"},
    {"G", "\033[31m暗\033[0m"},
    {"H", "\033[31m暗\033[0m"},
    {"I", "\033[31m暗\033[0m"},
    {"U", "\033[31m不\033[0m"},
    {"r", "车"},
    {"n", "马"},
    {"b", "象"},
    {"a", "士"},
    {"k", "将"},
    {"p", "卒"},
    {"c", "炮"},
    {"d", "暗"},
    {"e", "暗"},
    {"f", "暗"},
    {"g", "暗"},
    {"h", "暗"},
    {"i", "暗"},
    {"u", "不"},
};

char board::AIBoard4::_dir[91][8] = {{0}};
std::unordered_map<std::string, SCORE4> score_bean4;
std::unordered_map<std::string, KONGTOUPAO_SCORE4> kongtoupao_score_bean4;
std::unordered_map<std::string, THINKER4> thinker_bean4;

board::AIBoard4::AIBoard4(const char another_state[MAX], bool turn, int round, const unsigned char di[VERSION_MAX][2][123], short score, tp* tptable, std::unordered_map<std::string, bool>* hist) noexcept: 
                                                                                                                            ply(0),
                                                                                                                            version(0), 
                                                                                                                            round(round), 
                                                                                                                            turn(turn), 
                                                                                                                            original_turn(turn),
                                                                                                                            original_depth(0),
                                                                                                                            zobrist_hash(0), 
                                                                                                                            discount_factor(1.5),
                                                                                                                            score(score),
                                                                                                                            tptable(tptable),
                                                                                                                            hist(hist),
                                                                                                                            _kaijuku_file("../kaijuku"),
                                                                                                                            _myname("AI4"),
                                                                                                                            _has_initialized(false),
                                                                                                                            _score_func(NULL),
                                                                                                                            _kongtoupao_score_func(NULL){
    SetScoreFunction("complicated_score_function4", 0);
    SetScoreFunction("complicated_kongtoupao_score_function4", 1);
    memset(state_red, 0, sizeof(state_red));
    memset(state_black, 0, sizeof(state_black));
    strncpy(state_red, another_state, _chess_board_size);
    strncpy(state_black, another_state, _chess_board_size);
    if(turn){
        rotate(state_black);
    }else{
        rotate(state_red);
    }
    copy_pst(this -> pst, ::pstglobal[3]);
    CopyData(di);
    _initialize_dir();
    _initialize_zobrist();
    zobrist_cache.insert((zobrist_hash << 1)|original_turn);
    Scan();
    register_score_functions4();
    if(round == 0){
        read_kaijuku(_kaijuku_file, kaijuku);
    }
    if(!has_initialized){
        INITIALIZELUT4
    }
    has_initialized = true;
    _has_initialized = true;
}

void board::AIBoard4::_initialize_dir(){
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
    _dir[(int)'D'][1] = EAST;
    _dir[(int)'D'][2] = WEST;

    _dir[(int)'C'][0] = NORTH;
    _dir[(int)'C'][1] = EAST;
    _dir[(int)'C'][2] = SOUTH;
    _dir[(int)'C'][3] = WEST;

    _dir[(int)'H'][0] = NORTH;
    _dir[(int)'H'][1] = EAST;
    _dir[(int)'H'][2] = SOUTH;
    _dir[(int)'H'][3] = WEST;

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

void board::AIBoard4::SetScoreFunction(std::string function_name, int type){
    if(type == 0){
        _score_func = GetWithDefUnordered<std::string, SCORE4>(score_bean4, function_name, complicated_score_function4);
    }else if(type == 1){
        _kongtoupao_score_func = GetWithDefUnordered<std::string, KONGTOUPAO_SCORE4>(kongtoupao_score_bean4, function_name, complicated_kongtoupao_score_function4);
    }else if(type == 2){
        _thinker_func = GetWithDefUnordered<std::string, THINKER4>(thinker_bean4, function_name, thinker4);
    }
}

std::string board::AIBoard4::SearchScoreFunction(int type){
    if(type == 0){
        return ::SearchScoreFunction4(reinterpret_cast<void*>(_score_func), type);
    }else if(type == 1){
        return ::SearchScoreFunction4(reinterpret_cast<void*>(_kongtoupao_score_func), type);
    }else if(type == 2){
        return ::SearchScoreFunction4(reinterpret_cast<void*>(_thinker_func), type);
    }
    return "";
}

bool board::AIBoard4::Move(const unsigned char encode_from, const unsigned char encode_to, short score_step){
    const unsigned char reverse_encode_from = reverse(encode_from);
    const unsigned char reverse_encode_to = reverse(encode_to);
    score_cache.push({che, che_opponent, zu, zu_opponent,score, zobrist_hash});
    if(turn){
        cache.push({encode_from, encode_to, state_red[encode_to]});
        switch(state_red[encode_to]){
            case 'r': --che_opponent; break;
            case 'p': --zu_opponent; break;
        }
        zobrist_hash ^= zobrist[(int)state_red[encode_to]][encode_to];
        zobrist_hash ^= zobrist[(int)state_red[encode_from]][encode_from];
        if(state_red[encode_from] >= 'D' && state_red[encode_from] <= 'I'){
            state_red[encode_to] = 'U';
            state_black[reverse_encode_to] = 'u';
        }else{
            state_red[encode_to] = state_red[encode_from];
            state_black[reverse_encode_to] = state_black[reverse_encode_from];
        }
        state_red[encode_from] = '.';
        state_black[reverse_encode_from] = '.';
        zobrist_hash ^= zobrist[(int)state_red[encode_to]][encode_to];
    } else{
        cache.push({encode_from, encode_to, state_black[encode_to]});
        switch(state_black[encode_to]){
            case 'r': --che_opponent; break;
            case 'p': --zu_opponent; break;
        }
        zobrist_hash ^= zobrist[(int)state_red[reverse_encode_to]][reverse_encode_to];
        zobrist_hash ^= zobrist[(int)state_red[reverse_encode_from]][reverse_encode_from];
        if(state_black[encode_from] >= 'D' && state_black[encode_from] <= 'I'){
            state_black[encode_to] = 'U';
            state_red[reverse_encode_to] = 'u';
        }else{
            state_black[encode_to] = state_black[encode_from];
            state_red[reverse_encode_to] = state_red[reverse_encode_from];
        }
        state_black[encode_from] = '.';
        state_red[reverse_encode_from] = '.';
        zobrist_hash ^= zobrist[(int)state_red[reverse_encode_to]][reverse_encode_to];
    }
    turn = !turn;
    std::swap(che, che_opponent);
    std::swap(zu, zu_opponent);
    std::swap(covered, covered_opponent);
    score = -(score + score_step);
    if(turn){
       ++round;
    }
    auto zobrist_turn = (zobrist_hash << 1)|turn;
    bool retval = (zobrist_cache.find(zobrist_turn) == zobrist_cache.end());
    if(retval){
        zobrist_cache.insert(zobrist_turn);
    }
    lastinserts.push_back(retval);
    ++ply;
    return retval;
}

bool board::AIBoard4::NULLMove(){
    score_cache.push({che, che_opponent, zu, zu_opponent, score, zobrist_hash});
    std::swap(che, che_opponent);
    std::swap(zu, zu_opponent);
    std::swap(covered, covered_opponent);
    turn = !turn;
    auto zobrist_turn = (zobrist_hash << 1)|turn;
    bool retval = (zobrist_cache.find(zobrist_turn) == zobrist_cache.end());
    score = -score;
    if(retval){
        zobrist_cache.insert(zobrist_turn);
    }
    lastinserts.push_back(retval);
    ++ply;
    return retval;
}

void board::AIBoard4::UndoMove(int type){
    gameinfo g = score_cache.top();
    che = g.che;
    che_opponent = g.che_opponent;
    zu = g.zu;
    zu_opponent = g.zu_opponent;
    score = g.score;
    std::swap(covered, covered_opponent);
    score_cache.pop();
    if(!lastinserts.empty() && lastinserts.back()){
        zobrist_cache.erase((zobrist_hash<<1)|turn);
    }
    lastinserts.pop_back();
    zobrist_hash = g.zobrist_hash;
    if(type == 1){//非空移动
        const std::tuple<unsigned char, unsigned char, char> from_to_eat = cache.top();
        cache.pop();
        const unsigned char encode_from = std::get<0>(from_to_eat);
        const unsigned char encode_to = std::get<1>(from_to_eat);
        const char eat = std::get<2>(from_to_eat);
        if(turn){
            --round;
        }
        turn = !turn;
        const unsigned char reverse_encode_from = reverse(encode_from);
        const unsigned char reverse_encode_to = reverse(encode_to);
        if(turn){
            if(state_red[encode_to] == 'U'){
                state_red[encode_from] = LUT4[encode_from];
                state_red[encode_to] = eat;
                state_black[reverse_encode_from] = swapcase(LUT4[encode_from]);
                state_black[reverse_encode_to] = swapcase(eat);
            }else{
                state_red[encode_from] = state_red[encode_to];
                state_red[encode_to] = eat;
                state_black[reverse_encode_from] = state_black[reverse_encode_to];
                state_black[reverse_encode_to] = swapcase(eat);
            }
        }else{
            if(state_black[encode_to] == 'U'){
                state_black[encode_from] = LUT4[encode_from];
                state_black[encode_to] = eat;
                state_red[reverse_encode_from] = swapcase(LUT4[encode_from]);
                state_red[reverse_encode_to] = swapcase(eat);
            }else{
                state_black[encode_from] = state_black[encode_to];
                state_black[encode_to] = eat;
                state_red[reverse_encode_from] = state_red[reverse_encode_to];
                state_red[reverse_encode_to] = swapcase(eat);
            }
        }
    }else if(type == 0){
        turn = !turn;
    }
    --ply;
}

void board::AIBoard4::Scan(){
    all = 0;
    che = 0;
    che_opponent = 0;
    zu = 0;
    zu_opponent = 0;
    covered = 0;
    covered_opponent = 0;
    kongtoupao = 0;
    kongtoupao_opponent = 0;
    kongtoupao_score = 0;
    kongtoupao_score_opponent=0;
    const char *_state_pointer = turn?state_red:state_black;
    for(int i = 51; i <= 203; ++i){
        if((i & 15) < 3 || (i & 15) > 11) { continue; }
        const char p = _state_pointer[i];
        if(p != '.'){
            ++all;
        }
        if(p == 'R' || p == 'N' || p == 'B' || p == 'A' || p == 'K' || p == 'C' || p == 'P'){
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
            ++covered;
        }
        else if(p == 'r' || p == 'n' || p == 'b' || p == 'a' || p == 'k' || p == 'c' || p == 'p'){
            if(p == 'r'){
               ++che_opponent;
            }else if(p == 'p'){
               ++zu_opponent;
            }
        }
        else if(p >= 'd' && p <= 'i'){
            ++covered_opponent;
        }  
        else if(p == 'u'){
            ++covered_opponent;
        }
        if(p == 'C' && ((i & 15) == 7)){
            KongTouPao(_state_pointer, i, true);
        }
        if(p == 'c' && ((i & 15) == 7)){
            KongTouPao(_state_pointer, i, false);
        }
    }
    _kongtoupao_score_func(this, &kongtoupao_score, &kongtoupao_score_opponent);
}

short board::AIBoard4::ScanProtectors(){
    const char *_state_pointer = turn?state_red:state_black;
    protector = 4;
    protector_oppo = 4;
    if(_state_pointer[195] != 'D'){
        --protector;
    }
    if(_state_pointer[203] != 'D'){
        --protector;
    }
    if(_state_pointer[198] != 'G' && _state_pointer[198] != 'A'){
        --protector;
    }
    if(_state_pointer[200] != 'G' && _state_pointer[200] != 'A'){
        --protector;
    }
    if(_state_pointer[183] == 'A'){
        ++protector;
    }
    if(_state_pointer[51] != 'd'){
        --protector_oppo;
    }
    if(_state_pointer[59] != 'd'){
        --protector_oppo;
    }
    if(_state_pointer[54] != 'g' && _state_pointer[54] != 'a'){
        --protector_oppo;
    }
    if(_state_pointer[56] != 'g' && _state_pointer[56] != 'a'){
        --protector_oppo;
    }
    if(_state_pointer[71] == 'a'){
        ++protector_oppo;
    }
    unsigned char myself = protector >= 4 ? 0 : 4 - protector;
    unsigned char oppo = protector_oppo >= 4 ? 0 : 4 - protector_oppo;
    unsigned char penaltyarea[20] = {197, 198, 199, 200, 201, 181, 182, 183, 184, 185, 165, 166, 167, 168, 169, 149, 150, 151, 152, 153};
    unsigned char penaltyareaoppo[20] = {53, 54, 55, 56, 57, 69, 70, 71, 72, 73, 85, 86, 87, 88, 89, 101, 102, 103, 104, 105};
    short bonus = 0;
    const short delta = 10;
    for(int i = 0; i < 20; ++i){
        if(_state_pointer[penaltyareaoppo[i]] == 'P'){
            short tmp = (delta * (4 - L1[penaltyareaoppo[i]][71]) * oppo);
            if(penaltyareaoppo[i] <= 57){
                //底兵
                tmp /= 2;
            }
            assert(tmp >= 0);
            bonus += tmp;
        }
        if(_state_pointer[penaltyarea[i]] == 'p'){
            short tmp = (delta * (4 - L1[penaltyarea[i]][183]) * myself);
            if(penaltyarea[i] >= 197){
                tmp /= 2;
            }
            assert(tmp >= 0);
            bonus -= tmp;
        }
    }
    return bonus;
}

void board::AIBoard4::KongTouPao(const char* _state_pointer, int pos, bool myself){
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
        //kongtoupao = 0; //注释这行, 即使出将也不能化解空头炮
        return;
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
        //kongtoupao_opponent = 0; //注释这行, 即使出将也不能化解空头炮
        return;
    } //else
} //KongTouPao

template<bool needscore>
bool board::AIBoard4::GenMovesWithScore(scoretuple legal_moves[MAX_POSSIBLE_MOVES], int& num_of_legal_moves, std::pair<unsigned char, unsigned char>* killer, short& killer_score, unsigned char& mate_src, unsigned char& mate_dst, bool& killer_is_alive){
    num_of_legal_moves = 0;
    killer_score = 0;
    killer_is_alive = false;
    const char *_state_pointer = turn?state_red:state_black;
    for(unsigned char i = 51; i <= 203; ++i){
        if((i & 15) < 3 || (i & 15) > 11) { continue; }
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
                            legal_moves[num_of_legal_moves] = MAKETUPLE(0, i, j);
                            ++num_of_legal_moves;
                        } else{
                            ++cfoot;
                        }
                    }else{
                        if(islower(q)) {
                            legal_moves[num_of_legal_moves] = MAKETUPLE(0, i, j);
                            ++num_of_legal_moves;
                            if(q == 'k') { 
                                mate_src = i, mate_dst = j;
                                return true;
                            }
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
                    legal_moves[num_of_legal_moves] = MAKETUPLE(0, i, scanpos);
                    ++num_of_legal_moves;
                    mate_src = i, mate_dst = scanpos;
                    return true;
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
            if(i > 128 && p == 'P' && (d == EAST || d == WEST)) {
                break;
            }
            for(unsigned char j = i + d ;; j += d) {
                const char q = _state_pointer[j];
                if(q == ' ' || isupper(q)){
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
                legal_moves[num_of_legal_moves] = MAKETUPLE(0, i, j);
                ++num_of_legal_moves;
                if(q == 'k'){
                    mate_src = i, mate_dst = j;
                    return true;
                }
                if((p != 'D' && p != 'R') || islower(q)){
                    break;
                }
            } //j
        } //dir
    } //for
    if(needscore){
        for(int i = 0; i < num_of_legal_moves; ++i){
            unsigned char src = SRC(legal_moves[i]), dst = DST(legal_moves[i]);
            legal_moves[i].score = _score_func(this, _state_pointer, src, dst);
            if(killer && src == killer -> first && dst == killer -> second){
                killer_is_alive = true;
                killer_score = legal_moves[i].score;
            }
        }
        std::sort(legal_moves, legal_moves + num_of_legal_moves, [](const scoretuple& i, const scoretuple& j) -> bool {
            if(SCORE(i) > SCORE(j)) return true;
            if(SCORE(i) == SCORE(j) && SRC(i) > SRC(j)) return true;
            if(SCORE(i) == SCORE(j) && SRC(i) == SRC(j) && DST(i) > DST(j)) return true; 
            return false;
        });
    }
    return false;
}//GenMovesWithScore()


template<bool doublereverse>
bool board::AIBoard4::Mate(){
    if(doublereverse)
        turn = !turn;
    scoretuple legal_moves_tmp[MAX_POSSIBLE_MOVES];
    int num_of_legal_moves_tmp = 0;
    short killer_score = 0;
    unsigned char mate_src = 0, mate_dst = 0;
    bool killer_is_alive = false;
    bool mate = GenMovesWithScore<false>(legal_moves_tmp, num_of_legal_moves_tmp, NULL, killer_score, mate_src, mate_dst, killer_is_alive);
    if(doublereverse)
        turn = !turn;
    return mate;
}

bool board::AIBoard4::Executed(bool* oppo_mate, scoretuple legal_moves_tmp[], int num_of_legal_moves_tmp){
    //判断是否被对方将死
    //注意, 这个函数应该在mate_by_oppo为true的时候调用。如mate_by_oppo==false, 根本没将军, 调用没意义
    *oppo_mate = Mate<true>();
    if(!*oppo_mate){
        return false;
    }
    bool saved = false; //还有救?
    for(int i = 0; i < num_of_legal_moves_tmp; ++i){
        scoretuple tuple = legal_moves_tmp[i];
        Move(SRC(tuple), DST(tuple), SCORE(tuple));
        if(!Mate<false>()){
            saved = true;
        }
        UndoMove(1);
        if(saved){return false;}
    }
    return true;
}

#if DEBUG
bool board::AIBoard4::ExecutedDebugger(bool *oppo_mate){
    scoretuple legal_moves_tmp[MAX_POSSIBLE_MOVES];
    int num_of_legal_moves_tmp = 0;
    short killer_score = 0;
    unsigned char mate_src = 0, mate_dst = 0;
    bool killer_is_alive = false;
    if(GenMovesWithScore<false>(legal_moves_tmp, num_of_legal_moves_tmp, NULL, killer_score, mate_src, mate_dst, killer_is_alive)){
        return false;
    }
    return Executed(oppo_mate, legal_moves_tmp, num_of_legal_moves_tmp);
}
#endif

bool board::AIBoard4::Ismate_After_Move(unsigned char src, unsigned char dst){
    //判断本方在走完某步棋后是否对对方形成将军
    //return true: 形成将军
    //return false: 不形成将军
    //a9     a5(k)
    //a8(R)
    //a8a9后R位于a9形成将军return true
    //a8a7后不形成将军return false
    short score = 0;
    unsigned char mate_src, mate_dst;
    scoretuple legal_moves_tmp[MAX_POSSIBLE_MOVES];
    int num_of_legal_moves_tmp = 0;
    bool killer_is_alive = false;
    Move(src, dst, 0);
    turn = !turn;
    bool mate = GenMovesWithScore<false>(legal_moves_tmp, num_of_legal_moves_tmp, NULL, score, mate_src, mate_dst, killer_is_alive);
    turn = !turn;
    UndoMove(1);
    return mate;
}

void board::AIBoard4::CalcVersion(const int ver){
    short numr = 0, numb = 0; 
    numr += aidi[ver][1][INTR]; numr += aidi[ver][1][INTN];  numr += aidi[ver][1][INTB];  numr += aidi[ver][1][INTA];  numr += aidi[ver][1][INTC]; numr += aidi[ver][1][INTP]; 
    numb += aidi[ver][0][INTr]; numb += aidi[ver][0][INTn];  numb += aidi[ver][0][INTb];  numb += aidi[ver][0][INTa];  numb += aidi[ver][0][INTc]; numb += aidi[ver][0][INTp]; 
    aisumall[ver][1] = numr; aisumall[ver][0] = numb;
    if(numr > 0){
        double sumr = 0.0;
        for(const char c : MINGZI){
            sumr += pst[(int)c][0] * aidi[ver][1][(int)c] / discount_factor;
        }
        aiaverage[ver][1][0][0] = ::round(sumr / numr);
        for(int i = 51; i <= 203; ++i){
            sumr = 0.0;
            for(const char c : MINGZI){
                sumr += pst[(int)c][i] * aidi[ver][1][(int)c];
            }
            aiaverage[ver][1][1][i] = ::round(sumr / numr);
        }
    }
    if(numb > 0){
        double sumb = 0.0;
        for(const char c : MINGZI){
            sumb += pst[(int)c][0] * aidi[ver][0][((int)c)^32] / discount_factor;
        }
        aiaverage[ver][0][0][0] = ::round(sumb / numb);
        for(int i = 51; i <= 203; ++i){
            sumb = 0.0;
            for(const char c : MINGZI){
                sumb += pst[(int)c][i] * aidi[ver][0][((int)c)^32];
            }
            aiaverage[ver][0][1][i] = ::round(sumb / numb);
        }
    }
}

void board::AIBoard4::CopyData(const unsigned char di[VERSION_MAX][2][123]){
    memset(aiaverage, 0, sizeof(aiaverage));
    memset(aisumall, 0, sizeof(aisumall));
    memset(aidi, 0, sizeof(aidi));
    memcpy(aidi, di, sizeof(aidi));
    CalcVersion(0);
}

std::string board::AIBoard4::Think(int maxdepth){
    SetScoreFunction("thinker4", 2);
    return _thinker_func(this, maxdepth);
}


void board::AIBoard4::PrintPos(bool turn) const{
    printf("version = %d, turn = %d, this -> turn = %d, round = %d\n", version, turn, this -> turn, round);
    if(turn){
        printf("红方视角:\n");
    }else{
        printf("黑方视角:\n");
    }
    std::cout << std::endl << std::endl;
    for(int x = 3; x <= 12; ++x){
        std::cout << translate_x(x) << " ";
        for(int y = 3; y <= 11; ++y){
            std::cout << _getstringxy(x, y, turn);
        }
        std::cout << std::endl;
    }
    std::cout << "  ａｂｃｄｅｆｇｈｉ\n\n";
}

std::string board::AIBoard4::DebugPrintPos() const{
    std::string ret;
    ret += "che=" + std::to_string(che) + ", che_opponent=" + std::to_string(che_opponent) + ", zu=" + std::to_string(zu) + ", zu_opponent=" + std::to_string(zu_opponent) + "\n";
    if(turn){
        ret += "RED\n";   
    }else{
        ret += "BLACK\n";
    }
    for(int x = 3; x <= 12; ++x){
        ret += std::to_string(translate_x(x));
        ret += ' ';
        for(int y = 3; y <= 11; ++y){
            const char c = turn?state_red[encode(x, y)]:state_black[encode(x, y)];
            ret += c;
        }
        ret += '\n';
    }
    ret += "  abcdefghi\n";
    return ret;
}

void board::AIBoard4::print_raw_board(const char* board, const char* hint){
    std::cout << hint << std::endl;
    int row = 9;
    for(int i = 51; i < 203; i += 16){
        std::cout << row << " ";
        for(int j = i; j <= i+8; ++j){
            std::cout << board[j];
        }
        --row;
        std::cout << "\n";
    }
    std::cout << "***********\n  abcdefghi\n";
}


template<typename... Args>
void board::AIBoard4::print_raw_board(const char* board, const char* hint, Args... args){
    print_raw_board(board, hint);
    print_raw_board(args...);
}

inline short complicated_score_function4(board::AIBoard4* self, const char* state_pointer, unsigned char src, unsigned char dst){
    #define LOWER_BOUND -32768
    #define UPPER_BOUND 32767
    board::AIBoard4* bp = reinterpret_cast<board::AIBoard4*>(self);
    int version = bp -> version;
    int turn = bp -> turn ? 1 : 0;
    int che_char = bp -> turn ? (int)'R': (int)'r';
    int che_opponent_char = che_char ^ 32;
    int zu_char = bp -> turn ? (int)'P': (int)'p';
    char p = state_pointer[src], q = bp -> swapcase(state_pointer[dst]);
    int intp = (int)p, intq = (int)q;
    float score = 0.0;
    float possible_che = 0.0;
    if(bp -> aisumall[version][turn]){
        possible_che = (float)((bp -> covered) * bp -> aidi[version][turn][che_char])/bp -> aisumall[version][turn];
    }
    float zu_possibility = 0.0;
    if(bp -> aisumall[version][turn]){
        zu_possibility = (float)(bp -> aidi[version][turn][zu_char])/bp -> aisumall[version][turn];
    }
    float possible_che_opponent = 0.0;
    if(bp -> aisumall[version][1 - turn]){
        possible_che_opponent = (float)((bp -> covered_opponent) * bp -> aidi[version][1 - turn][che_opponent_char])/bp -> aisumall[version][1 - turn];
    }
    if(q == 'K'){
        return MATE_UPPER;
    }
    if(p == 'R' || p == 'N' || p == 'B' || p == 'A' || p == 'K' || p == 'C' || p == 'P'){
        score =  bp -> pst[intp][dst] -  bp -> pst[intp][src];
    }
    
    else{
        score = bp -> aiaverage[version][turn][1][dst] - bp -> aiaverage[version][turn][0][0];
        float che_zu_possibility = bp -> aisumall[version][turn] > 0 ? (bp -> aidi[version][turn][che_char] + bp -> aidi[version][turn][zu_char])/bp -> aisumall[version][turn] : 0.0;
        float scorediff = bp -> aiaverage[version][turn][0][0] * che_zu_possibility;
        if(p == 'D'){
            score -= (30 * (possible_che_opponent/2 + bp -> che_opponent - (q == 'R')));
        }else if(p == 'E'){
            auto judge_left = [&state_pointer]() -> bool {
                return state_pointer[147] == 'r' || state_pointer[148] == 'r';
            };

            auto judge_middle = [&state_pointer]() -> bool {
                return state_pointer[150] == 'r' || state_pointer[151] == 'r' || state_pointer[152] == 'r';
            };

            auto judge_right = [&state_pointer]() -> bool {
                return state_pointer[154] == 'r' || state_pointer[155] == 'r';
            };

            if(src == 196 && dst == 165 && state_pointer[149] == 'I'){
                if(judge_left() || judge_middle()){
                    score += scorediff;
                }
                else{
                    for(int scanpos = 133; scanpos > A9; scanpos -= 16){
                        if(state_pointer[scanpos] == 'r'){
                            score += scorediff;
                        }else if(state_pointer[scanpos] != '.'){
                            break;
                        }                       
                    }
                }
            }   

            if(src == 202 && dst == 169 && state_pointer[153] == 'I'){
                if(judge_right() || judge_middle()){
                    score += scorediff;
                }
                else{
                    for(int scanpos = 137; scanpos > A9; scanpos -= 16){
                        if(state_pointer[scanpos] == 'r'){
                            score += scorediff;
                        }else if(state_pointer[scanpos] != '.'){
                            break;
                        } 
                    }
                }
            }
        
        
        }else if(p == 'F'){
            if((src == 197 || src == 201) && dst == 167 && state_pointer[151] == 'I'){
                bool findche = false;
                for(int scanpos = 135; scanpos > A9; scanpos -= 16){
                    if(state_pointer[scanpos] == 'r'){
                        score += scorediff;
                        findche = true;
                        break;
                    }else if(state_pointer[scanpos] != '.'){
                        break;
                    } 
                }//for
                if(!findche){
                    for(int scanpos = 135; scanpos > 130; --scanpos){
                        if(state_pointer[scanpos] == 'r'){
                            score += scorediff;
                            findche = true;
                            break;
                        }else if(state_pointer[scanpos] != '.'){
                            break;
                        }
                    }
                }
                if(!findche){
                    for(int scanpos = 135; scanpos < 140; ++scanpos){
                        if(state_pointer[scanpos] == 'r'){
                            score += scorediff;
                            break;
                        }else if(state_pointer[scanpos] != '.'){
                            break;
                        }
                    }
                }
            }//if((src == 197 || src == 201) && dst == 167 && state_pointer[151] == 'I')
            
        
        }else if(p == 'H'){

            if(src == 164 && dst == 68 && state_pointer[52] == 'e'){
                short bonus = ::round(zu_possibility * bp -> aiaverage[version][turn][0][0]/2);
                score += bonus/2;
                if(state_pointer[53] != '.'){ //python BUG
                    score += bonus;
                }
                if(state_pointer[51] == 'd'){
                    score += bonus;
                }
            }
            
            if(src == 170 && dst == 74 && state_pointer[58] == 'e'){
                short bonus = ::round(zu_possibility * bp -> aiaverage[version][turn][0][0]/2);
                score += bonus/2;
                if(state_pointer[57] != '.'){ //python BUG
                    score += bonus;
                }
                if(state_pointer[59] == 'd'){
                    score += bonus;
                }
            }

            if((src == 164 && dst == 52 && state_pointer[52] == 'e' && (state_pointer[51] == 'd' || state_pointer[51] == 'r')) || \
                    (src == 170 && dst == 58 && state_pointer[58] == 'e' && (state_pointer[59] == 'd' || state_pointer[59] == 'r'))){
                        if((bp -> che) < (bp -> che_opponent) || bp -> che == 0) {score -= 150; }
                    }
         
        }else if(p == 'I' && state_pointer[src - 32] != 'r' && state_pointer[src - 32] != 'p'){
            if(state_pointer[src - 48] == 'i') {
                score += 25; //制约对方兵
            }
            else{
                score += 20;
            }
        }//else if I
    }//else
        
    if(q >= 'A' && q <= 'Z'){
        int k = 254 - dst;
        if(q == 'R' || q == 'N' || q == 'B' || q == 'A' || q == 'C' || q == 'P'){
            score +=  bp -> pst[intq][k];
        }
        else{
            if(q != 'U'){
                score += bp -> aiaverage[version][1 - turn][0][0];            
            }else{
                score += bp -> aiaverage[version][1 - turn][1][k];
            }
            if(q == 'D'){
                score +=  30 * (possible_che/2 + bp -> che);               
            }//if(q == 'D')
        }           
    }//capture
    if(score < LOWER_BOUND) return LOWER_BOUND;
    else if(score > UPPER_BOUND) return UPPER_BOUND;
    return (short)round(score);
}

inline void complicated_kongtoupao_score_function4(board::AIBoard4* bp, short* kongtoupao_score, short* kongtoupao_score_opponent){
    if(bp -> kongtoupao > bp -> kongtoupao_opponent){
        if((bp -> che >= bp -> che_opponent && bp -> che > 0) || bp -> kongtoupao >= 3)
            *kongtoupao_score = 180;
        else
            *kongtoupao_score = 140;
    }

    else if(bp -> kongtoupao_opponent > bp -> kongtoupao){
        if((bp -> che_opponent >= bp -> che && bp -> che_opponent > 0) || bp -> kongtoupao_opponent >= 3)
            *kongtoupao_score_opponent = 180;
        else
            *kongtoupao_score_opponent = 140;
    }
}

void register_score_functions4(){
    score_bean4.insert({"complicated_score_function4", complicated_score_function4});
    kongtoupao_score_bean4.insert({"complicated_kongtoupao_score_function4", complicated_kongtoupao_score_function4});
    thinker_bean4.insert({"thinker4", thinker4});
}

std::string SearchScoreFunction4(void* score_func, int type){
    if(type == 0){
        for(auto it = score_bean4.begin(); it != score_bean4.end(); ++it){
            if(it -> second == score_func){
                return it -> first;
            }
        } 
        return "";
    }else if(type == 1){
        for(auto it = kongtoupao_score_bean4.begin(); it != kongtoupao_score_bean4.end(); ++it){
            if(it -> second == score_func){
                return it -> first;
            }
        } 
        return "";
    }else if(type == 2){
        for(auto it = thinker_bean4.begin(); it != thinker_bean4.end(); ++it){
            if(it -> second == score_func){
                return it -> first;
            }
        } 
        return "";
    }
    return "";
}


std::string thinker4(board::AIBoard4* bp, int maxdepth){
    bp -> Scan();
    int depth = 0;
    auto start = std::chrono::high_resolution_clock::now();

    for(depth = 1; depth <= maxdepth; ++depth){
        unsigned char src = 0, dst = 0;
        bp -> moves.clear();
        short score = alphabeta4(bp, -MATE_UPPER, MATE_UPPER, depth, ROOT, true, true, src, dst);
        size_t int_ms = (size_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        if(score > BAN_VALUE || depth == maxdepth){
            if(depth <= 6){
                calleval4(bp, -MATE_UPPER, MATE_UPPER, {2, 4}, true, src, dst);
            }
            bp -> Scan();
            std::pair<unsigned char, unsigned char> move = {src, dst};
            if(move == std::pair<unsigned char, unsigned char>({0, 0})){
                unsigned char mate_src = 0, mate_dst = 0, src = 0, dst = 0;
                scoretuple legal_moves_tmp[MAX_POSSIBLE_MOVES];
                int num_of_legal_moves_tmp = 0;
                bool killer_is_alive = false;
                short killer_score = 0;
                bp -> GenMovesWithScore<true>(legal_moves_tmp, num_of_legal_moves_tmp, NULL, killer_score, mate_src, mate_dst, killer_is_alive);
                src = SRC(legal_moves_tmp[0]);
                dst = DST(legal_moves_tmp[0]);
                std::cout << "My name: " << bp -> GetName() <<" [AM I FAILED?]" << num_of_legal_moves_tmp << " My move: " << bp -> translate_ucci(src, dst) << ", duration = " << int_ms << ", depth = " << depth << "." << std::endl;
                if(num_of_legal_moves_tmp != 0){
                    return bp -> translate_ucci(src, dst);
                }
            }
            std::cout << "My name: " << bp -> GetName()  << " My move: " << bp -> translate_ucci(move.first, move.second) << ", duration = " << int_ms << ", depth = " << depth << "." << std::endl;
            return bp -> translate_ucci(move.first, move.second);
        }
    }
    return "";
}


short alphabeta4(board::AIBoard4* self, short alpha, short beta, int depth, int type, const bool nullmove, const bool nullmovenow, unsigned char& argmaxsrc, unsigned char& argmaxdst){
    if(type == ROOT){
        self -> original_depth = depth;
        self -> ply = 0;
        self -> Scan();
    }
    depth = std::max(depth, 0);
    scoretuple legal_moves_tmp[MAX_POSSIBLE_MOVES];
    std::pair<unsigned char, unsigned char> killer = {0, 0};
    unsigned char mate_src = 0, mate_dst = 0, killer_src = 0, killer_dst = 0;
    bool killer_is_alive = false;
    tp* hashnode = NULL;
    short val = 0, hashval = self -> ProbeHash(depth, alpha, beta, !(nullmove && nullmovenow && type == CUT), &hashnode), killer_score = 0;
    if(hashnode){
        killer_is_alive = true;
        killer_src = hashnode -> src;
        killer_dst = hashnode -> dst;
    }
    if(killer_is_alive){
        killer = {killer_src, killer_dst};
    }
    int num_of_legal_moves_tmp = 0;
    bool mate = (depth ? self -> GenMovesWithScore<true>(legal_moves_tmp, num_of_legal_moves_tmp, killer_is_alive?&killer:NULL, killer_score, mate_src, mate_dst, killer_is_alive) : \
            self -> GenMovesWithScore<false>(legal_moves_tmp, num_of_legal_moves_tmp, killer_is_alive?&killer:NULL, killer_score, mate_src, mate_dst, killer_is_alive));
    if(mate){
        short tmp = MATE_UPPER - 1 - self -> ply;
        self -> RecordHash(depth, tmp, tmp, hashfEXACT, mate_src, mate_dst, 1);
        argmaxsrc = mate_src, argmaxdst = mate_dst;
        self -> moves[{self -> zobrist_hash, MAKE}] = {self->translate_ucci(argmaxsrc, argmaxdst), tmp, tmp, alpha, beta, 1, -1, depth, type};
        return tmp;
    }
    if(self -> Executed(&mate, legal_moves_tmp, num_of_legal_moves_tmp)){
        short tmp = 2 + self -> ply - MATE_UPPER;
        self -> RecordHash(depth, tmp, tmp, hashfEXACT, 0, 0, 2);
        argmaxsrc = 0; argmaxdst = 0;
        self -> moves[{self -> zobrist_hash, MAKE}] = {self->translate_ucci(argmaxsrc, argmaxdst), tmp, tmp, alpha, beta, 2, -1, depth, type};
        return tmp;
    }
    if(type != ROOT && self -> hist -> find(self -> state_red) != self -> hist -> end()){
        return MATE_UPPER;
    }
    if(killer_is_alive && hashnode && hashval != -MATE_UPPER){
        argmaxsrc = hashnode -> src;
        argmaxdst = hashnode -> dst;
        bool retval = false;
        //try move
        if(!mate){
            retval = self -> Move(argmaxsrc, argmaxdst, 0);
            self -> UndoMove(1);
        }
        //try move ends
        if(mate || retval){
            self -> moves[{self -> zobrist_hash, MAKE}] = {self->translate_ucci(argmaxsrc, argmaxdst), hashval, hashnode -> score, alpha, beta, 0, hashnode -> recordplace, depth, type};
            return hashval;
        }
    }
    if(depth == 0){
        short val = self -> evaluate();
        self -> RecordHash(depth, val, 0, hashfEXACT, 0, 0, 3);
        self -> moves[{self -> zobrist_hash, MAKE}] = {"^<^<", val, 0, alpha, beta, 3, -1, depth, type};
        return val;
    }
    int hashf = hashfALPHA;
    short best = -MATE_UPPER;
    argmaxsrc = 0, argmaxdst = 0;
    std::unordered_map<std::string, short> ucci2val;
    do{
        std::string ucci, bestucci;
        if(type == CUT && !mate && nullmove && nullmovenow){
            unsigned char t1 = 0, t2 = 0;
            ucci = "^<^<";
            bool retval = self -> NULLMove();
            val = -alphabeta4(self, -beta, 1-beta, depth - 3, type, nullmove, false, t1, t2);
            ucci2val[ucci] = val;
            self -> UndoMove(0);
            if(retval && val >= beta){
                self -> RecordHash(std::max(depth, 3), val, 0, hashfBETA, 0, 0, 4);
                self -> moves[{self -> zobrist_hash, MAKE}] = debugtuple{ucci, val, 0, alpha, beta, 4, -1, depth, type};
                return val;
            }
        }
        if(killer_is_alive){
            ucci = self -> translate_ucci(killer_src, killer_dst);
            BANNED
            bool retval = self -> Move(killer_src, killer_dst, killer_score) || mate;
            SEARCHBODY
            self -> UndoMove(1);
            DECISION(killer_src, killer_dst, true, killer_score);
        }
        for(int i = 0; i < num_of_legal_moves_tmp; ++i){
            auto tuple = legal_moves_tmp[i];
            short score = SCORE(tuple);
            unsigned char src = SRC(tuple), dst = DST(tuple);
            if(killer_is_alive && src == killer_src && dst == killer_dst){
                continue;
            }
            ucci = self -> translate_ucci(src, dst);
            BANNED
            bool retval = self -> Move(src, dst, score) || mate;
            SEARCHBODY
            self -> UndoMove(1);
            DECISION(src, dst, false, score);
        }
    }while(false);
    for(auto it = ucci2val.begin(); it != ucci2val.end(); ++it){
        if(best > -WIN_VALUE && it -> second < -WIN_VALUE){
            self -> banned[SELFMASK].insert(it -> first);
        }
    }
    if(best == -MATE_UPPER){
        best = self -> ply - MATE_UPPER;
        argmaxsrc = argmaxdst = 0;
        self -> moves[{self -> zobrist_hash, MAKE}] = debugtuple{"^<^<", best, best, alpha, beta, 7, -1, depth, type};
    }
    self -> RecordHash(depth, best, self -> moves[{self -> zobrist_hash, MAKE}].score, hashf, argmaxsrc, argmaxdst, 7);
    return best;
}


short alphabeta_doublerecursive4(board::AIBoard4* self, const int ver, short alpha, short beta, std::vector<int>& depths, const int type, const bool nullmove, const bool nullmovenow, std::unordered_map<unsigned char, char>& uncertainty_dict, bool* needclamp, unsigned char& argmaxsrc, unsigned char& argmaxdst){
    int depth = std::max(depths[ver], 0);
    self -> Scan();
    if(type == ROOT){
        self -> original_depth = depth;
        self -> ply = self -> score = 0;
    }
    scoretuple legal_moves_tmp[MAX_POSSIBLE_MOVES];
    std::pair<unsigned char, unsigned char> killer = {0, 0};
    unsigned char mate_src = 0, mate_dst = 0, killer_src = 0, killer_dst = 0;
    bool killer_is_alive = false;
    tp* hashnode = NULL;
    short val = 0, hashval = self -> ProbeHash(depth, alpha, beta, !(nullmove && nullmovenow && type == CUT), &hashnode), killer_score = 0;
    if(hashnode){
        killer_is_alive = true;
        killer_src = hashnode -> src;
        killer_dst = hashnode -> dst;
    }
    if(killer_is_alive){
        killer = {killer_src, killer_dst};
    }
    int num_of_legal_moves_tmp = 0;
    bool mate = (depth ? self -> GenMovesWithScore<true>(legal_moves_tmp, num_of_legal_moves_tmp, killer_is_alive?&killer:NULL, killer_score, mate_src, mate_dst, killer_is_alive) : \
            self -> GenMovesWithScore<false>(legal_moves_tmp, num_of_legal_moves_tmp, killer_is_alive?&killer:NULL, killer_score, mate_src, mate_dst, killer_is_alive));
    if(mate){
        short tmp = MATE_UPPER - 1 - self -> ply;
        self -> RecordHash(depth, tmp, tmp, hashfEXACT, mate_src, mate_dst, 1);
        argmaxsrc = mate_src, argmaxdst = mate_dst;
        return tmp;
    }
    mate = self -> Mate<true>();
    if(type != ROOT && self -> hist -> find(self -> state_red) != self -> hist -> end()){
        return MATE_UPPER;
    }
    if(killer_is_alive && hashnode && hashval != -MATE_UPPER){
        argmaxsrc = hashnode -> src;
        argmaxdst = hashnode -> dst;
        bool retval = false;
        if(!mate){
            retval = self -> Move(argmaxsrc, argmaxdst, 0);
            self -> UndoMove(1);
        }
        if(mate || retval){
            return hashval;
        }
    }
    if(depth == 0){
        unsigned char t1 = 0, t2 = 0;
        short val = eval4(self, ver+1, alpha, beta, depths, nullmove, t1, t2);
        self -> RecordHash(depth, val, 0, hashfEXACT, 0, 0, 3);
        self -> moves[{self -> zobrist_hash, MAKE}] = {"^<^<", val, 0, alpha, beta, 3, -1, depth, type};
        return val;
    }
    int hashf = hashfALPHA;
    short best = -MATE_UPPER;
    argmaxsrc = 0, argmaxdst = 0;
    do{
        if(killer_is_alive){
            bool retval = self -> Move(killer_src, killer_dst, killer_score) || mate;
            SEARCHBODYWO
            self -> UndoMove(1);
            DECISIONWO(killer_src, killer_dst, true, killer_score);
        }
        for(int i = 0; i < num_of_legal_moves_tmp; ++i){
            auto tuple = legal_moves_tmp[i];
            short score = SCORE(tuple);
            unsigned char src = SRC(tuple), dst = DST(tuple);
            if(killer_is_alive && src == killer_src && dst == killer_dst){
                continue;
            }
            bool retval = self -> Move(src, dst, score) || mate;
            SEARCHBODYWO
            self -> UndoMove(1);
            DECISIONWO(src, dst, false, score);
        }
    }while(false);
    if(best == -MATE_UPPER){
        best = self -> ply - MATE_UPPER;
        argmaxsrc = argmaxdst = 0;
    }
    self -> RecordHash(depth, best, self -> moves[{self -> zobrist_hash, MAKE}].score, hashf, argmaxsrc, argmaxdst, 7);
    return best;
}

void _inner_recur(board::AIBoard4* self, const int ver, std::unordered_map<unsigned char, char>& uncertainty_dict, std::vector<unsigned char>& uncertainty_keys, \
    std::unordered_map<std::pair<int, int>, short, myhash<int, int>>& result_dict, std::unordered_map<std::pair<int, int>, short, myhash<int, int>>& counter_dict, \
    const int index, const int me, const int op, const short score, const short alpha, const short beta, \
    std::vector<int>& depths, const bool nullmove, unsigned char& argmaxsrc, unsigned char& argmaxdst){
    const int THRES = 300;
    bool needclamp = false;
    if(index == 0 && uncertainty_keys.empty()){
        result_dict[{1, 1}] += alphabeta_doublerecursive4(self, ver, alpha, beta, depths, ROOT, nullmove, nullmove, uncertainty_dict, &needclamp, argmaxsrc, argmaxdst);
        counter_dict[{1, 1}] += 1;
        return;
    }
    if((size_t)index >= uncertainty_keys.size()){
        self -> score = score;
        self -> CalcVersion(ver);
        short res =  alphabeta_doublerecursive4(self, ver, alpha, beta, depths, ROOT, nullmove, nullmove, uncertainty_dict, &needclamp, argmaxsrc, argmaxdst);
        result_dict[{me, op}] += ((needclamp && res >= THRES) ? THRES : res);
        counter_dict[{me, op}] += 1;
    }else{
        bool turn = self -> turn, notturn = !self -> turn;
        char* state_pointer = turn ? self -> state_red : self -> state_black;
        char* state_pointer_oppo = notturn ? self -> state_red : self -> state_black;
        unsigned char key = uncertainty_keys[index];
        char c = uncertainty_dict[key];
        switch(c){
            case 'U': {
                for(char c : MINGZI){
                    int intchar = turn ? (int)c : ((int)c) ^ 32;
                    if(self -> aidi[ver][turn][intchar] > 0){
                        --self -> aidi[ver][turn][intchar];
                        uint32_t zobrist_before = self -> zobrist_hash;
                        int zobrist_key = turn ? key : 254 - key;
                        self -> zobrist_hash ^= self -> zobrist[(int)self -> state_red[zobrist_key]][zobrist_key];
                        state_pointer[key] = c;
                        state_pointer_oppo[254 - key] = self -> swapcase(c);
                        self -> zobrist_hash ^= self -> zobrist[(int)self -> state_red[zobrist_key]][zobrist_key];
                        short score_diff = self -> pst[(int)c][key] - self -> aiaverage[ver-1][turn][1][key];
                        _inner_recur(self, ver, uncertainty_dict, uncertainty_keys, result_dict, counter_dict, index+1, me*(self -> aidi[ver][turn][intchar] + 1), op, score + score_diff/2, alpha, beta, depths, \
                            nullmove, argmaxsrc, argmaxdst);
                        state_pointer[key] = 'U';
                        state_pointer_oppo[254 - key] = 'u';
                        self -> zobrist_hash = zobrist_before;
                        ++self -> aidi[ver][turn][intchar];
                    }
                }
                break;
            }

            case 'u': {
                for(char c: MINGZI){
                    int intchar = notturn ? (int)c : ((int)c) ^ 32;
                    if(self -> aidi[ver][notturn][intchar] > 0){
                        --self -> aidi[ver][notturn][intchar];
                        uint32_t zobrist_before = self -> zobrist_hash;
                        int zobrist_key = turn ? key : 254 - key;
                        self -> zobrist_hash ^= self -> zobrist[(int)self -> state_red[zobrist_key]][zobrist_key];
                        state_pointer[key] = self -> swapcase(c);
                        state_pointer_oppo[254 - key] = c;
                        self -> zobrist_hash ^= self -> zobrist[(int)self -> state_red[zobrist_key]][zobrist_key];
                        short score_diff = self -> pst[(int)c][254 - key] - self -> aiaverage[ver-1][notturn][1][254 - key];
                        _inner_recur(self, ver, uncertainty_dict, uncertainty_keys, result_dict, counter_dict, index+1, me, op*(self -> aidi[ver][notturn][intchar] + 1), score-score_diff/2, alpha, beta, depths, \
                            nullmove, argmaxsrc, argmaxdst);
                        state_pointer[key] = 'u';
                        state_pointer_oppo[254 - key] = 'U';
                        self -> zobrist_hash = zobrist_before;
                        ++self -> aidi[ver][notturn][intchar];
                    }
                }
                break;
            }
        }//逐步展开暗子
    }
}


short eval4(board::AIBoard4* self, const int ver, const short alpha, const short beta, std::vector<int>& depths, const bool nullmove, unsigned char& argmaxsrc, unsigned char& argmaxdst){
    self -> original_turns[ver] = self -> turn;
    std::unordered_map<unsigned char, char> uncertainty_dict;
    std::vector<unsigned char> uncertainty_keys;
    std::unordered_map<std::pair<int, int>, short, myhash<int, int>> result_dict;
    std::unordered_map<std::pair<int, int>, short, myhash<int, int>> counter_dict;
    const char* state_pointer = self -> turn ? self -> state_red : self -> state_black;
    bool needclamp = false;
    std::unordered_map<unsigned char, char> empty_map;
    if(ver == 0){
        return alphabeta_doublerecursive4(self, 0, alpha, beta, depths, ROOT, nullmove, nullmove, empty_map, &needclamp, argmaxsrc, argmaxdst);
    }

    else if((size_t)ver >= depths.size()){
        return self -> evaluate();
    }

    else{
        short scoretmp = self -> score;
        memcpy(self -> aidi[ver], self -> aidi[ver-1], sizeof(self -> aidi[ver]));
        for(unsigned char i = 51; i <= 203; ++i){
            if((i & 15) < 3 || (i & 15) > 11) { continue; }
            if(state_pointer[i] == 'U' || state_pointer[i] == 'u'){
                uncertainty_dict[i] = state_pointer[i];
                uncertainty_keys.push_back(i);
            }
        }
        unsigned char x = 0, y = 0;
        _inner_recur(self, ver, uncertainty_dict, uncertainty_keys, result_dict, counter_dict, 0, 1, 1, self -> score, alpha, beta, depths, nullmove, x, y);
        int nu = 0, de = 0; //numerator, denominator;
        for(auto it = result_dict.begin(); it != result_dict.end(); ++it){
            auto& item = it -> first;
            auto combinations = item.first * item.second;
            nu += (self -> turn == self -> original_turns[ver-1] ? 1 : -1) * (it -> second) * combinations;
            de += counter_dict[item] * combinations;
        }
        self -> score = scoretmp;
        return self -> div(nu, de);
    }
    return 0;
}

short calleval4(board::AIBoard4* self, short alpha, short beta, std::vector<int> depths, const bool nullmove, unsigned char& argmaxsrc, unsigned char& argmaxdst){
    memset(self -> original_turns, self -> turn, sizeof(self -> original_turns));
    return eval4(self, 0, alpha, beta, depths, nullmove, argmaxsrc, argmaxdst);
}