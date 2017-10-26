#ifndef __REMINDER_MODULE_H__
#define __REMINDER_MODULE_H__

#include <queue>
#include <fstream>
#include <ctime>
#include <algorithm>

struct Note{
	std::string title;
	std::string text;
	int prio;
	//char *icon;

	bool operator ==(const Note& other){
		return (title == other.title and text == other.text);
	}
};

/**
 * Reminder is responsable to keep, verify and ask for notifications of
 * reminders, that may be alarms, taks, dates, etc. To do so, it keep a 
 * queue of reminders to be notified. Just one notification is sended by
 * cycle of the control thread. The queue is filled as the task search 
 * goes by the data struct. Currently, the data struct is a linked list, 
 * but it may be replaced by something more efficient, like a tree structure
 * or hashmap.
 * Tasks are stored in .todo files on data folder. The notifications are sent
 * by priority order. The priorities goes as follow:
 * 	@due 		(+5-due)
 *	@critical	(5)
 *	@high		(4)
 *	@today		(3)
 *	@low		(2)
 * Anything else has 0 priority.
 **/
class ReminderModule : public Module{

public:
	ReminderModule()
	:last_notification({"", "", -1}){}

	void loop() override{
		searchTasks();
		prepareTasks();
		if(!notification_queue.empty()){
			queueNotify();
		}
	}

private:

	/* One notification from notification_queue is sended by cycle of thread_controller */
	void queueNotify(){
		Note new_notification = notification_queue.front();
		notification_queue.pop();
		if(last_notification==new_notification)//Already notified...
			return;
		last_notification = new_notification;
		output( "(" + std::to_string(new_notification.prio) + ")" + new_notification.title + ":" + new_notification.text);
	}

	int countTabulations(std::string str){
		int count = 0;
		size_t nPos = str.find("\t", 0);
		while(nPos != std::string::npos){
			count++;
			nPos = str.find("\t", nPos+1);
		}

		return count;
	}

	/* Search for tasks and add to the queue of temporary notes */
	void searchTasks(){
		std::ifstream infile(datapath("map.todo"));
		std::string line;
		std::string subClass[5] = {"Task", "", "", "", ""};
		int tTabs=0;
		size_t nPos;

		while(std::getline(infile, line)){
			if( line == "NOTE" )
				break;
			if( line.find("✔") != std::string::npos)
				continue;
			tTabs = countTabulations(line);

			if( (nPos = line.find("☐")) != std::string::npos ){
				int prio = 0;
				/* Title */
				std::string title = "";
				for(int i=0;i<tTabs-1;++i){
					title += subClass[i];
					title += " - ";
				}
				title = title.substr(0, title.length()-3);
				/* Text */
				std::string text = line.substr(nPos+4, line.length());
				nPos = text.find("@");
				if(nPos!=std::string::npos){// Has @
					std::string prio_text = text.substr(nPos, text.length());
					text = text.substr(0, nPos);
					/* Priority */
					if(prio_text == "@low"){
						prio = 2;
					}
					else if(prio_text == "@today"){
						prio = 3;
					}
					else if(prio_text == "@high"){
						prio = 4;
					}
					else if(prio_text == "@critical"){
						prio = 5;
					}
					else if( (nPos = prio_text.find("@due")) != std::string::npos){
						time_t t = time(0);
						std::tm *now = localtime(&t);
						std::tm chd;
						prio_text = prio_text.substr(5, prio_text.length() -6);
						/* Year */
						nPos = prio_text.find("-");
						chd.tm_year = 100 + std::stoi(prio_text.substr(0, nPos));
						/* Month */
						prio_text = prio_text.substr(nPos+1, prio_text.length());
						nPos = prio_text.find("-");
						chd.tm_mon = std::stoi(prio_text.substr(0, nPos)) - 1;
						/* Day */
						prio_text = prio_text.substr(nPos+1, prio_text.length());
						chd.tm_mday = std::stoi(prio_text);
						/* Difference */
						int t1 = julian_day(now->tm_year, now->tm_mon, now->tm_mday);
						int t2 = julian_day(chd.tm_year, chd.tm_mon, chd.tm_mday);
						prio = std::max(5 + t1 - t2, 0);//*@due	(+5-due)
					}
					else{
						ERROR("Can't understand priority");
					}
				}
				/* Add note */
				temporary_notes.emplace_back(Note{title, text, prio});
				continue;
			}
			if( (nPos = line.find(":")) != std::string::npos ){
				subClass[tTabs] = line.substr(tTabs, nPos-tTabs);
			}
		}
	}
	/* Verify the temporary notes and add the tasks to the notify queue */
	void prepareTasks(){
		Note currentTask, newTask;
		int prio=-1;
		while(!temporary_notes.empty()){
			currentTask = temporary_notes.back();
			temporary_notes.pop_back();
			if(currentTask.prio > prio){
				newTask = currentTask;
				prio = currentTask.prio;
			}
		}
		if(prio > -1)
			notification_queue.push(newTask);//Add Notification to queue
	}

	std::queue <Note> notification_queue;
	std::vector <Note> temporary_notes;
	Note last_notification;
};


#endif