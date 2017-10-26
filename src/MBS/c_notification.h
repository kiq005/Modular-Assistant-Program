#ifndef __COMPONENT_NOTIFICATION_INTERFACE_H__
#define __COMPONENT_NOTIFICATION_INTERFACE_H__

#include <libnotify/notify.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

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

	/* Notify!
	 * Use libnotify to send notifications to the user.
	 */
	void notify(const std::string& title, const std::string& text, const NotifyUrgency& urgency = NOTIFY_URGENCY_LOW, const std::string& image_path = ""){
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

	void output(std::string msg) override{
		/* Notification Title */
		std::string title = "Notification: ";
		NotifyUrgency urgency;
		std::string icon;
		int prio = 0;
		if( msg[0]=='(' and msg[2]==')'){// Has prio
			prio = int(msg[1]);
		}
		size_t pos = msg.find(":");
		if(pos != std::string::npos){// Has Title
			title = msg.substr(0, pos);
			msg.erase(0, pos+1);
		}
		
		switch(prio){
			case 5:
				urgency = NOTIFY_URGENCY_CRITICAL;
				icon = imgpath("handy/clock-icon.png");
				break;
			case 4:
				urgency = NOTIFY_URGENCY_NORMAL;//CRITICAL;
				icon = imgpath("handy/clock-icon.png");
				break;
			case 3:
				urgency = NOTIFY_URGENCY_LOW;//NORMAL;
				icon = imgpath("handy/clock-icon.png");
				break;
			case 2:
				urgency = NOTIFY_URGENCY_LOW;//NORMAL;
				icon = imgpath("handy/clock-icon.png");
				break;
			case 1:
				urgency = NOTIFY_URGENCY_LOW;
				icon = imgpath("handy/clock-icon.png");
				break;
			default:
				urgency = NOTIFY_URGENCY_LOW;
				icon = imgpath("handy/book-icon.png");
		}

		/* Send notification */
		this->notify(title, msg, urgency, icon);		
	}
};

#endif