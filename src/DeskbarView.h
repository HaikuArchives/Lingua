#ifndef MINIVIEW_H
#define MINIVIEW_H

#include <View.h>

#include "DictWindow.h"

#define NEW_WIN		'nwin'
#define WORKSPACE	'wksp'
#define TRANS_CLIP	'tcpb'
#define STAY_ON_TOP	'sont'

class DeskbarView : public BView
{
	public:
		DeskbarView();
		DeskbarView(BMessage *msg);
		static DeskbarView *Instantiate(BMessage *data);
		virtual status_t Archive(BMessage *data, bool deep=true) const;
		virtual void AttachedToWindow();
		virtual void Draw(BRect updateRect);
		virtual void MessageReceived(BMessage *message);
		virtual void MouseDown(BPoint point);
		
	private:
		BPopUpMenu *PopUp;
		BMenuItem *NewWindow;
		BMenuItem *MoveToWorkspace;
		BMenuItem *TransClipboard;
		BMenuItem *StayOnTop;
		
		BMessenger *lingua_messenger;
		
		BBitmap *icon;
		DictWindow *newWin;
};

const unsigned char iconbits [] = {
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0x00,0x00,0x00,0xff,0x00,0x00,0x3f,0x3f,0x3f,0x00,0xff,0xff,0xff,0xff,
	0xff,0x00,0x3f,0x3f,0x3f,0x00,0x3f,0x3f,0x3f,0x10,0x10,0x3f,0x00,0xff,0xff,0xff,
	0xff,0x00,0x00,0x3f,0x3f,0x3f,0x00,0x3f,0x10,0x3f,0x3f,0x10,0x3f,0x00,0xff,0xff,
	0xff,0xff,0x00,0x00,0x1e,0x1e,0x18,0x00,0x3f,0x10,0x10,0x3f,0x10,0x3f,0x00,0xff,
	0xff,0xff,0x00,0x00,0x00,0x1c,0x1b,0x12,0x00,0x3f,0x10,0x10,0x3f,0x00,0x00,0xff,
	0xff,0xff,0x00,0xc5,0x00,0x00,0x19,0x00,0x00,0x00,0x3f,0x00,0x00,0x1e,0x00,0xff,
	0xff,0xff,0xff,0x00,0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x00,0xff,0xff,
	0xff,0xff,0xff,0xff,0x00,0xcb,0xcb,0x00,0xcb,0x00,0x15,0x00,0xd1,0x00,0x10,0xff,
	0xff,0xff,0xff,0xff,0xff,0x00,0xd1,0xd1,0x00,0x31,0x31,0x00,0x3f,0x3f,0x10,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x31,0x31,0x00,0x3f,0x3f,0x1e,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x31,0x10,0x17,0x00,0x00,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

#endif