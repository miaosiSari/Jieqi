// copyright 2021 miaosi@all rights reserved.
//#include <QCoreApplication>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "board/god.h"
#include "board/board.h"
#include "board/aiboard.h"
#include "global/global.h"
#include "score/score.h"

extern void register_scoring_functions();
extern bool read_score_table(const char* score_file);
extern bool debug(const char* debug_output_file);
extern bool initialize_wrapper(const char* score_file, const char* debug_output_file, float discount_factor=1.5);


int main(void) {
	#ifdef WIN32
	srand(time(NULL));
	#endif
    initialize_wrapper("../score.conf", "../kaijuku", "debug.log", 1.5);
    register_score_functions();
    board::AIBoard x;
    God g("../players.conf");
    g.StartGame();
    return 0;
}
