//
// Created by pawel on 14.05.2025.
//

#include "pcl.h"
#include <stdio.h>




// Queue
// TODO make separate library for this
struct Node {
	char value;
	struct Node* previous;
	struct Node* next;
};

struct Queue* init() {
	struct Queue *queue = malloc(sizeof(struct Queue));
	if (queue == NULL) {
		// TODO error
		return NULL;
	}
	queue->head = NULL;
	queue->tail = NULL;
	return queue;
}

int isEmpty(struct Queue *queue) {
	if (queue == NULL) {
		return -1;
	}
	if (queue->head == NULL) {
		return TRUE;
	}
	return FALSE;
}

void enqueue(struct Queue *queue, char input) {
	if (queue == NULL) {
		// TODO error
		return;
	}

	struct Node* newNode = malloc(sizeof(struct Node));
	if (newNode == NULL) {
		// TODO error
		return;
	}

	newNode->value = input;
	newNode->next = NULL;

	if (queue->head == NULL) {
		newNode->previous = NULL;
		queue->head = newNode;
		queue->tail = newNode;
		return;
	}

	newNode->previous = queue->tail;
	queue->tail->next = newNode;
	queue->tail = newNode;
}

char dequeue(struct Queue *queue) {
	if (isEmpty(queue)) {
		return -1;
	}
	struct Node* temp = queue->head;
	queue->head = queue->head->next;
	if (queue->head == NULL) {
		queue->tail = NULL;
	}
	char value = temp->value;
	free(temp);
	return value;
}

char peek(struct Queue *queue) {
	if (isEmpty(queue)) {
		return -1;
	}
	return queue->head->value;
}

DWORD WINAPI inputthread(LPVOID lpParam) {
	struct Console* console = (struct Console*)lpParam;

	while (1) {
		while (WaitForSingleObject(console->threadExitEvent, 1) == WAIT_TIMEOUT)
		{
			ExitThread(0);
		}
		DWORD read;
		INPUT_RECORD lpBuffer[1];
		PeekConsoleInput(console->inputHandle, lpBuffer, 1, &read);
		if (read == 0) {
			continue;
		}
		int result = ReadConsoleInput(console->inputHandle, lpBuffer, 1, &read);
		if (result == 0) {
			// error ReadConsoleInput failed
			ExitThread(-1);
		}

		switch (lpBuffer[0].EventType) {
			case FOCUS_EVENT: {
				if (console->FocusEvent != NULL) {
					console->FocusEvent(console, lpBuffer[0].Event.FocusEvent.bSetFocus);
				}
				break;
			}
			case KEY_EVENT: {
				if (console->KeyEvent != NULL) {
					console->KeyEvent(
						console,lpBuffer[0].Event.KeyEvent.uChar.AsciiChar, lpBuffer[0].Event.KeyEvent.bKeyDown);
				}
				if (lpBuffer[0].Event.KeyEvent.bKeyDown) {
					WaitForSingleObject(console->mutexHandle, INFINITE);
					enqueue(console->inputQueue, lpBuffer[0].Event.KeyEvent.uChar.AsciiChar);
					ReleaseMutex(console->mutexHandle);
				}
				break;
			}
			case MENU_EVENT: {
				// ignored
				break;
			}
			case MOUSE_EVENT: {
				if (console->MouseEvent != NULL) {

					console->MouseEvent(
							console,
							lpBuffer[0].Event.MouseEvent.dwMousePosition.Y,
							lpBuffer[0].Event.MouseEvent.dwMousePosition.X,
							(int)lpBuffer[0].Event.MouseEvent.dwButtonState,
							(int)lpBuffer[0].Event.MouseEvent.dwControlKeyState,
							(int)lpBuffer[0].Event.MouseEvent.dwEventFlags
						);
				}
				break;
			}
			case WINDOW_BUFFER_SIZE_EVENT: {
				if (console->ResizeEvent != NULL) {
					console->ResizeEvent(
							console,
							(int)lpBuffer[0].Event.WindowBufferSizeEvent.dwSize.Y,
							(int)lpBuffer[0].Event.WindowBufferSizeEvent.dwSize.X
						);
				}
				break;
			}
			default: break;
		}
	}
}

struct Console* start(void) {
	// TODO implement error handling
	// TODO implement saving console state to restore it back at end() function


	struct Console* console = malloc(sizeof(struct Console));
	console->inputHandle = GetStdHandle(STD_INPUT_HANDLE);

	DWORD fdwMode = ENABLE_ECHO_INPUT | ENABLE_INSERT_MODE | ENABLE_LINE_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE | ENABLE_WINDOW_INPUT | ENABLE_VIRTUAL_TERMINAL_INPUT;
	WINBOOL res2 = SetConsoleMode(console->inputHandle, fdwMode);

	console->mutexHandle = CreateMutex(NULL, FALSE, NULL);
	console->inputQueue = init();
	DWORD threadid;
	console->threadHandle = CreateThread(NULL, 0, inputthread, console, CREATE_SUSPENDED, &threadid);

	console->currentOutput = 1;

	console->outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleMode(console->outputHandle, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_LVB_GRID_WORLDWIDE);
	SetConsoleActiveScreenBuffer(console->outputHandle);

	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(console->outputHandle, &info);

	console->cursor = 0;
	console->height = info.dwSize.Y;
	console->width = info.dwSize.X;
	console->fr = 255;console->fg = 255;console->fb = 255;
	console->br = 0;console->bg= 0;console->bb = 0;

	console->buffer = malloc(sizeof(struct Cell) * console->width * console->height);
	for (int i = 0; i < console->height * console->width; ++i) {
		console->buffer[i].data = ' ';
		console->buffer[i].fr = 255;
		console->buffer[i].fg = 255;
		console->buffer[i].fb = 255;
		console->buffer[i].br = 0;
		console->buffer[i].bg = 0;
		console->buffer[i].bb = 0;
	}

	console->errorHandle = GetStdHandle(STD_ERROR_HANDLE);
	console->blockInput = TRUE;
	console->blockTimeout = 0;

	console->FocusEvent = NULL;
	console->KeyEvent = NULL;
	console->MouseEvent = NULL;
	console->ResizeEvent = NULL;

	ResumeThread(console->threadHandle);

	return console;
}

int end(struct Console* console) {
	// TODO implement error handling

	console->threadExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetEvent(console->threadExitEvent);
	WaitForSingleObject(console->threadHandle, INFINITE);
	CloseHandle(console->threadHandle);
	CloseHandle(console->threadExitEvent);
	CloseHandle(console->mutexHandle);

	CloseHandle(console->inputHandle);
	CloseHandle(console->outputHandle);
	CloseHandle(console->errorHandle);

	free(console);
}

int setforegroundcolor(struct Console *console, int r, int g, int b) {
	if (console == NULL) {
		return -1;
	}
	console->fr = r;
	console->fg = g;
	console->fb = b;
	return 0;
}

int setbackgroundcolor(struct Console *console, int r, int g, int b) {
	if (console == NULL) {
		return -1;
	}
	console->br = r;
	console->bg = g;
	console->bb = b;
	return 0;
}

int clearforegroundcolor(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	console->fr = 255;
	console->fg = 255;
	console->fb = 255;
	return 0;
}

int clearbackgroundcolor(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	console->br = 0;
	console->bg = 0;
	console->bb = 0;
	return 0;
	return 0;
}

int setinputblock(struct Console *console, BOOL blockinput) {
	if (console == NULL) {
		return -1;
	}
	console->blockInput = blockinput;
	return 0;
}

int getinputblock(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	return console->blockInput;
}

int setfocusevent(struct Console *console, void(*FocusEvent)(struct Console*, int)) {
	if (console == NULL) {
		return -1;
	}
	if (FocusEvent == NULL) {
		return -2;
	}
	console->FocusEvent = FocusEvent;
	return 0;
}

int unsetfocusevent(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	console->FocusEvent = NULL;
	return 0;
}

int setkeyevent(struct Console *console, void(*KeyEvent)(struct Console*, char, int)) {
	if (console == NULL) {
		return -1;
	}
	if (KeyEvent == NULL) {
		return -2;
	}
	console->KeyEvent = KeyEvent;
	return 0;
}

int unsetkeyevent(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	console->KeyEvent = NULL;
	return 0;
}

int setmouseevent(struct Console *console, void(*MouseEvent)(struct Console*, int, int, int, int, int)) {
	if (console == NULL) {
		return -1;
	}
	if (MouseEvent == NULL) {
		return -2;
	}
	console->MouseEvent = MouseEvent;
	return 0;
}

int unsetmouseevent(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	console->MouseEvent = NULL;
	return 0;
}

int setresizeevent(struct Console *console, void(*ResizeEvent)(struct Console*, int, int)) {
	if (console == NULL) {
		return -1;
	}
	if (ResizeEvent == NULL) {
		return -2;
	}
	console->ResizeEvent = ResizeEvent;
	return 0;
}

int unsetresizeevent(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	console->ResizeEvent = NULL;
	return 0;
}

int settimeout(struct Console *console, int timeout) {
	// TODO implement error handling in docs
	if (console == NULL) {
		return -1;
	}
	if (timeout < 0) {
		return -2;
	}
	console->blockTimeout = timeout;
	return 0;
}

int gettimeout(struct Console *console) {
	// TODO implement error handling
	return console->blockTimeout;
}

int gettitle(struct Console* console, char** title) {
	if (console == NULL) {
		return -1;
	}

	if (title == NULL) {
		return -2;
	}

	*title = malloc(MAX_PATH * sizeof(char));
	if (*title == NULL) {
		return -3;
	}

	GetConsoleTitleA(*title, MAX_PATH);
	return 0;
}

int settitle(struct Console* console, char* title) {
	if (console == NULL) {
		return -1;
	}

	if (title == NULL) {
		return -2;
	}

	if(!SetConsoleTitleA(title))
	{
		return -3;
	}
	return 1;
}

int getdimensions(struct Console* console, int* width, int* height) {
	if (width == NULL) {
		return -1;
	}
	if (height == NULL) {
		return -2;
	}
	if (console == NULL) {
		*width = 0;
		*height = 0;
		return -3;
	}

	*width = console->width;
	*height = console->height;
	return 1;
}

/**
 * pure ReadConsoleInput() wrapper without any non-blocking or timeout features
 * but checks console input buffer for other than keyboard events and fires them
 *
 * @param console pointer to struct Console
 * @return char from console input buffer
 */
char puregetchar(struct Console* console) {
	DWORD read;

	while (1) {
		INPUT_RECORD lpBuffer[1];
		int result = ReadConsoleInput(console->inputHandle, lpBuffer, 1, &read);
		if (result == 0 || read != 1) {
			// error ReadConsoleInput failed
			return -1;
		}

		switch (lpBuffer[0].EventType) {
			case FOCUS_EVENT: {
				if (console->FocusEvent != NULL) {
					console->FocusEvent(console, lpBuffer[0].Event.FocusEvent.bSetFocus);
				}
				break;
			}
			case KEY_EVENT: {
				if (console->KeyEvent != NULL) {
					console->KeyEvent(
						console,lpBuffer[0].Event.KeyEvent.uChar.AsciiChar, lpBuffer[0].Event.KeyEvent.bKeyDown);
				}
				if (lpBuffer[0].Event.KeyEvent.bKeyDown) {
					return lpBuffer[0].Event.KeyEvent.uChar.AsciiChar;
				}
			}
			case MENU_EVENT: {
				// ignored
				break;
			}
			case MOUSE_EVENT: {
				if (console->MouseEvent != NULL) {

					console->MouseEvent(
							console,
							lpBuffer[0].Event.MouseEvent.dwMousePosition.Y,
							lpBuffer[0].Event.MouseEvent.dwMousePosition.X,
							(int)lpBuffer[0].Event.MouseEvent.dwButtonState,
							(int)lpBuffer[0].Event.MouseEvent.dwControlKeyState,
							(int)lpBuffer[0].Event.MouseEvent.dwEventFlags
						);
				}
				break;
			}
			case WINDOW_BUFFER_SIZE_EVENT: {
				if (console->ResizeEvent != NULL) {
					console->ResizeEvent(
							console,
							(int)lpBuffer[0].Event.WindowBufferSizeEvent.dwSize.Y,
							(int)lpBuffer[0].Event.WindowBufferSizeEvent.dwSize.X
						);
				}
			}
			default: break;
		}
	}
}

char getchr(struct Console* console) {
	// TODO test all input
	// TODO implement error handling
	// TODO add virtual key codes for arrows, special keys etc.
	// TODO add unicode

	WaitForSingleObject(console->mutexHandle, INFINITE);
	if (!isEmpty(console->inputQueue)) {
		char c = dequeue(console->inputQueue);
		ReleaseMutex(console->mutexHandle);
		return c;
	}

	ReleaseMutex(console->mutexHandle);

	return -1;
/*
	if (!console->blockInput) { // non-blocking input behaviour
		INPUT_RECORD lpBuffer[1];
		DWORD read;
		BOOL b = PeekConsoleInput(console->inputHandle, lpBuffer, 1, &read);
		if (b == 0) {
			return -1;
		}
		if (read == 1) {
			// peeked something
			// lets kick it out of buffer and handle anything
			INPUT_RECORD buffer[1];
			ReadConsoleInput(console->inputHandle, buffer, 1, &read);

			switch (lpBuffer[0].EventType) {
				case FOCUS_EVENT: {
					if (console->FocusEvent != NULL) {
						console->FocusEvent(console, lpBuffer[0].Event.FocusEvent.bSetFocus);
					}
					break;
				}
				case KEY_EVENT: {
					if (console->KeyEvent != NULL) {
						console->KeyEvent(
						console, lpBuffer[0].Event.KeyEvent.uChar.AsciiChar, lpBuffer[0].Event.KeyEvent.bKeyDown);
					}
					return buffer[0].Event.KeyEvent.uChar.AsciiChar;
				}
				case MENU_EVENT: {
					// ignored
					break;
				}
				case MOUSE_EVENT: {
					if (console->MouseEvent != NULL) {

						console->MouseEvent(
						console,
								lpBuffer[0].Event.MouseEvent.dwMousePosition.Y,
								lpBuffer[0].Event.MouseEvent.dwMousePosition.X,
								(int)lpBuffer[0].Event.MouseEvent.dwButtonState,
								(int)lpBuffer[0].Event.MouseEvent.dwControlKeyState,
								(int)lpBuffer[0].Event.MouseEvent.dwEventFlags
							);
					}
					break;
				}
				case WINDOW_BUFFER_SIZE_EVENT: {
					if (console->ResizeEvent != NULL) {
						console->ResizeEvent(
						console,
								(int)lpBuffer[0].Event.WindowBufferSizeEvent.dwSize.Y,
								(int)lpBuffer[0].Event.WindowBufferSizeEvent.dwSize.X
							);
					}
				}
				default: break;
			}

			return 0;
		}
		if (read == 0) {
			// found nothing
			return 0;
		}
	}

	DWORD elapsedtimemiliseconds = 0;

	while (elapsedtimemiliseconds < console->blockTimeout) {
		DWORD start = GetTickCount();

		DWORD result = WaitForSingleObject(console->inputHandle, console->blockTimeout - elapsedtimemiliseconds);
		if (result == WAIT_TIMEOUT) {
			return 0;
		}

		INPUT_RECORD buffer[1];
		DWORD read;
		ReadConsoleInput(console->inputHandle, buffer, 1, &read);

		if (read == 0) {
			return 0;
		}

		if (buffer[0].EventType == KEY_EVENT && buffer[0].Event.KeyEvent.bKeyDown) {
			return buffer[0].Event.KeyEvent.uChar.AsciiChar;
		}

		DWORD stop = GetTickCount();
		elapsedtimemiliseconds += stop - start;
	}
	return 0;
	*/
}

int setchar(struct Console* console, char c) {
	// TODO docs
	if (console == NULL) {
		return -1;
	}

	console->buffer[console->cursor].data = c;
	console->buffer[console->cursor].fr = console->fr;
	console->buffer[console->cursor].fg = console->fg;
	console->buffer[console->cursor].fb = console->fb;
	console->buffer[console->cursor].br = console->br;
	console->buffer[console->cursor].bg = console->bg;
	console->buffer[console->cursor].bb = console->bb;
	if (console->cursor != console->width * console->height) {
		console->cursor++;
	}
	return 0;
}

int setcharcursor(struct Console* console, char c, unsigned int row, unsigned int col) {
	// TODO docs
	if (console == NULL) {
		return -1;
	}

	if (row >= console->height) {
		return -2;
	}

	if (col >= console->width) {
		return -3;
	}

	unsigned int cursorpos = console->cursor;
	console->cursor = col + row * console->width;
	setchar(console, c);
	console->cursor = cursorpos;
	return 0;
}

int getcharvariable(struct Console* console, char* c) {
	*c = puregetchar(console);
	return 0;
}

char getsignedintkeyboardin(struct Console* console, char** buffer, int* buffercount) {
	char ret = 0;

	if (*buffer == NULL) {
		*buffer = malloc(10 * sizeof(char));
		if (*buffer == NULL) {
			*buffercount = 0;
			return 0;
		}
	}
	*buffercount = 0;

	while (1) {
		char c = puregetchar(console);

		if (isdigit(c) || c == '-') {
			(*buffer)[(*buffercount)++] = c;
			ret = c;
			break;
		}
		ret = c;
		break;
	}

	if (isdigit(ret) || ret == '-') {
		while (1) {
			char c = puregetchar(console);

			if (isdigit(c)) {
				(*buffer)[(*buffercount)++] = c;
				if (*buffercount % 10 == 0) {
					char* temp = realloc(*buffer, (*buffercount + 10) * sizeof(char));
					if (temp == NULL) {
						free(*buffer);
						*buffer = NULL;
						*buffercount = 0;
						return 0;
					}
					*buffer = temp;
				}
			} else {
				ret = c;
				break;
			}
		}
	}

	(*buffer)[*buffercount] = '\0';
	return ret;
}

char getunsignedintkeyboardin(struct Console* console, char** buffer, int* buffercount) {
	char ret = 0;

	if (*buffer == NULL) {
		*buffer = malloc(10 * sizeof(char));
		if (*buffer == NULL) {
			*buffercount = 0;
			return 0;
		}
	}
	*buffercount = 0;

	while (1) {
		char c = puregetchar(console);
		if (isdigit(c)) {
			(*buffer)[(*buffercount)++] = c;
			if (*buffercount % 10 == 0) {
				char* temp = realloc(*buffer, (*buffercount + 10) * sizeof(char));
				if (temp == NULL) {
					free(*buffer);
					*buffer = NULL;
					*buffercount = 0;
					return 0;
				}
				*buffer = temp;
			}
		} else {
			ret = c;
			break;
		}
	}

	(*buffer)[*buffercount] = '\0';
	return ret;
}

char getshortvariable(struct Console* console, short* h) {
	if (h == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	char ret = getsignedintkeyboardin(console, &buffer, &buffercount);

	if (buffercount == 0 || ret <= 0) {
		free(buffer);
		return ret;
	}

	*h = 0;
	int power = 1;
	for (int i = buffercount - 1; i >= 1; --i) {
		*h += (buffer[i] - '0') * power;
		power *= 10;
	}
	if (buffer[0] == '-') {
		*h *= -1;
	} else {
		*h += (buffer[0] - '0') * power;
	}

	free(buffer);
	return ret;
}

char getunsignedshortvariable(struct Console* console, unsigned short* uh) {
	if (uh == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	char ret = getunsignedintkeyboardin(console, &buffer, &buffercount);

	if (buffercount == 0) {
		free(buffer);
		return ret;
	}

	*uh = 0;
	int power = 1;
	for (int i = buffercount - 1; i >= 0; --i) {
		*uh += (buffer[i] - '0') * power;
		power *= 10;
	}

	free(buffer);
	return ret;
}

char getintvariable(struct Console* console, int* d) {
	if (d == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	char ret = getsignedintkeyboardin(console, &buffer, &buffercount);

	if (buffercount == 0) {
		free(buffer);
		return ret;
	}

	*d = 0;
	int power = 1;
	for (int i = buffercount - 1; i >= 1; --i) {
		*d += (buffer[i] - '0') * power;
		power *= 10;
	}
	if (buffer[0] == '-') {
		*d *= -1;
	} else {
		*d += (buffer[0] - '0') * power;
	}

	free(buffer);
	return ret;
}

char getunsignedintvariable(struct Console* console, unsigned int* ud) {
	if (ud == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	char ret = getunsignedintkeyboardin(console, &buffer, &buffercount);

	if (buffercount == 0) {
		free(buffer);
		return ret;
	}

	*ud = 0;
	int power = 1;
	for (int i = buffercount - 1; i >= 0; --i) {
		*ud += (buffer[i] - '0') * power;
		power *= 10;
	}

	free(buffer);
	return ret;
}

char getlongvariable(struct Console* console, long* ld) {
	if (ld == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	char ret = getsignedintkeyboardin(console, &buffer, &buffercount);

	if (buffercount == 0) {
		free(buffer);
		return ret;
	}

	*ld = strtol(buffer, NULL, 10);

	free(buffer);
	return ret;
}

char getunsignedlongvariable(struct Console* console, unsigned long* uld) {
	if (uld == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	char ret = getunsignedintkeyboardin(console, &buffer, &buffercount);

	if (buffercount == 0) {
		free(buffer);
		return ret;
	}

	*uld = strtoul(buffer, NULL, 10);

	free(buffer);
	return ret;
}

char getlonglongvariable(struct Console* console, long long* lld) {
	if (lld == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	char ret = getsignedintkeyboardin(console, &buffer, &buffercount);

	if (buffercount == 0) {
		free(buffer);
		return ret;
	}

	*lld = strtoll(buffer, NULL, 10);

	free(buffer);
	return ret;
}

char getunsignedlonglongvariable(struct Console* console, unsigned long long* ulld) {
	if (ulld == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	char ret = getunsignedintkeyboardin(console, &buffer, &buffercount);

	if (buffercount == 0) {
		free(buffer);
		return ret;
	}

	*ulld = strtoull(buffer, NULL, 10);

	free(buffer);
	return ret;
}

char getfloatkeyboardin(struct Console* console, char** buffer, int* buffercount) {
	char ret = 0;

	if (*buffer == NULL) {
		*buffer = malloc(10 * sizeof(char));
		if (*buffer == NULL) {
			*buffercount = 0;
			return 0;
		}
	}
	*buffercount = 0;

	while (1) {
		char c = puregetchar(console);
		if (isdigit(c) || c == '-') {
			(*buffer)[(*buffercount)++] = c;
			ret = c;
			break;
		}
		ret = c;
		break;
	}

	if (isdigit(ret) || ret == '-') {
		while (1) {
			char c = puregetchar(console);
			if (isdigit(c) || c == '.') {
				(*buffer)[(*buffercount)++] = c;
				if (*buffercount % 10 == 0) {
					char* temp = realloc(*buffer, *buffercount * 2 * sizeof(char));
					if (temp == NULL) {
						free(*buffer);
						*buffer = NULL;
						*buffercount = 0;
						return 0;
					}
					*buffer = temp;
				}
			} else {
				ret = c;
				break;
			}
		}
	}

	(*buffer)[*buffercount] = '\0';
	return ret;
}

char getfloatvariable(struct Console* console, float* f) {
	if (f == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;
	char ret = getfloatkeyboardin(console, &buffer, &buffercount);
	*f = strtof(buffer, NULL);
	return ret;
}

char getdoublevariable(struct Console* console, double* lf) {
	if (lf == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;
	char ret = getfloatkeyboardin(console, &buffer, &buffercount);
	*lf = strtod(buffer, NULL);
	return ret;
}

char getlongdoublevariable(struct Console* console, long double* llf) {
	if (llf == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;
	char ret = getfloatkeyboardin(console, &buffer, &buffercount);
	*llf = strtold(buffer, NULL);
	return ret;
}

/**
 * Validates format string for getvariables() function
 *
 * @param format format string
 * @param validtokens array of string tokens sorted by size
 * @param tokens number of tokens
 * @return 0 if format is invalid 1 if valid
 */
int validateformatstring(char* format, char** validtokens, int tokens) {
	while (*format) {
		if (*format == '%') {
			format++;
			int matched = 0;
			for (int i = 0; i < tokens; i++) {
				char* token = validtokens[i] + 1;
				size_t size = strlen(token);
				if (strncmp(format, token, size) == 0) {
					format += size;
					matched = 1;
					break;
				}
			}
			if (!matched) {
				// format string is invalid
				return 0;
			}
		} else {
			format++;
		}
	}
	// format string is valid
	return 1;
}

int getvariables(struct Console *console, char *format, ...) {
	// TODO docs

	if (console == NULL) {
		return -1;
	}

	if (format == NULL) {
		return -2;
	}

	/*
	 * %c char
	 * %h short
	 * %uh unsigned short
	 * %d int
	 * %ud unsigned int
	 * %l long
	 * %ul unsigned long
	 * %ll long long
	 * %ull unsigned long long
	 * %f float
	 * %lf double
	 * %llf long double
	 *
	 * TODO %[int]s string
	 */

	// valid tokens sorted by length
	char* validtokens[] = {
		"%llf", "%ull", "%ll", "%uh", "%ud",
		"%lf", "%ul", "%c", "%h", "%d", "%l", "%f"
	};

	int tokenssize = sizeof(validtokens) / sizeof(validtokens[0]);

	if (!validateformatstring(format, validtokens, tokenssize)) {
		// TODO error
		// format string is not valid
		return -3;
	}

	int assignedvariables = 0;
	va_list args;
	va_start(args, format);

	while (*format) {
		char gotchar = 0;
		if (*format == '%') {
			format++;
			// TODO get rid of loop / optimalize
			for (int j = 0; j < tokenssize; j++) {
				char* token = validtokens[j] + 1;
				size_t tokensize = strlen(token);
				if (strncmp(format, token, tokensize) == 0) {
					// match
					format += tokensize;

					if (strcmp(token, "f") == 0) {
						float* f = va_arg(args, float*);
						gotchar = getfloatvariable(console, f);
						if (gotchar <= 0) {
							va_end(args);
							return -2;
						}
						assignedvariables++;
					} else if (strcmp(token, "l") == 0) {
						long* l = va_arg(args, long*);
						gotchar = getlongvariable(console, l);
						if (gotchar <= 0) {
							va_end(args);
							return -3;
						}
						assignedvariables++;
					} else if (strcmp(token, "d") == 0) {
						int* d = va_arg(args, int*);
						gotchar = getintvariable(console, d);
						if (gotchar <= 0) {
							va_end(args);
							return -4;
						}
						assignedvariables++;
					} else if (strcmp(token, "h") == 0) {
						short* h = va_arg(args, short*);
						gotchar = getshortvariable(console, h);
						if (gotchar <= 0) {
							va_end(args);
							return -5;
						}
						assignedvariables++;
					} else if (strcmp(token, "c") == 0) {
						char* c = va_arg(args, char*);
						getcharvariable(console, c);
						if (gotchar <= 0) {
							va_end(args);
							return -6;
						}
						assignedvariables++;
					} else if (strcmp(token, "ul") == 0) {
						unsigned long* ul = va_arg(args, unsigned long*);
						gotchar = getunsignedlongvariable(console, ul);
						if (gotchar <= 0) {
							va_end(args);
							return -7;
						}
						assignedvariables++;
					} else if (strcmp(token, "lf") == 0) {
						double* lf = va_arg(args, double*);
						gotchar = getdoublevariable(console, lf);
						if (gotchar <= 0) {
							va_end(args);
							return -8;
						}
						assignedvariables++;
					} else if (strcmp(token, "ud") == 0) {
						unsigned int* ud = va_arg(args, unsigned int*);
						gotchar = getunsignedintvariable(console, ud);
						if (gotchar <= 0) {
							va_end(args);
							return -9;
						}
						assignedvariables++;
					} else if (strcmp(token, "uh") == 0) {
						unsigned short* uh = va_arg(args, unsigned short*);
						gotchar = getunsignedshortvariable(console, uh);
						if (gotchar <= 0) {
							va_end(args);
							return -10;
						}
						assignedvariables++;
					} else if (strcmp(token, "ll") == 0) {
						long long* ll = va_arg(args, long long*);
						gotchar = getlonglongvariable(console, ll);
						if (gotchar <= 0) {
							va_end(args);
							return -11;
						}
						assignedvariables++;
					} else if (strcmp(token, "ull") == 0) {
						unsigned long long* ull = va_arg(args, unsigned long long*);
						gotchar = getunsignedlonglongvariable(console, ull);
						if (gotchar <= 0) {
							va_end(args);
							return -12;
						}
						assignedvariables++;
					} else if (strcmp(token, "llf") == 0) {
						long double* llf = va_arg(args, long double*);
						gotchar = getlongdoublevariable(console, llf);
						if (gotchar <= 0) {
							va_end(args);
							return -13;
						}
						assignedvariables++;
					}
					break;
				}
			}
		}

		if (gotchar == 0) {
			getcharvariable(console, &gotchar);
			if (gotchar <= 0) {
				va_end(args);
				return -14;
			}
		}

		if (*format == gotchar) {
			format++;
		}
		else {
			va_end(args);
			return -15;
		}
	}

	va_end(args);

	return assignedvariables;
}

int setstringformatted(struct Console* console, char *format, ...) {
	// TODO docs

	if (console == NULL) {
		return -1;
	}

	if (format == NULL) {
		return -2;
	}

	va_list ap, apcopy;
	va_start(ap, format);
	va_copy(apcopy, ap);

	int size = vsnprintf(NULL, 0, format, apcopy);
	va_end(apcopy);

	char* memory = malloc((size + 1) * sizeof(char));

	vsnprintf(memory, size + 1, format, ap);
	va_end(ap);

	setstring(console, memory);
	free(memory);

	return 0;
}

int setstringformattedcursor(struct Console* console, int row, int col, char* format, ...) {
	// TODO docs
	if (console == NULL) {
		return -1;
	}

	if (row < 0) {
		return -2;
	}

	if (row >= console->height) {
		return -3;
	}

	if (col < 0) {
		return -4;
	}

	if (col >= console->width) {
		return -5;
	}

	if (format == NULL) {
		return -6;
	}

	int nowrow, nowcol;
	getcursorposition(console, &nowrow, &nowcol);
	setcursorposition(console, row, col);

	va_list ap, apcopy;
	va_start(ap, format);
	va_copy(apcopy, ap);

	int size = vsnprintf(NULL, 0, format, apcopy);
	va_end(apcopy);

	char* memory = malloc((size + 1) * sizeof(char));

	vsnprintf(memory, size + 1, format, ap);
	va_end(ap);

	setstring(console, memory);
	free(memory);

	setcursorposition(console, nowrow, nowcol);

	return 0;
}

// TODO is still necessary?
struct GetPureCharThreadArgs {
	struct Console* console;
	char readChar;
};

DWORD puregetcharthread(LPVOID lpParam) {
	struct GetPureCharThreadArgs* args = lpParam;

	args->readChar = puregetchar(args->console);
	return 0;
}

int getstring(struct Console* console, char *buffer, size_t size) {
	// TODO docs

	if (console == NULL) {
		return -1;
	}

	if (buffer == NULL) {
		return -2;
	}

	DWORD elapsedtimemiliseconds = 0;
	int i = 0;
	while (i != size - 1 && elapsedtimemiliseconds < console->blockTimeout) {
		DWORD start = GetTickCount();

		struct GetPureCharThreadArgs args;
		args.console = console;
		args.readChar = 0;
		HANDLE thread = CreateThread(NULL, 0, puregetcharthread, &args, 0, NULL);

		DWORD result = WaitForSingleObject(thread, console->blockTimeout - elapsedtimemiliseconds);
		if (result == WAIT_TIMEOUT) {
			break;
		}

		char c = args.readChar;

		if (c == '\n' || c == '\r') {
			break;
		}
		if (c != 0 && isprint(c)) {
			buffer[i] = c;
			i++;
		}
		DWORD stop = GetTickCount();
		elapsedtimemiliseconds += stop - start;
	}
	buffer[i] = '\0';
	// TODO docs
	return i;
}

int getstringbuffer(struct Console* console, char *buffer, size_t size) {
	// TODO docs

	if (console == NULL) {
		return -1;
	}

	if (buffer == NULL) {
		return -2;
	}

	DWORD elapsedtimemiliseconds = 0;
	int i = 0;
	while (i != size - 1 && elapsedtimemiliseconds < console->blockTimeout) {
		DWORD start = GetTickCount();

		struct GetPureCharThreadArgs args;
		args.console = console;
		args.readChar = 0;
		HANDLE thread = CreateThread(NULL, 0, puregetcharthread, &args, 0, NULL);

		DWORD result = WaitForSingleObject(thread, console->blockTimeout - elapsedtimemiliseconds);
		if (result == WAIT_TIMEOUT) {
			break;
		}

		char c = args.readChar;

		if (c != 0 && isprint(c)) {
			buffer[i] = c;
			i++;
		}
		DWORD stop = GetTickCount();
		elapsedtimemiliseconds += stop - start;
	}
	buffer[i] = '\0';
	return 0;
}

int setstring(struct Console* console, char *string) {
	// TODO docs

	if (console == NULL) {
		return -1;
	}
	if (string == NULL) {
		return -2;
	}
	size_t size = strlen(string);
	for (int i = 0; i < size; ++i) {
		setchar(console, string[i]);
	}
	return 0;
}

int setstringcursor(struct Console* console, char *string, int row, int col) {
	// TODO docs

	if (console == NULL) {
		return -1;
	}

	if (string == NULL) {
		return -2;
	}

	if (row < 0) {
		return -3;
	}

	if (row > console->height - 1) {
		return -4;
	}

	if (col < 0) {
		return -5;
	}

	if (col > console->width - 1) {
		return -6;
	}

	int nowrow, nowcol;
	getcursorposition(console, &nowrow, &nowcol);
	setcursorposition(console, row, col);

	setstring(console, string);

	setcursorposition(console, nowrow, nowcol);

	return 0;
}

int clear(struct Console* console) {
	if (console == NULL) {
		return -1;
	}

	// TODO move default values to constants
	for (int i = 0; i < console->height * console->width; ++i) {
		console->buffer[i].data = ' ';
		console->buffer[i].fr = 255;
		console->buffer[i].fg = 255;
		console->buffer[i].fb = 255;
		console->buffer[i].br = 0;
		console->buffer[i].bg = 0;
		console->buffer[i].bb = 0;
	}
	console->cursor = 0;
	return 0;
}

int fill(struct Console* console, char c, unsigned int fr, unsigned int fg, unsigned int fb, unsigned int br, unsigned int bg, unsigned int bb) {
	// TODO docs
	if (console == NULL) {
		return -1;
	}

	if (fr > 255) {
		return -2;
	}

	if (fg > 255) {
		return -3;
	}

	if (fb > 255) {
		return -4;
	}

	if (br > 255) {
		return -5;
	}

	if (bg > 255) {
		return -6;
	}

	if (bb > 255) {
		return -7;
	}

	for (int i = 0; i < console->height * console->width; ++i) {
		console->buffer[i].data = c;
		console->buffer[i].fr = fr;
		console->buffer[i].fg = fg;
		console->buffer[i].fb = fb;
		console->buffer[i].br = br;
		console->buffer[i].bg = bg;
		console->buffer[i].bb = bb;
	}

	setcursorposition(console, 0, 0);

	return 0;
}

int set2darray(struct Console* console, char* array, unsigned int row, unsigned int col, unsigned int width, unsigned int height) {
	// TODO implement error handling

	if (console == NULL) {
		return -1;
	}

	if (array == NULL) {
		return -2;
	}

	if (height + row > console->height - 1) {
		return -3;
	}

	if (width + col > console->width - 1) {
		return -4;
	}

	for (int i = 0; i < height * width; ++i) {
		unsigned int r = row + i / width;
		unsigned int c = col + i % width;
		setcharcursor(console, array[i], r, c);
	}
	return 0;
}

int setcursorposition(struct Console* console, unsigned int row, unsigned int col) {
	if (console == NULL) {
		return -1;
	}
	if (row >= console->height) {
		return -2;
	}
	if (col >= console->width) {
		return -3;
	}

	console->cursor = col + row * console->width;
	return 0;
}

int getcursorposition(struct Console* console, unsigned int *row, unsigned int *col) {
	// TODO refactor
	if (console == NULL) {
		return -1;
	}
	if (row == NULL) {
		return -2;
	}
	if (col == NULL) {
		return -3;
	}

	*row = console->cursor / console->width;
	*col = console->cursor % console->width;
	return 0;
}

int refresh(struct Console* console) {
	// TODO remember about updating (specially buffer size)
	// TODO implement error handling

	if (console == NULL) {
		return -1;
	}

	unsigned int bufferSize = console->width * console->height * (19 + 19 + 1) + console->height + 6;
	char *outputBuffer = malloc(bufferSize);
	memset(outputBuffer, 0, bufferSize);
	int place = 0;
	if (outputBuffer == NULL) {
		return -2;
	}
	char buff[6];
	int add = sprintf(buff, "\x1B[1;1f");
	memcpy(&outputBuffer[place], buff, add);
	place += add;

	unsigned int fr = 255;
	unsigned int fg = 255;
	unsigned int fb = 255;
	unsigned int br = 0;
	unsigned int bg = 0;
	unsigned int bb = 0;
	for (int i = 0; i < console->height * console->width; ++i) {
		if (i > 0 && i % console->width == 0) {
			outputBuffer[place] = '\n';
			place++;
		}

		struct Cell c = console->buffer[i];

		if (fr != c.fr || fg != c.fg || fb != c.fb) {
			fr = c.fr;
			fg = c.fg;
			fb = c.fb;
			char colorbuff[19];
			add = sprintf(colorbuff, "\x1B[38;2;%d;%d;%dm", fr, fg, fb);
			memcpy(&outputBuffer[place], colorbuff, add);
			place += add;
		}
		if (br != c.br || bg != c.bg || bb != c.bb) {
			br = c.br;
			bg = c.bg;
			bb = c.bb;
			char colorbuff[19];
			add = sprintf(colorbuff, "\x1B[48;2;%d;%d;%dm", br, bg, bb);
			memcpy(&outputBuffer[place], colorbuff, add);
			place += add;
		}
		outputBuffer[place] = c.data;
		place++;
	}
	outputBuffer[place] = '\0';
	WriteConsoleA(console->outputHandle, outputBuffer, strlen(outputBuffer), NULL, NULL);
	free(outputBuffer);
	return 0;
}
