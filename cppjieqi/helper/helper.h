#ifndef helper_h
#define helper_h
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include "../board/aiboard.h"

#define NEWINFO(arr) new Info(arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8], arr[9], arr[10], arr[11])

struct Info{
   int result = 0;
   unsigned char che = 0;
   unsigned char che_opponent = 0;
   unsigned char zu = 0;
   unsigned char covered = 0;
   unsigned char covered_opponent = 0;
   unsigned char endline = 0;
   short score_rough = 0;
   unsigned char kongtoupao = 0;
   unsigned char kongtoupao_opponent = 0;
   short kongtoupao_score = 0;
   short kongtoupao_score_opponent = 0;
   Info()=default;
   ~Info()=default;
   Info(int result, unsigned char che, unsigned char che_opponent, unsigned char zu, unsigned char covered, unsigned char covered_opponent, unsigned char endline, short score_rough, unsigned char kongtoupao, \
      unsigned char kongtoupao_opponent, short kongtoupao_score, short kongtoupao_score_opponent):result(result), che(che), che_opponent(che_opponent), zu(zu), covered(covered), covered_opponent(covered_opponent), \
      endline(endline), score_rough(score_rough), kongtoupao(kongtoupao), kongtoupao_opponent(kongtoupao_opponent), kongtoupao_score(kongtoupao_score), kongtoupao_score_opponent(kongtoupao_score_opponent){

      }
   Info(const Info& anotherinfo):result(anotherinfo.result), che(anotherinfo.che), che_opponent(anotherinfo.che_opponent), zu(anotherinfo.zu), covered(anotherinfo.covered), covered_opponent(anotherinfo.covered_opponent), \
      endline(anotherinfo.endline), score_rough(anotherinfo.score_rough), kongtoupao(anotherinfo.kongtoupao), kongtoupao_opponent(anotherinfo.kongtoupao_opponent), kongtoupao_score(anotherinfo.kongtoupao_score), kongtoupao_score_opponent(anotherinfo.kongtoupao_score_opponent){

   }
};

namespace helper {
   class Helper{
   public:
      Helper();
      ~Helper();
      bool Read(const char* file);
      bool Compare();
      bool Debug(const char* debug_output_file);
      bool ReadAndCompare(const char* file, const char* debug_output_file);
   private:
      std::vector<board::AIBoard*> _aiboards;
      std::vector<Info*> _results;
   };
}
#endif
