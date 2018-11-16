#include <Application.h>
#include <Alert.h>
#include <Bitmap.h>
#include <Deskbar.h>
#include <Font.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <Roster.h>

#include "DeskbarView.h"
#include "DictApplication.h"
#include "Utilities.h"

DeskbarView::DeskbarView()
	: BView(BRect(0, 0, 15, 15), REPLICANT_NAME, B_FOLLOW_NONE, B_WILL_DRAW | B_PULSE_NEEDED)
{
}

DeskbarView::DeskbarView(BMessage *msg)
	: BView(msg)
{
	// Make the pop-up menu
	PopUp = new BPopUpMenu("popUpMenu", false, false);
	NewWindow = new BMenuItem("New Window", new BMessage(NEW_WIN));
	MoveToWorkspace = new BMenuItem("Move Windows Here", new BMessage(WORKSPACE));
	TransClipboard = new BMenuItem("Translate Clipboard", new BMessage(TRANS_CLIP));	
	StayOnTop = new BMenuItem("Stay On Top", new BMessage(STAY_ON_TOP));

	// Draw the icon
	icon = new BBitmap(BRect(0, 0, 15, 15), B_COLOR_8_BIT);
	icon->SetBits(iconbits, 16*16, 0, B_COLOR_8_BIT);
	SetViewColor(B_TRANSPARENT_COLOR);
	
	// Get the messenger of the Lingua app
	if (be_roster->IsRunning(APP_SIG))
	{
		team_id id = be_roster->TeamFor(APP_SIG);
		lingua_messenger = new BMessenger(APP_SIG, id);
	}	
}

DeskbarView *DeskbarView::Instantiate(BMessage *data)
{
	if (!validate_instantiation(data, "DeskbarView")) return NULL;
	return new DeskbarView(data);
}

status_t DeskbarView::Archive(BMessage *data, bool deep) const
{
	BView::Archive(data, deep);
	data->AddString("add_on", APP_SIG);
	data->AddString("class", "DeskbarView");
	
	return B_OK;
}

void DeskbarView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case NEW_WIN:
			lingua_messenger->SendMessage(NEW_LINGUA_WINDOW);
			break;
		
		case WORKSPACE:
			lingua_messenger->SendMessage(MOVE_WINS_HERE);
			break;
			
		case TRANS_CLIP:
			lingua_messenger->SendMessage(TRANS_CLIPBOARD);
			break;
			
		case STAY_ON_TOP:
			lingua_messenger->SendMessage(STAY_ON_TOP);
			if (StayOnTop->IsMarked()) 
				StayOnTop->SetMarked(false);
			else
				StayOnTop->SetMarked(true);
			break;
				
		
		default:
			BView::MessageReceived(message);
	}
}

void DeskbarView::AttachedToWindow()
{
	if (Parent()) {
		BFont font;
		PopUp->GetFont(&font);
		font.SetSize(10);
		PopUp->SetFont(&font);

		PopUp->AddItem(NewWindow);
		PopUp->AddItem(MoveToWorkspace);
		PopUp->AddItem(TransClipboard);
		PopUp->AddSeparatorItem();
		PopUp->AddItem(StayOnTop);
		PopUp->SetTargetForItems(this);
	}
}

void DeskbarView::Draw(BRect updateRect)
{
	BView::Draw(updateRect);

	SetDrawingMode(B_OP_OVER);
	SetHighColor(Parent()->ViewColor());
	FillRect(updateRect);
	
	MovePenTo(0, 0);
	DrawBitmap(icon);
}

void DeskbarView::MouseDown(BPoint point)
{
	BPoint pt;
	uint32 buttons;
	GetMouse(&pt, &buttons);
	
	if (buttons == B_SECONDARY_MOUSE_BUTTON) {
		// I would have used PostMessage here, but
		// it didn't seem to work. Screwy.
		MessageReceived(new BMessage(NEW_WIN));
	} else {
		ConvertToScreen(&point);
		PopUp->Go(point, true, false, true);
	}
}
