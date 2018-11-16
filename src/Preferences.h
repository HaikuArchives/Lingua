#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <Message.h>
#include <Path.h>
#include <File.h>

#define PREF_FILE	"lingua_settings"

class Preferences : public BMessage {
	public:
		Preferences();
		virtual void Load();
		virtual void Save();
		
		status_t SetRect(const char *name, BRect rect);
		status_t SetString(const char *name, const char *word);
		status_t SetBool(const char *name, bool &value);
		status_t SetInt32(const char *name, int32 number);
		
	private:
		BPath settingsPath;
		BFile settingsFile;
};

#endif