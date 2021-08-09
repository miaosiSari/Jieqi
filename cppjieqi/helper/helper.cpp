#include "helper.h"

helper::Helper::Helper(){

}

helper::Helper::~Helper(){
    //RAII
    for(board::Board* board : _boards){
        delete board;
    }
}

void helper::Helper::Read(const char* file){
    const int READ_INT = 0, READ_BOARD = 1, READ_COUNT = 2;
    int state = READ_INT;
    std::string line = "";
    std::ifstream instream(file);
    if(!instream.is_open()){
        printf("In helper --> Read(): instream is NOT open, return!\n");
        return;
    }
    char boardstate[257];
    memset(boardstate, 0, 257);
    while(std::getline(instream, line)){
         int id = -1, result = -1;
         bool turn = true;         
         std::string tmpline = subtrim(line);
         int len = tmpline.size(); 
         if(len == 0)
         {
            continue;
         }
         if(state == READ_INT){
            std::stringstream ss;
            ss << tmpline;
            int read_int = 0;
            while(ss >> id){
               if(id < 0 || read_int > 1){
                   break;
               }
               ++read_int;
            }
            if(id < 0 || read_int > 1){
                printf("In helper --> Read(), read illegal integer, id = %d, read_int = %d return!\n", id, read_int);
                return;
            }
         }else if(state == READ_BOARD){
            if(tmpline.size() != 257){
                printf("In helper --> Read(), tmpline.size() is %zu (not 257), ERROR!\n", tmpline.size());
                return;
            }
            tmpline = std::regex_replace(tmpline, std::regex("@"), " ");
            tmpline[256] == 'T'?turn = true:turn = false;
            tmpline.pop_back();
            assert(tmpline.size() == 256);
            strncpy(boardstate, tmpline.c_str(), 256);
            boardstate[256] = '\0';
            board::Board *board = new (std::nothrow) board::Board(boardstate, turn, 0);
            _boards.push_back(board);
         }else if(state == READ_COUNT){
            std::stringstream ss;
            ss << tmpline;
            int read_int = 0;
            while(ss >> result){
               if(result < 0 || read_int > 1){
                   break;
               }
               ++read_int;
            }
            if(result < 0 || read_int > 1){
                printf("In helper --> Read(), read illegal integer, result = %d, read_int = %d, return!\n", result, read_int);
                return;
            }
            _results.push_back(result);
         }
         state = (state + 1) % 3;
    }
    if(_boards.size() != _results.size()){
        printf("In helper --> Read(): Size not match: _boards.size() = %zu, _results.size() = %zu!\n", _boards.size(), _results.size());
        return;
    }
    printf("helper --> Read() Finishes! Read %zu groups of data!\n", _boards.size());
}

bool helper::Helper::Compare(){
    size_t num_of_groups = _boards.size();
    for(size_t i = 0; i < num_of_groups; ++i){
       board::Board* board = _boards[i];
       board -> GenMovesWithScore();
       if(board -> num_of_legal_moves != _results[i]){
           printf("i = %zu, board -> num_of_legal_moves = %d, _results[i] = %d\n", i, board -> num_of_legal_moves, _results[i]);
           board -> PrintPos(board->GetTurn());
           board -> PrintAllMoves();
           return false;
       }
    }
    return true;
}


