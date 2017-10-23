#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <python2.7/Python.h>
#include <iostream>
#include <string>
/* Throw Error with message */
#define ERROR(X) throw std::runtime_error(std::string( (X) ))
/* Math */
#define MAX(X, Y) ( (Y>X)?(Y):(X) )
#define MIN(X, Y) ( (Y<X)?(Y):(X) )
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

/* Creating Python Modules */
#define PythonLoopModule(MODULENAME, FILENAME, FUNCTIONNAME) \
class MODULENAME : public Module{ \
public: \
	MODULENAME(){} \
	void loop() override{ \
		std::string ret; \
		setenv("PYTHONPATH", pythonpath(),1); \
		Py_Initialize(); \
		PyObject* myModule = PyImport_ImportModule(FILENAME); \
		PyObject* myFunction = PyObject_GetAttrString(myModule,(char*)FUNCTIONNAME); \
		PyObject_CallFunction(myFunction, (char*)""); \
		Py_Finalize(); \
	} \
	void destroy(); \
};

#define PythonInputModule(MODULENAME, FILENAME, FUNCTIONNAME) \
class MODULENAME : public Module{ \
public: \
	MODULENAME(){} \
	std::string input(std::string inp){ \
		std::string ret; \
		setenv("PYTHONPATH", pythonpath(),1); \
		Py_Initialize(); \
		PyObject* myModule = PyImport_ImportModule(FILENAME); \
		PyObject* myFunction = PyObject_GetAttrString(myModule,(char*)FUNCTIONNAME); \
		PyObject* myResult = PyObject_CallFunction(myFunction, (char*)"s", inp.c_str() ); \
		ret = PyString_AsString(myResult); \
		Py_Finalize(); \
		return ret; \
	} \
	void destroy();\
}; 


#define PythonDestroy(MODULENAME, FILENAME, FUNCTIONNAME) \
void MODULENAME::destroy(){ \
	setenv("PYTHONPATH", pythonpath(),1); \
	Py_Initialize(); \
	PyObject* myModule = PyImport_ImportModule(FILENAME); \
	PyObject* myFunction = PyObject_GetAttrString(myModule,(char*)FUNCTIONNAME); \
	PyObject_CallFunction(myFunction, (char*)""); \
	Py_Finalize(); \
	active=false; \
}

#endif