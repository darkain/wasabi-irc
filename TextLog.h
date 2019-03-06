#ifndef __TEXTLOG_H_
#define __TEXTLOG_H_

#ifdef _USE_VBASE_
#include "../../../vBase/List.H"
#else
#include "List.H"
#endif


class TextLogDependant;


typedef enum {
	TEXT,					//basic text
	STEXT1,				//begin sepcial text section 1
	ENDSTEXT1,		//end special text section 1
	STEXT2,				//begin sepcial text section 2
	ENDSTEXT2,		//end special text section 2
	STEXT3,				//begin sepcial text section 3
	ENDSTEXT3,		//end special text section 3
	STEXT4,				//begin sepcial text section 4
	ENDSTEXT4,		//end special text section 4
	TIMESTAMP,		//inserts a timestamp.  data.time should contain the time to display
	SPACE,				//inserts a space inbetween words.  this is where wrapping occurs too
	CRLF,					//inserts a line break.  (cariage return, line feed)
	TAB,					//inserts a TAB, tabs are alligned
	HR,						//inserts a horizontal ruler
	SETCOLOR,			//set the text color.  data.color contains new color
	PUSHCOLOR,		//push a new color into the color stack.
	POPCOLOR,			//pop color from color stack
	DEFAULTCOLOR,	//forces text back to default color
	SETBGCOLOR,		//set the background color for the text.
	PUSHBGCOLOR,	//push a new background color into the bg color stack
	POPBGCOLOR,		//pop bg color from bg color stack
	DEFAULTBGCOLOR,	//forces text back to the default color
	SETBOLD,			//set text bold.  set data.boolean to true/false
	PUSHBOLD,			//push value into text bold stack
	POPBOLD,			//pop value from text bold stack
	DEFAULTBOLD,	//forces bold to be default value
	SETITALIC,		//set text italic.  set data.boolean for true/false
	PUSHITALIC,		//push value into text italic stack
	POPITALIC,		//pop value from text italic stack
	DEFAULTITALIC,	//forces italic to be default value
	SETUNDERLINE,		//set text underline.  set data.boolean to true/false
	PUSHUNDERLINE,	//push value into text underline stack
	POPUNDERLINE,		//pop value from text underline stack
	DEFAULTUNDERLINE,	//force italic to be default value
	SETSTRIKETHROUGH,	//not working yet
	PUSHSTRIKETHROUGH,
	POPSTRIKETHROUGH,
	DEFAULTSTRIKETHROUGH,
	SETFONTFACE,	//not working
	PUSHFONTFACE,
	POPFONTFACE,
	DEFAULTFONTFACE,
	SETFONTPOINT,	//not working
	PUSHFONTPOINT,
	POPFONTPOINT,
	DEFAULTFONTPOINT,
	IMAGE,			//not working
	URL,				//this will use the data.text as both the URL and text display
	STARTURL,		//this will use the data.text as the URL, and expects there to be text before ENDURL 
	ENDURL,			//end of a URL block.
//	SCRIPT,			//not working
	ENDSCRIPT,
} WordType;


typedef struct LogWord {
	union {
		const char *text;
		void *pointer;
		COLORREF colour;
		BOOL boolean;
		UINT data;
		time_t time;
	} data;
	WordType type;
	USHORT len;
	USHORT width;
	USHORT height;
} LogWord, *LPLogWord;


#define UNKNOWN_SIZE 0xFFFF


class TextLog : public vList<LPLogWord> {
	public:
		TextLog();
		virtual ~TextLog();

		//not used... maybe needsta be removed ??
		void lockCallbacks() { lockcallback = TRUE; }
		void unlockCallbacks();

		//returns the number of occurances of a specific word type
		//within the list.  such as..
		//getWordTypeCount(CRLF);
		//will return the number of CRLFs, so you know how many forced
		//line breaks there are
		int getWordTypeCount(WordType type);

		//add text to the log, and parse it for spaces
		void addText(const char *text, BOOL checkForUrl=FALSE);

		//added a "word" to the log, same as addText, but doesnt parse for
		//spaces, therefor, it wont wrap at all
		void addWord(const char *text, int len=-1, WordType type=TEXT);

		//if text == NULL, itll use the *url as the text display,
		//and only alloc ram once
		void addUrl(const char *url, const char *text=NULL);

		//checks to see if text is a valid URL.  can be overiden to check
		//for more possible URL types
		virtual BOOL isTextUrl(const char *text);

		//add a forced line break, a CRLF for those that know what one is
		void addLineBreak() { addData(0, CRLF); }

		//add a horizontal ruler.  a line seperator
		void addHorizontalRuler() { addData(0, HR); }

		//add a space, or multiple spaces.  text is parsed and wrapped
		//based on these space tags, not spaces themselves within the text
		void addSpace(int len=1) { addData(0, SPACE, len); }

		//try to avoid calling these directly.  use things like addText() or
		//addSpace(), stuff like that instead.  this way there isnt any confusion
		//with allocating and freeing memory
		void addData(UINT data, WordType type, USHORT len=UNKNOWN_SIZE, USHORT width=UNKNOWN_SIZE, USHORT height=UNKNOWN_SIZE);
		void addDataText(const char *data, WordType type, USHORT len=UNKNOWN_SIZE, USHORT width=UNKNOWN_SIZE, USHORT height=UNKNOWN_SIZE);

		//stuff for dependants
		void regDependant(TextLogDependant *depend) { dependants.addToEnd(depend); }
		void deregDependant(TextLogDependant *depend) { dependants.delItem(depend); }

		//stuff from vList<>
		virtual void onItemMoved(LPLogWord data);

		BOOL getShowUrl() { return showurl; }
		void setShowUrl(BOOL u);

		BOOL getShowTimestamp() { return showtimestamp; }
		void setShowTimestamp(BOOL s);

		virtual void popMenu() {}

	private:
		vList<TextLogDependant*> dependants;
		BOOL lockcallback;
		BOOL showurl;
		BOOL showtimestamp;
};


//inherit from this class to get callbacks from specified text log
class TextLogDependant {
	protected:
		TextLogDependant(TextLog *viewer=NULL);
		~TextLogDependant();

	public:
		//use this one
		void assignTextLog(TextLog *viewer);
		
		//do NOT use this one, use assignTextLog() instead
		void setTextLog(TextLog *viewer);

		//returns current text log we are assigned to
		TextLog *getAssignedTextLog() { return textlog; }

		//overide this to know when textlog assignment has changed
		virtual void onAssignTextLog(BOOL forced=FALSE) {}

		//overide this to know when there has been some change in text,
		//this way we can invalidate() the window when things are updated.  :)
		//later, i will make it a bit more advanced, so itll only invalidate()
		//if the text that is being displayed is changed.
		virtual void onTextChanged() {}

		/*
		//these will come in due time
		virtual void onTextAdded();
		virtual void onTextDeleted();
		*/

		void popMenu() { if (textlog) textlog->popMenu(); }

	protected:
		TextLog *textlog;
};


struct WordCoord {
	WordCoord() { ZeroMemory(this, sizeof(WordCoord)); }
	LPLogWord word;
	int x;
	int y;
	int line;
};


class WordCoordList : public vList<WordCoord*> {
	public:
		WordCoordList(bool autoDel=false) : vList<WordCoord*>(autoDel) {}
	/*
		virtual void onDelete(WordCoord *data) {
			if (autodel)
				delete data;
		}

		void setAutoDel(BOOL del) { autodel = del; }
		BOOL getAutoDel() { return autodel; }
	*/
		void addWord(LPLogWord word, int x, int y, int loc=0) {
			WordCoord *wrdcrd = (WordCoord*) malloc(sizeof(WordCoord)); //new WordCoord();
			wrdcrd->word = word;
			wrdcrd->word = word;
			wrdcrd->word = word;
			if (loc == 0) addToEnd(wrdcrd);
			else addToBegining(wrdcrd);
		}
	/*
	private:
		BOOL autodel;*/
};


#endif//__TEXTLOG_H_