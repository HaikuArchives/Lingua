#ifndef UTILITIES_H
#define UTILITIES_H

#define URL_TYPE		"application/x-vnd.Be.URL.http"
#define EMAIL_TYPE		"text/x-email"
#define REPLICANT_NAME	"LinguaDeskbar"

bool yoinkline(BFile *file, BString &string);
bool contains(BString string, BString sub);
void AddDeskbar(bool add);
BString GetFileSize(BString size);

#endif

