#pragma once

class PythonModule : public Module{
private:
	std::string fileName;
	PyObject* module;
	PyObject* inputFunction;
	PyObject* loopFunction;
	PyObject* destroyFunction;
public:
	PythonModule(std::string fileName){
		this->fileName = fileName;
		setenv("PYTHONPATH", pythonpath(),1);
		Py_Initialize();
		module = PyImport_ImportModule(fileName.c_str());
		inputFunction = PyObject_GetAttrString(module,(char*)"input");
		loopFunction = PyObject_GetAttrString(module,(char*)"loop");
		destroyFunction = PyObject_GetAttrString(module,(char*)"destroy");
		Py_Finalize();
	}

	std::string input(std::string inp){
		if( not inputFunction )
			return "";
		Py_Initialize();
		PyObject* myResult = PyObject_CallFunction(inputFunction, (char*)"s", inp.c_str() );
		std::string ret = PyString_AsString(myResult);
		Py_Finalize();
		return ret;
	}

	void loop(){
		if( not loopFunction )
			return;
		setenv("PYTHONPATH", pythonpath(),1);
		Py_Initialize();
		PyObject_CallFunction(loopFunction, (char*)"");
		Py_Finalize();
	}

	void destroy(){
		if(not destroyFunction )
			return;
		setenv("PYTHONPATH", pythonpath(),1);
		Py_Initialize();
		PyObject_CallFunction(destroyFunction, (char*)"");
		Py_Finalize();
		active=false;
	}
};