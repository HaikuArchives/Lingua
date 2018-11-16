#include <Application.h>
#include <Bitmap.h>
#include <Font.h>
#include <String.h>
#include <View.h>

#include <stdlib.h>

#include "DownloadItem.h"

DownloadItem::DownloadItem(BString name, BString mod, BString size)
	: BListItem()
{
	myName = name;
	myModDate = mod;
	mySize = size;
}

void DownloadItem::DrawItem(BView *owner, BRect frame, bool complete)
{
	// Presets for colors
	rgb_color hilite = { 153, 153, 153, 255 };
	rgb_color dark = { 0, 0, 0, 255 };

	BBitmap *icon = new BBitmap(BRect(0,0,15,15), B_CMAP8);

	// If the item is selected, hilight it
	if (IsSelected() || complete) {
		if (IsSelected()) {
			owner->SetHighColor(hilite);
			owner->SetLowColor(hilite);
			icon->SetBits(ZipIconOn, 16*16*8, 0, B_CMAP8);
		} else {
			owner->SetHighColor(owner->ViewColor());
			owner->SetLowColor(owner->ViewColor());
			icon->SetBits(ZipIconOff, 16*16*8, 0, B_CMAP8);
		}			
		owner->FillRect(frame);
	} else {
		owner->SetLowColor(owner->ViewColor());
		icon->SetBits(ZipIconOff, 16*16*8, 0, B_CMAP8);
	}
	owner->SetHighColor(dark);
	
	// Draw the zipfile icon
	owner->MovePenTo(frame.left+1, frame.top+1);
	owner->DrawBitmap(icon);

	// Draw the text in three columns
	owner->MovePenTo(frame.left+25, frame.bottom-4);
	owner->DrawString(myName.String());

	owner->MovePenTo(frame.left+100, frame.bottom-4);
	owner->DrawString(myModDate.String());

	float space = 60 - owner->StringWidth(mySize.String());
	owner->MovePenTo(frame.left+150+space, frame.bottom-4);
	owner->DrawString(mySize.String());
}

void DownloadItem::Update(BView *owner, const BFont *font)
{
	BListItem::Update(owner, font);

	font_height fontAttrs;
	font->GetHeight(&fontAttrs);
	float fontHeight = ceil(fontAttrs.ascent) + ceil(fontAttrs.descent);	
	SetHeight(fontHeight + 4);
}	

const char *DownloadItem::GetName()
{
	return myName.String();
}

void DownloadItem::SetOrigSize(float size)
{
	myOrigSize = size;
}

float DownloadItem::GetOrigSize()
{
	return myOrigSize;
}

