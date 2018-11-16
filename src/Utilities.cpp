#include <Alert.h>
#include <Deskbar.h>
#include <Entry.h>
#include <File.h>
#include <Roster.h>
#include <String.h>

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstdio>

#include "DictApplication.h"
#include "Utilities.h"

bool yoinkline(BFile *file, BString &string)
{
	// These are all static so we don't lose 'em between function calls
	static char buffer[4096];
	static off_t positionInBuffer;
	static ssize_t amt_read;
	
	string = ""; 	// Clear out old string

	// Fill up the buffer with the first chunk of code
	if (positionInBuffer == 0)
		amt_read = file->Read(&buffer, sizeof(buffer));

	while (amt_read > 0)
	{
		while (positionInBuffer < amt_read) 
		{
			// Return true if we hit a newline or the end of the file
			if (buffer[positionInBuffer] == '\n') {
				positionInBuffer++;
				return true;
			}
			string += buffer[positionInBuffer]; 
			positionInBuffer++;
		} 
		// Once the buffer runs out, grab some more and start again
		amt_read = file->Read(&buffer, sizeof(buffer)); 
		positionInBuffer = 0;
	}	
	return false;
}

bool contains(BString string, BString sub)
{
	const char *thestring = string.String();
	const char *thesub = sub.String();
	
	int32 stringLength = strlen(thestring);
	int32 subLength = strlen(thesub);
	
	if ((stringLength == 0) || (subLength == 0))
		return false;
	
	bool result = false;
		
	for (int32 i=0; i<=stringLength; i++) 
		if (tolower(thesub[0]) == tolower(thestring[i])) 
			if (strncasecmp(thesub, thestring + i, subLength) == 0) {
				result = true;
				break;
			}

	return result;
}

void AddDeskbar(bool add)
{
	BDeskbar *deskbar = new BDeskbar();

	if (add && !deskbar->HasItem(REPLICANT_NAME)) {
		entry_ref ref;
		be_roster->FindApp(APP_SIG, &ref);
		status_t err = deskbar->AddItem(&ref);
		
		if (err != B_OK) {
			char temp[255];
			sprintf(temp, "Error: %s", strerror(err));
			BAlert *alert = new BAlert("Info", temp, "d'oh!");
			alert->Go();
		}
	} else if (!add) {
		deskbar->RemoveItem(REPLICANT_NAME);
	}
	
	delete deskbar;
}

BString GetFileSize(BString size)
{
	BString result;
	float file_size = atol(size.String());
	
	if (file_size >= 1073741824) {
		file_size = ceil(file_size / 1073741824 * 100) / 100; 
		result << file_size << " GB";
	} else if (file_size >= 1048576) {
		file_size = ceil(file_size / 1048576 * 100) / 100;
		result << file_size << " MB";
	} else if (file_size >= 1024) {
		file_size = ceil(file_size / 1024 * 100) / 100;
		result << file_size << " KB";
	} else {
		result << file_size << " bytes";
	}
	
	return result;
}
