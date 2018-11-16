#include <Application.h>
#include <Box.h>
#include <Button.h>
#include <CheckBox.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <RadioButton.h>
#include <String.h>
#include <StringView.h>
#include <TabView.h>
#include <TextControl.h>

#include <cstdio>
#include <cstdlib>

#include "DictApplication.h"
#include "DictWindow.h"
#include "PrefWindow.h"
#include "Utilities.h"

PrefWindow::PrefWindow(BRect frame, Preferences *preferences)
	: BWindow(frame, "Lingua Preferences", B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	int32 i; 	// for various loops

	// Update loaded preferences
	prefs = preferences;
	prefs->Load();
	if (prefs->FindBool("savewordonquit", &saveWordOnQuit) != B_OK)
		saveWordOnQuit = true;
		
	if (prefs->FindBool("replicantenabled", &replicantEnabled) != B_OK)
		replicantEnabled = false;

	if (prefs->FindInt32("maxwords", &maxWords) != B_OK)
		maxWords = MAX_WORDS;

	if (prefs->FindBool("autolookword", &autoLookWord) != B_OK)
		autoLookWord = true;
	
	if (prefs->FindBool("autolooklang", &autoLookLang) != B_OK)
		autoLookLang = true;

	if (prefs->FindBool("autolooksearch", &autoLookSearch) != B_OK)
		autoLookSearch = false;
		
	if (prefs->FindBool("autolooktrans", &autoLookTrans) != B_OK)
		autoLookTrans = false;		

	if (prefs->FindBool("formatengtoforeign", &formatEngToForeign) != B_OK)
		formatEngToForeign = true;
		
	if (prefs->FindBool("makeforeignbold", &makeForeignBold) != B_OK)
		makeForeignBold = true;
		
	if (prefs->FindBool("makeforeigncolored", &makeForeignColored) != B_OK)
		makeForeignColored = false;
		
	if (prefs->FindBool("isnumbering", &isNumbering) != B_OK)
		isNumbering = false;

	if (prefs->FindInt32("fontsize", &fontSize) != B_OK)
		fontSize = DEF_FONT_SIZE;
		
	if (prefs->FindBool("fixedformat", &fixedFormat) != B_OK)
		fixedFormat = false;
	
	if (prefs->FindString("foreigncolor", &foreignColor) != B_OK) 
		foreignColor = "blue";
	
	// Grey container background
	BView *background = new BView(Bounds(), NULL, B_FOLLOW_ALL, B_WILL_DRAW);
	rgb_color grey = ui_color(B_PANEL_BACKGROUND_COLOR);
	background->SetViewColor(grey);
	background->SetLowColor(grey);
	AddChild(background);
	
	// Set up the preference tabs
	BRect r = Bounds();
	r.InsetBy(5, 5);
	r.bottom -= 35;		// Make space for the buttons
	
	tabView = new BTabView(r, "tab_view");
	tabView->SetViewColor(grey);
	background->AddChild(tabView);
	
	r = tabView->Bounds();
	r.InsetBy(5, 5);
	r.bottom -= tabView->TabHeight();
	float width = r.right;
	float height = r.bottom;
	
	Interface = new BView(r, NULL, B_FOLLOW_NONE, B_WILL_DRAW);
	Interface->SetViewColor(grey);
	interfaceTab = new BTab();
	tabView->AddTab(Interface, interfaceTab);
	interfaceTab->SetLabel("Interface");
			
	Results = new BView(r, NULL, B_FOLLOW_NONE, B_WILL_DRAW);
	Results->SetViewColor(grey);
	resultsTab = new BTab();
	tabView->AddTab(Results, resultsTab);
	resultsTab->SetLabel("Results");
	
	tabView->Select(0);

	/* =========== Interface Tab =========== */
	
	// Save Word On Quit
	SaveWordOnQuit = new BCheckBox(BRect(0, 0, width, 10), NULL, "Save last defined word on quit", new BMessage(PREF_SAVE_WORD));
	if (saveWordOnQuit)	SaveWordOnQuit->SetValue(B_CONTROL_ON);
	Interface->AddChild(SaveWordOnQuit);
	
	// Enable/Disable Desbark Replicant
	EnableReplicant = new BCheckBox(BRect(0, 20, width, 30), NULL, "Enable Deskbar Replicant", new BMessage(PREF_REPLICANT));
	if (replicantEnabled) EnableReplicant->SetValue(B_CONTROL_ON);
	Interface->AddChild(EnableReplicant);
	
	// Max Words in Menu
	char tempMaxWords[128];
	char tempNumber[128];
	sprintf(tempMaxWords, "%li", maxWords);
	MaxWords = new BPopUpMenu(tempMaxWords);
	
	for (i=5; i<=50; i+=5) {
		BMessage *maxMsg = new BMessage(PREF_MAX_WORDS);
		maxMsg->AddInt32("value", i);
		sprintf(tempNumber, "%li", i);
		MaxWords->AddItem(new BMenuItem(tempNumber, maxMsg));
	}
	
	MaxField = new BMenuField(BRect(0, 43, width, 53), NULL, "Maximum Words In Menu:", MaxWords);
	MaxField->SetDivider(140);
	Interface->AddChild(MaxField);
	
	// Auto Translate When Changed
	float inWidth = width - 20;
	BRect autoRect(5, 75, width-13, 170);
	BRect autoWordRect(10, 15, inWidth, 25);
	BRect autoLangRect(10, 32, inWidth, 42);
	BRect autoSearchRect(10, 49, inWidth, 59);
	BRect autoTransRect(10, 66, inWidth, 76);
	
	BBox *autoLookupBox = new BBox(autoRect);
	autoLookupBox->SetLabel("Auto Translate When Changed:");
	AutoLookWord = new BCheckBox(autoWordRect, NULL, "Word Menu", new BMessage(PREF_AUTO_WORD));
	AutoLookLang = new BCheckBox(autoLangRect, NULL, "Language Menu", new BMessage(PREF_AUTO_LANG));	
	AutoLookSearch = new BCheckBox(autoSearchRect, NULL, "\"Search For\" Options", new BMessage(PREF_AUTO_SRCH));
	AutoLookTrans = new BCheckBox(autoTransRect, NULL, "\"Translate\" Options", new BMessage(PREF_AUTO_TRNS));

	if (autoLookWord) AutoLookWord->SetValue(B_CONTROL_ON);
	if (autoLookLang) AutoLookLang->SetValue(B_CONTROL_ON);	
	if (autoLookSearch) AutoLookSearch->SetValue(B_CONTROL_ON);
	if (autoLookTrans) AutoLookTrans->SetValue(B_CONTROL_ON);
		
	autoLookupBox->SetViewColor(grey);
	autoLookupBox->SetLowColor(grey);
	autoLookupBox->AddChild(AutoLookWord);
	autoLookupBox->AddChild(AutoLookLang);
	autoLookupBox->AddChild(AutoLookSearch);
	autoLookupBox->AddChild(AutoLookTrans);
	Interface->AddChild(autoLookupBox);

	/* ========== Results Tab ========== */
	
	// Make Foreign Words Bold
	MakeForeignBold = new BCheckBox(BRect(0, 0, width, 10), NULL, "Make Foreign Words Bold", new BMessage(PREF_FOR_BOLD));
	if (makeForeignBold) MakeForeignBold->SetValue(B_CONTROL_ON);
	Results->AddChild(MakeForeignBold);
	
	// Color Foreign Words
	MakeForeignColored = new BCheckBox(BRect(0, 20, 130, 30), NULL, "Color Foreign Words", new BMessage(PREF_FOR_COLOR));
	if (makeForeignColored) MakeForeignColored->SetValue(B_CONTROL_ON);
	Results->AddChild(MakeForeignColored);
	
	ColorMenu = new BPopUpMenu(foreignColor.String());	
	const char *theColors[8] = {"red", "green", "blue", "orange", "purple", "tan", "sky", "aqua"};
	for (i=0; i<8; i++) {
		BMessage *colorMsg = new BMessage(PREF_COLOR);
		colorMsg->AddString("color", theColors[i]);
		ColorMenu->AddItem(new BMenuItem(theColors[i], colorMsg));
	}
	
	ColorField = new BMenuField(BRect(130, 18, width, 26), NULL, NULL, ColorMenu);
	if (makeForeignColored) ColorMenu->SetEnabled(true);
	else ColorMenu->SetEnabled(false);
	Results->AddChild(ColorField);	

	IsNumberingCheck = new BCheckBox(BRect(0, 40, width, 50), NULL, "Number Results", new BMessage(PREF_NUMBERING));
	if (isNumbering) IsNumberingCheck->SetValue(B_CONTROL_ON);
	Results->AddChild(IsNumberingCheck);

	// Font Size
	char tempFontSize[128];
	char tempSize[128];
	sprintf(tempFontSize, "%li", fontSize);
	FontSize = new BPopUpMenu(tempFontSize);
	int32 fontSizeNumbers[7] = {8, 9, 10, 11, 12, 14, 18};

	for (i=0; i<7; i++) {
		BMessage *fontMsg = new BMessage(PREF_FONT_SIZE);
		fontMsg->AddInt32("fontsize", fontSizeNumbers[i]);
		sprintf(tempSize, "%li", fontSizeNumbers[i]);
		FontSize->AddItem(new BMenuItem(tempSize, fontMsg));
	}

	FontSizeField = new BMenuField(BRect(0, 60, width, 70), NULL, "Font Size: ", FontSize);
	FontSizeField->SetDivider(60);
	Results->AddChild(FontSizeField);

	// BBox "line" after the font size
	BBox *line = new BBox(BRect(2, 90, width-10, 91));
	Results->AddChild(line);

	// Fixed Format
	BRect defFormatRect(0, 100, width, height);
	BRect fixFormatRect(0, 0, 200, 20);
	BRect engToForeignRect(10, 20, 250, 30);
	BRect foreignToEngRect(10, 38, 250, 50);
		
	BView *defFormatView = new BView(defFormatRect, NULL, B_FOLLOW_NONE, B_WILL_DRAW);
	FixFormatCheck = new BCheckBox(fixFormatRect, NULL, "Fixed Output Format",	new BMessage(PREF_FIX_FORMAT));
	EngToForeign = new BRadioButton(engToForeignRect, NULL, "English word - Foreign definition", new BMessage(PREF_ENG_FOR));
	ForeignToEng = new BRadioButton(foreignToEngRect, NULL, "Foreign definition - English word", new BMessage(PREF_FOR_ENG));
	
	if (fixedFormat) {
		FixFormatCheck->SetValue(B_CONTROL_ON);
		EngToForeign->SetEnabled(true);
		ForeignToEng->SetEnabled(true);
	} else {
		EngToForeign->SetEnabled(false);
		ForeignToEng->SetEnabled(false);
	}

	if (formatEngToForeign)	EngToForeign->SetValue(B_CONTROL_ON);
	else ForeignToEng->SetValue(B_CONTROL_ON);

	defFormatView->SetViewColor(grey);
	defFormatView->SetLowColor(grey);
	
	defFormatView->AddChild(FixFormatCheck);
	defFormatView->AddChild(EngToForeign);	
	defFormatView->AddChild(ForeignToEng);	
	Results->AddChild(defFormatView);

	/* ====== Okay and Cancel Buttons ========= */
	
	BRect buttonRect(62, height+28, 242, height+68);
	BRect cancelBox(0, 10, 80, 20);
	BRect okayBox(90, 10, 170, 20);
	BView *buttonView = new BView(buttonRect, NULL, B_FOLLOW_NONE, B_WILL_DRAW);
	Cancel = new BButton(cancelBox, NULL, "Cancel", new BMessage(PREF_CANCEL));	
	Okay = new BButton(okayBox, NULL, "Okay", new BMessage(PREF_OKAY));
	Okay->MakeDefault(true);

	buttonView->SetViewColor(grey);
	buttonView->SetLowColor(grey);	
	buttonView->AddChild(Cancel);
	buttonView->AddChild(Okay);
	background->AddChild(buttonView);
	
	Show();
}

void PrefWindow::MessageReceived(BMessage *message)
{
	switch (message->what)
	{
		case PREF_OKAY:
			// Save all preferences and close the window
			prefs->SetBool("savewordonquit", saveWordOnQuit);
			prefs->SetInt32("maxwords", maxWords);
			prefs->SetBool("replicantenabled", replicantEnabled);
			prefs->SetBool("autolookword", autoLookWord);
			prefs->SetBool("autolooklang", autoLookLang);
			prefs->SetBool("autolooksearch", autoLookSearch);
			prefs->SetBool("autolooktrans", autoLookTrans);	
			prefs->SetBool("makeforeignbold", makeForeignBold);
			prefs->SetBool("makeforeigncolored", makeForeignColored);
			prefs->SetBool("isnumbering", isNumbering);
			prefs->SetInt32("fontsize", fontSize);
			prefs->SetBool("formatengtoforeign", formatEngToForeign);
			prefs->SetBool("fixedformat", fixedFormat);
			prefs->SetString("foreigncolor", foreignColor.String());
			prefs->Save();
			
		case PREF_CANCEL:
			// Remove the replicant if it wasn't previously enabled
 			if (prefs->FindBool("replicantenabled", &replicantEnabled) != B_OK || !replicantEnabled)
				AddDeskbar(false);
			
			// Enable the replicant if it was previously enabled
 			if (replicantEnabled)
 				AddDeskbar(true);	
				
			// Close the window without saving		
			PostMessage(B_QUIT_REQUESTED);
			break;

		case PREF_SAVE_WORD:
			if (SaveWordOnQuit->Value() == B_CONTROL_ON)
				saveWordOnQuit = true;
			else
				saveWordOnQuit = false;
			break;

		case PREF_REPLICANT:
			if (EnableReplicant->Value() == B_CONTROL_ON) {
				replicantEnabled = true;
				AddDeskbar(true);
			} else {
				replicantEnabled = false;
				AddDeskbar(false);
			}
			break;

		case PREF_MAX_WORDS:
			message->FindInt32("value", &maxWords);
			break;
			
		case PREF_AUTO_WORD:
			if (AutoLookWord->Value() == B_CONTROL_ON)
				autoLookWord = true;
			else
				autoLookWord = false;
			break;
			
		case PREF_AUTO_LANG:
			if (AutoLookLang->Value() == B_CONTROL_ON)
				autoLookLang = true;
			else
				autoLookLang = false;
			break;
					
		case PREF_AUTO_SRCH:
			if (AutoLookSearch->Value() == B_CONTROL_ON)
				autoLookSearch = true;
			else
				autoLookSearch = false;
			break;
					
		case PREF_AUTO_TRNS:
			if (AutoLookTrans->Value() == B_CONTROL_ON)
				autoLookTrans = true;
			else
				autoLookTrans = false;
			break;
							
		case PREF_FOR_BOLD:
			if (MakeForeignBold->Value() == B_CONTROL_ON)
				makeForeignBold = true;
			else
				makeForeignBold = false;
			break;		

		case PREF_FOR_COLOR:
			if (MakeForeignColored->Value() == B_CONTROL_ON) {
				makeForeignColored = true;
				ColorMenu->SetEnabled(true);
			} else {
				makeForeignColored = false;
				ColorMenu->SetEnabled(false);
			}
			break;

		case PREF_COLOR:
			message->FindString("color", &foreignColor);
			break;

		case PREF_NUMBERING:
			if (IsNumberingCheck->Value() == B_CONTROL_ON) 
				isNumbering = true;
			else
				isNumbering = false;
			break;			

		case PREF_FONT_SIZE:
			message->FindInt32("fontsize", &fontSize);
			break;			

		case PREF_FIX_FORMAT:
			if (FixFormatCheck->Value() == B_CONTROL_ON) {
				fixedFormat = true;
				EngToForeign->SetEnabled(true);
				ForeignToEng->SetEnabled(true);
			} else {
				fixedFormat = false;
				EngToForeign->SetEnabled(false);
				ForeignToEng->SetEnabled(false);
			}
			break;

		case PREF_ENG_FOR:
			formatEngToForeign = true;
			break;
			
		case PREF_FOR_ENG:
			formatEngToForeign = false;
			break;
					
		default:
			BWindow::MessageReceived(message);
	}
}
