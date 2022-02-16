#include "board.h"


const int board::Board::_chess_board_size = CHESS_BOARD_SIZE;

#if !DEBUG
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
#else

const char board::Board::_initial_state[MAX] = 
                    "                "
                    "                "
                    "                "
                    "   .....k...    "
                    "   .........    "
                    "   .........    "
                    "   .....r...    "
                    "   .........    "
                    "   .........    "
                    "   ..C.R....    "
                    "   .........    "
                    "   .........    "
                    "   ....K....    "
                    "                "
                    "                "
                    "                ";


#endif

const std::unordered_map<std::string, std::string> board::Board::uni_pieces = {
	#ifdef WIN32
	{".", "．"},
    {"R", "俥"},
    {"N", "傌"},
    {"B", "相"},
    {"A", "仕"},
    {"K", "帅"},
    {"P", "兵"},
    {"C", "炮"},
    {"D", "红"},
    {"E", "红"},
    {"F", "红"},
    {"G", "红"},
    {"H", "红"},
    {"I", "红"},
    {"r", "车"},
    {"n", "马"},
    {"b", "象"},
    {"a", "士"},
    {"k", "将"},
    {"p", "卒"},
    {"c", "包"},
    {"d", "黑"},
    {"e", "黑"},
    {"f", "黑"},
    {"g", "黑"},
    {"h", "黑"},
    {"i", "黑"}
	#else
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
	#endif
};

char board::Board::_dir[91][8] = {{0}};

board::Board::Board() noexcept: finished(false),
                      turn(true),
                      round(0),
                      _has_initialized(false){
    memset(state_red, 0, sizeof(state_red));
    memset(state_black, 0, sizeof(state_black));
    strncpy(state_red, _initial_state, _chess_board_size);
    strncpy(state_black, _initial_state, _chess_board_size);
    #if DEBUG
    if(turn){
        rotate(state_black);
    }else{
        rotate(state_red);
    }
    #endif
    memset(_is_legal_move, false, sizeof(_is_legal_move));
    memset(legal_moves, 0, sizeof(legal_moves));
    _initialize_dir();
    GenerateRandomMap();
    hist[state_red] = false;
    initialize_di();
    _has_initialized = true;
}

void board::Board::Reset(std::unordered_map<bool, std::unordered_map<unsigned char, char>>* random_map){
    hist.clear();
    finished = false;
    turn = true;
    round = 0;
    memset(state_red, 0, sizeof(state_red));
    memset(state_black, 0, sizeof(state_black));
    strncpy(state_red, _initial_state, _chess_board_size);
    strncpy(state_black, _initial_state, _chess_board_size);
    #if DEBUG
    if(turn){
        rotate(state_black);
    }else{
        rotate(state_red);
    }
    #endif
    state_red[_chess_board_size] = '\0';
    state_black[_chess_board_size] = '\0';
    memset(_is_legal_move, false, sizeof(_is_legal_move));
    _initialize_dir();
    if(random_map){
        this -> random_map = std::move(*random_map);
    }else{
        GenerateRandomMap();
    }
    hist[state_red] = false;
    initialize_di();
    #if DEBUG && BLACK
    turn = false;
    #endif
}

void board::Board::initialize_di(){
    memset(this -> di, 0, sizeof(this -> di));
    std::unordered_set<int> redplaces = {195, 196, 197, 198, 200, 201, 202, 203, 164, 170, 147, 149, 151, 153, 155};
    std::unordered_set<int> blackplaces = {59, 58, 57, 56, 54, 53, 52, 51, 90, 84, 107, 105, 103, 101, 99};
    for(int pos: redplaces){
        for(int i = 0; i < VERSION_MAX; ++i){
           di[i][1][(int)random_map[true][pos]] += (state_red[pos] >= 'D' && state_red[pos] <= 'I') ? 1 : 0;
        }
    }
    for(int pos: blackplaces){
        for(int i = 0; i < VERSION_MAX; ++i){
           di[i][0][(int)random_map[true][pos]] += (state_red[pos] >= 'd' && state_red[pos] <= 'i') ? 1 : 0;
        }
    }
    memmove(this -> di_red, this -> di, sizeof(this -> di));
    memmove(this -> di_black, this -> di, sizeof(this -> di));
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

const std::vector<std::string>& board::Board::GetHistory() const{
    return _board_history;
}

std::vector<std::string> board::Board::GetStateString() const{
    std::string tmp_red(state_red);
    tmp_red = tmp_red.substr(0, board::Board::_chess_board_size);
    std::string tmp_black(state_black);
    tmp_black = tmp_black.substr(0, board::Board::_chess_board_size);
    return (std::vector<std::string>){tmp_red, tmp_black};
}


std::tuple<int, bool, std::string, std::string> board::Board::GetTuple() const{
    std::string tmp_red(state_red);
    tmp_red = tmp_red.substr(0, board::Board::_chess_board_size);
    std::string tmp_black(state_red);
    tmp_black = tmp_black.substr(0, board::Board::_chess_board_size);
    std::tuple<int, bool, std::string, std::string> ret(round, turn, tmp_red, tmp_black);
    return ret;
}

const std::unordered_map<std::string, std::string>& board::Board::GetUniPieces() const{
    return uni_pieces;
}

void board::Board::PrintPos(bool turn, bool iscovered=true, bool god=false, bool swapcasewhenblack=false) const{
    if(turn){
        if(god)
            printf("上帝视角(红):\n");
        else
            printf("红方视角:\n");
    }else{
        if(god)
            printf("上帝视角(黑):\n");
        else
            printf("黑方视角:\n");
    }
    std::cout << std::endl << std::endl;
    for(int x = 3; x <= 12; ++x){
        std::cout << translate_x(x) << " ";
        for(int y = 3; y <= 11; ++y){
            std::cout << _getstringxy(x, y, turn, iscovered, swapcasewhenblack);
        }
        std::cout << std::endl;
    }
    std::cout << "  ａｂｃｄｅｆｇｈｉ\n\n";
}

std::shared_ptr<InfoDict> board::Board::Move(const std::string ucci, const bool check){
    //the ucci string is in "a0a1“ format.
    //Please check https://www.xqbase.com/protocol/cchess_ucci.htm
    assert(ucci.size() == 4);
    const int y1 = (int)(ucci[0] - 'a');
    const int x1 = (int)(ucci[1] - '0');
    const int y2 = (int)(ucci[2] - 'a');
    const int x2 = (int)(ucci[3] - '0');
    return Move(x1, y1, x2, y2, check);
}

std::shared_ptr<InfoDict> board::Board::Move(const char* ucci, const bool check){
    //the ucci string is in "a0a1“ format.
    //Please check https://www.xqbase.com/protocol/cchess_ucci.htm
    assert(strlen(ucci) == 4);
    const int y1 = (int)(ucci[0] - 'a');
    const int x1 = (int)(ucci[1] - '0');
    const int y2 = (int)(ucci[2] - 'a');
    const int x2 = (int)(ucci[3] - '0');
    return Move(x1, y1, x2, y2, check);
}

std::shared_ptr<InfoDict> board::Board::Move(const int x1, const int y1, const int x2, const int y2, const bool check){
    if(finished){
        return nullptr;
    }
    int encode_from = translate_x_y(x1, y1);
    int encode_to = translate_x_y(x2, y2);
    int reverse_encode_from = reverse(encode_from);
    int reverse_encode_to = reverse(encode_to);
    char eat = '.', eat_rb = '.', eat_check = '.';
    int eat_type = 0, eat_type_tmp = 0;

    if(check) {
        if(_is_legal_move[encode_from][encode_to] == false){
            return std::shared_ptr<InfoDict>(new InfoDict(false, turn, round, false, eat, eat_rb, eat_type, x1, y1, x2, y2, eat_check));
        }
    }

    if(turn){
        eat = state_red[encode_to];
        char eat_tmp = eat;
        FIND(eat_tmp, encode_to, turn);
        eat_check = eat_tmp;
        eat_type = eat_type_tmp;
        eat_rb = eat;
        for(int i = 0; i < VERSION_MAX && eat_type == 2; ++i){
            di_red[i][0][(int)eat_check] -= 1;
        }
        state_red[encode_to] = state_red[encode_from];
        FIND(state_red[encode_to], encode_from, turn);
        for(int i = 0; i < VERSION_MAX && eat_type_tmp == 2; ++i){//红方暗子翻出, 双方都减1
            di_red[i][1][(int)state_red[encode_to]] -= 1; 
            di_black[i][1][(int)state_red[encode_to]] -= 1;
        }
        state_red[encode_from] = '.';
        state_black[reverse_encode_to] = state_black[reverse_encode_from];
        FIND(state_black[reverse_encode_to], reverse_encode_from, !turn);
        assert(swapcase(state_black[reverse_encode_to]) == state_red[encode_to]);
        state_black[reverse_encode_from] = '.';
    } else{
        eat = state_black[encode_to];
        char eat_tmp = eat;
        FIND(eat_tmp, encode_to, turn);
        eat_check = eat_tmp;
        eat_type = eat_type_tmp;
        eat_rb = swapcase(eat);
        //黑吃红暗子减di_black
        for(int i = 0; i < VERSION_MAX && eat_type == 2; ++i){
            di_black[i][1][(int)swapcase(eat_check)] -= 1;
        }
        state_black[encode_to] = state_black[encode_from];
        FIND(state_black[encode_to], encode_from, turn);
        for(int i = 0; i < VERSION_MAX && eat_type_tmp == 2; ++i){//黑方暗子翻出, 双方都减1
            di_red[i][0][(int)swapcase(state_black[encode_to])] -= 1; 
            di_black[i][0][(int)swapcase(state_black[encode_to])] -= 1;
        }
        state_black[encode_from] = '.';
        state_red[reverse_encode_to] = state_red[reverse_encode_from];
        FIND(state_red[reverse_encode_to], reverse_encode_from, !turn);
        assert(swapcase(state_black[encode_to]) == state_red[reverse_encode_to]);
        state_red[reverse_encode_from] = '.';
    }
    std::shared_ptr<InfoDict> p(new InfoDict(true, turn, round, (eat == 'k'), eat, eat_rb, eat_type, x1, y1, x2, y2, eat_check));
    turn = !turn;
    hist[state_red] = turn;
    if(turn){
       ++round;
    }
    return p;
}

void board::Board::DebugDI(){
    printf("红方视角:\n");
    for(char c: MINGZI){
        if(c == 'K') {
            continue;
        }
        printf("红方字符=%c, 红方相应个数: %d, 黑方字符=%c, 黑方相应个数=%d\n", c, di_red[0][1][(int)c], swapcase(c), di_red[0][0][(int)swapcase(c)]);
    }
    printf("黑方视角:\n");
    for(char c: MINGZI){
        if(c == 'K') {
            continue;
        }
        printf("红方字符=%c, 红方相应个数: %d, 黑方字符=%c, 黑方相应个数=%d\n", c, di_black[0][1][(int)c], swapcase(c), di_black[0][0][(int)swapcase(c)]);
    }
}

void board::Board::GenMovesWithScore(){
    memset(_is_legal_move, false, sizeof(_is_legal_move));
    const char *_state_pointer = turn?state_red:state_black;
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
                            _is_legal_move[(int)i][(int)j] = true;
                        } else{
                            ++cfoot;
                        }
                    }else{
                        if(islower(q)) {
                            _is_legal_move[(int)i][(int)j] = true;
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
                    _is_legal_move[(int)i][(int)scanpos] = true;
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
                _is_legal_move[(int)i][(int)j] = true;
                if((p != 'D' && p != 'H' && p != 'C' && p != 'R') || islower(q)){
                    break;
                }
            } //j
        } //dir
    } //for
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

void board::Board::TranslateSingle(unsigned char i, char ucci[3]){
    int x1 = 12 - (i >> 4);
    int y1 = (i & 15) - 3;
    ucci[0] = 'a' + y1;
    ucci[1] = '0' + x1;
    ucci[2] = '\0';
}

void board::Board::Print_ij_ucci(unsigned char i, unsigned char j){
    printf("i = %d, j = %d", i, j);
    char ucci[5];
    board::Board::Translate(i, j, ucci);
    printf(" (ucci = %s).\n", ucci);
}

void board::Board::GenerateRandomMap(){
    auto pop_chess = [](std::vector<char>& v, char c){
        for(std::vector<char>::iterator it = v.begin(); it != v.end(); ){
            if(*it == c){
                v.erase(it++);
                return;
            }
            else{
                it++;
            }
        }
    };

    std::vector<char> chararray_red = {'R', 'R', 'N', 'N', 'B', 'B', 'A', 'A', 'C', 'C', 'P', 'P', 'P', 'P', 'P'};
    std::vector<char> chararray_black = {'r', 'r', 'n', 'n', 'b', 'b', 'a', 'a', 'c', 'c', 'p', 'p', 'p', 'p', 'p'};
    for(int i = 51; i <= 203; ++i){
        if(::isupper(state_red[i]) && state_red[i] != 'K' && MINGZI.find(state_red[i]) != std::string::npos){
            pop_chess(chararray_red, state_red[i]);
        }
        if(::islower(state_red[i]) && state_red[i] != 'k' && MINGZI.find(swapcase(state_red[i])) != std::string::npos){
            pop_chess(chararray_black, state_red[i]);
        }
    }

    std::vector<unsigned char> position_red = {TXY(0, 0), TXY(0, 1), TXY(0, 2), TXY(0, 3), TXY(0, 5), TXY(0, 6), \
        TXY(0, 7), TXY(0, 8), TXY(2, 1), TXY(2, 7), TXY(3, 0), TXY(3, 2), TXY(3, 4), TXY(3, 6), TXY(3, 8)};
    std::vector<unsigned char> position_black = {TXY(9, 0), TXY(9, 1), TXY(9, 2), TXY(9, 3), TXY(9, 5), TXY(9, 6), \
        TXY(9, 7), TXY(9, 8), TXY(7, 1), TXY(7, 7), TXY(6, 0), TXY(6, 2), TXY(6, 4), TXY(6, 6), TXY(6, 8)};
    int size = 15;
    SV(chararray_red); 
    SV(chararray_black);
    std::unordered_map<unsigned char, char> r, b;
    for(int i = 0, cnt = 0; i < size; ++i){
        if(state_red[position_red[i]] >= 'D' && state_red[position_red[i]] <= 'I'){
            r[position_red[i]] = chararray_red[cnt];
            b[reverse(position_red[i])] = swapcase(chararray_red[cnt]);
            ++cnt;
        }else{
            continue;
        }
    }
    for(int i = 0, cnt = 0; i < size; ++i){
        if(state_red[position_black[i]] >= 'd' && state_red[position_black[i]] <= 'i'){
            r[position_black[i]] = chararray_black[cnt]; 
            b[reverse(position_black[i])] = swapcase(chararray_black[cnt]);
            ++cnt;
        }else{
            continue;
        }
    }
    random_map[true] = r;
    random_map[false] = b;
}
