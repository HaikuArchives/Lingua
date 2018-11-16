/* 
 Lingua!
 A Multilingual Translation Application for the BeOS
 Copyright (c) 2001 Aevum Industries
*/

#include <Application.h>
#include <Alert.h>
#include <ClassInfo.h>
#include <Clipboard.h>
#include <File.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Screen.h>
#include <String.h>
#include <math.h>

#include "DeskbarView.h"
#include "DictApplication.h"

// For use with the deskbar replicant
BView* instantiate_deskbar_item()
{
	return new DeskbarView();
}

DictApplication::DictApplication()
	: BApplication(APP_SIG)
{
	PostMessage(NEW_LINGUA_WINDOW);
}

void DictApplication::AboutRequested()
{
	float theWidth, theHeight;
	float aboutWidth = 220.0;
	float aboutHeight = 100.0;

	BScreen *theScreen = new BScreen();
	BRect aboutRect = theScreen->Frame();
	theWidth = aboutRect.right;
	theHeight = aboutRect.bottom;
	
	aboutRect.left = (theWidth - aboutWidth) / 2;
	aboutRect.top = (theHeight - aboutHeight) / 3;
	aboutRect.right = aboutRect.left + aboutWidth;
	aboutRect.bottom = aboutRect.top + aboutHeight;	
	aboutWin = new AboutWindow(aboutRect, APP_VER);
}

void DictApplication::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case NEW_LINGUA_WINDOW:
		{
			BRect windowSize;
			int32 last = 0;
			
			int32 totalWins = CountWindows();
			if (totalWins == 0) {
				windowSize.left = WIN_LEFT;
				windowSize.top = WIN_TOP;
				windowSize.right = WIN_LEFT + WIN_WIDTH;
				windowSize.bottom = WIN_TOP + WIN_HEIGHT;
			} else {
				for (int32 i=0; i<totalWins; i++)
					if (is_kind_of(WindowAt(i), DictWindow)	&& (WindowAt(i)->Workspaces() & (uint32)(1 << current_workspace())))
						last = i;
						
				windowSize = WindowAt(last)->Frame();
				windowSize.OffsetBy(15.0, 15.0);						
			}
		
			newWindow = new DictWindow(windowSize);
		}
			break;
		
		case STAY_ON_TOP:
			for (int32 i=0; i<CountWindows(); i++) {
				if (is_kind_of(WindowAt(i), DictWindow)) {
					if (WindowAt(i)->Feel() == B_FLOATING_ALL_WINDOW_FEEL)
						WindowAt(i)->SetFeel(B_NORMAL_WINDOW_FEEL);
					else
						WindowAt(i)->SetFeel(B_FLOATING_ALL_WINDOW_FEEL);					
				}
			}
			break;
			
		case MOVE_WINS_HERE:
			for (int32 i=0; i<CountWindows(); i++) {
				WindowAt(i)->SetWorkspaces(B_CURRENT_WORKSPACE);
				WindowAt(i)->Activate();
			}
			break;
			
		case TRANS_CLIPBOARD:
		{
			const char *text;
			int32 textlen;
			int32 last = 0;
			BString out_string;
			
			BMessage *clip = (BMessage *)NULL;
			BMessage *word = new BMessage(RECEIVED_WORD);
			
			// Get the clipboard contents
			if (be_clipboard->Lock()) {
				clip = be_clipboard->Data();
				clip->FindData("text/plain", B_MIME_TYPE, (const void **)&text, &textlen);
				be_clipboard->Unlock();
			}

			// Check if the word is valid
			if (text != NULL)
				out_string.SetTo(text, textlen);
			else
				break;
			
			// Find the last DictWindow in the current workspace. Some
			// crazy math-stuff involved here. Beware!
			for (int32 i=0; i<CountWindows(); i++) 
				if (is_kind_of(WindowAt(i), DictWindow)	&& (WindowAt(i)->Workspaces() & (uint32)(1 << current_workspace())))
					last = i;
			
			// Define the word
			word->AddString("word", out_string.String());
			WindowAt(last)->PostMessage(word);
			WindowAt(last)->Activate();
		}
			break;
		
		case UPDATE_WORD_MENU:
		{
			BString queryWord;
			int32 maxWords;
			
			msg->FindString("queryWord", &queryWord);
			msg->FindInt32("maxWords", &maxWords);
			
			for (int32 i=0; i<CountWindows(); i++) 
				if (is_kind_of(WindowAt(i), DictWindow))
					((DictWindow *)WindowAt(i))->UpdateWordMenu(queryWord.String(), maxWords);
		}
			break;
		
		default:
			BApplication::MessageReceived(msg);
	}
}

int main()
{
	DictApplication *Lingua = new DictApplication;
	Lingua->Run();	
	delete Lingua;
	
	return 0;
}
