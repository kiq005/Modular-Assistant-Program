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

	std::string input(std::string input){
		if(input == "I'm ready!"){
			return "$CONF$Are you sure?";
		}
		return "";
	}

	std::string respond(std::string input){
		if(input == "yes"){
			return "$CONF$Write: I'm sure!";
		}
		if(input == "I'm sure!"){
			return "I knew it!";
		}
		return "";
	}
};