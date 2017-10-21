#ifndef __MBS_H__
#define __MBS_H__

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <thread>
#include <chrono>
#include "global.h"

/* Class Headers */
class Manager;
class Module;
class OutputComponent;

/* Module Control */
#define SECONDS *1
#define MINUTES *60
#define HOURS *3600

/* Output Component Class. They are responsible to display
 * outputs to the user. The default output component send
 * the mensages to the default output (if you not pipe, it
 * is the terminal).
 */
class OutputComponent{
public:
	virtual void output(std::string msg){
		std::cout << msg << std::endl;
	}
};

/* Module Class, that does specific actions.
 * Currently, it doesn't have sub-modules or input components, but 
 * it is a goal.
 * On the module declaration, you can use the functions:
 * 	.setOutputComponent: Define a output component.
 *	.setLoopTime: Define loop time.
 */
class Module{
private:
	bool active = true;
	OutputComponent *outputComponent;

public:
	Manager *manager = NULL;
	int loopTime = -1;
	/* Constructor */
	Module(){
		outputComponent = new OutputComponent();
	}
	// TODO: Create Destructor
	// TODO: Create Copy Constructor

	/* A module is 'smaller' than other if it's loop time is 'smaller'. */
	bool operator <(const Module& other){
		return this->loopTime < other.loopTime;
	}
	/* Define the output component */
	template <typename T>
	Module& setOutputComponent(T *oc){
		this->outputComponent = oc;
		return *this;
	}
	/* Define the loop cycle time for the module. 
	 * i < 1, means that the module doesn't loop.
	 */
	Module& setLoopTime(int i){
		this->loopTime = i;
		return *this;
	}
	/* Input function, return a output for a given input.
	 * Shall be overriden by input modules.
	 */
	virtual std::string input(std::string inp){
		(void)inp;
		return "";
	}
	/* Loop function. Shall be overridden by loop modules.*/
	virtual void loop(){return;}
	/* Send a message to the output component */
	inline void output(std::string msg){
		outputComponent->output(msg);
	}
	/* Return true if the module is active */
	inline bool isActive() const {return active;}
	/* Define the module as inactive, and let the Manager destroy it in
	 * the next loop cycle.
	 */
	inline void destroy() {active=false;};
};

bool sortByLoopTime(const std::unique_ptr<Module> &A, const std::unique_ptr<Module> &B){
	return A->loopTime < B->loopTime;
}

/* 
 * Manager:
 * Control modules, doing updated and inputs.
 */
class Manager{
private:
	std::vector<std::unique_ptr<Module>> modules;

	long t;// Mantém a quantidade de ciclos de loop realizados
	bool r;// Verifica se deve atualizar
public:
	/* Constructor */
	Manager():t(0),r(false){}

	/* 
	 * Returns the output from the first input module that answers.
	 */
	std::string input(std::string inp){
		std::string out;
		for(auto& m : modules){
			out = m->input(inp);
			//std::cout << "OUT: " << out << out.length() << std::endl;
			if( out.length()>0 ){
				//std::cout << "Found output." << std::endl;
				return out;
			}
		}
		return "";
	}

	/* Input Thread:
	 * Thread that runs while active is true. It receive a input and pass through all
	 * input modules, get a input and send it to default output.
	 */
	void input_thread(){
		while(active()){
			std::string str, ret;
			std::getline(std::cin,  str);// TODO: It shall use a default input module!
			//start_clock;
			ret = input(str);
			//end_and_show;
			std::cout << ret << std::endl;// TODO: It shall use a default output module
		}
	}
	/* Loop Thread:
	 * Thread that runs while active is true. It refresh the manager, loop over the modules
	 * and sleep for one second.
	 */
	void loop_thread(){
		while(active()){
			refresh();
			loop();
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	/* Loop:
	 * Loop over the modules.
	 */
	void loop(){
		t++;
		for(auto& m : modules){
			if(m->loopTime<1)
				continue;
			if(m->loopTime==1 or t%(m->loopTime)==0)
				m->loop();
		}
	}
	/* 
	 * Remove inactive modules
	 */
	void refresh(){
		//if(not r) return;
		modules.erase(std::remove_if(std::begin(modules), std::end(modules),
			[](const std::unique_ptr<Module> &tempModule) {
				return !tempModule->isActive();
			}),
			std::end(modules));
		//r = false;
	}
	/* Add Module:
	 * Add a new module to the Manager. The module class should be passed
	 * as the typename T, and the constructor arguments follow the call.
	 * Example:
	 * 		manager.addModule<TestModule>("Always");
	 */
	template <typename T, typename... TArgs>
	T& addModule(TArgs&&... mArgs){
		T* m(new T(std::forward<TArgs>(mArgs)...));
		m->manager = this;
		std::unique_ptr<Module> uPtr( m );
		modules.emplace_back(std::move(uPtr));
		return *m;
	}
	/* Prepare:
	 * Do optimizations to help in the Manager performance.
	 */
	void prepare(){
		// There is no optimizations yet...
		//std::sort(modules.begin(), modules.end(), sortByLoopTime);
	}
	/* Prepare the manager, and initialize the threads */
	void init(){
		prepare();
		/* Threads */
		std::thread t_i(&Manager::input_thread, this);// input thread
		std::thread t_l(&Manager::loop_thread, this); // loop thread
		t_i.join();
		t_l.join();
	}
	/* True, while the number of modules is greater than 0 */
	bool active(){
		return modules.size() > 0;
	}
	/* Shall Refresh:
	 * This function were used to avoid refreshing when were not
	 * necessary, but it is not in use anymore. Corrections should
	 * be made to optimize the refresh function.
	 */ 
	void shallRefresh(){
		r = true;
	}
};

#endif