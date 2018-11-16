#include <Application.h>
#include <FindDirectory.h>

#include "Preferences.h"

Preferences::Preferences()
	: BMessage('pref')
{
	Load();
}

/* ======= Two primary functions ====== */

void Preferences::Load()
{
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath) == B_OK) {
		settingsPath.Append(PREF_FILE);
		settingsFile.SetTo(settingsPath.Path(), B_READ_ONLY);
		if (settingsFile.InitCheck() == B_OK)
			Unflatten(&settingsFile);
	}
}

void Preferences::Save()
{
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath) == B_OK) {
		settingsPath.Append(PREF_FILE);
		settingsFile.SetTo(settingsPath.Path(), B_WRITE_ONLY | B_CREATE_FILE);
		if (settingsFile.InitCheck() == B_OK)
			Flatten(&settingsFile);
	}
}

/* === Little functions for changing individual settings === */

status_t Preferences::SetRect(const char *name, BRect rect)
{
	if (ReplaceRect(name, rect) == B_NAME_NOT_FOUND) 
		return AddRect(name, rect);
	else
		return ReplaceRect(name, rect);
}

status_t Preferences::SetString(const char *name, const char *word)
{
	if (ReplaceString(name, word) == B_NAME_NOT_FOUND)
		return AddString(name, word);
	else
		return ReplaceString(name, word);
}

status_t Preferences::SetBool(const char *name, bool &value)
{
	if (ReplaceBool(name, value) == B_NAME_NOT_FOUND)
		return AddBool(name, value);
	else
		return ReplaceBool(name, value);	
}
	
status_t Preferences::SetInt32(const char *name, int32 number)
{
	if (ReplaceInt32(name, number) == B_NAME_NOT_FOUND)
		return AddInt32(name, number);
	else
		return ReplaceInt32(name, number);	
}
