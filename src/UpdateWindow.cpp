#include <Application.h>
#include <Button.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <List.h>
#include <ListView.h>
#include <NetworkKit.h>
#include <Path.h>
#include <Roster.h>
#include <ScrollView.h>
#include <StatusBar.h>
#include <String.h>
#include <StringView.h>
#include <TextView.h>

#include <cstdlib>
#include <cstdio>

#include "DictApplication.h"
#include "DownloadItem.h"
#include "Utilities.h"
#include "UpdateWindow.h"

UpdateWindow::UpdateWindow(BRect frame)
	: BWindow(frame, "Update Dictionaries", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	// This becomes true when they download, and is checked to see if the button
	// should start a new download or kill the current one
	isDownloading = false;
	isUpdating = false;

	float winWidth = Bounds().right;
	float winHeight = Bounds().bottom;
	SetSizeLimits(winWidth, winWidth, winHeight-100, winHeight+350);

	// Background view
	BView *background = new BView(Bounds(), NULL, B_FOLLOW_ALL, B_WILL_DRAW);
	rgb_color grey = ui_color(B_PANEL_BACKGROUND_COLOR);
	background->SetViewColor(grey);
	background->SetLowColor(grey);
	AddChild(background); 
	
	// Labels for file listings
	BStringView *LabelName = new BStringView(BRect(35, 0, 110, 18), NULL, "Language:", B_FOLLOW_NONE, B_WILL_DRAW);
	background->AddChild(LabelName);
	BStringView *LabelMod = new BStringView(BRect(110, 0, 170, 18), NULL, "Modified:", B_FOLLOW_NONE, B_WILL_DRAW);
	background->AddChild(LabelMod);
	BStringView *LabelSize = new BStringView(BRect(170, 0, winWidth, 18), NULL, "File Size:", B_FOLLOW_NONE, B_WILL_DRAW);
	background->AddChild(LabelSize);
	
	// Listing of files
	BRect outputRect(10, 20, winWidth-10, winHeight-90);
	outputRect.right -= B_V_SCROLL_BAR_WIDTH;
	Options = new BListView(outputRect, "Items", B_MULTIPLE_SELECTION_LIST, B_FOLLOW_ALL, B_WILL_DRAW | B_FRAME_EVENTS);
	Options->SetSelectionMessage(new BMessage(SELECTED));

	Scroller = new BScrollView("ListScroll", Options, B_FOLLOW_ALL, B_WILL_DRAW, false, true); 
	background->AddChild(Scroller);

	// Status Bar
	BRect statusRect(10, winHeight-80, winWidth-10, winHeight-70);
	Status = new BStatusBar(statusRect, NULL);
	Status->SetBarHeight(16);
	background->AddChild(Status);

	// Update Button
	BRect r1;
	r1.left = winWidth / 2 - 100;
	r1.right = r1.left + 90;
	r1.top = winHeight - 35;
	r1.bottom = r1.top + 23;
	UpdateBtn = new BButton(r1, "UpdtButton", "Update", new BMessage(UPDATE));
	background->AddChild(UpdateBtn);	

	// Download Button
	BRect r2;
	r2.left = winWidth / 2 + 10;
	r2.right = r2.left + 90;
	r2.top = winHeight - 35;
	r2.bottom = r2.top + 20;
	DownloadBtn = new BButton(r2, "DnldButton", "Download", new BMessage(DOWNLOAD));
	DownloadBtn->MakeDefault(true);
	DownloadBtn->SetEnabled(false);
	background->AddChild(DownloadBtn);
	
	Show();
}

bool UpdateWindow::QuitRequested()
{
	// Kill of any threads still running
	kill_thread(update_thread);
	kill_thread(download_thread);
	
	// Run the Unzip function again, just in case they canceled a download
	UnzipFiles();
	
	return true;
}

void UpdateWindow::MessageReceived(BMessage *message)
{
	switch (message->what)
	{
		case SELECTED:
		{
			bool isSelected = false;
			for (int32 i=0; i<Options->CountItems(); i++) {
				if (Options->IsItemSelected(i)) {
					isSelected = true;
					break;
				}
			}
			
			if (isSelected)
				DownloadBtn->SetEnabled(true);
			else
				DownloadBtn->SetEnabled(false);
		}
			break;					
		
		case UPDATE:
			if (isUpdating) {
				kill_thread(update_thread);
				PostMessage(DONE_UPDATE);
				break;
			}
			
			isUpdating = true;
			UpdateBtn->SetLabel("Stop Update");
			DownloadBtn->SetEnabled(false);
			update_thread = spawn_thread(_Update, "Update Thread", B_NORMAL_PRIORITY, (void *)this);
			resume_thread(update_thread);
			break;
		
		case DONE_UPDATE:
			isUpdating = false;
			UpdateBtn->SetLabel("Update");
			Lock();
			Status->Reset();
			Status->Update(0, "Update Finished", NULL);
			Unlock();
			break;
		
		case DOWNLOAD:
			if (isDownloading) {
				kill_thread(download_thread);
				PostMessage(DONE_DL);
				break;
			}
			
			isDownloading = true;
			DownloadBtn->SetLabel("Stop Downloads");
			UpdateBtn->SetEnabled(false);
			download_thread = spawn_thread(_Download, "Download Thread", B_NORMAL_PRIORITY, (void *)this);
			resume_thread(download_thread);			
			break;

		case DONE_DL:
			isDownloading = false;
			DownloadBtn->SetLabel("Download");
			UpdateBtn->SetEnabled(true);
			Lock();
			Status->Reset();
			Status->Update(0, "Download Finished", NULL);
			Unlock();
			break;
			
		default:
			BWindow::MessageReceived(message);
	}
}

void UpdateWindow::FrameResized(float width, float height)
{
	UpdateBtn->MoveTo(width / 2 - 100, height - 35);
	DownloadBtn->MoveTo(width / 2 + 7, height - 38);
	Status->MoveTo(10, height - 80);
	
	// Refresh the BScrollBar
	Lock();
	Scroller->Draw(Scroller->Bounds());
	Unlock();
	
	BWindow::FrameResized(width, height);
}

int32 UpdateWindow::_Download(void *data)
{
	UpdateWindow *itself = (UpdateWindow *)data;
	itself->Download();
	return 0;
}

void UpdateWindow::Download()
{
	int32 i;
	float filesize;
	BString path, filename, thefirst, thelast;

	// Calculate how many files to acquire
	int32 totalStuff = 0;
	for (i=0; i<Options->CountItems(); i++)
		if (Options->ItemAt(i)->IsSelected())
			totalStuff++;
			
	// Find where the application is
	app_info appInfo;
	be_roster->GetAppInfo(APP_SIG, &appInfo);
	BEntry appEntry(&appInfo.ref);
	appEntry.GetParent(&appEntry);
	BPath dictsPath(&appEntry);

	// Move into the dicts directory, one level above the app
	path << dictsPath.Path() << "/dicts/";

	// Download one file at a time
	int32 current = 0;
	for (i=0; i<Options->CountItems(); i++) {
		if (Options->ItemAt(i)->IsSelected()) {
			filesize = ((DownloadItem *)Options->ItemAt(i))->GetOrigSize();
			filename << ((DownloadItem *)Options->ItemAt(i))->GetName() << ".zip";
			thefirst << "Downloading " << filename << "...";
			thelast << "(" << ++current << "/" << totalStuff << ")";
			
			Lock();
			Status->Reset(thefirst.String(), thelast.String());
			Unlock();
			
			GetFile(filename.String(), path.String(), filesize);
			
			// Clear out the strings, else they build up
			thefirst = thelast = filename = "";
		}
	}

	Lock();
	Status->Reset("Unzipping Files...");
	Unlock();

	UnzipFiles();
	
	PostMessage(DONE_DL); 
}

int32 UpdateWindow::_Update(void *data)
{
	UpdateWindow *itself = (UpdateWindow *)data;
	itself->Update();
	return 0;
}

void UpdateWindow::Update()
{
	BString file_name, mod_date, temp_size, file_size;
	BString line;

	// Clean out the old info
	Lock();
	Options->RemoveItems(0, Options->CountItems());
	Unlock();
	
	// Get the latest info
	BString info;
	status_t err = GetInfo(info);
	if (err != B_OK) {
		isUpdating = false;

		Lock();
		Status->Reset();
		Status->Update(0, "Error: Couldn't Connect!", NULL);
		UpdateBtn->SetLabel("Update");
		Unlock();
		
		return;		
	}
	
	// Break into parts
	int32 len = info.Length() + 1;
	char *data = info.LockBuffer(len);
	
	line = strtok(data, "\n");
	do {	
		int32 bar1 = line.FindFirst("|");
		int32 bar2 = line.FindFirst("|", bar1+1);
		
		line.CopyInto(file_name, 0, bar1-4);
		line.CopyInto(mod_date, bar1+1, bar2-bar1-1);
		line.CopyInto(temp_size, bar2+1, line.Length()-bar2);
		file_size = GetFileSize(temp_size);
		
		// Make new list items for each file
		DownloadItem *Item = new DownloadItem(file_name, mod_date, file_size);
		Item->SetOrigSize(atol(temp_size.String()));

		Lock();
		Options->AddItem(Item);
		Unlock();
	} while ((line = strtok(NULL, "\n")) != NULL);
	
	info.UnlockBuffer(len);
		
	PostMessage(DONE_UPDATE);
}	

void UpdateWindow::UnzipFiles()
{
	BEntry entry;
	BString name, path, tounzip, todelete;
	char temp[B_FILE_NAME_LENGTH];
	
	app_info appInfo;
	be_roster->GetAppInfo(APP_SIG, &appInfo);
	BEntry appEntry(&appInfo.ref);
	appEntry.GetParent(&appEntry);
	BPath dictsPath(&appEntry);

	path << dictsPath.Path() << "/dicts/";
	BDirectory dictsDir(path.String());
	
	// Check that there are some files ready
	if (dictsDir.InitCheck() != B_OK || dictsDir.CountEntries() <= 0)
		return;
	
	while (dictsDir.GetNextEntry(&entry) != B_ENTRY_NOT_FOUND) {
		entry.GetName(temp);
		name = temp;
		if (name.IFindLast(".zip") != B_ERROR) {
			tounzip << "unzip -u " << path.String() << name << " -d " << path.String();
			todelete << "rm -f " << path.String() << name;
			system(tounzip.String());
			system(todelete.String());
		}
		tounzip = todelete = "";
	}
}

status_t UpdateWindow::GetFile(const char *name, const char *path, float filesize)
{
	BNetEndpoint socket;
	BNetBuffer buffer(4096);
	BFile file;
	int32 got = 0;

	float delta = 1024 / filesize * 100;

	BString command;
	command << "GET /lingua/dicts/" << name << " HTTP/1.1\r\n";
	command << "Host: www.aevum.net\r\n";
	command << "\r\n";
	
	BString filename;
	filename << path << name;

	if (socket.Connect("www.aevum.net", 80) != B_OK)
		return B_ERROR;

	socket.Send(command.String(), command.Length());

	while (got <= filesize) {
		got += socket.Receive(buffer, 1024);
		Lock();
		Status->Update(delta);
		Unlock();
	}
	socket.Close();

	file.SetTo(filename.String(), B_CREATE_FILE | B_WRITE_ONLY);
	file.Write(buffer.Data(), buffer.Size());
	
	return B_OK;
}

status_t UpdateWindow::GetInfo(BString &results)
{
	BNetEndpoint socket;
	BNetBuffer buffer(4096);
	BFile file;

	BString command;
	command << "GET /lingua/dicts/info.txt HTTP/1.1\r\n";
	command << "Host: www.aevum.net\r\n";
	command << "\r\n";

	Lock();
	Status->Update(33, "Connecting...");
	Unlock();
	status_t err = socket.Connect("www.aevum.net", 80);
	if (err != B_OK)
		return B_ERROR;

	socket.Send(command.String(), command.Length());
	
	Lock();
	Status->Update(33, "Receiving Info...");
	Unlock();

	// Let's just assume the info.txt file doesn't go over 4 KB
	socket.Receive(buffer, 4096);
	socket.Close();
	
	// Strip off the top part, just leave the pure, sweet data
	char tmp[4096];
	sprintf(tmp, "%s", buffer.Data());
	results = tmp;
	
	int32 spaces = results.FindFirst("\r\n\r\n");
	results.Remove(0, spaces+4);

	Lock();
	Status->Reset();
	Status->Update(0, "Updated Info Received", NULL);
	Unlock();
	
	return B_OK;
}
