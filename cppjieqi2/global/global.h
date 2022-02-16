#ifndef global_h
#define global_h

#define DEBUG 0
#define BLACK 0
#include <map>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <regex>
#include <functional>
#include <iostream>
#include <type_traits>

namespace board{
   struct Thinker;
}

//GetWithDef: If K does not exist, return the default value!
template <typename K, typename V>
V GetWithDef(const  std::map<K,V>& m, const K  key, const V& defval) {
   typename std::map<K,V>::const_iterator it = m.find( key );
   if ( it == m.end() ) {
      return defval;
   }
   else {
      return it->second;
   }
}

template <typename K, typename V>
V GetWithDefUnordered(const std::unordered_map<K,V>& m, const K& key, const V& defval) {
   typename std::unordered_map<K,V>::const_iterator it = m.find(key);
   if(it == m.end()) {
      return defval;
   }
   else {
      return it->second;
   }
}

template<typename T>
inline void hash_combine(std::size_t& seed, const T& val)
{
    std::hash<T> hasher;
    seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

//  taken from https://stackoverflow.com/a/7222201/916549
//
template<typename S, typename T>
struct myhash
{
    inline size_t operator()(const std::pair<S, T>& val) const
    {
        size_t seed = 0;
        hash_combine(seed, val.first);
        hash_combine(seed, val.second);
        return seed;
    }
};

template <typename T, typename U, typename V>
bool GreaterTuple(const std::tuple<T, U, V> &i, const std::tuple<T, U, V> &j) {
        return std::get<0>(i) > std::get<0>(j);
}

template<typename T>
class Singleton{
private:
   static T* _t;
public:
   static T* get();
   static void deleteT();
   static bool reset();
};

template<typename T>
T* Singleton<T>::_t = new (std::nothrow) T;

template<typename T>
T* Singleton<T>::get(){
   return _t;
}

template<typename T>
void Singleton<T>::deleteT(){
   delete Singleton<T>::_t;
}

template<typename T>
bool Singleton<T>::reset(){
   delete _t;
   _t = new (std::nothrow) T;
   return (_t != NULL);
}

inline std::string trim(const std::string &s)
{
    auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
    auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
    return (wsback <= wsfront ? std::string() : std::string(wsfront,wsback));
}

inline std::string sub(const std::string &s){
    return std::regex_replace(s, std::regex(","), " ");
}

inline std::string subtrim(const std::string &s){
    return sub(trim(s));
}

struct InfoDict{
   bool islegal;
   bool turn;
   int round;
   bool win;
   char eat;
   char eat_rb;
   int eat_type; //0:'.', 1:明子, 2:暗子
   int src_x;
   int src_y;
   int dst_x;
   int dst_y;
   char eat_check;
   InfoDict()=delete;
   InfoDict(const bool islegal, const bool turn, const int round, bool win, const char eat, const char eat_rb, const int eat_type, const int src_x, const int src_y, const int dst_x, const int dst_y, const char eat_check):islegal(islegal), turn(turn), round(round), win(win), eat(eat), eat_rb(eat_rb), eat_type(eat_type), src_x(src_x), src_y(src_y), dst_x(dst_x), dst_y(dst_y), eat_check(eat_check){}
   InfoDict(const InfoDict& another){
      islegal = another.islegal;
      turn = another.turn;
      round = another.round;
      eat = another.eat;
      eat_rb = another.eat_rb;
      eat_type = another.eat_type;
      src_x = another.src_x;
      src_y = another.src_y;
      dst_x = another.dst_x;
      dst_y = another.dst_y;
   }
   ~InfoDict()=default;
};

struct tp{
   uint64_t key;
   bool turn;
   int alphadepth;
   int betadepth;
   short alphaval;
   short betaval;
   short score;
   unsigned char src;
   unsigned char dst;
   int recordplace;
};

#endif
