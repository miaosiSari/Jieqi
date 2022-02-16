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
#include "human.h"
#include "../global/global.h"
#include "../score/score.h"
#include "../board/aiboard4.h"


#define INVALID -1
#define NORMAL 0
#define BLACK_WIN 1
#define RED_WIN 2
#define WASTE 3
#define DRAW 4
#define MAX_ROUNDS 200

#define NEWRED new board::AIBoard4(board_pointer -> state_red, board_pointer -> turn, board_pointer -> round, board_pointer -> di_red, 0, tptable, &board_pointer -> hist)
#define NEWBLACK new board::AIBoard4(board_pointer -> state_black, board_pointer -> turn, board_pointer -> round, board_pointer -> di_black, 0, tptable, &board_pointer -> hist)

struct God{
    char eat = '.';
    bool ok = false;
    int type1 = 0;
    int type2 = 0;
    size_t winning_threshold_class = 0;
    size_t redwin = 0;
    size_t blackwin = 0;
    size_t draw = 0;
    tp* tptable;
    std::string file;
    std::string logfile;
    std::vector<std::tuple<char, int, int, char>> red_eat_black;
    std::vector<std::tuple<char, int, int, char>> black_eat_red;
    std::unordered_set<std::string> hist_cache;
    board::Board* board_pointer;
    std::unique_ptr<board::Thinker> thinker1; //Red Thinker
    std::unique_ptr<board::Thinker> thinker2; //Black Thinker
    God()=delete;
    God(const char* file);
    ~God();
    bool GetTurn();
    int StartThinker(std::ofstream* of);
    void Play(std::string logfile);
    void Play();
    int StartGame();
    int StartGameLoop(size_t winning_threshold);
    int StartGameLoop();
    int StartGameLoopAlternatively(size_t winning_threshold);//红黑交替
    int StartGameLoopAlternatively();//红黑交替
    std::string PrintEat(bool turn, bool SHOWDARK);

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

    std::function<std::string(const char, bool, bool)> isdot = [this](const char c, bool isdark, bool turn) -> std::string {
        if(c == '.') return "";
        std::string ret = getstring(turn? ::toupper(c) : ::tolower(c));
        if(isdark){
            ret = ret + "(暗) ";
        }else{
            ret = ret + " ";
        }
        return ret;
    };
};

#endif