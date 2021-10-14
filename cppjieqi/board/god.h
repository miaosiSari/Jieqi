#ifndef god_h
#define god_h

#include <memory>
#include <vector>
#include <string.h>
#include <unordered_set>
#include <iostream>
#include <functional>
#include <stdio.h>
#include "board.h"
#include "thinker.h"
#include "aiboard1.h"
#include "human.h"
#include "../global/global.h"
#include "../score/score.h"

namespace board{
    extern std::unordered_map<std::string, std::function<Thinker*(const char[], bool, int, const unsigned char [5][2][123], short, std::unordered_map<std::string, bool>)>> bean;  //define in ../global/global.cpp
}

#define INVALID -1
#define NORMAL 0
#define DRAW 0
#define BLACK_WIN 1
#define RED_WIN 2
#define MAX_ROUNDS 200
#define NEWRED(X) board::get_withprefix("AIBoard", X, board_pointer -> state_red, board_pointer -> turn, board_pointer -> round, board_pointer -> di_red, 0, board_pointer -> hist)
#define NEWBLACK(X) board::get_withprefix("AIBoard", X, board_pointer -> state_black, board_pointer -> turn, board_pointer -> round, board_pointer -> di_black, 0, board_pointer -> hist)

struct God{
    char eat = '.';
    bool ok = false;
    int type1 = 0;
    int type2 = 0;
    size_t redwin = 0;
    size_t blackwin = 0;
    size_t draw = 0;
    std::string file;
    std::vector<std::tuple<char, int, int, char>> red_eat_black;
    std::vector<std::tuple<char, int, int, char>> black_eat_red;
    std::unordered_set<std::string> hist_cache;
    board::Board* board_pointer;
    std::unique_ptr<board::Thinker> thinker1; //Red Thinker
    std::unique_ptr<board::Thinker> thinker2; //Black Thinker
    God()=delete;
    God(const char* file);
    bool Reset(const char* another_file, bool clear_winning_log);
    ~God();
    bool GetTurn();
    int StartThinker();
    int StartGameLoop(unsigned winning_threshold);
    int StartGame();
    std::string PrintEat(bool turn);

    std::function<std::string(const char)> getstring = [](const char c) -> std::string {
        std::string ret;
        const std::string c_string(1, c);
        ret = GetWithDefUnordered<std::string, std::string>(board::Board::uni_pieces, c_string, c_string);
        return ret;
    };

    std::function<bool(std::string)> check_legal = [](std::string s){
        if(s.size() != 4) return false;
        if(!(s[0] >= 'a' && s[0] <= 'i')) return false;
        if(!(s[1] >= '0' && s[1] <= '9')) return false;    
        if(!(s[2] >= 'a' && s[2] <= 'i')) return false;
        if(!(s[3] >= '0' && s[3] <= '9')) return false;  
        return true;
    };

    std::function<std::string(const char, bool, bool, bool)> isdot = [this](const char c, bool isdark, bool isend, bool turn) -> std::string {
        if(c == '.') return "";
        std::string ret = getstring(turn? ::toupper(c) : ::tolower(c));
        if(isdark && isend){
            ret = ret + "(暗).";
        }else if(isdark && !isend){
            ret = ret + "(暗), ";
        }else if(!isdark && isend){
            ret = ret + ".";
        }else{
            ret = ret + ", ";
        }
        return ret;
    };
};

#endif