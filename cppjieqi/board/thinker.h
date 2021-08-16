#ifndef thinker_h
#define thinker_h

namespace board{
    class Thinker{
    public:
        virtual std::string GetResult() = 0;
        virtual void CopyData(char di[5][2][123], char eat, bool turn) = 0;
    };
}

#endif