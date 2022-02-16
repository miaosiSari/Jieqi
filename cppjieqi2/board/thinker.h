#ifndef thinker_h
#define thinker_h

#include "../global/global.h"
#include "../score/score.h"

namespace board{
    struct Thinker{
        bool thinker_type; //true: Human, false: AI
        int retry_num;
        bool turn;
        virtual std::string Think(int maxdepth) = 0;
    };
}

#endif
