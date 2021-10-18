#include "god.h"

namespace board{
   template<typename... Args>
   Thinker* get(std::string x, Args... args){
      if(bean.find(x) != bean.end()){
         return bean[x](args...);
      }
      return NULL;
   }

   template<typename... Args>
   Thinker* get_withprefix(std::string prefix, std::string x, Args... args){
      return get(prefix + x, args...);
   }

   template<typename T, typename... Args>
   typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, Thinker*>::type get_withprefix(std::string prefix, T x, Args... args){
      return get(prefix + std::to_string(x), args...);
   }
}

template<typename T>
bool isT(std::string s, T* i){
   //用于判断字符串是否为整数/浮点型
   std::stringstream ss;
   ss << s;
   ss >> *i;
   if(!ss.fail() && ss.eof()) { 
      return true; 
   }else{
      *i = -1;
      return false;
   }
}


God::God(const char* file): ok(true), redwin(0), blackwin(0), draw(0), file(file){
   if(!file || (this -> file).size() == 0){
      ok = false;
      return;
   }
   board_pointer = Singleton<board::Board>::get();
   std::ifstream instream(file);
   int counter = 0;
   std::string line;
   if(!instream.is_open()){
      ok = false;
      assert(false);
      return;
   }
   while(std::getline(instream, line)){
      if(counter >= 2){
         ok = false;
      }
      line = trim(line);

      if(counter == 0){
         if(!isT<int>(line, &type1)){
            ok = false;
         }
      }

      if(counter == 1){
         if(!isT<int>(line, &type2)){
            ok = false;
         }
      }

      ++counter;
   }
   printf("I am the referee. You two must listen to me! player1 = %d, player2 = %d\n", type1, type2);
   assert(ok);
   ok = true;
   instream.close();
}

God::~God(){
   Singleton<board::Board>::deleteT();
   if(thinker1) thinker1.reset();
   if(thinker2) thinker2.reset();
}

bool God::Reset(const char* another_file, bool clear_winning_log){
   if(clear_winning_log){
      redwin = 0;
      blackwin = 0;
      draw = 0;
   }
   ok = Singleton<board::Board>::reset();
   assert(ok);
   board_pointer = Singleton<board::Board>::get();
   if(another_file){
      file = another_file;
   }
   std::ifstream instream(file.c_str());
   int counter = 0;
   std::string line;
   if(!instream.is_open()){
      ok = false;
   }
   while(std::getline(instream, line)){
      if(counter >= 2){
         ok = false;
      }
      line = trim(line);
      if(counter == 0){
         if(!isT<int>(line, &type1)){
            ok = false;
         }
      }

      if(counter == 1){
         if(!isT<int>(line, &type2)){
            ok = false;
         }
      }
      ++counter;
   }
   printf("I am the referee. You two must listen to me! player1 = %d, player2 = %d\n", type1, type2);
   ok = true;
   instream.close();
   assert(ok);
   return true;
}


int God::StartThinker(){
   if(!ok) return -1;
   board_pointer -> GenMovesWithScore();
   board_pointer -> CopyToIsLegalMove();
   #if DEBUG
   board_pointer -> DebugDI();
   #endif
   if(board_pointer -> turn){
      if(type1 == 0){
         printf("红方行棋!\n");
         std::cout << PrintEat(board_pointer -> turn) << std::endl;
         board_pointer -> PrintPos(board_pointer -> turn, true, false, true);
         thinker1.reset(new board::Human(board_pointer -> turn, board_pointer -> round));
      }else{
         thinker1.reset(NEWRED(type1));
      }
      if(!thinker1){
         printf("红空指针!\n");
         return BLACK_WIN;
      }
      thinker1 -> thinker_type = type1;
      thinker1 -> retry_num = thinker1 -> thinker_type?1:5;
      for(int i = 0; i < thinker1 -> retry_num; ++i){
         std::string think_result = thinker1 -> Think(); // This function might cost a lot of time!
         if(trim(think_result) == "W"){
            return RED_WIN;
         }else if(trim(think_result) == "R"){
            return BLACK_WIN;
         }else if(trim(think_result) == "D"){
            return DRAW;
         }else if(trim(think_result) == "P"){
            return WASTE;
         }
         if(!check_legal(think_result)) continue;
         std::shared_ptr<InfoDict> p = board_pointer -> Move(think_result, true);
         if(p && p -> islegal){
            red_eat_black.push_back({p -> eat, p -> eat_type, 195 - 16 * p -> dst_x + p -> dst_y, p -> eat_check});
            board_pointer -> PrintPos(!board_pointer -> turn, true, false, true);
            printf("第%d轮红方行棋结束\n========================================\n\n", board_pointer -> round);
            return p -> win ? RED_WIN : NORMAL;
         }
      }
      printf("红方连续%d个无效棋步, 判输!\n", thinker1 -> retry_num);
      return BLACK_WIN;
   }else{
      if(type2 == 0){
         printf("黑方行棋!\n");
         std::cout << PrintEat(board_pointer -> turn) << std::endl;
         board_pointer -> PrintPos(board_pointer -> turn, true, false, true);
         thinker2.reset(new board::Human(board_pointer -> turn, board_pointer -> round));
      }else{
         thinker2.reset(NEWBLACK(type2));
      }
      if(!thinker2){
         printf("黑空指针!\n");
         return RED_WIN;
      }
      thinker2 -> thinker_type = type2;
      thinker2 -> retry_num = thinker2 -> thinker_type?1:5;
      for(int i = 0; i < thinker2 -> retry_num; ++i){
         std::string think_result = thinker2 -> Think(); // This function might cost a lot of time!
         if(trim(think_result) == "R"){
            return RED_WIN;
         }else if(trim(think_result) == "W"){
            return BLACK_WIN;
         }else if(trim(think_result) == "D"){
            return DRAW;
         }else if(trim(think_result) == "P"){
            return WASTE;
         }
         if(!check_legal(think_result)) continue;
         std::shared_ptr<InfoDict> p = board_pointer -> Move(think_result, true);
         if(p && p -> islegal){
            black_eat_red.push_back({p -> eat, p -> eat_type, 195 - 16 * p -> dst_x + p -> dst_y, p -> eat_check});
            board_pointer -> PrintPos(!board_pointer -> turn, true, false, true);
            printf("第%d轮黑方行棋结束\n========================================\n\n", board_pointer -> round);
            return p -> win ? BLACK_WIN : NORMAL;
         }
      }
      printf("黑方连续%d个无效棋步, 判输!\n", thinker2 -> retry_num);
      return RED_WIN;
   }
   return NORMAL;
}

int God::StartGame(){
   red_eat_black.clear();
   black_eat_red.clear();
   int result = NORMAL;
   board_pointer -> Reset();
   while(result == NORMAL && board_pointer -> round < MAX_ROUNDS){
      result = StartThinker();
      if(result == -1) {
         printf("BUG!\n");
         return -1;
      }
      if(result == RED_WIN){
         printf("红胜!\n");
         ++redwin;
         return RED_WIN;
      }else if(result == BLACK_WIN){
         printf("黑胜!\n");
         ++blackwin;
         return BLACK_WIN;
      }
      if(result == WASTE){
         return WASTE;
      }
      if(result == DRAW){
         ++draw;
         return DRAW;
      }
   }
   ++draw;
   return DRAW;
}

int God::StartGameLoop(unsigned winning_threshold){
   size_t i = 0;
   redwin = 0;
   draw = 0;
   blackwin = 0;
   size_t maxgame = 2 * winning_threshold - 1;
   while(i < maxgame && draw < maxgame){
      int state = StartGame();
      printf("红%zu : 和%zu : 黑%zu\n", redwin, draw, blackwin);
      if(state != DRAW && state != WASTE){
         ++i;
      }
      if(redwin >= winning_threshold){
         printf("红方最终胜利!\n");
         return RED_WIN;
      }
      if(blackwin >= winning_threshold){
         printf("黑方最终胜利!\n");
         return BLACK_WIN;
      }
   }
   printf("握手言和\n");
   return DRAW;
}

int God::StartGameLoopAlternatively(unsigned winning_threshold){
   redwin = 0;
   draw = 0;
   blackwin = 0;
   size_t player1win = 0;
   size_t player2win = 0;
   size_t i = 0;
   bool cnt = true;
   size_t maxgame = 2 * winning_threshold - 1;
   while(i < maxgame && draw < maxgame){
      printf("cnt = %d. Player1_win %zu : Draw %zu : Player2_win %zu\n", cnt, player1win, draw, player2win);
      int state = StartGame();
      if(cnt){
         if(state == RED_WIN){
            ++player1win;
            ++i;
         }else if(state == BLACK_WIN){
            ++player2win;
            ++i;
         }
      }else{
         if(state == RED_WIN){
            ++player2win;
            ++i;
         }else if(state == BLACK_WIN){
            ++player1win;
            ++i;
         }
      }
      if(player1win >= winning_threshold){
         printf("玩家1获胜!\n");
         return 1;
      }else if(player2win >= winning_threshold){
         printf("玩家2获胜!\n");
         return 2;
      }
      cnt = !cnt;
      std::swap(type1, type2);
   }
   printf("握手言和!\n");
   return 0;
}

bool God::GetTurn(){
   return board_pointer -> turn;
}

std::string God::PrintEat(bool turn){
   std::string ret;
   if(turn){
      ret += "黑吃红: ";
      for(size_t i = 0; i < black_eat_red.size(); ++i){
         #ifdef SHOWDARK
         auto tuple = black_eat_red[i];
         if(std::get<1>(tuple) == 2){
            char covered = board_pointer -> random_map[!turn][std::get<2>(tuple)];
            assert(covered == std::get<3>(tuple));
            ret += isdot(covered, true, (i == (black_eat_red.size() - 1)), true);
         }else{
            ret += isdot(std::get<0>(tuple), false, (i == (black_eat_red.size() - 1)), true);
         }
         #else
         ret += isdot(std::get<0>(black_eat_red[i]), false, (i == (black_eat_red.size() - 1)), true);
         #endif
      }
      ret += '\n';
      ret += "红吃黑: ";
      for(size_t i = 0; i < red_eat_black.size(); ++i){
         auto tuple = red_eat_black[i];
         if(std::get<1>(tuple) == 2){
            char covered = board_pointer -> random_map[turn][std::get<2>(tuple)];
            assert(covered == std::get<3>(tuple));
            ret += isdot(covered, true, (i == (red_eat_black.size() - 1)), false);
         }else{
            ret += isdot(std::get<0>(tuple), false, (i == (red_eat_black.size() - 1)), false);
         }
      }
   }else{
      ret += "红吃黑: ";
      for(size_t i = 0; i < red_eat_black.size(); ++i){
         #ifdef SHOWDARK
         auto tuple = red_eat_black[i];
         if(std::get<1>(tuple) == 2){
            char covered = board_pointer -> random_map[!turn][std::get<2>(tuple)];
            assert(covered == std::get<3>(tuple));
            ret += isdot(covered, true, (i == (red_eat_black.size() - 1)), false);
         }else{
            ret += isdot(std::get<0>(tuple), false, (i == (red_eat_black.size() - 1)), false);
         }
         #else
         ret += isdot(std::get<0>(red_eat_black[i]), false, (i == (red_eat_black.size() - 1)), false);
         #endif
      }
      ret += '\n';
      ret += "黑吃红: ";
      for(size_t i = 0; i < black_eat_red.size(); ++i){
         auto tuple = black_eat_red[i];
         if(std::get<1>(tuple) == 2){
            char covered = board_pointer -> random_map[turn][std::get<2>(tuple)];
            assert(covered == std::get<3>(tuple));
            ret += isdot(covered, true, (i == (black_eat_red.size() - 1)), true);
         }else{
            ret += isdot(std::get<0>(tuple), false, (i == (black_eat_red.size() - 1)), true);
         }
      }
   }
   return ret;
}
