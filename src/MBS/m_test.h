#pragma once
#include <iostream>

class TestModule : public Module{
	std::string name;
public:
	TestModule(std::string name){
		this->name = name;
	}

	void loop() override{
		output(name);
	}
};