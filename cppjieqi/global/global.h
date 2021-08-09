#ifndef global_h
#define global_h

#include <map>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <regex>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */


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
class Singleton{
private:
   static T* _t;
public:
   static T* get();
   static void deleteT();
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

#endif
