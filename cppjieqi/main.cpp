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

const char test[MAX] = 
                    "                "
                    "                "
                    "                "
                    "   ....k....    "
                    "   .........    "
                    "   .........    "
                    "   .........    "
                    "   .........    "
                    "   .........    "
                    "   .........    "
                    "   .H.......    "
                    "   .........    "
                    "   ....K....    "
                    "                "
                    "                "
                    "                ";

int main(void) {
    helper::Helper h;
    register_score_functions();
    initialize_wrapper("../score.conf", "debug.log");
    //bool res = h.ReadAndCompare("../../log/log.txt", "score_debug.log"); 
    //std::cout << "ReadAndCompare = " << std::boolalpha << res << std::endl;
    /*
    board::AIBoard b = board::AIBoard();
    b.GenMovesWithScore();
    size_t start = (size_t)clock();
    //b.SetScoreFunction((std::string)"trivial_score_function", 0);
    for(int i = 0; i < 1; ++i){
        b.GenMovesWithScore();
    }
    std::cout << b.SearchScoreFunction(0) << std::endl;
    printf("%d\n", b.num_of_legal_moves);
    size_t end = (size_t)clock();
    printf("%d %lf\n", b.num_of_legal_moves, (double)(end - start)/CLOCKS_PER_SEC);
    */
    God g("../players.conf");
    printf("%d\n", g.StartGame());
    return 0;
}
