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
#include "aiboard.h"
#include "human.h"
#include "../global/global.h"
#include "../score/score.h"

#define INVALID -1
#define NORMAL 0
#define DRAW 0
#define BLACK_WIN 1
#define RED_WIN 2
#define MAX_ROUNDS 200

extern unsigned char di[VERSION_MAX][2][123];

struct God{
    char di[VERSION_MAX][2][123];
    char di_red[VERSION_MAX][2][123];
    char di_black[VERSION_MAX][2][123];
    char eat = '.';
    bool ok = false;
    bool type1 = false;
    bool type2 = false;
    std::vector<std::tuple<char, int, int>> red_eat_black;
    std::vector<std::tuple<char, int, int>> black_eat_red;
    std::unordered_set<std::string> hist_cache;
    board::Board* board_pointer;
    std::unique_ptr<board::Thinker> thinker1; //Red Thinker
    std::unique_ptr<board::Thinker> thinker2; //Black Thinker
    God()=delete;
    God(const char* file);
    ~God();
    void initialize_di();
    bool GetTurn();
    int StartThinker();
    int StartGame();
    std::string PrintEat(bool turn);
    std::function<bool(std::string)> check_legal = [](std::string s){
        if(s.size() != 4) return false;
        if(!(s[0] >= 'a' && s[0] <= 'i')) return false;
        if(!(s[1] >= '0' && s[1] <= '9')) return false;    
        if(!(s[2] >= 'a' && s[2] <= 'i')) return false;
        if(!(s[3] >= '0' && s[3] <= '9')) return false;  
        return true;
    };
};

#endif