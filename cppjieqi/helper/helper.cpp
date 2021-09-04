#include "helper.h"

helper::Helper::Helper(){

}

helper::Helper::~Helper(){
    //RAII
    for(board::AIBoard* _aiboard : _aiboards){
        delete _aiboard;
    }
    for(Info* _result : _results){
        delete _result;
    }
}

bool helper::Helper::Read(const char* file){
    const int READ_INT = 0, READ_BOARD = 1, READ_COUNT = 2, READ_ROOTED = 3;
    int state = READ_INT;
    std::string line = "";
    std::ifstream instream(file);
    if(!instream.is_open()){
        printf("In helper --> Read(): instream is NOT open, return!\n");
        return false;
    }
    char boardstate[257];
    memset(boardstate, 0, 257);
    int tmp[100], lineno=0;
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
                printf("In helper --> Read(), lineno = %d, read illegal integer, id = %d, read_int = %d return!\n", lineno, id, read_int);
                return false;
            }
        }else if(state == READ_BOARD){
            if(tmpline.size() != 257){
                printf("In helper --> Read(), lineno = %d, tmpline.size() is %zu (not 257), ERROR!\n", lineno, tmpline.size());
                return false;
            }
            tmpline = std::regex_replace(tmpline, std::regex("@"), " ");
            tmpline[256] == 'T'?turn = true:turn = false;
            tmpline.pop_back();
            assert(tmpline.size() == 256);
            strncpy(boardstate, tmpline.c_str(), 256);
            boardstate[256] = '\0';
            board::AIBoard *aiboard = new (std::nothrow) board::AIBoard(boardstate, turn, 0, di);
            _aiboards.push_back(aiboard);
        }else if(state == READ_COUNT){
            memset(tmp, 0, sizeof(tmp));
            std::stringstream ss;
            ss << tmpline;
            int read_int = 0;
            while(ss >> tmp[read_int]){
                if(read_int > 13){
                   break;
                }
                ++read_int;
            }
            if(read_int != 13 || tmp[12] != 1){
                printf("In helper --> Read(), lineno = %d, read illegal integer, result = %d, read_int = %d, return!\n", lineno, result, read_int);
                return false;
            }
        }else if(state == READ_ROOTED){
            _results.push_back(NEWINFO(tmp, tmpline));
        }
        state = (state + 1) % 4;
        ++lineno;
    }
    if(_aiboards.size() != _results.size()){
        printf("In helper --> Read(): Size not match: _boards.size() = %zu, _results.size() = %zu!\n", _aiboards.size(), _results.size());
        return false;
    }
    printf("helper --> Read() Finishes! Read %zu groups of data!\n", _aiboards.size());
    return true;
}

bool helper::Helper::Compare(){
    size_t num_of_groups = _aiboards.size();
    for(size_t i = 0; i < num_of_groups; ++i){
        board::AIBoard* aiboard = _aiboards[i];
        aiboard -> SetScoreFunction("", 0);
        aiboard -> PrintPos(aiboard -> turn);
        aiboard -> GenMovesWithScore();
        aiboard -> Rooted();
        aiboard -> Scan();
        if(aiboard -> num_of_legal_moves != _results[i] -> result){
           printf("i = %zu, aiboard -> num_of_legal_moves = %d, _results[i] -> num_of_legal_moves = %d\n", i, aiboard -> num_of_legal_moves, _results[i] -> result);
           return false;
        }
        if(aiboard -> che != _results[i] -> che){
           printf("i = %zu, aiboard -> che = %d, _results[i] -> che = %d\n", i, aiboard -> che, _results[i] -> che);
           return false;
        }
        if(aiboard -> che_opponent != _results[i] -> che_opponent){
           printf("i = %zu, aiboard -> che_opponent = %d, _results[i] -> che_opponent = %d\n", i, aiboard -> che_opponent, _results[i] -> che_opponent);
           return false;
        }
        if(aiboard -> zu != _results[i] -> zu){
           printf("i = %zu, aiboard -> zu = %d, _results[i] -> zu = %d\n", i, aiboard -> zu, _results[i] -> zu);
           return false;
        }
        if(aiboard -> covered != _results[i] -> covered){
           printf("i = %zu, aiboard -> covered = %d, _results[i] -> covered = %d\n", i, aiboard -> covered, _results[i] -> covered);
           return false;
        }
        if(aiboard -> covered_opponent != _results[i] -> covered_opponent){
           printf("i = %zu, aiboard -> covered_opponent = %d, _results[i] -> covered_opponent = %d\n", i, aiboard -> covered_opponent, _results[i] -> covered_opponent);
           return false;
        }
        if(aiboard -> endline != _results[i] -> endline){
           printf("i = %zu, aiboard -> covered_opponent = %d, _results[i] -> covered_opponent = %d\n", i, aiboard -> covered_opponent, _results[i] -> covered_opponent);
           return false;
        }
        if(aiboard -> score_rough != _results[i] -> score_rough){
           printf("i = %zu, aiboard -> score_rough = %d, _results[i] -> score_rough = %d\n", i, aiboard -> score_rough, _results[i] -> score_rough);
           return false;
        }
        if(aiboard -> kongtoupao != _results[i] -> kongtoupao){
           printf("i = %zu, aiboard -> kongtoupao = %d, _results[i] -> kongtoupao = %d\n", i, aiboard -> kongtoupao, _results[i] -> kongtoupao);
           return false;
        }
        if(aiboard -> kongtoupao_opponent != _results[i] -> kongtoupao_opponent){
           printf("i = %zu, aiboard -> kongtoupao_opponent = %d, _results[i] -> kongtoupao_opponent = %d\n", i, aiboard -> kongtoupao_opponent, _results[i] -> kongtoupao_opponent);
           return false;
        }
        if(aiboard -> kongtoupao_score != _results[i] -> kongtoupao_score){
           printf("i = %zu, aiboard -> kongtoupao_score = %d, _results[i] -> kongtoupao_score = %d\n", i, aiboard -> kongtoupao_score, _results[i] -> kongtoupao_score);
           return false;
        }
        if(aiboard -> kongtoupao_score_opponent != _results[i] -> kongtoupao_score_opponent){
           printf("i = %zu, aiboard -> kongtoupao_score_opponent = %d, _results[i] -> kongtoupao_score_opponent = %d\n", i, aiboard -> kongtoupao_score_opponent, _results[i] -> kongtoupao_score_opponent);
           return false;
        }
        std::string resstring;
        if(aiboard -> rooted_chesses.size() == 0) {
            resstring = "@";
        }else{
            for(auto pos : aiboard -> rooted_chesses){
                resstring += (std::to_string(pos) + "@");
            }
        }
        if(resstring != _results[i] -> rooted_str){
            printf("i = %zu, resstring = %s, _results[i] -> rooted_str = %s", i, resstring.c_str(), _results[i] -> rooted_str.c_str());
            return false;
        }
        printf("Check %zu successful!\n", i);
    }
    return true;
}

bool helper::Helper::Debug(const char* debug_output_file){
    std::ofstream out_trunc(debug_output_file, std::ios::trunc|std::ios::out);
    std::ofstream out(debug_output_file, std::ios::app|std::ios::out);
    if(!out.is_open()){
        printf("File %s is not open!\n", debug_output_file);
        return false;
    }
    size_t num_of_groups = _aiboards.size();
    //size_t num_of_groups = 5;
    for(size_t i = 0; i < num_of_groups; ++i){
        board::AIBoard* aiboard = _aiboards[i];
        aiboard -> SetScoreFunction("complicated_score_function", 0);
        aiboard -> GenMovesWithScore();
        out << "\n" << i << "\n";
        out << (aiboard -> DebugPrintPos(aiboard -> turn));
        for(int j = 0; j < aiboard -> num_of_legal_moves; ++j){
            std::tuple<short, unsigned char, unsigned char> t = aiboard -> legal_moves[j];
            int x1 = 12 - (std::get<1>(t) >> 4);
            int y1 = (std::get<1>(t) & 15) - 3;
            int x2 = 12 - (std::get<2>(t) >> 4);
            int y2 = (std::get<2>(t) & 15) - 3;
            std::string ucci = "    ";
            ucci[0] = 'a' + y1;
            ucci[1] = '0' + x1;
            ucci[2] = 'a' + y2;
            ucci[3] = '0' + x2;
            out << j << " " << ucci << " " << std::get<0>(t) << "\n";
        }
    }
    return true;
}

bool helper::Helper::ReadAndCompare(const char* file, const char* debug_output_file=""){
    printf("STEP 1: Read from %s\n", file);
    if(!Read(file)) {
        printf("Read %s failed!\n", file);
        return false;
    }
    printf("STEP 2: Compare!\n");
    if(Compare()){
        printf("Compare result: true!\n");
    }else{
        printf("Compare result: false!\n");
        return false;
    }
    if(debug_output_file && strlen(debug_output_file) > 0){
        printf("STEP 3: DEBUG > %s\n", debug_output_file);
        return Debug(debug_output_file);
    }else{
        printf("SKIP Debug Step!\n");
        return true;
    }
    return true;
}


