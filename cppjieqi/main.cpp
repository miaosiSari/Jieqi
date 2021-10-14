// copyright 2021 miaosi@all rights reserved.
//#include <QCoreApplication>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "global/global.h"
#include "board/god.h"
#include "score/score.h"

extern bool read_score_table(const char* score_file);
extern bool debug(const char* debug_output_file);
extern bool initialize_wrapper(const char* score_file, const char* debug_output_file, float discount_factor=1.5);


int main(void) {
    srand(time(NULL));
    initialize_wrapper("../score.conf", "../kaijuku", "debug.log", 1.5);
    God g("../players.conf");
    g.StartGameLoop(19);
    return 0;
}
