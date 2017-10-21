#ifndef __COMPONENT_NOTIFICATION_INTERFACE_H__
#define __COMPONENT_NOTIFICATION_INTERFACE_H__

#include <libnotify/notify.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define LOW NOTIFY_URGENCY_LOW
#define NORMAL NOTIFY_URGENCY_NORMAL
#define CRITICAL NOTIFY_URGENCY_CRITICAL

#define ICON_BOOK "./img/handy/book-icon.png"
#define ICON_CALENDAR "./img/handy/calendar-icon.png"
#define ICON_CLOCK "./img/handy/clock-icon.png"
#define ICON_MAIL "./img/handy/mail-icon.png"

/* NotificationComponent
 * Is a output component that send the messages via notifications. This is a 
 * Linux specific module (it may work on Debian based). Other modules shall
 * be created to handle notifications in other systems (or a generic module
 * to handle it all).
 */ 
class NotificationComponent : public OutputComponent{
public:
	NotificationComponent () {
		if(!notify_init("MAP!"))
			ERROR("Error: Couldn't initialize notification interface.");
	}
	~NotificationComponent () {
		notify_uninit();
	}

	void notification_destroy();

	void notify(const std::string& title, const std::string& text, const NotifyUrgency& urgency, const std::string& image_path);
	void notify(const std::string& title, const std::string& text, const NotifyUrgency& urgency);
	void notify(const std::string& title, const std::string& text, const std::string& image_path);
	void notify(const std::string& title, const std::string& text);	

	void output(std::string msg) override{
		/* Simple message */
		if(msg.length()<4 or msg[0]!='@' or msg[1]!='@'){
			this->notify("Message: ", msg, LOW, ICON_BOOK);
			return;
		}
		/* Custom message. It is passed as @@Priority|Title|Text */
		msg.erase(0, 2);

		/* Prio */
		size_t pos = msg.find("|");
		int prio = stoi(msg.substr(0, pos));
		msg.erase(0, pos+1);
		/* Title */
		pos = msg.find("|");
		std::string title = msg.substr(0, pos);
		msg.erase(0, pos+1);
		/* Text */
		std::string text = msg.substr(0, msg.length()-1);
		/* Icon and Urgency*/
		NotifyUrgency urgency;
		std::string icon;
		switch(prio){
			case 5:
				title = "(5) " + title;
				urgency = CRITICAL;
				icon = ICON_CLOCK;
				break;
			case 4:
				title = "(4) " + title;
				urgency = NORMAL;//CRITICAL;
				icon = ICON_CLOCK;
				break;
			case 3:
				title = "(3) " + title;
				urgency = LOW;//NORMAL;
				icon = ICON_CLOCK;
				break;
			case 2:
				title = "(2) " + title;
				urgency = LOW;//NORMAL;
				icon = ICON_CLOCK;
				break;
			case 1:
				title = "(1) " + title;
				urgency = LOW;
				icon = ICON_CLOCK;
				break;
			default:
				urgency = LOW;
				icon = ICON_CLOCK;
		}
		/* Send notification */
		this->notify(title, text, urgency, icon);		
	}
};

void NotificationComponent::notify(const std::string& title, const std::string& text, const NotifyUrgency& urgency){
	notify(title, text, urgency, "");
}

void NotificationComponent::notify(const std::string& title, const std::string& text, const std::string& image_path){
	notify(title, text, NOTIFY_URGENCY_LOW, image_path);
}

void NotificationComponent::notify(const std::string& title, const std::string& text){
	notify(title, text, NOTIFY_URGENCY_LOW, "");
}
/* Notify!
 * Use libnotify to send notifications to the user.
 */
void NotificationComponent::notify(const std::string& title, const std::string& text, const NotifyUrgency& urgency, const std::string& image_path){
	NotifyNotification * msg;
	msg = notify_notification_new(title.c_str(), text.c_str(), NULL);
	notify_notification_set_urgency(msg, urgency);
	if(image_path.length() > 0){
		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path.c_str(), NULL);
		notify_notification_set_image_from_pixbuf(msg, pixbuf);
	}
	if(!notify_notification_show(msg, NULL))
		ERROR("Couldn't send notification.");
	g_object_unref(G_OBJECT(msg));
}

#endif