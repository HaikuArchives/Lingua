#ifndef DICTAPP_H
#define DICTAPP_H

#include <Application.h>

#include "AboutWindow.h"
#include "DictWindow.h"

#define APP_SIG 	"application/x-vnd.AE-Lingua"
#define APP_VER 	"2.0"

#define NEW_LINGUA_WINDOW	'linw'
#define STAY_ON_TOP			'sont'
#define MOVE_WINS_HERE		'mvwh'
#define TRANS_CLIPBOARD		'trcb'
#define UPDATE_WORD_MENU	'upwm'

const float WIN_WIDTH =	410.0;
const float WIN_HEIGHT = 450.0;
const float WIN_LEFT = 75.0;
const float WIN_TOP = 75.0;

extern "C" _EXPORT BView* instantiate_deskbar_item();

class DictApplication : public BApplication {
	public:
		DictApplication();
		virtual void AboutRequested();
		virtual void MessageReceived(BMessage *msg);

	private:
		AboutWindow *aboutWin;
		DictWindow *newWindow;
};

#endif