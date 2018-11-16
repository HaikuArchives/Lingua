/*
 Copius code for the snazzy URLView was taken from William Kakes's
 splendid URLView. Check his stuff out at http://www.tallhill.com
 The class was sufficiently mangled by Syn.Terra for Lingua use.
 */

#include <Application.h>
#include <Bitmap.h>
#include <Button.h>
#include <Roster.h>
#include <String.h>
#include <StringView.h>

#include "AboutWindow.h"
#include "Utilities.h"

AboutWindow::AboutWindow(BRect frame, const char *appVersion)
	: BWindow(frame, "About Lingua", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	// Note that the background view is added now, rather than later. This
	// is because the image icon needs the background view to be added
	// before it can attach, otherwise the app crashes immediately.
	rgb_color white = {255, 255, 255, 255};
	rgb_color red = {153, 0, 0, 255};

	BRect aRect(frame);
	aRect.OffsetTo(B_ORIGIN);
	BView *background = new BView(aRect, "bgnd", B_FOLLOW_NONE, B_WILL_DRAW);
	background->SetViewColor(white);
	background->SetLowColor(white);
	AddChild(background);

	// Icon Image
	BRect imgRect(10, 10, 42, 42);
	BView *imgView = new BView(imgRect, "imgview", B_FOLLOW_NONE, B_WILL_DRAW);
	imgView->SetViewColor(white);
	imgView->SetLowColor(white);
	BBitmap *icon = new BBitmap(BRect(0,0,31,31), B_CMAP8, true);
	icon->SetBits(IconBits, 32 * 32 * 8, 0, B_CMAP8);
	
	background->AddChild(imgView);	
	imgView->SetViewBitmap(icon);

	// Heading Message
	float fw = aRect.Width();
	float fh = aRect.Height();

	BRect headRect(50, 10, fw, 33);
	BString headmsg;
	headmsg << "Lingua " << appVersion;
	heading = new BStringView(headRect, "hdng", headmsg.String());
	heading->SetFontSize(22);
	background->AddChild(heading);

	// Subheading
	BRect subheadRect(50, 35, fw, 45);
	subheading = new BStringView(subheadRect, "sbhd", "A Multilingual Translation Program");
	subheading->SetFontSize(10);
	subheading->SetHighColor(red);
	background->AddChild(subheading);	

	// Info text
	BRect codedRect(10, 60, fw-75, 70);
	codedby = new BStringView(codedRect, NULL, "Coded by Syn.Terra");
	codedby->SetFontSize(10);
	background->AddChild(codedby);
	
	BRect emailRect(10, 72, fw-75, 82);
	email = new URLView(emailRect, "dream@aevum.net", "mailto:dream@aevum.net");
	email->SetFontSize(10);
	background->AddChild(email);
	
	BRect webRect(10, 84, fw-75, 94);
	web = new URLView(webRect, "http://www.aevum.net", "http://www.aevum.net");
	web->SetFontSize(10);
	background->AddChild(web);

	// Okay button
	BRect okayRect(fw-70, fh-35, fw-10, fh-25);
	Okay = new BButton(okayRect, "okb", "Cool", new BMessage(B_QUIT_REQUESTED));
	Okay->MakeDefault(true);
	background->AddChild(Okay);

	Show();
}

URLView::URLView(BRect frame, const char *label, const char *theurl)
	: BStringView(frame, NULL, label, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW )
{
	// Colors are fun!
	rgb_color blue = {0, 105, 153, 255};
	rgb_color liteblue = {0, 153, 204, 255};
	rgb_color red = {0, 153, 204, 255};

	color = blue;
	hiliteColor = liteblue;
	clickColor = red;

	url = new BString(theurl);
	selected = false;
}
		

URLView::~URLView()
{
	delete url;
}

void URLView::AttachedToWindow()
{
	SetHighColor(color);
		
	if(Parent()) {
		SetLowColor(Parent()->ViewColor());
		SetViewColor(Parent()->ViewColor());
	}
}

void URLView::MouseMoved(BPoint point, uint32 transit, const BMessage *msg)
{
	// If we just entered, make it the hi-lite color
	if (GetTextRect().Contains(point)) {
		SetHighColor(hiliteColor);
		Redraw();
	}
	
	// If we just left, make it the original color
	if (transit == B_EXITED_VIEW) {
		SetHighColor(color);
		Redraw();
	}
	
	BStringView::MouseMoved(point, transit, msg);
}

void URLView::MouseDown(BPoint point) {
	if (GetTextRect().Contains(point)) {
		SetHighColor(clickColor);
		Redraw();
		selected = true;
	}
}

void URLView::MouseUp(BPoint point)
{
	// If the link was clicked on run the program
	// that should handle the URL.
	if (selected && GetTextRect().Contains(point)) 
		LaunchURL();

	selected = false;
	SetHighColor(color);

	Redraw();
}

void URLView::WindowActivated(bool active) {
	// Be sure that if the user clicks on a link prompting the opening of
	// a new window (i.e. launching NetPositive) the URL is not left drawn
	// with the hover color.
	if (!active) {
		SetHighColor(color);
		Redraw();
	}
}

BRect URLView::GetTextRect() {
	// This function will return a BRect that contains only the text,
	// so the mouse can change and the link will be activated only when
	// the mouse is over the text itself, not just within the view.
	const char *textArray[1];
	textArray[0] = Text();
	
	escapement_delta delta;
	delta.nonspace = 0;
	delta.space = 0;
	escapement_delta escapements[1];
	escapements[0] = delta;
	
	BRect returnMe;
	BRect rectArray[1];
	rectArray[0] = returnMe;
	
	BFont font;
	GetFont(&font);
	font.GetBoundingBoxesForStrings(textArray, 1, B_SCREEN_METRIC, escapements, rectArray);

	BRect frame = Frame();
	frame.OffsetTo(B_ORIGIN);
	returnMe = rectArray[0];
	
	// Get the height of the current font.
	font_height height;
	GetFontHeight(&height);
	float descent = height.descent;
	
	// Account for rounding of the floats when drawn to avoid
	// one-pixel-off errors.
	float lowerBound = 0;
	if ((((int)descent) * 2) != ((int)(descent * 2)))
		lowerBound = 1;
	
	// Adjust the bounding box to reflect where the text is in the view.
	returnMe.bottom += 1;
	returnMe.OffsetTo(B_ORIGIN);
	float rectHeight = returnMe.Height();
	returnMe.bottom = frame.bottom - descent;
	returnMe.top = returnMe.bottom - rectHeight;
	returnMe.OffsetBy(0.0, -(1 + lowerBound));

	return returnMe;
}

void URLView::LaunchURL() {
	if (url->FindFirst("mailto:") == 0) {
		char *link = url->LockBuffer(0);
		be_roster->Launch(EMAIL_TYPE, 1, &link);
		url->UnlockBuffer();
	}
	else if (url->FindFirst("http://") == 0) {
		char *link = url->LockBuffer(0);
		be_roster->Launch(URL_TYPE, 1, &link);
		url->UnlockBuffer();
	}
}

void URLView::Redraw() {
	// Redraw the link without flicker.
	BRect frame = Frame();
	frame.OffsetTo(B_ORIGIN);
	Draw(frame);
}
