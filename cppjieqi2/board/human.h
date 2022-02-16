#ifndef human_h
#define human_h

#include "../global/global.h"
#include "../score/score.h"
#include "thinker.h"
#include <iostream>

namespace board{
    struct Human: public Thinker{
    	bool turn;
    	int round;
        Human();
        Human(bool turn, int round);
        virtual ~Human();
        virtual std::string Think(int maxdepth);
    };
}
#endif
