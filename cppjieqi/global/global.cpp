#include "global.h"

namespace board{
    std::unordered_map<std::string, std::function<Thinker*(const char[], bool, int, const unsigned char [5][2][123], short, std::unordered_map<std::string, bool>*)>> bean; 
    int register_func(std::string x, std::function<Thinker*(const char[], bool, int, const unsigned char [5][2][123], short score, std::unordered_map<std::string, bool>*)> y){
       bean.insert({x, y});
       return 0;
    }
}
