#include "board.h"

const int board::Board::_chess_board_size = CHESS_BOARD_SIZE;
const char board::Board::_initial_state[MAX] = 
                    "               \n"
                    "               \n"
                    "               \n"
                    "   defgkgfed   \n"
                    "   .........   \n"
                    "   .h.....h.   \n"
                    "   i.i.i.i.i   \n"
                    "   .........   \n"
                    "   .........   \n"
                    "   I.I.I.I.I   \n"
                    "   .H.....H.   \n"
                    "   .........   \n"
                    "   DEFGKGFED   \n"
                    "               \n"
                    "               \n"
                    "               \n";

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

board::Board::Board() noexcept:_has_initialized(false),
                      _turn(true),
                      _round(0),
                      _score_func(NULL),
                      _num_of_legal_moves(0) {
    memset(_state, 0, sizeof(_state));
    strncpy(_state, _initial_state, _chess_board_size);
    memset(_is_legal_move, false, sizeof(_is_legal_move));
    _log = Singleton<log::Log>::get();
    _has_initialized = true;
}

void Reset() noexcept{
	_turn = true;
	_round = 0;
	_score_func = NULL;
	_num_of_legal_moves = 0;
	memset(_state, 0, sizeof(_state));
    strncpy(_state, _initial_state, _chess_board_size);
    memset(_is_legal_move, false, sizeof(_is_legal_move));
    _log = Singleton<log::Log>::get();
}

std::vector<int> board::Board::GetInfo() const{
    std::vector<int> ret(_state, _state+board::Board::_chess_board_size);
    ret.push_back(_turn?1:0);
    ret.push_back(_round);
    _log -> Write("board::Board::GetInfo");
    return ret;
}

const std::vector<std::string>& board::Board::GetHistory() const{
    return _board_history;
}

std::string board::Board::GetStateString() const{
    std::string tmp(_state);
    tmp = tmp.substr(0, board::Board::_chess_board_size);
    _log -> Write("board::Board::GetStateString");
    return tmp;
}

bool board::Board::GetTurn() const{
	_log -> Write("board::Board::GetTurn");
    return _turn;
}

bool board::Board::GetRound() const{
	_log -> Write("board::Board::GetRound");
    return _round;
}

std::tuple<int, bool, std::string> board::Board::GetTuple() const{
	_log -> Write("board::Board::GetTuple");
    std::string tmp(_state);
    tmp = tmp.substr(0, board::Board::_chess_board_size);
    std::tuple<int, bool, std::string> ret(_round, _turn, tmp);
    return ret;
}

const std::unordered_map<std::string, std::string>& board::Board::GetUniPieces() const{
	_log -> Write("board::Board::GetUniPieces");
	return _uni_pieces;
}

void board::Board::PrintPos() const{
	_log -> Write("board::Board::PrintPos");
    if(_turn){
        printf("红方行棋:\n");
    }else{
        printf("黑方行棋:\n");
    }
    std::cout << std::endl << std::endl;
    for(int x = 3; x <= 12; ++x){
    	std::cout << translate_x(x) << " ";
    	for(int y = 3; y <= 11; ++y){
    		std::cout << _getstringxy(x, y);
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
	if(check) {
        if(_is_legal_move[encode_from][encode_to] == false){
        	return;
        }
	}
	_state[encode_to] = _state[encode_from];
	_state[encode_from] = '.';
}

void board::Board::GenMovesWithScore(){
    //To make it more efficient, this implementation is rather dirty
    _num_of_legal_moves = 0;
    for(unsigned char i = 51; i <= 203; ++i){
    	char p = _state[i];
    	if(!(p >= 'A' && p <= 'Z') ! p == 'U') {
    		continue;
    	}
    	if(p == 'K'){
    		
    	}
    }
}

inline void board::Board::_reset_num_of_legal_moves() const{
	_num_of_legal_moves = 0;
}

const int get_number_of_legal_moves() const{
	return _num_of_legal_moves;
}

