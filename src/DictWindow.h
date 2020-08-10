#ifndef _DICT_WINDOW_H
#define _DICT_WINDOW_H

#include <Box.h>
#include <FilePanel.h>
#include <Menu.h>
#include <TextControl.h>
#include <TextView.h>
#include <Window.h>

#include "Preferences.h"
#include "PrefWindow.h"
#include "UpdateWindow.h"

#define DEF_FONT_SIZE	10
#define MAX_WORDS		10
#define WIDE_TEXT		2000

#define LINGUA_URL		"https://web.archive.org/web/20090306211015/http://www.aevum.net/lingua/"
#define GITHUB_URL		"https://github.com/HaikuArchives/Lingua/"
#define DEFAULT_LANG	"Latin"

#define GITHUB_PAGE		'ghpg'
#define CHANGE_LANG		'clan'
#define CLEAR_RESULTS	'cler'
#define DEFINE_WORD		'dfwd'
#define DICT_UPDATES	'dctu'
#define DONE_QUERY		'dnqy'
#define HELP_MSG		'hlpm'
#define LINGUA_WEBSITE	'linw'
#define NEW_WINDOW		'nwin'
#define PAGE_SETUP		'pgst'
#define PREFERENCES		'pref'
#define PRINT_RESULTS	'prnt'
#define QUIT_APP		'qapp'
#define RECEIVED_WORD	'rwrd'
#define SAVE_RESULTS	'savr'
#define SEND_TEXT		'sndt'
#define TYPE_TEXT		'tytx'
#define WORD_SELECTED	'wdsl'
#define WORD_WRAP		'wdwp' 

#define WHOLE_WORD		'whlw'
#define PARTIAL_WORD	'prtw'
#define FROM_ENGLISH	'fmen'
#define TO_ENGLISH		'toen'

class DictWindow : public BWindow {

	struct queryParams {
		DictWindow *itself;
		BFile *theFile;
		BString theString;
	};

	static int32 _DictQuery(void *data) {
		struct queryParams *q = (struct queryParams *)data;
		q->itself->DictQuery(q->theFile, q->theString);
		return 0;
	}

	public:
		DictWindow(BRect frame);
		virtual bool QuitRequested();
		virtual void MessageReceived(BMessage *message);
		virtual void FrameResized(float width, float height);
		virtual void Zoom(BPoint corner, float x, float y);
		
		virtual void ChangeLang(const char *language);
		virtual void UpdateWordMenu(const char *word, int32 maxOptions);
		virtual void UpdateLanguageMenu(BDirectory dir);
		virtual void DictQuery(BFile *dictFile, BString queryWord);
		virtual void PrintResult(BString english, BString foreign);
		virtual void EnableControls(bool how);
		
		status_t Save(BMessage *message);
		status_t SetUpPage();
		status_t Print();

	private:
		BFilePanel *SavePanel;
		BTextControl *WordBox;
		BTextView *DefText;
		BScrollView *Scroller;

		PrefWindow *thePrefWin;
		Preferences prefs;

		BMenu *WordMenu;
		BMenuField *WordField;
		BMenu *LangMenu;
		BMenuField *LangField;
		BButton *DefButton;
		BRadioButton *WholeWord;
		BRadioButton *PartialWord;
		BRadioButton *FromEnglish;
		BRadioButton *ToEnglish;
		BBox *TransBox;
		BBox *TopBox;
		BBox *BottomBox;

		BMenuBar *MenuBar;
		BMenu *fileMenu;
		BMenu *editMenu;
		BMenu *defsMenu;
		BMenu *helpMenu;
		BMenuItem *WordWrap;

		BMessage *printSettings;
		
		BRect defBox;
		BRect defTextBox;
		BRect defTextWide;
		
		UpdateWindow *updateWin;

		int32 currentFontSize;
		
		node_ref dictRef;

		bool isSearching;
		thread_id query_thread;

		bool isTall;
		BRect shortRect;
		BPoint shortPoint;

		BString queryWord;
		BString dictLanguage;
		BString foreignColor;
		text_run_array foreignword;
		text_run_array normalword;
		const char *savedWord;
		float winWidth;
		float winHeight;
		bool replicantEnabled;
		bool fromEnglish;
		bool wholeWord;
		bool saveWordOnQuit;
		bool autoLookWord;
		bool autoLookLang;
		bool autoLookSearch;
		bool autoLookTrans;		
		bool formatEngToForeign;
		bool makeForeignBold;
		bool makeForeignColored;
		bool wordWrap;
		bool fixedFormat;
		bool isNumbering;
		int32 totalMatches;
		int32 numbering;
		int32 maxWords;
};

#endif
