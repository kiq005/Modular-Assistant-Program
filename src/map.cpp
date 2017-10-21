#include "map.h"
/**
 * Modular Assistant Program
 **/

int main(){
	/* Components */
	//MAPOutputComponent *moc = new MishaOutputComponent();
	NotificationComponent *nc = new NotificationComponent();
	/* Module Manager */
	Manager manager;
	//manager.addModule<TestModule>("Always").setLoopTime(1);
	//manager.addModule<TestModule>("Module 3-1 sec").setLoopTime(3 SECONDS);
	//manager.addModule<TestModule>("Module 4 sec").setLoopTime(4 SECONDS).setOutputComponent(nc);
	//manager.addModule<TestModule>("Module 5 sec").setLoopTime(5 SECONDS);
	//manager.addModule<TestModule>("Module 3-2 sec").setLoopTime(3 SECONDS);
	//manager.addModule<TestModule>("Module 2 sec").setLoopTime(2 SECONDS);
	//manager.addModule<PythonModule>().setLoopTime(2 SECONDS);
	/* Loop Modules */
	manager.addModule<ReminderModule>().setLoopTime(5).setOutputComponent(nc);
	/* Input Modules */
	manager.addModule<DarlaModule>();
	manager.addModule<CalculatorModule>();
	manager.addModule<PythonInput>();
	/* Run the Program */
	manager.init();

	return 0;
}
