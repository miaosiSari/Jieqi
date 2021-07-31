#include "board.h"
#include "../global/global.h"

const int board::Board::_chess_board_size = 256;
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
                      _round(0) {
    
    memset(_state, 0, sizeof(_state));
    strncpy(_state, _initial_state, _chess_board_size);
    _has_initialized = true;
}

std::vector<int> board::Board::GetInfo() const{
    std::vector<int> ret(_state, _state+board::Board::_chess_board_size);
    ret.push_back(_turn?1:0);
    ret.push_back(_round);
    return ret;
}

const std::vector<std::string>& board::Board::GetHistory() const{
    return _board_history;
}

std::string board::Board::GetStateString() const{
    std::string tmp(_state);
    tmp = tmp.substr(0, board::Board::_chess_board_size);
    return tmp;
}

bool board::Board::GetTurn() const{
    return _turn;
}

bool board::Board::GetRound() const{
    return _round;
}

std::tuple<int, bool, std::string> board::Board::GetTuple() const{
    std::string tmp(_state);
    tmp = tmp.substr(0, board::Board::_chess_board_size);
    std::tuple<int, bool, std::string> ret(_round, _turn, tmp);
    return ret;
}

const std::unordered_map<std::string, std::string>& board::Board::GetUniPieces() const{
	return _uni_pieces;
}

void board::Board::PrintPos() const{
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

void board::Board::move(std::pair<int, int> start, std::pair<int, int> end, bool check=false){
	move(start.first, start.second, end.first, end.second, check);
}

void board::Board::move(std::string ucci){
	//the ucci string is in "a0a1“ format.
	//Please check https://www.xqbase.com/protocol/cchess_ucci.htm
	assert(ucci.size() == 4);
    int x1 = (int)()
}

