// copyright 2021 miaosi@all rights reserved.
//#include <QCoreApplication>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "board/god.h"
#include "board/board.h"
#include "board/aiboard.h"
#include "log/log.h"
#include "global/global.h"
#include "score/score.h"
#include "helper/helper.h"

extern void register_scoring_functions();
extern bool read_score_table(const char* score_file);
extern bool debug(const char* debug_output_file);
extern bool initialize_wrapper(const char* score_file, const char* debug_output_file, float discount_factor=1.5);

int main(void) {
    initialize_wrapper("../score.conf", "../kaijuku", "debug.log", 1.5);
    register_score_functions();
    helper::Helper h;
    h.ReadAndCompare("../../log/py_log.txt", "cpp_score.txt");
    board::AIBoard b = board::AIBoard();
    //printf("%d\n", g.StartGame());
    return 0;
}
