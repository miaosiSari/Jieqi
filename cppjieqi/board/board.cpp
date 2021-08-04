#include "board.h"
   
const int board::Board::_chess_board_size = CHESS_BOARD_SIZE;
const char board::Board::_initial_state[MAX] = 
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

const std::unordered_map<std::string, std::string> board::Board::_uni_pieces = {
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
    {"i", "暗"}
};

char board::Board::_dir[91][8] = {{0}};

board::Board::Board() noexcept: num_of_legal_moves(0),
                      _has_initialized(false),
                      _turn(true),
                      _round(0),
                      _score_func(NULL){
    memset(_state_red, 0, sizeof(_state_red));
    memset(_state_black, 0, sizeof(_state_black));
    strncpy(_state_red, _initial_state, _chess_board_size);
    strncpy(_state_black, _initial_state, _chess_board_size);
    memset(_is_legal_move, false, sizeof(_is_legal_move));
    _log = Singleton<logclass::Log>::get();
    _has_initialized = true;
    _initialize_dir();
}

void board::Board::Reset() noexcept{
    _turn = true;
    _round = 0;
    _score_func = NULL;
    num_of_legal_moves = 0;
    memset(_state_red, 0, sizeof(_state_red));
    memset(_state_black, 0, sizeof(_state_black));
    strncpy(_state_black, _initial_state, _chess_board_size);
    memset(_is_legal_move, false, sizeof(_is_legal_move));
    _log = Singleton<logclass::Log>::get();
    _initialize_dir();
}

void board::Board::_initialize_dir(){
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

void board::Board::SetScoreFunction(std::string function_name){
    _score_func = GetWithDefUnordered<std::string, SCORE>(function_bean, function_name, trivial_score_function);
}

const std::vector<std::string>& board::Board::GetHistory() const{
    return _board_history;
}

std::vector<std::string> board::Board::GetStateString() const{
    std::string tmp_red(_state_red);
    tmp_red = tmp_red.substr(0, board::Board::_chess_board_size);
    std::string tmp_black(_state_black);
    tmp_black = tmp_black.substr(0, board::Board::_chess_board_size);
    _log -> Write("board::Board::GetStateString");
    return (std::vector<std::string>){tmp_red, tmp_black};
}

bool board::Board::GetTurn() const{
    _log -> Write("board::Board::GetTurn");
    return _turn;
}

void board::Board::SetTurn(bool turn){
    _turn = turn;
}

bool board::Board::GetRound() const{
    _log -> Write("board::Board::GetRound");
    return _round;
}

std::tuple<int, bool, std::string, std::string> board::Board::GetTuple() const{
    _log -> Write("board::Board::GetTuple");
    std::string tmp_red(_state_red);
    tmp_red = tmp_red.substr(0, board::Board::_chess_board_size);
    std::string tmp_black(_state_red);
    tmp_black = tmp_black.substr(0, board::Board::_chess_board_size);
    std::tuple<int, bool, std::string, std::string> ret(_round, _turn, tmp_red, tmp_black);
    return ret;
}

const std::unordered_map<std::string, std::string>& board::Board::GetUniPieces() const{
    _log -> Write("board::Board::GetUniPieces");
    return _uni_pieces;
}

void board::Board::PrintPos(bool turn) const{
    _log -> Write("board::Board::PrintPos");
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

void board::Board::Move(const std::pair<int, int> start, const std::pair<int, int> end, const bool check){
    _log -> Write("board::Board::Move(const std::pair<int, int> start, const std::pair<int, int> end, const bool check)");
    Move(start.first, start.second, end.first, end.second, check);
}

void board::Board::Move(const std::string ucci, const bool check){
    //the ucci string is in "a0a1“ format.
    //Please check https://www.xqbase.com/protocol/cchess_ucci.htm
    _log -> Write("board::Board::Move(const std::string ucci, const bool check)");
    assert(ucci.size() == 4);
    const int y1 = (int)(ucci[0] - 'a');
    const int x1 = (int)(ucci[1] - '0');
    const int y2 = (int)(ucci[2] - 'a');
    const int x2 = (int)(ucci[3] - '0');
    Move(x1, y1, x2, y2, check);
}

void board::Board::Move(const char* ucci, const bool check){
    //the ucci string is in "a0a1“ format.
    //Please check https://www.xqbase.com/protocol/cchess_ucci.htm
    _log -> Write("board::Board::Move(const char* ucci, const bool check)");
    assert(strlen(ucci) == 4);
    const int y1 = (int)(ucci[0] - 'a');
    const int x1 = (int)(ucci[1] - '0');
    const int y2 = (int)(ucci[2] - 'a');
    const int x2 = (int)(ucci[3] - '0');
    Move(x1, y1, x2, y2, check);
}

void board::Board::Move(const int x1, const int y1, const int x2, const int y2, const bool check){
    _log -> Write("board::Board::Move(const int x1, const int y1, const int x2, const int y2, const bool check)");
    int encode_from = translate_x_y(x1, y1);
    int encode_to = translate_x_y(x2, y2);
    int reverse_encode_from = reverse(encode_from);
    int reverse_encode_to = reverse(encode_to);

    if(check) {
        if(_is_legal_move[encode_from][encode_to] == false){
            return;
        }
    }
    if(_turn){
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
    _turn = !_turn;
}

void board::Board::GenMovesWithScore(){
    //To make it more efficient, this implementation is rather dirty
    num_of_legal_moves = 0;
    memset(legal_moves, 0, sizeof(legal_moves));
    const char *_state_pointer = _turn?_state_red:_state_black;
    if(!_score_func) {
        SetScoreFunction((std::string)"trivial_score_function");
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
                            legal_moves[num_of_legal_moves] = std::make_tuple(_score_func(_state_pointer, i, j), i, j);
                            ++num_of_legal_moves;
                        } else{
                            ++cfoot;
                        }
                    }else{
                        if(islower(q)) {
                            legal_moves[num_of_legal_moves] = std::make_tuple(_score_func(_state_pointer, i, j), i, j);
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
                    legal_moves[num_of_legal_moves] = std::make_tuple(_score_func(_state_pointer, i, scanpos), i, scanpos);
                    ++num_of_legal_moves;
                } else {
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
                else if(p == 'N' || p == 'E'){
                    int n_diff_x = ((int)(j - i)) & 15;
                    if(n_diff_x == 2 || n_diff_x == 14){
                        if(_state_pointer[i + n_diff_x == 2?1:-1] != '.'){
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
                legal_moves[num_of_legal_moves] = std::make_tuple(_score_func(_state_pointer, i, j), i, j);
                ++num_of_legal_moves;
                if((p != 'N' && p != 'D' && p != 'N' && p != 'C') || islower(q)){
                    break;
                }
            } //j
        } //dir
    } //for
    std::sort(legal_moves, legal_moves + num_of_legal_moves, GreaterTuple<unsigned short, unsigned char, unsigned char>);
}//GenMovesWithScore()

void board::Board::Translate(unsigned char i, unsigned char j, char ucci[5]){
    int x1 = 12 - (i >> 4);
    int y1 = (i & 15) - 3;
    int x2 = 12 - (j >> 4);
    int y2 = (j & 15) - 3;
    ucci[0] = 'a' + y1;
    ucci[1] = '0' + x1;
    ucci[2] = 'a' + y2;
    ucci[3] = '0' + x2;
    ucci[4] = '\0';
}

void board::Board::Print_ij_ucci(unsigned char i, unsigned char j){
    printf("i = %d, j = %d", i, j);
    char ucci[5];
    board::Board::Translate(i, j, ucci);
    printf(" (ucci = %s).\n", ucci);
}

void board::Board::PrintAllMoves(){
    char ucci[5];
    int src = 0, dst = 0;
    unsigned short score = 0;
    printf("In board/board.cpp/PrintAllMoves()!\n");
    for(int i = 0; i < num_of_legal_moves; ++i){
       std::tuple<unsigned short, unsigned char, unsigned char> t = legal_moves[i];
       score = std::get<0>(t);
       src = std::get<1>(t);
       dst = std::get<2>(t);
       Translate(src, dst, ucci);
       printf("[%d]src = %d, dst = %d, ucci = %s, score = %u\n", i, src, dst, ucci, score);
    }
}

