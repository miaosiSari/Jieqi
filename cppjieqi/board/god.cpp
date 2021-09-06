#include "god.h"
#include "board.h"

God::God(const char* file): redwin(0), blackwin(0), file(file){
   if(!file || (this -> file).size() == 0){
      ok = false;
      return;
   }
   board_pointer = Singleton<board::Board>::get();
   if(!board_pointer){
      ok = false;
      return;
   }
   initialize_di();
   std::ifstream instream(file);
   int counter = 0;
   std::string line;
   if(!instream.is_open()){
      ok = false;
      return;
   }
   while(std::getline(instream, line)){
      if(counter >= 2){
         ok = false;
         return;
      }
      line = trim(line);
      if(line.size() != 1) {
         ok = false;
         return;
      }
      if(line[0] != '0' && line[0] != '1'){
         ok = false;
         return;
      }else if(line[0] == '0'){
         if(counter == 0){
            type1 = false;
         }else{
            type2 = false;
         }
      }else if(line[0] == '1'){
         if(counter == 0){
            type1 = true;
         }else{
            type2 = true;
         }
      }
      ++counter;
   }
   printf("I am the referee. You two must listen to me!\n");
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
   }
   ok = Singleton<board::Board>::reset();
   if(!ok) return false;
   board_pointer = Singleton<board::Board>::get();
   initialize_di();
   if(another_file){
      file = another_file;
   }
   std::ifstream instream(file.c_str());
   int counter = 0;
   std::string line;
   if(!instream.is_open()){
      ok = false;
      return false;
   }
   while(std::getline(instream, line)){
      if(counter >= 2){
         ok = false;
         return false;
      }
      line = trim(line);
      if(line.size() != 1) {
         ok = false;
         return false;
      }
      if(line[0] != '0' && line[0] != '1'){
         ok = false;
         return false;
      }else if(line[0] == '0'){
         if(counter == 0){
            type1 = false;
         }else{
            type2 = false;
         }
      }else if(line[0] == '1'){
         if(counter == 0){
            type1 = true;
         }else{
            type2 = true;
         }
      }
      ++counter;
   }
   printf("I am the referee. You two must listen to me!\n");
   ok = true;
   instream.close();
   return true;
}

void God::initialize_di(){
   memmove(this -> di, ::di, sizeof(::di));
   memmove(this -> di_red, ::di, sizeof(::di));
   memmove(this -> di_black, ::di, sizeof(::di));
}

int God::StartThinker(){
   if(!ok) return -1;
   board_pointer -> GenMovesWithScore();
   board_pointer -> CopyToIsLegalMove();
   if(board_pointer -> turn){
      if(type1){
         printf("红方行棋!\n");
         std::cout << PrintEat(board_pointer -> turn) << std::endl;
         board_pointer -> PrintPos(board_pointer -> turn, true, false, true);
         thinker1.reset(new board::Human(board_pointer -> turn, board_pointer -> round));
      }else{
         thinker1.reset(new board::AIBoard(board_pointer -> state_red, board_pointer -> turn, board_pointer -> round, this -> di_red, 0));
      }
      thinker1 -> thinker_type = type1;
      thinker1 -> retry_num = thinker1 -> thinker_type?3:1;
      for(int i = 0; i < thinker1 -> retry_num; ++i){
         std::string think_result = thinker1 -> Think(); // This function might cost a lot of time!
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
      if(type2){
         printf("黑方行棋!\n");
         std::cout << PrintEat(board_pointer -> turn) << std::endl;
         board_pointer -> PrintPos(board_pointer -> turn, true, false, true);
         thinker2.reset(new board::Human(board_pointer -> turn, board_pointer -> round));
      }else{
         thinker2.reset(new board::AIBoard(board_pointer -> state_black, board_pointer -> turn, board_pointer -> round, this -> di_black, 0));
      }
      thinker2 -> thinker_type = type1;
      thinker2 -> retry_num = thinker1 -> thinker_type?3:1;
      for(int i = 0; i < thinker2 -> retry_num; ++i){
         std::string think_result = thinker2 -> Think(); // This function might cost a lot of time!
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
   int result = NORMAL;
   board_pointer -> Reset();
   while(result == NORMAL && board_pointer -> round < MAX_ROUNDS){
      result = StartThinker();
      if(result == RED_WIN){
         printf("红胜!\n");
         return RED_WIN;
      }else if(result == BLACK_WIN){
         printf("黑胜!\n");
         return BLACK_WIN;
      }
   }
   return DRAW;
}

bool God::GetTurn(){
   return board_pointer -> turn;
}

std::string God::PrintEat(bool turn){
   std::string ret;
   if(turn){
      ret += "黑吃红: ";
      for(size_t i = 0; i < black_eat_red.size(); ++i){
         ret += isdot(std::get<0>(black_eat_red[i]), false, (i == (black_eat_red.size() - 1)));
      }
      ret += '\n';
      ret += "红吃黑: ";
      for(size_t i = 0; i < red_eat_black.size(); ++i){
         auto tuple = red_eat_black[i];
         if(std::get<1>(tuple) == 2){
            char covered = board_pointer -> random_map[turn][std::get<2>(tuple)];
            assert(covered == std::get<3>(tuple));
            ret += isdot(covered, true, (i == (red_eat_black.size() - 1)));
         }else{
            ret += isdot(std::get<0>(tuple), false, (i == (red_eat_black.size() - 1)));
         }
      }
   }else{
      ret += "红吃黑: ";
      for(size_t i = 0; i < red_eat_black.size(); ++i){
         ret += isdot(std::get<0>(red_eat_black[i]), false, (i == (red_eat_black.size() - 1)));
      }
      ret += '\n';
      ret += "黑吃红: ";
      for(size_t i = 0; i < black_eat_red.size(); ++i){
         auto tuple = black_eat_red[i];
         if(std::get<1>(tuple) == 2){
            char covered = board_pointer -> random_map[turn][std::get<2>(tuple)];
            assert(covered == std::get<3>(tuple));
            std::cout << "turn = " << turn << ", place = " << std::get<2>(tuple) << ", char = " << covered << ".\n";
            ret += isdot(covered, true, (i == (black_eat_red.size() - 1)));
         }else{
            ret += isdot(std::get<0>(tuple), false, (i == (black_eat_red.size() - 1)));
         }
      }
   }
   return ret;
}