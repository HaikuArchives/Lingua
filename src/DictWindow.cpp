#include <Application.h>
#include <Alert.h>
#include <Button.h>
#include <Box.h>
#include <ClassInfo.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FilePanel.h>
#include <Font.h>
#include <List.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <Node.h>
#include <NodeMonitor.h>
#include <Path.h>
#include <Point.h>
#include <PrintJob.h>
#include <RadioButton.h>
#include <Roster.h>
#include <Screen.h>
#include <ScrollView.h>
#include <String.h>
#include <StringView.h>
#include <TextControl.h>
#include <TranslationUtils.h>

#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "DictApplication.h"
#include "DictWindow.h"
#include "Utilities.h"

DictWindow::DictWindow(BRect frame)
	: BWindow(frame, "Lingua", B_DOCUMENT_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	int32 i;					// For various loops
	isSearching = false; 		// True when searching, false when "Stop" is pushed
	
	/* ======================== Load Previous Settings ======================== */
	
	// If the replicant was previously launched, relaunch it
	if ((prefs.FindBool("replicantenabled", &replicantEnabled) == B_OK) && replicantEnabled)
		AddDeskbar(true);
	else
		AddDeskbar(false);
	
	// Save the word in the wordbox when you quit?
	if (prefs.FindBool("savewordonquit", &saveWordOnQuit) != B_OK)
		saveWordOnQuit = true;

	// Possible saved defining word, set to empty if it's not saved
	if (saveWordOnQuit) {
		if (prefs.FindString("savedword", &savedWord) != B_OK) 
			savedWord = "";
	} else {
		savedWord = "";
	}

	// Maximum words in the "Word:" menu. This is also re-evaluated
	// every time a search is performed.	
	if (prefs.FindInt32("maxwords", &maxWords) != B_OK)
		maxWords = MAX_WORDS;

	// Language to define to, defaults to Latin
	if (prefs.FindString("dictlanguage", &dictLanguage) != B_OK)
		dictLanguage = DEFAULT_LANG;
	
	// "Search For" and "Language" radio buttons
	if (prefs.FindBool("fromenglish", &fromEnglish) != B_OK) 
		fromEnglish = true;	
	if (prefs.FindBool("wholeword", &wholeWord) != B_OK)
		wholeWord = false;

	// Size of font where the definitions are spit out
	if (prefs.FindInt32("fontsize", &currentFontSize) != B_OK)
		currentFontSize = DEF_FONT_SIZE;
	
	// Whether or not word wrap is on
	if (prefs.FindBool("wordwrap", &wordWrap) != B_OK)
		wordWrap = true;

	// If this is the only window, try and get the saved size. If it's not the only window,
	// or there were no saved settings, use the frame rect that was passed.
	BScreen theScreen;
	BRect wholeScreen = theScreen.Frame();
	BRect windowSize;
	if (be_app->CountWindows() == 1) {
		if (prefs.FindRect("winsize", &windowSize) != B_OK)
			windowSize = frame;
	} else {
		windowSize = frame;
	}

	// Make sure it's not sized offscreen
	if ((windowSize.left > wholeScreen.Width()) || (windowSize.top > wholeScreen.Height())) {
		MoveTo(WIN_LEFT, WIN_TOP);
		winWidth = WIN_WIDTH;
		winHeight = WIN_HEIGHT;
	} else {
		MoveTo(windowSize.left, windowSize.top);
		winWidth = windowSize.Width();
		winHeight = windowSize.Height();
	}
	ResizeTo(winWidth, winHeight);
	
	// Make sure the window doesn't get shrunk too far
	SetSizeLimits(WIN_WIDTH, 4000, 300, 4000);
	
	// The window defaults to not being tall
	isTall = false;
	
	/* ========================== Outer Box ============================= */

	// Define the background, so the color matches
	BRect backRect = Bounds();
	backRect.top = 18;
	BView *background = new BView(backRect, NULL, B_FOLLOW_ALL_SIDES, 0);
	rgb_color grey = ui_color(B_PANEL_BACKGROUND_COLOR);
	background->SetViewColor(grey);
	background->SetLowColor(grey);
	AddChild(background);
	
	/* ========================= Top Controls =========================== */

	//	Word dropdown menu
	BRect wordRect(10, 10, 70, 30);
	WordMenu = new BMenu("Word:");

	const char *queryWord;
	if (prefs.FindString("words", &queryWord) == B_OK) {
		for (i=0; prefs.FindString("words", i, &queryWord) == B_OK; i++) {
			BMessage *wrdmsg = new BMessage(WORD_SELECTED);
			wrdmsg->AddString("word", queryWord);
			WordMenu->AddItem(new BMenuItem(queryWord, wrdmsg));
		}
	}
	WordField = new BMenuField(wordRect, "WordPopUp", NULL, WordMenu);
	
	background->AddChild(WordField);

	// Add the input text box
	BRect textRect(65, 12, 165, 30);
	WordBox = new BTextControl(textRect, NULL, NULL, savedWord, NULL, B_FOLLOW_NONE, B_WILL_DRAW | B_NAVIGABLE);
	WordBox->SetModificationMessage(new BMessage(TYPE_TEXT));
	background->AddChild(WordBox);
	WordBox->MakeFocus(true);

	// Stick the button in there
	BRect buttonRect(315, 9, 395, 13);	
	DefButton = new BButton(buttonRect, "DefBtnName", "Translate", new BMessage(DEFINE_WORD));
	DefButton->MakeDefault(true);
	background->AddChild(DefButton);

	// Enable or disable the define button, depending on a possible saved word
	int32 savedWordLen = strlen(savedWord);
	if (savedWordLen > 0) {
		for (i=0; i<savedWordLen; i++) {
			if (!isspace(savedWord[i]))
				DefButton->SetEnabled(true);
			else
				DefButton->SetEnabled(false);
		}
	} else {
		DefButton->SetEnabled(false);
	}

	// Radio buttons between 'partial' and 'whole word'
	BRect box1(10, 40, 200, 95);
	BBox *searchBox = new BBox(box1, NULL);
	searchBox->SetLabel("Search For");
	
	BRect partBox(10, 13, 180, 30);
	BRect wholeBox(10, 31, 180, 43);

	PartialWord = new BRadioButton(partBox, "PartRadio", "Phrases Containing This Word",
		new BMessage(PARTIAL_WORD));
	searchBox->AddChild(PartialWord);		
	WholeWord = new BRadioButton(wholeBox, "WholeRadio", "This Word Exactly",
		new BMessage(WHOLE_WORD));
	searchBox->AddChild(WholeWord);

	if (wholeWord)
		WholeWord->SetValue(B_CONTROL_ON);
	else
		PartialWord->SetValue(B_CONTROL_ON);
	
	background->AddChild(searchBox);
	
	// Radio buttons between 'to english' and 'from english'
	BRect box2(210, 40, 400, 95);
	TransBox = new BBox(box2, NULL);
	TransBox->SetLabel("Translate");
	
	BRect fromEngBox(10, 13, 175, 30);
	BRect toEngBox(10, 31, 175, 43);
	
	FromEnglish = new BRadioButton(fromEngBox, "FromEngRadio", NULL, new BMessage(FROM_ENGLISH));
	TransBox->AddChild(FromEnglish);
	ToEnglish = new BRadioButton(toEngBox, "ToEngRadio", NULL, new BMessage(TO_ENGLISH));
	TransBox->AddChild(ToEnglish);

	// Adjust the labels according to the language
	ChangeLang(dictLanguage.String());

	if (fromEnglish)
		FromEnglish->SetValue(B_CONTROL_ON);
	else
		ToEnglish->SetValue(B_CONTROL_ON);
	
	background->AddChild(TransBox);
	
	/* ========== Stuff for finding the dicts subdirectory ============
	   Note that all this is implimented after the above BRadioButtons,
	   because the UpdateLanguageMenu function may alter their labels
	   and they need to be attached first, else we segfault.	       */
	
	app_info appInfo;
	be_roster->GetAppInfo(APP_SIG, &appInfo);
	BEntry appEntry(&appInfo.ref);
	appEntry.GetParent(&appEntry);
	BDirectory dictsDir(&appEntry);

	if (dictsDir.SetTo(&dictsDir, "dicts") != B_OK) {
		BAlert *doh = new BAlert("doh", "Couldn't open dictionary directory!\n\nLingua can't find a a folder called \"dicts\" in the same folder as the application. Without it, Lingua can't do any translations.", "doh!", NULL, NULL, 
			B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_STOP_ALERT);
		doh->Go();
	} else {
		// Assemble the pop-up list, depending on what files are in the dict directory
		BString langFileName = dictLanguage;
		langFileName += ".txt";
		if (dictsDir.Contains(langFileName.String()))
			LangMenu = new BMenu(dictLanguage.String());
		else
			LangMenu = new BMenu(DEFAULT_LANG);
		
		LangMenu->SetLabelFromMarked(true);		
		
		BRect langRect(170, 10, 320, 20);  
		LangField = new BMenuField(langRect, "LangPopUp", "Language:", LangMenu);
		LangField->SetDivider(60);
		LangField->SetAlignment(B_ALIGN_RIGHT);
		background->AddChild(LangField);

		UpdateLanguageMenu(dictsDir);
	
		// Set up a node watcher to monitor the dicts directory
		dictsDir.GetNodeRef(&dictRef);
		watch_node(&dictRef, B_WATCH_ALL, this);
	}	

	/* ========================= Bottom Text Area =========================== */
	
	defBox = backRect;
	defBox.top = 105;
	defBox.right -= B_V_SCROLL_BAR_WIDTH;
	defBox.bottom -= B_H_SCROLL_BAR_HEIGHT + 18;

	defTextBox = defBox;
	defTextBox.top = 0;
	defTextBox.InsetBy(3, 0);

	defTextWide = defTextBox;
	defTextWide.right = WIDE_TEXT;
	
	DefText = new BTextView(defBox, "results", defTextBox, B_FOLLOW_ALL, B_WILL_DRAW | B_FRAME_EVENTS);
	BFont theFontSize;
	theFontSize.SetSize((float)currentFontSize);
	DefText->SetFontAndColor(&theFontSize);
	DefText->SetStylable(true);
	DefText->MakeEditable(false);

	Scroller = new BScrollView("Scroller", DefText, B_FOLLOW_ALL, 0, true, true);
	background->AddChild(Scroller);	
	
	/* ============================== Menu Bar =============================== */
	
	BRect menuBarRect = Bounds();
	menuBarRect.bottom = 18;
	
	MenuBar = new BMenuBar(menuBarRect, "MenuBar");
	AddChild(MenuBar);
	
	// ---- File Menu ----
	fileMenu = new BMenu("File");
	fileMenu->AddItem(new BMenuItem("New Window", new BMessage(NEW_WINDOW),	'N', B_COMMAND_KEY));
	fileMenu->AddItem(new BMenuItem("Save Results...", new BMessage(SAVE_RESULTS), 'S', B_COMMAND_KEY));
	fileMenu->AddItem(new BMenuItem("Dictionary Updates...", new BMessage(DICT_UPDATES), 'U', B_COMMAND_KEY));
	fileMenu->AddSeparatorItem();
	fileMenu->AddItem(new BMenuItem("Page Setup...", new BMessage(PAGE_SETUP)));
	fileMenu->AddItem(new BMenuItem("Print...", new BMessage(PRINT_RESULTS), 'I', B_COMMAND_KEY));	
	fileMenu->AddSeparatorItem();
	fileMenu->AddItem(new BMenuItem("About Lingua", new BMessage(B_ABOUT_REQUESTED)));	
	fileMenu->AddSeparatorItem();
	fileMenu->AddItem(new BMenuItem("Close", new BMessage(B_QUIT_REQUESTED), 'W', B_COMMAND_KEY));
	fileMenu->AddItem(new BMenuItem("Quit", new BMessage(QUIT_APP),	'Q', B_COMMAND_KEY));

	// ---- Edit Menu ----
	editMenu = new BMenu("Edit");
	BMenuItem *txt;
	editMenu->AddItem(txt = new BMenuItem("Copy", new BMessage(B_COPY), 'C'));
	txt->SetTarget(NULL, this);	
	editMenu->AddItem(txt = new BMenuItem("Paste", new BMessage(B_PASTE), 'V'));
	txt->SetTarget(NULL, this);	
	editMenu->AddItem(txt = new BMenuItem("Select All",	new BMessage(B_SELECT_ALL), 'A'));
	txt->SetTarget(NULL, this);	
	editMenu->AddSeparatorItem();
	editMenu->AddItem(new BMenuItem("Preferences...", new BMessage(PREFERENCES), 'P', B_COMMAND_KEY));
		
	// ---- Translations Menu ----
	defsMenu = new BMenu("Translations");
	defsMenu->AddItem(WordWrap = new BMenuItem("Word Wrap",	new BMessage(WORD_WRAP), 'R', B_COMMAND_KEY));
	defsMenu->AddItem(new BMenuItem("Clear Results", new BMessage(CLEAR_RESULTS)));
	if (wordWrap)
		WordWrap->SetMarked(true);
	else
		WordWrap->SetMarked(false);
	
	// ----- Help Menu -----
	helpMenu = new BMenu("Help");
	helpMenu->AddItem(new BMenuItem("Documentation...", new BMessage(HELP_MSG)));
	helpMenu->AddSeparatorItem();
	helpMenu->AddItem(new BMenuItem("Lingua Website", new BMessage(LINGUA_WEBSITE)));
	helpMenu->AddItem(new BMenuItem("BeBits Page", new BMessage(BEBITS_PAGE)));
	
	MenuBar->AddItem(fileMenu);
	MenuBar->AddItem(editMenu);
	MenuBar->AddItem(defsMenu);
	MenuBar->AddItem(helpMenu);

	Show();
}

void DictWindow::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case TYPE_TEXT:
			const char *thetext;
			int32 textlen, i;

			thetext = WordBox->Text();
			textlen = strlen(thetext);
			
			// Test if something is there, and if it's more than just spaces. Code later
			// on tests if the DefButton is enabled, assuming that if it is, there must
			// be something valid to define. 
			if (textlen > 0) {
				for (i=0; i<textlen; i++) {
					if (!isspace(thetext[i]))
						DefButton->SetEnabled(true);
					else
						DefButton->SetEnabled(false);
				}
			} else {
				DefButton->SetEnabled(false);
			}
			break;		
		
		case RECEIVED_WORD:
		// This is called when something other than the original Lingua window,
		// such as the Deskbar replicant, wants to translate a word.
		{
			BString word;
			if (message->FindString("word", &word) != B_OK)
				break;

			WordBox->SetText(word.String());
			PostMessage(DEFINE_WORD);
		}
			break;			
		
		case SEND_TEXT:
			// Stop if the button isn't enabled; else define the word	
			if (!DefButton->IsEnabled())
				break;
	
		case DEFINE_WORD:
		// Make sure that any code that posts the DEFINE_WORD message first checks
		// if the DefButton is enabled. If it's not, there isn't anything to define,
		// and searching would be a bad, bad thing. Therefore, to translate, post
		// a SEND_TEXT message instead of a DEFINE_WORD unless you check the integrity
		// of the string to define yourself.
		{
			
			BFile *DictFile;
			BString DictFilePath;

			// If they're already searching, they must have hit the "stop" button
			// so post a done message and stop defining
			if (isSearching) {
				isSearching = false;
				PostMessage(DONE_QUERY);
				break;
			}			

			isSearching = true;
			totalMatches = 0;

			// Get the formatting preference
			prefs.Load();
			if (prefs.FindBool("formatengtoforeign", &formatEngToForeign) != B_OK)
				formatEngToForeign = true;
				
			if (prefs.FindBool("makeforeignbold", &makeForeignBold) != B_OK) 
				makeForeignBold = true;
				
			if (prefs.FindBool("makeforeigncolored", &makeForeignColored) != B_OK) 
				makeForeignColored = false;
				
			if (prefs.FindBool("fixedformat", &fixedFormat) != B_OK)
				fixedFormat = false;
				
			if (prefs.FindInt32("maxwords", &maxWords) != B_OK)
				maxWords = MAX_WORDS;
				
			if (prefs.FindString("foreigncolor", &foreignColor) != B_OK)
				foreignColor = "blue";
				
			if (prefs.FindInt32("fontsize", &currentFontSize) != B_OK)
				currentFontSize = DEF_FONT_SIZE;
				
			if (prefs.FindBool("isnumbering", &isNumbering) != B_OK)
				isNumbering = false;
			
			// Disable the controls so they can't fuck anything up
			DefButton->SetLabel("Stop");
			DefText->MakeSelectable(false);
			EnableControls(false);

			// Clear out old results
			DefText->SetText("");
			
			// Strip leading and trailing whitespace from the word
			queryWord = WordBox->Text();
			while (queryWord[0] == ' ')
				queryWord.RemoveFirst(" ");
			while (queryWord[queryWord.Length()-1] == ' ')
				queryWord.RemoveLast(" ");
				
			// Update the window's Title tab to reflect the new word
			BString title;
			title << "Lingua: " << queryWord << " (searching...)";
			SetTitle(title.String());
				
			// Tell the app to add the new word into the "Word:" list of
			// each currently open window. This keeps the windows in sync
			BMessage *updateWordMsg = new BMessage(UPDATE_WORD_MENU);
			updateWordMsg->AddString("queryWord", queryWord);
			updateWordMsg->AddInt32("maxWords", maxWords);
			be_app->PostMessage(updateWordMsg);

			// Open the dictionary, depending on the language, by appending a .txt ending
			// to the language and opening up that file
			app_info appInfo;
			be_roster->GetAppInfo(APP_SIG, &appInfo);
			BEntry appEntry(&appInfo.ref);
			appEntry.GetParent(&appEntry);
			BPath dictsPath(&appEntry);

			DictFilePath << "dicts/" << dictLanguage << ".txt";
			status_t checker = dictsPath.Append(DictFilePath.String());
			if (checker != B_OK) {
				DefText->Insert("Language path was screwy: ");
				DefText->Insert(DictFilePath.String());
				PostMessage(DONE_QUERY);
				break;
			}
			
			// If the dictionary opens, find all the matching words and print
			DictFile = new BFile(dictsPath.Path(), B_READ_ONLY);
			status_t err = DictFile->InitCheck();
			if (err != B_OK) {
				DefText->Insert("Couldn't find a dictionary file at ");
				DefText->Insert(dictsPath.Path());
				PostMessage(DONE_QUERY);
				break;
			}
			
			// Define style for the two types of fonts, plus colors
			rgb_color black = {0, 0, 0, 255};
			rgb_color red = {204, 0, 0, 255};
			rgb_color green = {0, 102, 0, 255};
			rgb_color blue = {0, 0, 204, 204};
			rgb_color orange = {204, 102, 0, 255};
			rgb_color purple = {102, 0, 102, 255};
			rgb_color tan = {153, 153, 102, 255};
			rgb_color sky = {0, 102, 153, 255};
			rgb_color aqua = {0, 102, 102, 255};
			
			foreignword.count = 1;
			foreignword.runs[0].offset = 0;
			
			// Choose the color. I've *got* to find a better way of doing this.
			if (makeForeignColored) {
				if (foreignColor == "red")
					foreignword.runs[0].color = red;
				else if (foreignColor == "green")
					foreignword.runs[0].color = green;
				else if (foreignColor == "orange")
					foreignword.runs[0].color = orange;
				else if (foreignColor == "purple")
					foreignword.runs[0].color = purple;
				else if (foreignColor == "tan")
					foreignword.runs[0].color = tan;
				else if (foreignColor == "sky")
					foreignword.runs[0].color = sky;
				else if (foreignColor == "aqua")
					foreignword.runs[0].color = aqua;
				else
					foreignword.runs[0].color = blue;
			} else {
				foreignword.runs[0].color = black;
			}
		
			// Bold words need to be at 2 sizes larger than normal to show properly
			if (makeForeignBold) {
				foreignword.runs[0].font = *be_bold_font;
				foreignword.runs[0].font.SetSize((float)currentFontSize + 2.0);
			} else {
				foreignword.runs[0].font = *be_plain_font;
				foreignword.runs[0].font.SetSize((float)currentFontSize);					
			} 
			
			normalword.count = 1;				
			normalword.runs[0].offset = 0;
			normalword.runs[0].font = *be_plain_font;
			normalword.runs[0].font.SetSize((float)currentFontSize);			
			normalword.runs[0].color = black; 

			// Make a struct to hold all the data passed to the threadfunction
			struct queryParams *data = new queryParams;
			data->itself = this;
			data->theFile = DictFile;
			data->theString = queryWord;
			
			// Run a dictionary query
			query_thread = spawn_thread(_DictQuery, "Dictionary Query", B_NORMAL_PRIORITY, data);
			resume_thread(query_thread);
		}
			break;
			
		case DONE_QUERY:
		{
			BString endresult;
			
			// If isSearching is true, they must have hit the "stop" button, so
			// kill the thread query thread
			if (isSearching) {
				isSearching = false;
				kill_thread(query_thread);
			}
			
			// Set window title to reflect number of results
			BString title;
			if (totalMatches == 0)
				title << "Lingua: " << queryWord << " (no results)";
			else if (totalMatches == 1)
				title << "Lingua: " << queryWord << " (1 result)";
			else
				title << "Lingua: " << queryWord << " (" << totalMatches << " results)";
				
			SetTitle(title.String());
			
			// Re-enable all the controls
			DefButton->SetLabel("Translate");
			DefText->MakeSelectable(true);
			EnableControls(true);
			
			// Select the text in the WordBox
			WordBox->TextView()->Select(0, strlen(WordBox->Text()));
		}	
			break;

		case B_NODE_MONITOR:
		{
			// Update the Language pop-up menu every time a file is added,
			// removed, or modified in the dicts directory 
			app_info appInfo;
			be_roster->GetAppInfo(APP_SIG, &appInfo);
			BEntry appEntry(&appInfo.ref);
			appEntry.GetParent(&appEntry);
			BDirectory dictsDir(&appEntry);
		
			if (dictsDir.SetTo(&dictsDir, "dicts") != B_OK) {
				BAlert *doh = new BAlert("doh", "Couldn't open dictionary directory!\n\nLingua can't find a a folder called \"dicts\" in the same folder as the application. Without it, Lingua can't do any translations.", "doh!", NULL, NULL, 
					B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_STOP_ALERT);
				doh->Go();
			} else {
				UpdateLanguageMenu(dictsDir);
			}
		}
			break;

		case WHOLE_WORD:
			wholeWord = true;
			if (prefs.FindBool("autolooksearch", &autoLookSearch) != B_OK)
				autoLookSearch = false;
			if (autoLookSearch)
				PostMessage(SEND_TEXT);
			break;
			
		case PARTIAL_WORD:
			wholeWord = false;
			if (prefs.FindBool("autolooksearch", &autoLookSearch) != B_OK)
				autoLookSearch = false;
			if (autoLookSearch)
				PostMessage(SEND_TEXT);
			break;
			
		case FROM_ENGLISH:
			fromEnglish = true;
			if (prefs.FindBool("autolooktrans", &autoLookTrans) != B_OK)
				autoLookTrans = false;
			if (autoLookTrans)
				PostMessage(SEND_TEXT);
			break;
	
		case TO_ENGLISH:
			fromEnglish = false;
			if (prefs.FindBool("autolooktrans", &autoLookTrans) != B_OK)
				autoLookTrans = false;
			if (autoLookTrans)
				PostMessage(SEND_TEXT);
			break;

		case WORD_SELECTED:
			const char *newword;
			if (message->FindString("word", &newword) == B_OK) {
				WordBox->SetText(newword);
				
				if (prefs.FindBool("autolookword", &autoLookWord) != B_OK) 
					autoLookWord = true;
				if (autoLookWord)
					PostMessage(SEND_TEXT);
			}
			break;

		case CHANGE_LANG:
			const char *lang;
			if (message->FindString("lang", &lang) == B_OK) {
				ChangeLang(lang);
				if (prefs.FindBool("autolooklang", &autoLookLang) != B_OK) 
					autoLookLang = true;
				if (autoLookLang)
					PostMessage(SEND_TEXT);
			}
			break;

		case NEW_WINDOW:
			be_app->PostMessage(NEW_LINGUA_WINDOW); 
			break;

		case SAVE_RESULTS:
			SavePanel = new BFilePanel(B_SAVE_PANEL, new BMessenger(this), NULL,
				B_FILE_NODE, false);
			SavePanel->Show();			
			break;
		
		case B_SAVE_REQUESTED:
			Save(message);
			break;
			
		case DICT_UPDATES:
		{
			// Check if an Update window is already open
			bool winOpen = false;
			int32 winNum;
			for (winNum=0; winNum<be_app->CountWindows(); winNum++) {
				if (is_kind_of(be_app->WindowAt(winNum), UpdateWindow)) {
					winOpen = true;
					break;
				}
			}
			
			// If so, make it the focus window
			if (winOpen) {
				be_app->WindowAt(winNum)->Activate();
			} else {	
				// Make a new Update Dictionaries window in the middle of the screen
				BScreen *theScreen = new BScreen();
				BRect updateBox = theScreen->Frame();
				float theWidth = updateBox.right;
				float theHeight = updateBox.bottom;
				
				updateBox.left = (theWidth - UPDATE_WIDTH) / 2;
				updateBox.top = (theHeight - UPDATE_HEIGHT) / 2;
				updateBox.right = updateBox.left + UPDATE_WIDTH;
				updateBox.bottom = updateBox.top + UPDATE_HEIGHT;				
	
				updateWin = new UpdateWindow(updateBox);
				updateWin->Show();
			}
		}
			break;
		
		case PAGE_SETUP:
			SetUpPage();
			break;
			
		case PRINT_RESULTS:
		{
			status_t printresult = Print();
			if (printresult != B_OK) {
				BAlert *printError = new BAlert("printer error", "Could not print, problems abound.", "OK");
				printError->Go();
			}
		}
			break;
			
		case B_ABOUT_REQUESTED:
			be_app->PostMessage(B_ABOUT_REQUESTED);
			break;

		case QUIT_APP:
			// Close everything down
			PostMessage(B_QUIT_REQUESTED);
			AddDeskbar(false);
			be_app->PostMessage(B_QUIT_REQUESTED);
			break;

		case PREFERENCES:
		{
			// Check if a preferences window is already open
			bool prefWinOpen = false;
			int32 winNum;
			for (winNum=0; winNum<be_app->CountWindows(); winNum++) {
				if (is_kind_of(be_app->WindowAt(winNum), PrefWindow)) {
					prefWinOpen = true;
					break;
				}
			}
			
			// If so, make it the focus window
			if (prefWinOpen) {
				be_app->WindowAt(winNum)->Activate();
			} else {
				// Make a new prefs window in the middle of the screen
				BScreen *theScreen = new BScreen();
				BRect prefBox = theScreen->Frame();
				float theWidth = prefBox.right;
				float theHeight = prefBox.bottom;
				
				prefBox.left = (theWidth - PREF_WIDTH) / 2;
				prefBox.top = (theHeight - PREF_HEIGHT) / 2;
				prefBox.right = prefBox.left + PREF_WIDTH;
				prefBox.bottom = prefBox.top + PREF_HEIGHT;				
	
				thePrefWin = new PrefWindow(prefBox, &prefs);
				thePrefWin->Show();
			}
		}
			break;

		case WORD_WRAP:
		{
			BMenuItem *wordWrapCheck;
			BRect textRect;
			
			wordWrapCheck = MenuBar->FindItem("Word Wrap");
			if (wordWrapCheck->IsMarked()) {
				wordWrapCheck->SetMarked(false);
				wordWrap = false;
				DefText->SetTextRect(defTextWide);
			} else {
				wordWrapCheck->SetMarked(true);
				wordWrap = true;
				textRect = DefText->TextRect();
				textRect.right = textRect.left + (winWidth - B_V_SCROLL_BAR_WIDTH - 3.0);
				DefText->SetTextRect(textRect);
			}
		}
			break;
			
		case CLEAR_RESULTS:
			DefText->SetText("");
			SetTitle("Lingua");
			break;

		case HELP_MSG:
		{
			// This next segment is some of the most bizarre crap I've had to conjure
			// to make C++ do what I want it to. Run away while you still can!
			app_info appInfo;
			be_roster->GetAppInfo(APP_SIG, &appInfo);
			BEntry appEntry(&appInfo.ref);
			appEntry.GetParent(&appEntry);
			BPath docsPath(&appEntry);
			if ((docsPath.Append("docs") != B_OK) || (docsPath.Append("index.html") != B_OK)) {
				DefText->Insert("Couldn't open documentation directory!");
				break;
			}
			const char *fuckDoIHateConstChars = docsPath.Path();
			char *help_url = const_cast<char *>(fuckDoIHateConstChars);
			be_roster->Launch(URL_TYPE, 1, &help_url);	
		}
			break;  

		case LINGUA_WEBSITE:
		{
			char *lingua_url = LINGUA_URL;
			be_roster->Launch(URL_TYPE, 1, &lingua_url);
		}
			break;
			
		case BEBITS_PAGE:
		{
			char *bebits_url = BEBITS_URL;
			be_roster->Launch(URL_TYPE, 1, &bebits_url);
		}
			break;
		
		default:
			BWindow::MessageReceived(message);
	}
}

bool DictWindow::QuitRequested()
{
	// Save all settings
	prefs.SetRect("winsize", Frame());
	prefs.SetBool("fromenglish", fromEnglish);
	prefs.SetBool("wholeword", wholeWord);
	prefs.SetBool("wordwrap", wordWrap);
	prefs.SetString("dictlanguage", dictLanguage.String());
	prefs.SetString("savedword", WordBox->Text());
	prefs.SetInt32("fontsize", currentFontSize);
	
	prefs.RemoveName("words");
	int32 i;
	for (i=0; i<WordMenu->CountItems(); i++)
		prefs.AddString("words", WordMenu->ItemAt(i)->Label());
	
	prefs.Save();

	// Kill any query threads
	if (isSearching) kill_thread(query_thread);

	// Stop the node watcher	
	watch_node(&dictRef, B_STOP_WATCHING, this);

	// If this is the last DictWindow, clean everything up
	int32 totalWindows = 0;
	for (i=0; i<be_app->CountWindows(); i++)
		if (is_kind_of(be_app->WindowAt(i), DictWindow))
			totalWindows++;
	
	if (totalWindows == 1) {
		be_app->PostMessage(B_QUIT_REQUESTED);		// Quit the whole app
		AddDeskbar(false);							// Remove the Deskbar replicant
	}

	return true;
}

void DictWindow::FrameResized(float width, float height)
{
	// If word wrap is on, keep it small. Otherwise, resize to the
	// outer boundaries, which are pretty big. Or something.
	BRect textRect = DefText->TextRect();
	if (MenuBar->FindItem("Word Wrap")->IsMarked())
		textRect.right = textRect.left + (width - B_V_SCROLL_BAR_WIDTH - 3.0);
	else
		textRect.right = textRect.left + (WIDE_TEXT + B_V_SCROLL_BAR_WIDTH - 3.0);
		
	DefText->SetTextRect(textRect);
	
	// Update the winWidth variable, for turning word wrap on and off,
	// and the winHeight variable, just for fun.
	winWidth = width;
	winHeight = height;
	
	// And to make everything special...
	BWindow::FrameResized(width, height);
}

status_t DictWindow::Save(BMessage *message)
{
	entry_ref ref;
	const char *name;
	status_t err;
	
	// Find the directory and filename
	if ((err = message->FindRef("directory", &ref)) != B_OK)
		return err;
	if ((err = message->FindString("name", &name)) != B_OK)
		return err;

	// Make a new BDirectory object, check if that went okay
	BDirectory dir(&ref);
	if ((err = dir.InitCheck()) != B_OK)
		return err;

	// Create a new file, check if that was cool
	BFile file(&dir, name, B_READ_WRITE | B_CREATE_FILE);
	if ((err = file.InitCheck()) != B_OK)
		return err;

	// Write the text in the definitions field into the file, using
	// the absurdly handy WriteStyledEditFile function
	err = BTranslationUtils::WriteStyledEditFile(DefText, &file);
	
	return err;
} 

void DictWindow::Zoom(BPoint corner, float x, float y)
{
	BRect theFrame = Frame();
	float winLeft = theFrame.left;
	float winTop = 25.0;	// Height of window tab
	float winWidth = theFrame.right - theFrame.left;

	if (!isTall) {
		isTall = true;
		shortRect = Frame();
		shortPoint.Set(shortRect.left, shortRect.top);		
		corner.Set(winLeft, winTop);	
		ResizeTo(winWidth, y);
		MoveTo(corner);
	} else {
		isTall = false;
		ResizeTo(shortRect.right - shortRect.left, shortRect.bottom - shortRect.top);
		MoveTo(shortPoint);
	}		
	
	// Do something with the x variable so it don't cause compiler errors.
	// I know there's a way around this, but don't look a gift shoe on the other side.
	// Or something.
	x += 12.3;
}

status_t DictWindow::SetUpPage()
{
	status_t result;
	BPrintJob printJob("Lingua Results");
	
	// Display configuration panel
	result = printJob.ConfigPage();

	// Save pointer to settings message
	printSettings = printJob.Settings();
	
	return result;	
}

status_t DictWindow::Print() 
{ 
	BPrintJob printJob("Lingua Results"); 
	 
	if (SetUpPage() != B_OK) { 
		return B_OK;	// Job was canceled, but we don't want an error alert 
	}  

	printJob.SetSettings(new BMessage(*printSettings));
 
 	if(printJob.ConfigJob() == B_OK) 
	{ 
		delete printSettings; 
		printSettings = printJob.Settings(); 
	} 

	printJob.BeginJob(); 
	bool can_continue = printJob.CanContinue();

	// Calculate the number of pages 
	int32 firstPage = printJob.FirstPage(); 
	int32 lastPage = printJob.LastPage();

	float printHeight = printJob.PrintableRect().bottom;
	float heightOfResults = DefText->TextHeight(0, totalMatches);
	int32 theLastPage = (int32)ceil(heightOfResults / printHeight);
	
	if (theLastPage < lastPage)
		lastPage = theLastPage;

	if ((lastPage - firstPage) < 0) {
		return B_ERROR;
	}
	
	// Calculate how many lines can fit on a page
	BRect printRect(printJob.PrintableRect());
	float heightOfLine = DefText->LineHeight(0);
	float linesPerPage = floor(printHeight / heightOfLine);
	float pageAdjust = linesPerPage * heightOfLine;
	printRect.bottom = pageAdjust;

	// Spool each page, using the wide DefText rectangle
	for (int i=firstPage; can_continue && i<=lastPage; i++) {
		printRect.OffsetTo(0, (i-1)*pageAdjust);
		printJob.DrawView(DefText, printRect, BPoint(0, 0)); 
		printJob.SpoolPage();
		can_continue = printJob.CanContinue(); 
	} 
	 
	// If we're clear, commit the job and print the damn thing 
	if (can_continue) { 
		printJob.CommitJob(); 
	} else {
		return B_ERROR; 
	}		 
	
	return B_OK; 
}

void DictWindow::DictQuery(BFile *dictFile, BString queryWord)
{
	bool foundWord = false;
	int32 tabPos;
	BString english;
	BString foreign;
	BString line;
	
	numbering = 1;
	
	while (yoinkline(dictFile, line) && isSearching) {
		// Flush the old words, find the new ones
		english = foreign = line;
		tabPos = line.IFindFirst("\t");
		english.Remove(tabPos, line.Length()-tabPos);
		foreign.Remove(0, tabPos+1);
		
		// Check to see if we still have a word
		if (tabPos == B_ERROR)
			continue;					
		
		// Do language comparisons, using handy-dandy functions like ICompare
		// instead of making everything lowercase, which was retarded. Thanks
		// to Zippy for the tippy... oh god that was awful. Sorry. 
		if (wholeWord) {
			if (fromEnglish && ICompare(english, queryWord) == 0)
				foundWord = true;
			else if (!fromEnglish && ICompare(foreign, queryWord) == 0)
				foundWord = true;
		} else {
			if (fromEnglish && contains(english, queryWord)) 
				foundWord = true;
			else if (!fromEnglish && contains(foreign, queryWord))
				foundWord = true;
		}
		
		// Print the line if a match is found, with the right format
		if (foundWord) {
			PrintResult(english, foreign);
			totalMatches++;
			numbering++;
			foundWord = false;
		}
	}
	
	// Once the query has run through the file completely...
	isSearching = false;
	PostMessage(DONE_QUERY);
}

void DictWindow::PrintResult(BString english, BString foreign)
{
	Lock();

	if (isNumbering) {
		BString number;
		number << numbering << ". ";
		DefText->Insert(number.String(), &normalword);
	}
	
	if ((fromEnglish && !fixedFormat) || (formatEngToForeign && fixedFormat)) {
		DefText->Insert(english.String(), &normalword);
		DefText->Insert(" - ", &normalword);
		DefText->Insert(foreign.String(), &foreignword);
	} else {
		DefText->Insert(foreign.String(), &foreignword);
		DefText->Insert(" - ", &normalword);
		DefText->Insert(english.String(), &normalword);
	}
	DefText->Insert("\n");

	Unlock();
}

void DictWindow::ChangeLang(const char *language)
{
	dictLanguage = language;

	BString label1;
	label1 << "From English To " << language;
	BString label2;
	label2 << "From " << language << " To English";

	FromEnglish->SetLabel(label1.String());
	ToEnglish->SetLabel(label2.String());
}

void DictWindow::UpdateWordMenu(const char *word, int32 maxOptions)
{
	// If the word is already on the list, remove it first
	for (int32 i=0; i<WordMenu->CountItems(); i++) 
		if (strcmp(WordMenu->ItemAt(i)->Label(), word) == 0) 
			delete WordMenu->RemoveItem(i);

	// Add the new word to the list
	BMessage *wrdmsg = new BMessage(WORD_SELECTED);
	wrdmsg->AddString("word", word);
	WordMenu->AddItem(new BMenuItem(word, wrdmsg), 0);

	// Clear out the extra menu options
	while (WordMenu->CountItems() > maxOptions)
		delete WordMenu->RemoveItem(maxOptions);
		
	// Save the settings so that newly opened windows will have
	// all the previous "Word:" menu options
	prefs.RemoveName("words");
	for (int32 i=0; i<WordMenu->CountItems(); i++)
		prefs.AddString("words", WordMenu->ItemAt(i)->Label());
	prefs.Save();	
}

void DictWindow::UpdateLanguageMenu(BDirectory dir)
{
	BEntry fileEntry;
	BString fileName;
	BMenuItem *tmp;
	char tmpFileName[B_FILE_NAME_LENGTH];

	// Delete all items in the pop up menu
	int32 zero = 0;
	while (LangMenu->CountItems() > zero)
		delete LangMenu->RemoveItem(zero);

	// Add in the new entries 
	while (dir.GetNextEntry(&fileEntry) != B_ENTRY_NOT_FOUND) {
		fileEntry.GetName(tmpFileName);
		fileName = tmpFileName;
		fileName.RemoveLast(".txt");
		BMessage *msg = new BMessage(CHANGE_LANG);
		msg->AddString("lang", fileName.String());
		tmp = new BMenuItem(fileName.String(), msg);
		LangMenu->AddItem(tmp);
		
		if (fileName == dictLanguage)
			LangMenu->ItemAt(LangMenu->IndexOf(tmp))->SetMarked(true);
	}
	
	// If they moved the current language out of the dicts directory,
	// change the language to the topmost one instead
	if (LangMenu->FindItem(dictLanguage.String()) == NULL) {
		if (LangMenu->CountItems() > 0) {
			// Set it to the topmost file
			ChangeLang(LangMenu->ItemAt(0)->Label());
			if (!isSearching)
				EnableControls(true);
		} else {
			// No files left, disable everything
			LangMenu->AddItem(new BMenuItem("<no files>", new BMessage('none')));
			EnableControls(false);
		}			
		LangMenu->ItemAt(0)->SetMarked(true);
	} else if (!isSearching) {
		EnableControls(true);
	}	
}

void DictWindow::EnableControls(bool how)
{
	WordBox->SetEnabled(how);
	WordMenu->SetEnabled(how);
	WholeWord->SetEnabled(how);
	PartialWord->SetEnabled(how);
	FromEnglish->SetEnabled(how);
	ToEnglish->SetEnabled(how);
	LangMenu->SetEnabled(how);
}
