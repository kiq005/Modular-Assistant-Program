#pragma once
#include <iostream>

/* This class is just a test. It is a Output component, 
 * that send the message to the stdout preceded by 'MAP:'.
 */
class MAPOutputComponent : public OutputComponent{
public:

	void output(std::string msg) override{
		std::cout << "MAP: " << msg << std::endl;
	}
};