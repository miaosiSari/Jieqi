#include "aiboard.h"

std::unordered_map<int, char> LUT = {
   {195, 'D'},
   {196, 'E'},
   {197, 'F'},
   {198, 'G'},
   {200, 'G'},
   {201, 'F'},
   {202, 'E'},
   {203, 'D'},
   {164, 'H'},
   {170, 'H'},
   {147, 'I'},
   {149, 'I'},
   {151, 'I'},
   {153, 'I'}
};

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
                    "   .........    "
                    "   I.I.I.I.I    "
                    "   .H.....H.    "
                    "   .........    "
                    "   DEFGKGFED    "
                    "                "
                    "                "
                    "                ";

const std::unordered_map<std::string, std::string> board::AIBoard::_uni_pieces = {
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

char board::AIBoard::_dir[91][8] = {{0}};
std::unordered_map<std::string, SCORE> score_bean;
std::unordered_map<std::string, KONGTOUPAO_SCORE> kongtoupao_score_bean;
std::unordered_map<std::string, THINKER> thinker_bean;

board::AIBoard::AIBoard() noexcept: num_of_legal_moves(0),
                      version(0),
                      round(0),
                      turn(true),
                      _has_initialized(false),
                      _score_func(NULL),
                      _kongtoupao_score_func(NULL){
    CopyData(di);
    memset(_state_red, 0, sizeof(_state_red));
    memset(_state_black, 0, sizeof(_state_black));
    CLEAR_STACK(cache);
    strncpy(_state_red, _initial_state, _chess_board_size);
    strncpy(_state_black, _initial_state, _chess_board_size);
    _initialize_dir();
    Scan();
    _has_initialized = true;
}


board::AIBoard::AIBoard(const char another_state[MAX], bool turn, int round, const unsigned char di[5][2][123]) noexcept{
    CopyData(di);
    this -> turn = turn;
    this -> round = round;
    memset(_state_red, 0, sizeof(_state_red));
    memset(_state_black, 0, sizeof(_state_black));
    strncpy(_state_red, another_state, _chess_board_size);
    strncpy(_state_black, another_state, _chess_board_size);
    CLEAR_STACK(cache);
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

board::AIBoard::AIBoard(const board::AIBoard& another_board) noexcept{
    this -> turn = another_board.turn;
    this -> round = another_board.round;
    this -> version = another_board.version;
    this -> cache = another_board.cache;
    memset(_state_red, 0, sizeof(_state_red));
    memset(_state_black, 0, sizeof(_state_black));
    memset(aiaverage, 0, sizeof(aiaverage));
    memset(aisumall, 0, sizeof(aisumall));
    memset(aidi, 0, sizeof(aidi));
    strncpy(this -> _state_red, another_board._state_red, _chess_board_size);
    strncpy(this -> _state_black, another_board._state_black, _chess_board_size);
    memmove(aiaverage, another_board.aiaverage, sizeof(aiaverage));
    memmove(aisumall, another_board.aisumall, sizeof(aisumall));
    memmove(aidi, another_board.aidi, sizeof(aidi));
    _initialize_dir();
    Scan();
    _has_initialized = true;
}

board::AIBoard::~AIBoard(){

}

void board::AIBoard::Reset() noexcept{
    CopyData(di);
    turn = true;
    round = 0;
    _score_func = NULL;
    num_of_legal_moves = 0;
    CLEAR_STACK(cache);
    memset(_state_red, 0, sizeof(_state_red));
    memset(_state_black, 0, sizeof(_state_black));
    memset(aiaverage, 0, sizeof(aiaverage));
    memset(aisumall, 0, sizeof(aisumall));
    memset(aidi, 0, sizeof(aidi));
    strncpy(_state_red, _initial_state, _chess_board_size);
    strncpy(_state_black, _initial_state, _chess_board_size);
    _initialize_dir();
    Scan();
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

void board::AIBoard::SetScoreFunction(std::string function_name, int type){
    if(type == 0){
        _score_func = GetWithDefUnordered<std::string, SCORE>(score_bean, function_name, trivial_score_function);
    }else if(type == 1){
        _kongtoupao_score_func = GetWithDefUnordered<std::string, KONGTOUPAO_SCORE>(kongtoupao_score_bean, function_name, trivial_kongtoupao_score_function);
    }else if(type == 2){
        _thinker_func = GetWithDefUnordered<std::string, THINKER>(thinker_bean, function_name, trivial_thinker);
    }
}

std::string board::AIBoard::SearchScoreFunction(int type){
    if(type == 0){
        return ::SearchScoreFunction(reinterpret_cast<void*>(_score_func), type);
    }else if(type == 1){
        return ::SearchScoreFunction(reinterpret_cast<void*>(_kongtoupao_score_func), type);
    }else if(type == 2){
        return ::SearchScoreFunction(reinterpret_cast<void*>(_thinker_func), type);
    }
    return "";
}

std::vector<std::string> board::AIBoard::GetStateString() const{
    std::string tmp_red(_state_red);
    tmp_red = tmp_red.substr(0, board::AIBoard::_chess_board_size);
    std::string tmp_black(_state_black);
    tmp_black = tmp_black.substr(0, board::AIBoard::_chess_board_size);
    return (std::vector<std::string>){tmp_red, tmp_black};
}

void board::AIBoard::Move(const std::string ucci){
    //the ucci string is in "a0a1“ format.
    //Please check https://www.xqbase.com/protocol/cchess_ucci.htm
    assert(ucci.size() == 4);
    const int y1 = (int)(ucci[0] - 'a');
    const int x1 = (int)(ucci[1] - '0');
    const int y2 = (int)(ucci[2] - 'a');
    const int x2 = (int)(ucci[3] - '0');
    Move(translate_x_y(x1, y1), translate_x_y(x2, y2));
}

void board::AIBoard::Move(const char* ucci){
    //the ucci string is in "a0a1“ format.
    //Please check https://www.xqbase.com/protocol/cchess_ucci.htm
    assert(strlen(ucci) == 4);
    const int y1 = (int)(ucci[0] - 'a');
    const int x1 = (int)(ucci[1] - '0');
    const int y2 = (int)(ucci[2] - 'a');
    const int x2 = (int)(ucci[3] - '0');
    Move(translate_x_y(x1, y1), translate_x_y(x2, y2));
}

void board::AIBoard::Move(const unsigned char encode_from, const unsigned char encode_to){
    const unsigned char reverse_encode_from = reverse(encode_from);
    const unsigned char reverse_encode_to = reverse(encode_to);
    if(turn){
        cache.push({encode_from, encode_to, _state_red[encode_to]});
        if(_state_red[encode_from] >= 'D' && _state_red[encode_from] <= 'I'){
            _state_red[encode_to] = 'U';
            _state_red[encode_from] = '.';
            _state_black[reverse_encode_to] = 'u';
            _state_black[reverse_encode_from] = '.';
        }else{
            _state_red[encode_to] = _state_red[encode_from];
            _state_red[encode_from] = '.';
            _state_black[reverse_encode_to] = _state_black[reverse_encode_from];
            _state_black[reverse_encode_from] = '.';
        }
    } else{
        cache.push({encode_from, encode_to, _state_black[encode_to]});
        if(_state_black[encode_from] >= 'D' && _state_black[encode_from] <= 'I'){
            _state_black[encode_to] = 'U';
            _state_black[encode_from] = '.';
            _state_red[reverse_encode_to] = 'u';
            _state_red[reverse_encode_from] = '.';
        }else{
            _state_black[encode_to] = _state_black[encode_from];
            _state_black[encode_from] = '.';
            _state_red[reverse_encode_to] = _state_red[reverse_encode_from];
            _state_red[reverse_encode_from] = '.';
        }
    }
    turn = !turn;
    if(turn){
       ++round;
    }
}

void board::AIBoard::UndoMove(){
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
        if(_state_red[encode_to] == 'U'){
            _state_red[encode_from] = LUT[encode_from];
            _state_red[encode_to] = eat;
            _state_black[reverse_encode_from] = swapcase(LUT[encode_from]);
            _state_black[reverse_encode_to] = swapcase(eat);
        }else{
            _state_red[encode_from] = _state_red[encode_to];
            _state_red[encode_to] = eat;
            _state_black[reverse_encode_from] = _state_black[reverse_encode_to];
            _state_black[reverse_encode_to] = swapcase(eat);
        }
    }else{
        if(_state_black[encode_to] == 'U'){
            _state_black[encode_from] = LUT[encode_from];
            _state_black[encode_to] = eat;
            _state_red[reverse_encode_from] = swapcase(LUT[encode_from]);
            _state_red[reverse_encode_to] = swapcase(eat);
        }else{
            _state_black[encode_from] = _state_black[encode_to];
            _state_black[encode_to] = eat;
            _state_red[reverse_encode_from] = _state_red[reverse_encode_to];
            _state_red[reverse_encode_to] = swapcase(eat);
        }
    }
}

void board::AIBoard::Scan(){
    che = 0;
    che_opponent = 0;
    zu = 0;
    covered = 0;
    covered_opponent = 0;
    endline = 0;
    score_rough = 0;
    kongtoupao = 0;
    kongtoupao_opponent = 0;
    kongtoupao_score = 0;
    kongtoupao_score_opponent=0;
    const char *_state_pointer = turn?_state_red:_state_black;
    if(!_kongtoupao_score_func){
        SetScoreFunction(std::string("complicated_kongtoupao_score_function"), 1);
    }
    for(int i = 51; i <= 203; ++i){
        if((i & 15) < 3 || (i & 15) > 11) { continue; }
        const char p = _state_pointer[i];
        if((i >> 4) == 3 && (p == 'd' || p == 'e' || p == 'f' || p == 'g' || p == 'r' || p == 'n' || p == 'c')){
            ++endline;
        }
        if(p == 'R' || p == 'N' || p == 'B' || p == 'A' || p == 'K' || p == 'C' || p == 'P'){
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
            score_rough += aiaverage[version][turn?1:0][1][i];
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
            score_rough -= aiaverage[version][turn?0:1][1][254 - i];
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
        kongtoupao = 0;
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
        kongtoupao_opponent = 0;
        return;
    } //else
} //KongTouPao

void board::AIBoard::GenMovesWithScore(){
    //To make it more efficient, this implementation is rather dirty
    num_of_legal_moves = 0;
    memset(legal_moves, 0, sizeof(legal_moves));
    const char *_state_pointer = turn?_state_red:_state_black;
    if(!_score_func) {
        SetScoreFunction(std::string("complicated_score_function"), 0);
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
                legal_moves[num_of_legal_moves] = std::make_tuple(_score_func(this, _state_pointer, i, j), i, j);
                ++num_of_legal_moves;
                if((p != 'D' && p != 'R') || islower(q)){
                    break;
                }
            } //j
        } //dir
    } //for
    std::sort(legal_moves, legal_moves + num_of_legal_moves, GreaterTuple<short, unsigned char, unsigned char>);
}//GenMovesWithScore()

void board::AIBoard::Rooted(){
    rooted_chesses.clear();
    const char *_state_pointer = turn?_state_red:_state_black;
    for(unsigned char i = 51; i <= 203; ++i){
        char p = _state_pointer[i];
        int intp = (int)p;
        if(!::isupper(p) || p == 'U'){
            continue;
        }
        if(p == 'C' || p == 'H'){
            for(unsigned char cnt = 0; cnt < 8; ++cnt){
                if(_dir[intp][cnt] == 0){
                    break;
                }
                const char d = _dir[intp][cnt];
                int cfoot = 0;
                for(int j = i + d;; j += d){
                    char q = _state_pointer[j];
                    if(q == ' ') break;
                    if(cfoot == 0 && q == '.') continue;
                    else if(cfoot == 0 && q != '.') ++cfoot;
                    else if(cfoot == 1 && ::islower(q)) break;
                    else if(cfoot == 1 && ::isupper(q)) {rooted_chesses.insert(j); break;}
                }
            }
        }
        else{
            for(unsigned char cnt = 0; cnt < 8; ++cnt){
                if(_dir[intp][cnt] == 0){
                    break;
                }
                const char d = _dir[intp][cnt];
                if(i > 128 && p == 'P' && (d == EAST || d == WEST)) {
                    break;
                }
                for(int j = i + d;; j += d){
                    char q = _state_pointer[j];
                    if(q == ' ' || ::islower(q)) { break; }
                    else if(p == 'K' && (j < 160 || (j & 15) > 8 || (j & 15) < 6)) { break; }
                    else if(p == 'G' && j != 183) { break; }
                    else if(p == 'N' || p == 'E'){
                        int n_diff_x = ((int)(j - i)) & 15;
                        if(n_diff_x == 2 || n_diff_x == 14){
                            if(_state_pointer[i + (n_diff_x == 2?1:-1)] != '.') { break; }
                        }else{
                            if(j > i && _state_pointer[i + 16] != '.') { break; }
                            if(j < i && _state_pointer[i - 16] != '.') { break; }
                        }
                    }
                    else if((p == 'B' || p == 'F') && _state_pointer[i + d/2] != '.') { break; }
                    if(::isupper(q)){
                        rooted_chesses.insert(j);
                        break;
                    }
                    if(p != 'D' && p != 'R'){
                        break;
                    }
                } //for
            }//for dir
        }//else 
    }//for i
}


void board::AIBoard::CopyData(const unsigned char di[5][2][123]){
    memset(aiaverage, 0, sizeof(aiaverage));
    memset(aisumall, 0, sizeof(aisumall));
    memset(aidi, 0, sizeof(aidi));
    memcpy(aidi, di, sizeof(aidi));
    const float discount_factor = 1.5;
    for(int version = 0; version < VERSION_MAX; ++version){
        AISUM(version);
        if(numr > 0){
            double sumr = 0.0;
            for(const char c : MINGZI){
                sumr += pst[(int)c][0] * aidi[version][1][(int)c] / discount_factor;
            }
            aiaverage[version][1][0][0] = ::round(sumr / numr);
            for(int i = 51; i <= 203; ++i){
                sumr = 0.0;
                for(const char c : MINGZI){
                    sumr += pst[(int)c][i] * aidi[version][1][(int)c];
                }
                aiaverage[version][1][1][i] = ::round(sumr / numr);
            }
        }
        if(numb > 0){
            double sumb = 0.0;
            for(const char c : MINGZI){
                sumb += pst[(int)c][0] * aidi[version][0][((int)c)^32] / discount_factor;
            }
            aiaverage[version][0][0][0] = ::round(sumb / numb);
            for(int i = 51; i <= 203; ++i){
                sumb = 0.0;
                for(const char c : MINGZI){
                    sumb += pst[(int)c][i] * aidi[version][0][((int)c)^32];
                }
                aiaverage[version][0][1][i] = ::round(sumb / numb);
            }
        }
    }
    for(auto i = 51; i < 204; ++i){
        //printf("aiaverage[0][1][1][i] = %d\n", aiaverage[0][1][1][i]);
        //printf("pst['C'][%d] = %d\n", i, pst[(int)'C'][i]);
        //std::cout << aiaverage[0][1][1][i] << " " << pst[(int)'C'][i] << "\n";
    }
}

std::string board::AIBoard::Think(){
    SetScoreFunction("random_thinker", 2);
    return _thinker_func(this, true?_state_red:_state_black);
}


void board::AIBoard::PrintPos(bool turn) const{
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

std::string board::AIBoard::DebugPrintPos(bool turn) const{
    std::string ret;
    if(turn){
        ret += "RED\n";   
    }else{
        ret += "BLACK\n";
    }
    for(int x = 3; x <= 12; ++x){
        ret += std::to_string(translate_x(x));
        ret += ' ';
        for(int y = 3; y <= 11; ++y){
            const char c = turn?_state_red[encode(x, y)]:_state_black[encode(x, y)];
            ret += c;
        }
        ret += '\n';
    }
    ret += "  abcdefghi\n";
    return ret;
}


inline short trivial_score_function(void* self, const char* state_pointer, unsigned char src, unsigned char dst){
    board::AIBoard* bp = reinterpret_cast<board::AIBoard*>(self);
    return 0;
}

inline short complicated_score_function(void* self, const char* state_pointer, unsigned char src, unsigned char dst){
    #define LOWER_BOUND -32768
    #define UPPER_BOUND 32767
    board::AIBoard* bp = reinterpret_cast<board::AIBoard*>(self);
    int version = bp -> version;
    int turn = bp -> turn ? 1 : 0;
    int che_char = bp -> turn ? (int)'R': (int)'r';
    int che_opponent_char = che_char ^ 32;
    int zu_char = bp -> turn ? (int)'P': (int)'p';
    int shi_opponent_char = bp -> turn ? (int)'a': (int)'A';
    char p = state_pointer[src], q = bp -> swapcase(state_pointer[dst]);
    int intp = (int)p, intq = (int)q;
    float score = 0.0;
    float possible_che = 0.0;
    if(bp -> aisumall[version][turn]){
        possible_che = (float)((bp -> covered) * bp -> aidi[version][turn][che_char])/bp -> aisumall[version][turn];
    }
    float possible_che_opponent = 0.0;
    if(bp ->  aisumall[version][1 - turn]){
        possible_che_opponent = (float)((bp -> covered_opponent) * bp -> aidi[version][1 - turn][che_opponent_char])/bp -> aisumall[version][1 - turn];
    }
    float zu_possibility = 0.0;
    if(bp -> aisumall[version][turn]){
        zu_possibility = (float)(bp -> aidi[version][turn][zu_char])/bp -> aisumall[version][turn];
    }
    float shi_possibility_opponent = 0.0, base_possibility = 1.0;
    if(bp -> aisumall[version][1 - turn]){
        shi_possibility_opponent = (float)(bp -> aidi[version][1 - turn][shi_opponent_char])/bp -> aisumall[version][1 - turn]; 
    }
    if(state_pointer[54] == 'g') { base_possibility *= (1 - shi_possibility_opponent);}
    if(state_pointer[56] == 'g') { base_possibility *= (1 - shi_possibility_opponent);}
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
                    if((src == 51 || src == 52) && state_pointer[53] == 'f' &&  state_pointer[54] == 'g') { break; }
                    if((src == 59 || src == 58) && state_pointer[57] == 'f' &&  state_pointer[56] == 'g') { break; }
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
        }//else if( p == 'R')
    }
    
    else{
        score = bp -> aiaverage[version][turn][1][dst] - bp -> aiaverage[version][turn][0][0] + 20;
        if(p == 'D'){
            if(bp -> score_rough < -150){
                score -= (45 * (possible_che_opponent/2 + bp -> che_opponent));
            }else{
                score -= (30 * (possible_che_opponent/2 + bp -> che_opponent));
            }
        }else if(p == 'E'){
            if(src == 196 && dst == 165 && state_pointer[149] == 'I'){
                for(int scanpos = 133; scanpos > A9; scanpos -= 16){
                    if(state_pointer[scanpos] == 'r'){
                        score += bp -> aiaverage[version][turn][0][0] / 2;
                    }else if(state_pointer[scanpos] != '.'){
                        break;
                    }                       
                }
            }   

            if(src == 202 && dst == 169 && state_pointer[153] == 'I'){
                for(int scanpos = 137; scanpos > A9; scanpos -= 16){
                    if(state_pointer[scanpos] == 'r'){
                        score += bp -> aiaverage[version][turn][0][0] / 2;
                    }else if(state_pointer[scanpos] != '.'){
                        break;
                    } 
                }
            }
        
        
        }else if(p == 'F'){
            if((src == 197 || src == 201) && dst == 167 && state_pointer[151] == 'I'){
                bool findche = false;
                for(int scanpos = 135; scanpos > A9; scanpos -= 16){
                    if(state_pointer[scanpos] == 'r'){
                        score += bp -> aiaverage[version][turn][0][0] / 2;
                        findche = true;
                        break;
                    }else if(state_pointer[scanpos] != '.'){
                        break;
                    } 
                }//for
                if(!findche){
                    for(int scanpos = 135; scanpos > 130; --scanpos){
                        if(state_pointer[scanpos] == 'r'){
                            score += bp -> aiaverage[version][turn][0][0] / 2;
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
                            score += bp -> aiaverage[version][turn][0][0] / 2;
                            break;
                        }else if(state_pointer[scanpos] != '.'){
                            break;
                        }
                    }
                }
            }//if((src == 197 || src == 201) && dst == 167 && state_pointer[151] == 'I')
            
        
        }else if(p == 'G'){
            if(src == 200 && state_pointer[59] != 'd' && state_pointer[59] != 'r' && state_pointer[56] != 'a' && state_pointer[71] != 'a' \
                    && (state_pointer[71] == 'p' || state_pointer[87] != 'n')){
                        int cheonleidao = 0;
                        int che_opponent_onleidao = 0;
                        for(int scanpos = 184; scanpos > A9; scanpos -= 16){
                            if(state_pointer[scanpos] == 'R') { ++cheonleidao; }
                            else if(state_pointer[scanpos] == 'r'){ ++che_opponent_onleidao; }
                        }
                        if(cheonleidao > che_opponent_onleidao && possible_che >= possible_che_opponent) { score += 40 * base_possibility;}                         
                    }
                   
            else if(src == 198 && state_pointer[51] != 'd' && state_pointer[51] != 'r' && state_pointer[54] != 'a' && state_pointer[71] != 'a' \
                    && (state_pointer[71] == 'p' || state_pointer[87] != 'n')){ //Should by else if, Python BUG!
                        int cheonleidao = 0;
                        int che_opponent_onleidao = 0;
                        for(int scanpos = 182; scanpos > A9; scanpos -= 16){
                            if(state_pointer[scanpos] == 'R') { ++cheonleidao; }
                            else if(state_pointer[scanpos] == 'r'){ ++che_opponent_onleidao; }
                        }
                        if(cheonleidao > che_opponent_onleidao && possible_che >= possible_che_opponent) { score += 40 * base_possibility;}                             
                    }
            
            
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
                        if((src == 164 && state_pointer[148] == 'p') || (src == 170 && state_pointer[154] == 'p')) {;}
                        if((bp -> che) < (bp -> che_opponent) || bp -> che == 0 || (bp -> score_rough < 150)) {score -= 100; }
                        //else if(che == che_opponent): Python BUG
                    }
                
                
         
        }else if(p == 'I'){
                if(state_pointer[src - 32] == 'r' || state_pointer[src - 32] == 'p'){
                    score -= bp -> aiaverage[version][turn][0][0]/2;    
                }else if(state_pointer[src - 32] == 'n' || state_pointer[src - 32] == 'c'){ // Python BUG
                    score += 30;
                }else if(state_pointer[src - 48] == 'i'){
                    score += 30;    
                }else{
                    score += 20;    
                }
        }//else if I
    }//else
        
    if(q >= 'A' && q <= 'Z'){
        int k = 254 - dst;
        if(q == 'R' || q == 'N' || q == 'B' || q == 'A' || q == 'C' || q == 'P'){
            score += pst[intq][k];
            if(q == 'P' && state_pointer[dst + 32] == 'I'){
                score += 30;  
            }
        }
        else{
            if(q != 'U'){
                score += bp -> aiaverage[version][1 - turn][0][0];
                if(q == 'I'){
                    score += 10;
                }                   
            }else{
                score += bp -> aiaverage[version][1 - turn][1][k];
                if((dst >> 4) == 7 && (dst & 1) == 1){
                    score += 30;  
                }
            }
            if(q == 'D'){
                if(bp -> score_rough > 150){
                    score +=  45 * (possible_che/2 + bp -> che);
                }else{
                    score +=  30 * (possible_che/2 + bp -> che);
                }                   
            }//if(q == 'D')
        }           
    }//capture
    
    if(score < LOWER_BOUND) return LOWER_BOUND;
    else if(score > UPPER_BOUND) return UPPER_BOUND;
    return (short)round(score);
}


inline void trivial_kongtoupao_score_function(void* self, short* kongtoupao_score, short* kongtoupao_score_opponent){
    board::AIBoard* bp = reinterpret_cast<board::AIBoard*>(self);
}

inline void complicated_kongtoupao_score_function(void* self, short* kongtoupao_score, short* kongtoupao_score_opponent){
    board::AIBoard* bp = reinterpret_cast<board::AIBoard*>(self);
    if(bp -> kongtoupao > bp -> kongtoupao_opponent){
        if((bp -> che >= bp -> che_opponent && bp -> che > 0) || bp -> kongtoupao >= 3)
            *kongtoupao_score = 100;
        else
            *kongtoupao_score = 70;
    }

    else if(bp -> kongtoupao_opponent > bp -> kongtoupao){
        if((bp -> che_opponent >= bp -> che && bp -> che_opponent > 0) || bp -> kongtoupao_opponent >= 3)
            *kongtoupao_score_opponent = 100;
        else
            *kongtoupao_score_opponent = 70;
    }
}


std::string trivial_thinker(void* self, const char* state_pointer){
    board::AIBoard* bp = reinterpret_cast<board::AIBoard*>(self);
    bp -> GenMovesWithScore();
    int index = 0;
    return bp -> translate_ucci(std::get<1>(bp -> legal_moves[index]), std::get<2>(bp -> legal_moves[index]));
}


std::string random_thinker(void* self, const char* state_pointer){
    board::AIBoard* bp = reinterpret_cast<board::AIBoard*>(self);
    bp -> GenMovesWithScore();
    srand(time(NULL));
    int index = rand() % bp -> num_of_legal_moves;
    return bp -> translate_ucci(std::get<1>(bp -> legal_moves[index]), std::get<2>(bp -> legal_moves[index]));
}


void register_score_functions(){
    score_bean.insert({"trivial_score_function", trivial_score_function});
    score_bean.insert({"complicated_score_function", complicated_score_function});
    kongtoupao_score_bean.insert({"trivial_kongtoupao_score_function", trivial_kongtoupao_score_function});
    kongtoupao_score_bean.insert({"complicated_kongtoupao_score_function", complicated_kongtoupao_score_function});
    thinker_bean.insert({"trivial_thinker", trivial_thinker});
    thinker_bean.insert({"random_thinker", random_thinker});
}

std::string SearchScoreFunction(void* score_func, int type){
    if(type == 0){
        for(auto it = score_bean.begin(); it != score_bean.end(); ++it){
            if(it -> second == score_func){
                return it -> first;
            }
        } 
        return "";
    }else if(type == 1){
        for(auto it = kongtoupao_score_bean.begin(); it != kongtoupao_score_bean.end(); ++it){
            if(it -> second == score_func){
                return it -> first;
            }
        } 
        return "";
    }else if(type == 2){
        for(auto it = thinker_bean.begin(); it != thinker_bean.end(); ++it){
            if(it -> second == score_func){
                return it -> first;
            }
        } 
        return "";
    }
    return "";
}