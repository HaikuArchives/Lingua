# Lingua Readme

![screenshot](lingua.png)

## 1. About

Lingua is a multiple language translation utility. It translates words to and from English into several languages, including Latin, German, Dutch, Italian, French, Spanish, Indonesian, and Portuguese. It can only do simple word and phrase translations, not entire sentences or paragraphs, but is still very useful for those times when you forget how to say "love" in Latin (it's "amare").

Lingua originally used the dictionary text files supplied by GPLTrans. I've taken it upon myself to clean up the dictionary files and have plans to expand them (see 3. How To Help below). These dictionary files are part of an open project, and thus, are missing many words, both esoteric and mundane, e.g. there is currently no definition for "troutslap" in Portuguese. 

Questions, comments, and bug reports can be submitted to dream@aevum.net with some mention of Lingua in the subject line. Any comments or complaints about the dictionary files themselves (e.g. "Why can't I find the definition for foobar in Spanish?") should not be submitted, but will be handled separately.

## 2. Installation and Usage

If you're reading this, you've already unzipped the Lingua zipfile. You can put the Lingua folder anywhere you want. The only installation requirement is that the "dicts" folder, containing all the dictionary textfiles, must be in the same directory as the Lingua binary. The "docs" folder should also be in the same directory. 

Using Lingua should be very straightforward. Once the application is opened, you simply enter a word into the text box, pick which language you want to translate to or from, and hit the "Translate" button. Results will appear in the large white text area beneath the controls. The title tab reports the current progress or number of results. 

See the documentation for more specific information. 

## 3. How To Help

There are two aspects to Lingua: the program, and the dictionary files. The program is my responsibility, but keeping the dictionary files accurate and up to date is more work than I alone can handle. 

If you're interested in donating some time and effort towards cleaning up and expanding the dictionary files, contact me at dream@aevum.net with your name and what languages you'd like to help with. Also, visit www.aevum.net/lingua/project.html for more details on the project and its current progress.

Because Lingua isn't my entire life, I won't expect your contributions to be your entire life either. All I'm asking for is several hours of time to make this program more useful to you, as well as everyone else. I'll do what I can myself, but the more help I get, the better.

## 4. Thanks To

Lingua owes its existence to many people and groups, including (in no recognizable order):

- Zippy, for giving Lingua it's sexy new name
- Chris, Daniel Berlin, jonas@kirilla, Linn, and jason@flipside for excellent code help
- Ove Peter for the replicant suggestions and beta testing
- The BeShare crowd for bugs and suggestions: kramerj, Technix, stippi, doc, obsidian, Alan, Zathros, Agni, Pete, legodude (and his gatofisch), and then some
- Sylvie De Roy, Carlos Hasan, and the rest of the Lingua Project contributors
- The Napster, OpenTracker, BeShare, and Scooby source code
- BeCodeTalk, for reminding me how little I know about C++
- GPLTrans, for providing the original dictionary files
- Eddie, for being such a damn good code editor
- Girhardelli hot cocoa, for keeping me alert (mmmmm...)

## 5. Change Log

Version 2.0
Added:
- Deskbar replicant with various new features
- Much more useful and dynamic Update Dictionaries window
- Preferences panel, with multiple tabs
- Help menu in top menu bar, to reduce clutter on the File menu
- Option to automatically translate on certain control changes
- Clickable links in the "About" window
- Option to number results
- Window tab changes to reflect the current query and number of results

Changed:
- "Language" menu label adjusts when files are moved in and out of the dicts subdirectory
- If the last used dictionary file is no longer in the dicts subdirectory when you run Lingua again, the "Language" menu shifts to the alphabetically topmost file
- If there aren't any dictionary files in the dicts subdirectory, all controls are disabled
- Definition text is still selectable, but no longer editable or removable
- Text in the Word box is automatically re-selected after queries

Fixed:
- Crashing bug if you quit during a query
- Ability to open multiple Preferences or Update Dictionaries windows
- Results appearing after where you clicked in results box during a query

Version 1.5.1
- Fixed bug where the max words in "Word:" menu weren't been kept
- Fixed bug where queries with extended characters weren't being done accurately
- Adjusted interface to be more font-sensitive

Version 1.5
- Queries now run on a separate threads, so no more window lockups. Yay for pervasive multithreading!
- "Translate" button becomes a "Stop" button when a query is in progress
- Added printing capabilities
- Added automated Update Dictionaries window
- Clicking the Zoom button resizes vertically only, instead of filling up the screen
- Added 3 new colors for the output: tan, sky, and aqua
- Changed "Definitions" menu to "Translations"
- Fixed some memory leaks. Thanks jason!

Version 1.1
- First update including new dictionary files
- Added "Dictionary Updates" option to File menu
- Fixed crashing bug in dict entries with no tab character
- Implimented drop down "Word" menu with the most recent queries
- Added multiple color options for Foreign results (not just blue)
- Added a node monitor to the dicts folder, so the Language menu will be updated in real time when dictionary files are added or removed

Version 1.0
 - Initial release. Everything looks cool. 

                                                                            - [ EOF ] -                                                                          
