#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <python2.7/Python.h>
#include <iostream>
#include <string>
#include <vector>
/* Throw Error with message */
#define ERROR(X) throw std::runtime_error(std::string( (X) ))
#define SAY(X) std::cout << "Debug: " <<  (X) << " [ " << __FILE__ << " : " << __LINE__ << " ]" << std::endl;
/* Calculate Elapsed Time */
#define start_clock clock_t _begin_ = clock();
#define end_clock clock_t _end_ = clock();
#define elapsed double _elapsed_ = double(_end_ - _begin_) / CLOCKS_PER_SEC;
#define show_time std::cout << "[Time: " << _elapsed_ << "]" << std::endl;
#define end_and_show end_clock elapsed show_time
/* Path functions */ 
// TODO: Path work differently to Unix and Windows systems, these funcions should handle it.
inline std::string datapath( const std::string& file){
	return "./data/" + file;
}

inline std::string imgpath( const std::string& file){
	return "./img/" + file;
}

inline std::string logpath( const std::string& file){
	return "./log/" + file;
}

inline char* pythonpath() {
	return (char*)"./src/Python";
}

/* Given Y, M, D, returns how many days since -4712 (JDN:Julian Day Number) */
inline int julian_day( const int& Y, const int& M, const int& D){
	return 367*Y-(7*(Y+5001+(M-9)/7))/4+(275*M)/9+D+1729777;
}
/* Given a JDN, return weekday */
inline int julian_wday(const int& JDN){
	return (JDN + 1) % 7;
}

/* TODO: Use native functions!!! */
typedef std::vector<std::string> vstring;
const std::string delim = "?!.;,";

inline bool isPunc(char c) {
	return delim.find(c) != std::string::npos;
}

size_t replace(std::string &str, std::string substr1, std::string substr2) {
	size_t pos = str.find(substr1);
	if(pos != std::string::npos) {
		str.erase(pos, substr1.length());
		str.insert(pos, substr2);
	}
	return pos;
}

void trimRight(std::string &str, std::string delim) {
	size_t pos = str.find_last_not_of(delim);
	if(pos != std::string::npos) {
		str.erase(pos + 1, str.length());
	}
}

void trimLeft(std::string &str, std::string delim) {
	size_t pos = str.find_first_not_of(delim);
	if(pos != std::string::npos) {
		str.erase(0, pos);
	}
}

// trim all left and right characters that is specified by
// the string "characters"
void trimLR(std::string &str, std::string characters) {
	trimLeft(str, characters);
	trimRight(str, characters);
}

void tokenize(const std::string &str, vstring &v) {
	std::string buffer;
	for(size_t i = 0; i < str.length(); ++i) {
		if(!isPunc(str[i]) && !isspace(str[i]) && str[i] != '.') {
			buffer += str[i];
		} else if(!buffer.empty()) {
			v.push_back(buffer);
			buffer.erase();
		}
	}
	if((v.empty() && !buffer.empty()) || !buffer.empty()) {
		v.push_back(buffer);
	}
}

#endif