#include "precomp.h"
#include "../../bfc/std.h"
#include "TextLog.H"


TextLog::TextLog() {
	lockcallback = FALSE;
	showurl = TRUE;
	showtimestamp = TRUE;
}


TextLog::~TextLog() {
}


BOOL TextLog::isTextUrl(const char *text) {
	for (int i=0; i<8; i++) { if (text[i] == NULL) return FALSE; }

	static char www[] = "www.";
	static char http[] = "http:";
	static char ftp[] = "ftp:";
	static char mailto[] = "mailto:";
	static char str[8];

	memcpy(str, text, 7);
	str[8] = NULL;
	if ( (str[5] == ':') | (str[5] == '.') ) str[6] = NULL;
	if ( (str[4] == ':') | (str[4] == '.') ) str[5] = NULL;

	if (strcmpi(www, str) == 0) return TRUE;
	if (strcmpi(http, str) == 0) return TRUE;
	if (strcmpi(ftp, str) == 0) return TRUE;
	if (strcmpi(mailto, str) == 0) return TRUE;
	return FALSE;
}


void TextLog::addText(const char *text, BOOL checkForUrl) {
	if (text == NULL) return;
	if (*text == NULL) return;

	char buffer[1024];

	const char *wordBegin = text;
	while (1) {
		if ( (*text == ' ') | (*text == NULL) ) {
			int len = text-wordBegin;
			memcpy(buffer, wordBegin, len);
			buffer[len] = NULL;
			if (checkForUrl) {
				if (isTextUrl(buffer)) addUrl(buffer);
				else addWord(buffer, len+1);
			}
			else addWord(buffer, len+1);
			wordBegin = text;

			while (*text == ' ') text++;
			len = text-wordBegin;
			addSpace(len);
			wordBegin = text;
		}

		if (*text == NULL) break;
		text++;
	}

	TextLogDependant *d = dependants.getFirst();
	while (d) {
		d->onTextChanged();
		d = dependants.getNext(d);
	}
}


void TextLog::addUrl(const char *url, const char *text) {
	if (text == NULL) addWord(url, -1, URL);
	else {
		addDataText(url, STARTURL);
		addText(text);
		addData(0, ENDURL);
	}
}


void TextLog::addWord(const char *text, int len, WordType type) {
	if (text == NULL) return;
	if (*text == NULL) return;
	if (len == -1) len = strlen(text);
	if (len > 0xFFFE) return;
	LogWord *word = (LPLogWord) malloc(sizeof(LogWord));
	word->data.text = (const char*) malloc(len+1);
	memcpy((char*)word->data.text, text, len+1);
	word->len = len;
	word->type = type;
	word->width = UNKNOWN_SIZE;
	word->height = UNKNOWN_SIZE;
	addToEnd(word);
}


void TextLog::addData(UINT data, WordType type, USHORT len, USHORT width, USHORT height) {
	LogWord *word = (LPLogWord) malloc(sizeof(LogWord));
	word->data.data = data;
	word->type = type;
	word->len = len;
	word->width = width;
	word->height = height;
	addToEnd(word);
}


void TextLog::addDataText(const char *data, WordType type, USHORT len, USHORT width, USHORT height) {
	if (len == UNKNOWN_SIZE) len = strlen(data);
	LogWord *word = (LPLogWord) malloc(sizeof(LogWord));
	word->data.text = (const char*) malloc(len+1);
	memcpy((char*)word->data.text, data, len+1);
	word->len = len;
	word->type = type;
	word->width = UNKNOWN_SIZE;
	word->height = UNKNOWN_SIZE;
	addToEnd(word);
}


void TextLog::setShowUrl(BOOL u) {
	showurl = u;

	TextLogDependant *d = dependants.getFirst();
	while (d) {
		d->onTextChanged();
		d = dependants.getNext(d);
	}
}


void TextLog::setShowTimestamp(BOOL s) {
	showtimestamp = s;

	TextLogDependant *d = dependants.getFirst();
	while (d) {
		d->onTextChanged();
		d = dependants.getNext(d);
	}
}



int TextLog::getWordTypeCount(WordType type) {
	int count = 0;
	LPLogWord theword = getFirst();
	while (theword) {
		if (theword->type == type) count++;
		theword = getNext(theword);
	}
	return count;
}


void TextLog::onItemMoved(LPLogWord data) {
	TextLogDependant *dep = dependants.getFirst();
	while (dep) {
		dep->onTextChanged();
		dep = dependants.getNext(dep);
	}
}


TextLogDependant::TextLogDependant(TextLog *viewer) {
	textlog = NULL;
	assignTextLog(viewer);
}


TextLogDependant::~TextLogDependant() {
	if (textlog) textlog->deregDependant(this);
}


void TextLogDependant::assignTextLog(TextLog *viewer) {
	if (textlog) textlog->deregDependant(this);
	textlog = viewer;
	if (textlog) textlog->regDependant(this);
	onAssignTextLog();
}


void TextLogDependant::setTextLog(TextLog *viewer) {
	textlog = viewer;
	if (textlog) textlog->regDependant(this);
	onAssignTextLog(TRUE);
}
