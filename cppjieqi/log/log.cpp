#include "log.h"

log::Log::Log():_config(NULL), _os(&std::cout){

}

void log::Log::SetConfig(const char* config, bool append){
    Reset();
    size_t len = (size_t)strlen(config);
    _os = &std::cout;
    if(len > 0){
    	if(_config != config) {
	        _config = (char *)calloc(1, (len+1)*sizeof(char));
            strncpy(_config, config, len);
        }
        if (append){
            _fout.open(_config, std::ios_base::app);
        } else{
        	_fout.open(_config);
        }
        _os = &_fout;
        _append = append;
        std::cout << "In log::Log::SetConfig(const char* config): os.success == " << std::boolalpha << !_os -> fail() << std::endl;
    }
}

void log::Log::SetConfig(std::string config, bool append){
	int len = config.size();
	char tmp[len+1];
	strncpy(tmp, config.c_str(), len);
	tmp[len] = '\0';
	SetConfig(config, append);
}

void log::Log::Reset(){
    if(!_config){
       assert(_os == &std::cout || _os == NULL);
    } else{
       free(_config);
       _config = NULL;
       _os = &std::cout;
    } 
}

void log::Log::Write(const std::string content) const{
	std::time_t result = std::time(NULL);
	std::string timestr = std::asctime(std::localtime(&result));
	timestr.pop_back();
	if(_os -> fail()){
		std::cout << RED << "[Open file failed!]" << RESET << ", " << timestr << ": " << content << std::endl;
	} else{
		(*_os) << timestr << ": " << content << std::endl;
	}
}


log::Log::~Log(){
    Reset();
    if(_fout.is_open()) {
        _fout.close();
    }
    _os = NULL;
}