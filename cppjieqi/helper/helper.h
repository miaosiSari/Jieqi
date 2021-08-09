#ifndef helper_h
#define helper_h
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include "../board/board.h"

namespace helper {
   class Helper{
   public:
      Helper();
      ~Helper();
      void Read(const char* file);
      bool Compare();
   private:
      std::vector<board::Board*> _boards;
      std::vector<int> _results;
   };
}
#endif
