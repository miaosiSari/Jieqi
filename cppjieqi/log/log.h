#ifndef log_h
#define log_h

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../global/global.h"

namespace logclass{
    class Log{
    public:
        Log();
        ~Log();
        Log(const Log&)=default;
        void SetConfig(const char* config, bool=true);
        void SetConfig(std::string config, bool=true);
        void Write(const std::string content) const;
        void Reset();
    private:
        char* _config;
        std::ostream* _os;
        std::ofstream _fout;
        bool _append;
    };
}
#endif