#ifndef UPDATEWINDOW_H
#define UPDATEWINDOW_H

#include <Window.h>

#define UPDATE_WIDTH	250
#define UPDATE_HEIGHT	320

#define DOWNLOAD	'dnld'
#define DONE_DL		'dndl'
#define DONE_UPDATE	'dnup'
#define SELECTED	'selc'
#define UPDATE		'updt'

class UpdateWindow : public BWindow {
	public:
		UpdateWindow(BRect frame);
		virtual void MessageReceived(BMessage *message);
		virtual bool QuitRequested();
		virtual void FrameResized(float width, float height);
		
		static int32 _Update(void *data);
		static int32 _Download(void *data);
		
		virtual void Update();
		virtual void Download();
		virtual void UnzipFiles();
		status_t GetFile(const char *name, const char *path, float filesize);
		status_t GetInfo(BString &results);		
		
	private:
		BButton *DownloadBtn, *UpdateBtn;
		BListView *Options;
		BScrollView *Scroller;
		BStatusBar *Status;
		
		thread_id update_thread, download_thread;
		
		bool isDownloading, isUpdating;
};

#endif
