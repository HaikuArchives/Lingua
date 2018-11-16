#ifndef PREFWINDOW_H
#define PREFWINDOW_H

#include <Window.h>
#include <String.h>

#include "Preferences.h"

#define PREF_WIDTH		240
#define PREF_HEIGHT		250

#define PREF_OKAY		'poky'
#define PREF_CANCEL		'pccl'
#define PREF_COLOR		'pcbl'
#define PREF_FIX_FORMAT	'pfxf'
#define PREF_FONT_SIZE	'pffz'
#define PREF_FOR_BOLD	'pfnb'
#define PREF_FOR_COLOR	'pfcl'
#define PREF_FOR_ENG	'pfte'
#define PREF_NUMBERING	'prnm'
#define PREF_SAVE_WORD	'pswd'
#define PREF_ENG_FOR	'petf'
#define PREF_MAX_WORDS	'pmxw'
#define PREF_REPLICANT	'prep'

#define PREF_AUTO_WORD	'pawd'
#define PREF_AUTO_LANG	'palg'
#define PREF_AUTO_SRCH	'pasr'
#define PREF_AUTO_TRNS	'patn'

class PrefWindow : public BWindow {
	public:
		PrefWindow(BRect frame, Preferences *preferences);
		virtual void MessageReceived(BMessage *message);
		
	private:
		BView *Interface;
		BView *Results;
		BTabView *tabView;
		BTab *interfaceTab;
		BTab *resultsTab;
	
		BString dictLanguage;
		BCheckBox *SaveWordOnQuit;
		BPopUpMenu *MaxWords;
		BMenuField *MaxField;
		BCheckBox *EnableReplicant;

		BCheckBox *AutoLookWord;
		BCheckBox *AutoLookLang;
		BCheckBox *AutoLookSearch;
		BCheckBox *AutoLookTrans;
		
		BCheckBox *MakeForeignBold;
		BCheckBox *MakeForeignColored;
		BCheckBox *FixFormatCheck;
		BCheckBox *IsNumberingCheck;
		BPopUpMenu *ColorMenu;
		BMenuField *ColorField;
		BPopUpMenu *FontSize;
		BMenuField *FontSizeField;
		BRadioButton *EngToForeign;
		BRadioButton *ForeignToEng;
		BButton *Cancel;
		BButton *Okay;

		Preferences *prefs;
		
		BString foreignColor;
		int32 maxWords;
		int32 fontSize;
		bool replicantEnabled;
		bool saveWordOnQuit;
		bool autoLookWord;
		bool autoLookLang;
		bool autoLookSearch;
		bool autoLookTrans;
		bool makeForeignBold;
		bool makeForeignColored;
		bool formatEngToForeign;
		bool fixedFormat;
		bool isNumbering;
};

#endif

