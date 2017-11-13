#include "map.h"
/**
 * Modular Assistant Program
 **/

int main(){
	/* Components */
	NotificationComponent *nc = new NotificationComponent();
	/* Module Manager */
	Manager manager;
	/* Modules */
	//manager.addModule<TestModule>("Always").setLoopTime(1);
	//manager.addModule<TestModule>("Module 3-1 sec").setLoopTime(3 SECONDS);
	//manager.addModule<TestModule>("Module 4 sec").setLoopTime(4 SECONDS).setOutputComponent(nc);
	//manager.addModule<TestModule>("Module 5 sec").setLoopTime(5 SECONDS);
	//manager.addModule<TestModule>("Module 3-2 sec").setLoopTime(3 SECONDS);
	//manager.addModule<TestModule>("Module 2 sec").setLoopTime(2 SECONDS);
	//manager.addModule<PythonModule>("p_module2").setLoopTime(2 SECONDS);

	//manager.addModule<TestModule>("Please Confirm...");
	manager.addModule<ReminderModule>().setLoopTime(5 SECONDS).setOutputComponent(nc);
	manager.addModule<DarlaModule>();
	//manager.addModule<CalculatorModule>();
	//manager.addModule<PythonModule>("p_module1");

	manager.addModule<ChatterModule>("Map");
	/* Run the Program */
	manager.init();

	return 0;
}
