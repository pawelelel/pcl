//
// Created by pawel on 14.05.2025.
//

#include "pcl.h"

#include <math.h>
#include <stdio.h>


HANDLE mutexHandle;


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
		return;
	}

	struct Node* newNode = malloc(sizeof(struct Node));
	if (newNode == NULL) {
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
		return 0;
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
		return 0;
	}
	return queue->head->value;
}

DWORD WINAPI inputthread(LPVOID lpParam) {
	struct Console* console = lpParam;

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
					enqueue(console->inputQueue, lpBuffer[0].Event.KeyEvent.uChar.AsciiChar);
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
				int height = lpBuffer[0].Event.WindowBufferSizeEvent.dwSize.Y;
				int width = lpBuffer[0].Event.WindowBufferSizeEvent.dwSize.X;
				struct Cell* newbuffer = malloc(sizeof(struct Cell) * width * height);
				WaitForSingleObject(mutexHandle, INFINITE);
				struct Cell* prebuffer = console->buffer;

				for (int i = 0; i < height * width; ++i) {
					newbuffer[i].data = console->defaultchar;
					newbuffer[i].foregroundRed = console->defaultForegroundRed;
					newbuffer[i].foregroundGreen = console->defaultForegroundGreen;
					newbuffer[i].foregroundBlue = console->defaultForegroundBlue;
					newbuffer[i].backgroundRed = console->defaultBackgroundRed;
					newbuffer[i].backgroundGreen = console->defaultBackgroundGreen;
					newbuffer[i].backgroundBlue = console->defaultBackgroundBlue;
				}

				// copy prebuffer to newbuffer
				for (unsigned int i = 0; i < console->width * console->height; i++) {
					unsigned int row = i / console->width;
					unsigned int col = i % console->width;

					unsigned int newcursor = col + row * width;
					if (newcursor >= width * height) {
						continue;
					}
					newbuffer[newcursor] = prebuffer[i];
				}

				free(prebuffer);
				console->buffer = newbuffer;
				console->width = width;
				console->height = height;
				console->cursor = 0;
				ReleaseMutex(mutexHandle);
				if (console->ResizeEvent != NULL) {
					console->ResizeEvent(console, height, width);
				}
				break;
			}
			default: break;
		}
	}
}

struct Console* start(void) {
	struct Console* console = malloc(sizeof(struct Console));
	console->inputHandle = GetStdHandle(STD_INPUT_HANDLE);

	DWORD fdwMode = ENABLE_ECHO_INPUT | ENABLE_INSERT_MODE | ENABLE_LINE_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE | ENABLE_WINDOW_INPUT | ENABLE_VIRTUAL_TERMINAL_INPUT;
	SetConsoleMode(console->inputHandle, fdwMode);

	mutexHandle = CreateMutex(NULL, FALSE, NULL);
	WaitForSingleObject(mutexHandle, INFINITE);
	console->inputQueue = init();
	DWORD threadid;
	console->threadHandle = CreateThread(NULL, 0, inputthread, console, CREATE_SUSPENDED, &threadid);

	console->outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleMode(console->outputHandle, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_LVB_GRID_WORLDWIDE);
	SetConsoleActiveScreenBuffer(console->outputHandle);

	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(console->outputHandle, &info);

	console->cursor = 0;
	console->cursorstyle = BLINKING_BAR;
	console->height = info.dwSize.Y;
	console->width = info.dwSize.X;
	console->foregroundRed = 255;console->foregroundGreen = 255;console->foregroundBlue = 255;
	console->backgroundRed = 0;console->backgroundGreen= 0;console->backgroundBlue = 0;

	console->bold = FALSE;
	console->dim = FALSE;
	console->italic = FALSE;
	console->underline = FALSE;
	console->blinking = FALSE;
	console->strikethrough = FALSE;
	console->doubleunderline = FALSE;

	console->defaultchar = ' ';
	console->defaultForegroundRed = 255;
	console->defaultForegroundGreen = 255;
	console->defaultForegroundBlue = 255;
	console->defaultBackgroundRed = 0;
	console->defaultBackgroundGreen = 0;
	console->defaultBackgroundBlue = 0;

	console->buffer = malloc(sizeof(struct Cell) * console->width * console->height);
	for (int i = 0; i < console->height * console->width; ++i) {
		console->buffer[i].data = console->defaultchar;
		console->buffer[i].foregroundRed = console->defaultForegroundRed;
		console->buffer[i].foregroundGreen = console->defaultForegroundGreen;
		console->buffer[i].foregroundBlue = console->defaultForegroundBlue;
		console->buffer[i].backgroundRed = console->defaultBackgroundRed;
		console->buffer[i].backgroundGreen = console->defaultBackgroundGreen;
		console->buffer[i].backgroundBlue = console->defaultBackgroundBlue;

		console->buffer[i].bold = console->bold;
		console->buffer[i].dim = console->dim;
		console->buffer[i].italic = console->italic;
		console->buffer[i].underline = console->underline;
		console->buffer[i].blinking = console->blinking;
		console->buffer[i].strikethrough = console->strikethrough;
		console->buffer[i].doubleunderline = console->doubleunderline;
	}

	console->errorHandle = GetStdHandle(STD_ERROR_HANDLE);
	console->blockInput = TRUE;
	console->blockTimeout = 0;

	console->FocusEvent = NULL;
	console->KeyEvent = NULL;
	console->MouseEvent = NULL;
	console->ResizeEvent = NULL;

	ReleaseMutex(mutexHandle);
	ResumeThread(console->threadHandle);

	return console;
}

int end(struct Console* console) {
	if (console == NULL) {
		return -1;
	}

	console->threadExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetEvent(console->threadExitEvent);
	WaitForSingleObject(console->threadHandle, INFINITE);
	CloseHandle(console->threadHandle);
	CloseHandle(console->threadExitEvent);
	CloseHandle(mutexHandle);

	free(console);

	printf("\x1B[1;1f");
	return 0;
}

int setcursorstyle(struct Console *console, int style) {
	if (console == NULL) {
		return -1;
	}

	switch (style) {
		case BLINKING_BLOCK: {
			char* blinkBlock = "\x1B[1 q";
			WaitForSingleObject(mutexHandle, INFINITE);
			WriteConsoleA(console->outputHandle, blinkBlock, strlen(blinkBlock), NULL, NULL);
			ReleaseMutex(mutexHandle);
			return 0;
		}
		case STEADY_BLOCK: {
			char* steadyBlock = "\x1B[2 q";
			WaitForSingleObject(mutexHandle, INFINITE);
			WriteConsoleA(console->outputHandle, steadyBlock, strlen(steadyBlock), NULL, NULL);
			ReleaseMutex(mutexHandle);
			return 0;
		}
		case BLINKING_UNDERLINE: {
			char* blinkUnderline = "\x1B[3 q";
			WaitForSingleObject(mutexHandle, INFINITE);
			WriteConsoleA(console->outputHandle, blinkUnderline, strlen(blinkUnderline), NULL, NULL);
			ReleaseMutex(mutexHandle);
			return 0;
		}
		case STEADY_UNDERLINE: {
			char* steadyUnderline = "\x1B[4 q";
			WaitForSingleObject(mutexHandle, INFINITE);
			WriteConsoleA(console->outputHandle, steadyUnderline, strlen(steadyUnderline), NULL, NULL);
			ReleaseMutex(mutexHandle);
			return 0;
		}
		case BLINKING_BAR: {
			char* blinkBar = "\x1B[5 q";
			WaitForSingleObject(mutexHandle, INFINITE);
			WriteConsoleA(console->outputHandle, blinkBar, strlen(blinkBar), NULL, NULL);
			ReleaseMutex(mutexHandle);
			return 0;
		}
		case STEADY_BAR: {
			char* steadyBar = "\x1B[6 q";
			WaitForSingleObject(mutexHandle, INFINITE);
			WriteConsoleA(console->outputHandle, steadyBar, strlen(steadyBar), NULL, NULL);
			ReleaseMutex(mutexHandle);
			return 0;
		}
		default: {
			return -2;
		}
	}

	//shouldn't happed
	return -2;
}

int unsetcursorstyle(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->cursorstyle = BLINKING_BAR;
	ReleaseMutex(mutexHandle);
	return 0;
}

int showcursor(struct Console *console) {
	if (console == NULL) {
		return -1;
	}

	char* show = "\x1B[?25h";
	WaitForSingleObject(mutexHandle, INFINITE);
	WriteConsoleA(console->outputHandle, show, strlen(show), NULL, NULL);
	ReleaseMutex(mutexHandle);

	return 0;
}

int hidecursor(struct Console *console) {
	if (console == NULL) {
		return -1;
	}

	char* hide = "\x1B[?25l";
	WaitForSingleObject(mutexHandle, INFINITE);
	WriteConsoleA(console->outputHandle, hide, strlen(hide), NULL, NULL);
	ReleaseMutex(mutexHandle);

	return 0;
}

int setfontbold(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->bold = TRUE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int unsetfontbold(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->bold = FALSE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int getfontbold(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	return console->bold;
	ReleaseMutex(mutexHandle);
}

int setfontdim(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->dim = TRUE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int unsetfontdim(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->dim = FALSE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int getfontdim(struct Console *console) {
	if (console == NULL) {
		return -1;
	}

	WaitForSingleObject(mutexHandle, INFINITE);
	BOOL dim = console->dim;
	ReleaseMutex(mutexHandle);

	return dim;
}

int setfontitalic(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->italic = TRUE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int unsetfontitalic(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->italic = FALSE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int getfontitalic(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	BOOL italic = console->italic;
	ReleaseMutex(mutexHandle);
	return italic;
}

int setfontunderline(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->underline = TRUE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int unsetfontunderline(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->underline = FALSE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int getfontunderline(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	BOOL underline = console->underline;
	ReleaseMutex(mutexHandle);
	return underline;
}

int setfontblinking(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->blinking = TRUE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int unsetfontblinking(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->blinking = FALSE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int getfontblinking(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	BOOL blinking = console->blinking;
	ReleaseMutex(mutexHandle);
	return blinking;
}

int setfontstrikethrough(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->strikethrough = TRUE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int unsetfontstrikethrough(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->strikethrough = FALSE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int getfontstrikethrough(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	BOOL strikethrough = console->strikethrough;
	ReleaseMutex(mutexHandle);
	return strikethrough;
}

int setfontdoubleunderline(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->doubleunderline = TRUE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int unsetfontdoubleunderline(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->doubleunderline = FALSE;
	ReleaseMutex(mutexHandle);
	return 0;
}

int getfontdoubleunderline(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	BOOL doubleunderline = console->doubleunderline;
	ReleaseMutex(mutexHandle);
	return doubleunderline;
}

int setforegroundcolor(struct Console *console, unsigned int red, unsigned int green, unsigned int blue) {
	if (console == NULL) {
		return -1;
	}
	if (red > 255) {
		return -2;
	}
	if (green > 255) {
		return -3;
	}
	if (blue > 255) {
		return -4;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->foregroundRed = red;
	console->foregroundGreen = green;
	console->foregroundBlue = blue;
	ReleaseMutex(mutexHandle);
	return 0;
}

int setbackgroundcolor(struct Console *console, int red, int green, int blue) {
	if (console == NULL) {
		return -1;
	}
	if (red > 255) {
		return -2;
	}
	if (green > 255) {
		return -3;
	}
	if (blue > 255) {
		return -4;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->backgroundRed = red;
	console->backgroundGreen = green;
	console->backgroundBlue = blue;
	ReleaseMutex(mutexHandle);
	return 0;
}

int clearforegroundcolor(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->foregroundRed = console->defaultForegroundRed;
	console->foregroundGreen = console->defaultForegroundGreen;
	console->foregroundBlue = console->defaultForegroundBlue;
	ReleaseMutex(mutexHandle);
	return 0;
}

int clearbackgroundcolor(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->backgroundRed = console->defaultBackgroundRed;
	console->backgroundGreen = console->defaultBackgroundGreen;
	console->backgroundBlue = console->defaultBackgroundBlue;
	ReleaseMutex(mutexHandle);
	return 0;
}

int setinputblock(struct Console *console, BOOL blockinput) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->blockInput = blockinput;
	ReleaseMutex(mutexHandle);
	return 0;
}

int getinputblock(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	const int b =  console->blockInput;
	ReleaseMutex(mutexHandle);
	return b;
}

int setfocusevent(struct Console *console, void(*FocusEvent)(struct Console*, int)) {
	if (console == NULL) {
		return -1;
	}
	if (FocusEvent == NULL) {
		return -2;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->FocusEvent = FocusEvent;
	ReleaseMutex(mutexHandle);
	return 0;
}

int unsetfocusevent(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->FocusEvent = NULL;
	ReleaseMutex(mutexHandle);
	return 0;
}

int setkeyevent(struct Console *console, void(*KeyEvent)(struct Console*, char, int)) {
	if (console == NULL) {
		return -1;
	}
	if (KeyEvent == NULL) {
		return -2;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->KeyEvent = KeyEvent;
	ReleaseMutex(mutexHandle);
	return 0;
}

int unsetkeyevent(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->KeyEvent = NULL;
	ReleaseMutex(mutexHandle);
	return 0;
}

int setmouseevent(struct Console *console, void(*MouseEvent)(struct Console*, int, int, int, int, int)) {
	if (console == NULL) {
		return -1;
	}
	if (MouseEvent == NULL) {
		return -2;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->MouseEvent = MouseEvent;
	ReleaseMutex(mutexHandle);
	return 0;
}

int unsetmouseevent(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->MouseEvent = NULL;
	ReleaseMutex(mutexHandle);
	return 0;
}

int setresizeevent(struct Console *console, void(*ResizeEvent)(struct Console*, unsigned int, unsigned int)) {
	if (console == NULL) {
		return -1;
	}
	if (ResizeEvent == NULL) {
		return -2;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->ResizeEvent = ResizeEvent;
	ReleaseMutex(mutexHandle);
	return 0;
}

int unsetresizeevent(struct Console *console) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->ResizeEvent = NULL;
	ReleaseMutex(mutexHandle);
	return 0;
}

int settimeout(struct Console *console, unsigned int timeout) {
	if (console == NULL) {
		return -1;
	}
	WaitForSingleObject(mutexHandle, INFINITE);
	console->blockTimeout = timeout;
	ReleaseMutex(mutexHandle);
	return 0;
}

int gettimeout(struct Console *console) {
	if (console == NULL) {
		return -1;
	}

	WaitForSingleObject(mutexHandle, INFINITE);
	const unsigned int b = console->blockTimeout;
	ReleaseMutex(mutexHandle);
	return b;
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
	return 0;
}

int getdimensions(struct Console* console, unsigned int* width, unsigned int* height) {
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

	WaitForSingleObject(mutexHandle, INFINITE);
	*width = console->width;
	*height = console->height;
	ReleaseMutex(mutexHandle);
	return 0;
}

/**
 * pure ReadConsoleInput() wrapper without any non-blocking or timeout features
 *
 * @param console pointer to struct Console
 * @return char from console input buffer
 */
char puregetchar(struct Console* console) {
	while (1) {
		WaitForSingleObject(mutexHandle, INFINITE);

		const char c = dequeue(console->inputQueue);
		if (c != 0) {
			ReleaseMutex(mutexHandle);
			return c;
		}

		ReleaseMutex(mutexHandle);
	}
}

struct GetPureCharThreadArgs {
	struct Console* console;
	char readChar;
};

DWORD puregetcharthread(LPVOID lpParam) {
	struct GetPureCharThreadArgs* args = lpParam;

	args->readChar = puregetchar(args->console);
	return 0;
}

char getchr(struct Console* console) {
	// TODO add virtual key codes for arrows, special keys etc.
	// TODO add unicode

	if (console == NULL) {
		return -1;
	}

	WaitForSingleObject(mutexHandle, INFINITE);
	const int bi = console->blockInput;
	const unsigned int bt = console->blockTimeout;
	ReleaseMutex(mutexHandle);

	if (!bi) {
		// non-blocking input behaviour
		WaitForSingleObject(mutexHandle, INFINITE);
		const char c = dequeue(console->inputQueue);
		ReleaseMutex(mutexHandle);
		return c;
	}

	if (bt <= 0) {
		const char c = puregetchar(console);
		return c;
	}

	struct GetPureCharThreadArgs args;
	args.console = console;
	args.readChar = 0;
	HANDLE thread = CreateThread(NULL, 0, puregetcharthread, &args, 0, NULL);

	const DWORD result = WaitForSingleObject(thread, bt);
	if (result == WAIT_TIMEOUT) {
		return -2;
	}

	const char c = args.readChar;

	return c;
}

int setchar(struct Console* console, char c) {
	if (console == NULL) {
		return -1;
	}

	switch (c) {
		case '\n': {
			WaitForSingleObject(mutexHandle, INFINITE);
			console->cursor += console->width;
			console->cursor -= console->cursor % console->width;
			if (console->cursor >= console->width * console->height) {
				console->cursor = 0;
			}
			ReleaseMutex(mutexHandle);
			return 0;
		}
		case '\a': {
			// not supported
			WaitForSingleObject(mutexHandle, INFINITE);
			c = console->defaultchar;
			ReleaseMutex(mutexHandle);
			break;
		}
		case '\b': {
			WaitForSingleObject(mutexHandle, INFINITE);
			if (console->cursor == 0) {
				ReleaseMutex(mutexHandle);
				return 0;
			}
			console->cursor--;
			console->buffer[console->cursor].data = console->defaultchar;
			console->buffer[console->cursor].foregroundRed = console->foregroundRed;
			console->buffer[console->cursor].foregroundGreen = console->foregroundGreen;
			console->buffer[console->cursor].foregroundBlue = console->foregroundBlue;
			console->buffer[console->cursor].backgroundRed = console->backgroundRed;
			console->buffer[console->cursor].backgroundGreen = console->backgroundGreen;
			console->buffer[console->cursor].backgroundBlue = console->backgroundBlue;
			ReleaseMutex(mutexHandle);
			return 0;
		}
		case '\v': {
			WaitForSingleObject(mutexHandle, INFINITE);
			console->cursor += console->width;
			if (console->cursor >= console->width * console->height) {
				console->cursor = 0;
			}
			ReleaseMutex(mutexHandle);
			return 0;
		}
		case '\r': {
			WaitForSingleObject(mutexHandle, INFINITE);
			console->cursor -= console->cursor % console->width;
			ReleaseMutex(mutexHandle);
			return 0;
		}
		case '\f': {
			WaitForSingleObject(mutexHandle, INFINITE);
			console->cursor = 0;
			ReleaseMutex(mutexHandle);
			return 0;
		}
		default: break;
	}

	// v \n - new line
	// v \a - not supported
	// v \b - backspace
	// v \v - vertical enter
	// v \r - carrige return
	// v \f - set cursor 0 0
	// v \t - tab

	WaitForSingleObject(mutexHandle, INFINITE);
	console->buffer[console->cursor].data = c;
	console->buffer[console->cursor].foregroundRed = console->foregroundRed;
	console->buffer[console->cursor].foregroundGreen = console->foregroundGreen;
	console->buffer[console->cursor].foregroundBlue = console->foregroundBlue;
	console->buffer[console->cursor].backgroundRed = console->backgroundRed;
	console->buffer[console->cursor].backgroundGreen = console->backgroundGreen;
	console->buffer[console->cursor].backgroundBlue = console->backgroundBlue;

	console->buffer[console->cursor].bold = console->bold;
	console->buffer[console->cursor].dim = console->dim;
	console->buffer[console->cursor].italic = console->italic;
	console->buffer[console->cursor].underline = console->underline;
	console->buffer[console->cursor].blinking = console->blinking;
	console->buffer[console->cursor].strikethrough = console->strikethrough;
	console->buffer[console->cursor].doubleunderline = console->doubleunderline;
	if (console->cursor != console->width * console->height) {
		console->cursor++;
	}
	ReleaseMutex(mutexHandle);
	return 0;
}

int setcharformatted(struct Console* console, char c, unsigned int foregroundRed, unsigned int foregroundGreen, unsigned int foregroundBlue,
						unsigned int backgroundRed, unsigned int backgroundGreen, unsigned int backgroundBlue,
						BOOL bold, BOOL dim, BOOL italic, BOOL underline, BOOL blinking, BOOL strikethrough, BOOL doubleunderline) {
	if (console == NULL) {
		return -1;
	}

	if (foregroundRed > 255) {
		return -2;
	}

	if (foregroundGreen > 255) {
		return -3;
	}

	if (foregroundBlue > 255) {
		return -4;
	}

	if (backgroundRed > 255) {
		return -5;
	}

	if (backgroundGreen > 255) {
		return -6;
	}

	if (backgroundBlue > 255) {
		return -7;
	}

	if (bold != TRUE && bold != FALSE) {
		return -8;
	}

	if (dim != TRUE && dim != FALSE) {
		return -9;
	}

	if (italic != TRUE && italic != FALSE) {
		return -10;
	}

	if (underline != TRUE && underline != FALSE) {
		return -11;
	}

	if (blinking != TRUE && blinking != FALSE) {
		return -12;
	}

	if (strikethrough != TRUE && strikethrough != FALSE) {
		return -13;
	}

	if (doubleunderline != TRUE && doubleunderline != FALSE) {
		return -14;
	}



	switch (c) {
		case '\n': {
			WaitForSingleObject(mutexHandle, INFINITE);
			console->cursor += console->width;
			console->cursor -= console->cursor % console->width;
			if (console->cursor >= console->width * console->height) {
				console->cursor = 0;
			}
			ReleaseMutex(mutexHandle);
			return 0;
		}
		case '\a': {
			// not supported
			WaitForSingleObject(mutexHandle, INFINITE);
			c = console->defaultchar;
			ReleaseMutex(mutexHandle);
			break;
		}
		case '\b': {
			WaitForSingleObject(mutexHandle, INFINITE);
			if (console->cursor == 0) {
				ReleaseMutex(mutexHandle);
				return 0;
			}
			console->cursor--;
			console->buffer[console->cursor].data = console->defaultchar;
			console->buffer[console->cursor].foregroundRed = console->foregroundRed;
			console->buffer[console->cursor].foregroundGreen = console->foregroundGreen;
			console->buffer[console->cursor].foregroundBlue = console->foregroundBlue;
			console->buffer[console->cursor].backgroundRed = console->backgroundRed;
			console->buffer[console->cursor].backgroundGreen = console->backgroundGreen;
			console->buffer[console->cursor].backgroundBlue = console->backgroundBlue;
			ReleaseMutex(mutexHandle);
			return 0;
		}
		case '\v': {
			WaitForSingleObject(mutexHandle, INFINITE);
			console->cursor += console->width;
			if (console->cursor >= console->width * console->height) {
				console->cursor = 0;
			}
			ReleaseMutex(mutexHandle);
			return 0;
		}
		case '\r': {
			WaitForSingleObject(mutexHandle, INFINITE);
			console->cursor -= console->cursor % console->width;
			ReleaseMutex(mutexHandle);
			return 0;
		}
		case '\f': {
			WaitForSingleObject(mutexHandle, INFINITE);
			console->cursor = 0;
			ReleaseMutex(mutexHandle);
			return 0;
		}
		default: break;
	}

	// v \n - new line
	// v \a - not supported
	// v \b - backspace
	// v \v - vertical enter
	// v \r - carrige return
	// v \f - set cursor 0 0
	// v \t - tab

	WaitForSingleObject(mutexHandle, INFINITE);
	console->buffer[console->cursor].data = c;
	console->buffer[console->cursor].foregroundRed = foregroundRed;
	console->buffer[console->cursor].foregroundGreen = foregroundGreen;
	console->buffer[console->cursor].foregroundBlue = foregroundBlue;
	console->buffer[console->cursor].backgroundRed = backgroundRed;
	console->buffer[console->cursor].backgroundGreen = backgroundGreen;
	console->buffer[console->cursor].backgroundBlue = backgroundBlue;

	console->buffer[console->cursor].bold = bold;
	console->buffer[console->cursor].dim = dim;
	console->buffer[console->cursor].italic = italic;
	console->buffer[console->cursor].underline = underline;
	console->buffer[console->cursor].blinking = blinking;
	console->buffer[console->cursor].strikethrough = strikethrough;
	console->buffer[console->cursor].doubleunderline = doubleunderline;
	if (console->cursor != console->width * console->height) {
		console->cursor++;
	}
	ReleaseMutex(mutexHandle);
	return 0;
}

int setcharcursor(struct Console* console, char c, unsigned int row, unsigned int col) {
	if (console == NULL) {
		return -1;
	}

	WaitForSingleObject(mutexHandle, INFINITE);

	if (row >= console->height) {
		ReleaseMutex(mutexHandle);
		return -2;
	}

	if (col >= console->width) {
		ReleaseMutex(mutexHandle);
		return -3;
	}

	unsigned int cursorpos = console->cursor;
	console->cursor = col + row * console->width;
	ReleaseMutex(mutexHandle);

	setchar(console, c);

	WaitForSingleObject(mutexHandle, INFINITE);
	console->cursor = cursorpos;
	ReleaseMutex(mutexHandle);
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
int validateformatstringforgetvariables(char* format, char** validtokens, int tokens) {
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
	if (console == NULL) {
		return -1;
	}

	if (format == NULL) {
		return -2;
	}

	/*
	 * TODO %s string
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
	 */

	// valid tokens sorted by length
	char* validtokens[] = {
		"%llf", "%ull", "%ll", "%uh", "%ud",
		"%lf", "%ul", "%c", "%h", "%d", "%l", "%f"
	};

	int tokenssize = sizeof(validtokens) / sizeof(validtokens[0]);

	if (!validateformatstringforgetvariables(format, validtokens, tokenssize)) {
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
			for (int i = 0; i < tokenssize; i++) {
				char* token = validtokens[i] + 1;
				size_t tokensize = strlen(token);
				if (strncmp(format, token, tokensize) == 0) {
					// match
					format += tokensize;

					if (strcmp(token, "f") == 0) {
						float* f = va_arg(args, float*);
						gotchar = getfloatvariable(console, f);
						if (gotchar <= 0) {
							va_end(args);
							return -4;
						}
						assignedvariables++;
						break;
					}
					if (strcmp(token, "l") == 0) {
						long* l = va_arg(args, long*);
						gotchar = getlongvariable(console, l);
						if (gotchar <= 0) {
							va_end(args);
							return -5;
						}
						assignedvariables++;
						break;
					}
					if (strcmp(token, "d") == 0) {
						int* d = va_arg(args, int*);
						gotchar = getintvariable(console, d);
						if (gotchar <= 0) {
							va_end(args);
							return -6;
						}
						assignedvariables++;
						break;
					}
					if (strcmp(token, "h") == 0) {
						short* h = va_arg(args, short*);
						gotchar = getshortvariable(console, h);
						if (gotchar <= 0) {
							va_end(args);
							return -7;
						}
						assignedvariables++;
						break;
					}
					if (strcmp(token, "c") == 0) {
						char* c = va_arg(args, char*);
						getcharvariable(console, c);
						if (gotchar <= 0) {
							va_end(args);
							return -8;
						}
						assignedvariables++;
						break;
					}
					if (strcmp(token, "ul") == 0) {
						unsigned long* ul = va_arg(args, unsigned long*);
						gotchar = getunsignedlongvariable(console, ul);
						if (gotchar <= 0) {
							va_end(args);
							return -9;
						}
						assignedvariables++;
						break;
					}
					if (strcmp(token, "lf") == 0) {
						double* lf = va_arg(args, double*);
						gotchar = getdoublevariable(console, lf);
						if (gotchar <= 0) {
							va_end(args);
							return -10;
						}
						assignedvariables++;
						break;
					}
					if (strcmp(token, "ud") == 0) {
						unsigned int* ud = va_arg(args, unsigned int*);
						gotchar = getunsignedintvariable(console, ud);
						if (gotchar <= 0) {
							va_end(args);
							return -11;
						}
						assignedvariables++;
						break;
					}
					if (strcmp(token, "uh") == 0) {
						unsigned short* uh = va_arg(args, unsigned short*);
						gotchar = getunsignedshortvariable(console, uh);
						if (gotchar <= 0) {
							va_end(args);
							return -12;
						}
						assignedvariables++;
						break;
					}
					if (strcmp(token, "ll") == 0) {
						long long* ll = va_arg(args, long long*);
						gotchar = getlonglongvariable(console, ll);
						if (gotchar <= 0) {
							va_end(args);
							return -13;
						}
						assignedvariables++;
						break;
					}
					if (strcmp(token, "ull") == 0) {
						unsigned long long* ull = va_arg(args, unsigned long long*);
						gotchar = getunsignedlonglongvariable(console, ull);
						if (gotchar <= 0) {
							va_end(args);
							return -14;
						}
						assignedvariables++;
						break;
					}
					if (strcmp(token, "llf") == 0) {
						long double* llf = va_arg(args, long double*);
						gotchar = getlongdoublevariable(console, llf);
						if (gotchar <= 0) {
							va_end(args);
							return -15;
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
				return -16;
			}
		}

		if (*format == gotchar) {
			format++;
		}
		else {
			va_end(args);
			return -16;
		}
	}

	va_end(args);

	return assignedvariables;
}

BOOL validateformatstringforsetstringformatted(char *format) {
	if (format == NULL) {
		return FALSE;
	}

	const size_t length = strlen(format);

	BOOL openToken = FALSE;
	for (int i = 0; i < length; ++i) {
		char token = format[i];

		// variable check
		if (token == '%') {
			// take next token
			i++;
			token = format[i];
			openToken = TRUE;
			switch (token) {
				case '%':
				case 's':
				case 'd':
				case 'h':
				case 'c':
				case 'f': {
					// standard one letter tokens
					openToken = FALSE;
					break;
				}
				case 'l': {
					openToken = FALSE; // %l
					if (i + 1 < length) {
						switch (format[i + 1]) {
							case 'l': {
								i++; // %ll
								if (i + 1 < length && format[i + 1] == 'f') {
									i++; // %llf
									break;
								}
								break;
							}
							case 'f': {
								i++; // %lf
								break;
							}
							default: ;
						}
					}
					break;
				}
				case 'u': { // unsigned
					if (i + 1 < length) {
						switch (format[i + 1]) {
							case 'l': {
								openToken = FALSE; // %ul
								i++;
								if (i + 1 < length && format[i + 1] == 'l') {
									i++; // %ull
								}
								break;
							}
							case 'd':
							case 'h': {
								openToken = FALSE;
								i++; // %ud and %uh
								break;
							}
							default: ;
						}
					}
					break;
				}
				case '.': { // floating point with precision
					while (TRUE) {
						if (i + 1 < length && isdigit(format[i + 1])) {
							i++; // precision
						}
						else {
							break;
						}
					}
					if (i + 1 < length) {
						switch (format[i + 1]) {
							case 'f': {
								openToken = FALSE; // %.<int>f
								i++;
								break;
							}
							case 'l': {
								i++; // %.<int>lf
								if (i + 1 < length && format[i + 1] == 'l') {
									openToken = FALSE; // %.<int>llf
									i++;
								}
								break;
							}
							default: break;
						}
					}
					break;
				}
				default: ;
			}
		}
		else if (token == '@') {
			// style check
			i++;
			token = format[i];
			openToken = TRUE;

			// font tokens
			switch (token) {
				case 'b':
				case 'd':
				case 'i':
				case 'l':
				case 's':
				case 'c':
				case '@': {
					// standard one letter tokens
					openToken = FALSE;
					break;
				}
				case 'u': {
					openToken = FALSE;
					if (i + 1 < length && format[i + 1] == 'u') {
						i++;
					}
					break;
				}
				case 'r': {
					if (i + 1 < length) {
						i++;
						switch (format[i]) {
							case 'b':
							case 'd':
							case 'i':
							case 'l':
							case 's':{
								// standard one letter tokens remover
								openToken = FALSE;
								break;
							}
							case 'u': {
								openToken = FALSE;
								if (i + 1 < length && format[i + 1] == 'u') {
									i++;
								}
								break;
							}
						}
					}
				}
				default: break;
			}

			// color tokens
			int semicolons = 0;
			while (TRUE) {
				if (i + 1 < length) {
					i++;
					if (format[i] == ';') {
						semicolons++;
						continue;
					}
					if (!isdigit(format[i])) {
						break;
					}
				}
				else {
					break;
				}
			}

			if (semicolons == 2) {
				if (format[i] == 'f' || format[i] == 'b') {
					openToken = FALSE;
					i++;
				}
				else {
					return FALSE;
				}
			}
			else {
				return FALSE;
			}
		}
		// all other tokens are good
	}
	if (openToken) {
		// format string suddenly ends with unfinished token
		return FALSE;
	}

	// format string is valid
	return TRUE;
}

int setstringformatted(struct Console* console, char *format, ...) {
	if (console == NULL) {
		return -1;
	}

	if (format == NULL) {
		return -2;
	}

	if (!validateformatstringforsetstringformatted(format)) {
		return -3;
	}

	// valid tokens sorted by length
	char* validtokens[] = {
		"%ull", "%ll", "%uh", "%ud",
		"%ul", "%c", "%h", "%d", "%l", "%s", "%%"
	};

	int tokenssize = sizeof(validtokens) / sizeof(validtokens[0]);

	va_list args;
	va_start(args, format);
	int length = 0;

	while (*format) {

		// standard printf behaviour
		if (*format == '%') {
			format++;

			// floating point numbers printing
			char* precisionstr = malloc(20 * sizeof(char));
			int precisionstrsize = 10;

			if (*format == '.') {
				// precision detected
				// float match

				format++;

				int i = 0;
				while (isdigit(*format)) {
					precisionstr[i] = *format;
					i++;
					if (i == precisionstrsize) {
						precisionstrsize *= 2;
						if (!realloc(precisionstr, precisionstrsize)) {
							// error
							free(precisionstr);
							va_end(args);
							return -4;
						}
					}
					format++;
				}
				precisionstr[i] = '\0';
			}
			else {
				precisionstr[0] = '5';
				precisionstr[1] = '\0';
			}

			// lets handle that float
			// float
			if (strncmp(format, "f", 1) == 0) {
				format++;
				float f = va_arg(args, double);

				char* formatstr = malloc((precisionstrsize + 4) * sizeof(char));
				memset(formatstr, 0, (precisionstrsize + 4) * sizeof(char));
				formatstr[0] = '%';
				formatstr[1] = '.';
				strcat(formatstr, precisionstr);
				strcat(formatstr, "f\0");

				int size = snprintf(NULL, 0, formatstr, f);
				char* memory = malloc(size * sizeof(char));
				snprintf(memory, size, formatstr, f);
				setstring(console, memory);
				free(memory);
				length += size;
				free(formatstr);
			}
			// double
			if (strncmp(format, "lf", 2) == 0) {
				format += 2;
				double lf = va_arg(args, double);

				char* formatstr = malloc((precisionstrsize + 4) * sizeof(char));
				memset(formatstr, 0, (precisionstrsize + 4) * sizeof(char));
				formatstr[0] = '%';
				formatstr[1] = '.';
				strcat(formatstr, precisionstr);
				strcat(formatstr, "lf\0");

				int size = snprintf(NULL, 0, formatstr, lf);
				char* memory = malloc(size * sizeof(char));
				snprintf(memory, size, formatstr, lf);
				setstring(console, memory);
				free(memory);
				length += size;
				free(formatstr);
			}
			// long double
			if (strncmp(format, "llf", 3) == 0) {
				format += 3;
				long double llf = va_arg(args, long double);

				char* formatstr = malloc((precisionstrsize + 4) * sizeof(char));
				memset(formatstr, 0, (precisionstrsize + 4) * sizeof(char));
				formatstr[0] = '%';
				formatstr[1] = '.';
				strcat(formatstr, precisionstr);
				strcat(formatstr, "Lf\0");

				int size = snprintf(NULL, 0, formatstr, llf);
				char* memory = malloc(size * sizeof(char));
				snprintf(memory, size, formatstr, llf);
				setstring(console, memory);
				free(memory);
				length += size;
				free(formatstr);
			}
			free(precisionstr);

			// standard tokens
			for (int i = 0; i < tokenssize; i++) {
				char* token = validtokens[i] + 1;
				size_t tokensize = strlen(token);
				if (strncmp(format, token, tokensize) == 0) {
					// match
					format += tokensize;
					if (strcmp(token, "%") == 0) {
						setchar(console, '%');
						length++;
						break;
					}
					if (strcmp(token, "s") == 0) {
						char* s = va_arg(args, char*);
						setstring(console, s);
						length += (int)strlen(s);
						break;
					}
					if (strcmp(token, "l") == 0) {
						long l = va_arg(args, long);
						int len = snprintf(NULL, 0, "%ld", l);
						char* longstring = malloc((len+1) * sizeof(char));
						snprintf(longstring, len+1, "%ld", l);
						setstring(console, longstring);
						length += (int)strlen(longstring);
						free(longstring);
						break;
					}
					if (strcmp(token, "d") == 0) {
						int d = va_arg(args, int);
						int len = snprintf(NULL, 0, "%d", d);
						char* intstr = malloc((len+1) * sizeof(char));
						snprintf(intstr, len+1, "%d", d);
						setstring(console, intstr);
						length += (int)strlen(intstr);
						free(intstr);
						break;
					}
					if (strcmp(token, "h") == 0) {
						short h = va_arg(args, int);
						int len = snprintf(NULL, 0, "%hd", h);
						char* shortstr = malloc((len+1) * sizeof(char));
						snprintf(shortstr, len+1, "%hd", h);
						setstring(console, shortstr);
						length += (int)strlen(shortstr);
						free(shortstr);
						break;
					}
					if (strcmp(token, "c") == 0) {
						const char c = va_arg(args, int);
						setchar(console, c);
						length++;
						break;
					}
					if (strcmp(token, "ul") == 0) {
						unsigned long ul = va_arg(args, unsigned long);
						int len = snprintf(NULL, 0, "%lu", ul);
						char* unsignedlongstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedlongstr, len+1, "%lu", ul);
						setstring(console, unsignedlongstr);
						length += (int)strlen(unsignedlongstr);
						free(unsignedlongstr);
						break;
					}
					if (strcmp(token, "ud") == 0) {
						unsigned int ud = va_arg(args, unsigned int);
						int len = snprintf(NULL, 0, "%u", ud);
						char* unsignedlongstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedlongstr, len+1, "%u", ud);
						setstring(console, unsignedlongstr);
						length += (int)strlen(unsignedlongstr);
						free(unsignedlongstr);
						break;
					}
					if (strcmp(token, "uh") == 0) {
						unsigned short uh = va_arg(args, int);
						int len = snprintf(NULL, 0, "%hu", uh);
						char* unsignedshortstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedshortstr, len+1, "%hu", uh);
						setstring(console, unsignedshortstr);
						length += (int)strlen(unsignedshortstr);
						free(unsignedshortstr);
						break;
					}
					if (strcmp(token, "ll") == 0) {
						long long ll = va_arg(args, long long);
						int len = snprintf(NULL, 0, "%lld", ll);
						char* unsignedlongstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedlongstr, len+1, "%lld", ll);
						setstring(console, unsignedlongstr);
						length += (int)strlen(unsignedlongstr);
						free(unsignedlongstr);
						break;
					}
					if (strcmp(token, "ull") == 0) {
						unsigned long long ull = va_arg(args, unsigned long long );
						int len = snprintf(NULL, 0, "%llu", ull);
						char* unsignedlongstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedlongstr, len+1, "%llu", ull);
						setstring(console, unsignedlongstr);
						length += (int)strlen(unsignedlongstr);
						free(unsignedlongstr);
						break;
					}
				}
			}
			continue;
		}

		// font styles
		/*
		 * %ull unsigned long long
		 * %ll long long
		 * %uh unsigned short
		 * %ud unsigned int
		 * %ul unsigned long
		 * %c char
		 * %h short
		 * %d int
		 * %l long
		 * %s string
		 * %% '%' character
		 *
		 * Possible styles
		 * @<number>;<number>;<number>f foreground color
		 * @<number>;<number>;<number>b background color
		 * @b bold
		 * @rb remove bold
		 *
		 * @d dim
		 * @rd remove dim
		 *
		 * @i italic
		 * @ri remove italic
		 *
		 * @u underline
		 * @ru remove underline
		 *
		 * @l blinking
		 * @rl remove blinking
		 *
		 * @s strikethrough
		 * @rs remove strikethrough
		 *
		 * @uu doubleunderline
		 * @ruu remove doubleunderline
		 *
		 * @c clear all
		 *
		 * @@ at character
		*/
		if (*format == '@') {
			format++;

			if (*format == '@') {
				format++;
				setchar(console, '@');
				length++;
			}
			else if (*format == 'c') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->bold = FALSE;
				console->dim = FALSE;
				console->italic = FALSE;
				console->underline = FALSE;
				console->blinking = FALSE;
				console->strikethrough = FALSE;
				console->doubleunderline = FALSE;

				console->foregroundRed = console->defaultForegroundRed;
				console->foregroundGreen = console->defaultForegroundGreen;
				console->foregroundBlue = console->defaultForegroundBlue;

				ReleaseMutex(mutexHandle);
			}
			else if (*format == 'b') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->bold = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "rb", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->bold = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (*format == 'd') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->dim = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "rd", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->dim = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (*format == 'i') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->italic = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "ri", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->italic = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (*format == 'u') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->underline = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "ru", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->underline = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (*format == 'l') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->blinking = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "rl", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->blinking = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (*format == 's') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->strikethrough = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "rs", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->strikethrough = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "uu", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->doubleunderline = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "ruu", 3) == 0) {
				format += 3;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->doubleunderline = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (isdigit(*format)) {
				int r, g, b;
				r = g = b = 0;
				r = strtol(format, &format, 10);
				format++;
				g = strtol(format, &format, 10);
				format++;
				b = strtol(format, &format, 10);
				if (*format == 'f') {
					WaitForSingleObject(mutexHandle, INFINITE);
					console->foregroundRed = r;
					console->foregroundGreen = g;
					console->foregroundBlue = b;
					ReleaseMutex(mutexHandle);
				}
				else if (*format == 'b') {
					WaitForSingleObject(mutexHandle, INFINITE);
					console->backgroundRed = r;
					console->backgroundGreen = g;
					console->backgroundBlue = b;
					ReleaseMutex(mutexHandle);
				}
				format++;
			}

			continue;
		}

		// standard non-controlling character
		setchar(console, *format);
		length++;
		format++;
	}

	va_end(args);
	return length;
}

int setstringformattedcursor(struct Console* console, int row, int col, char* format, ...) {
	if (console == NULL) {
		return -1;
	}

	if (row < 0) {
		return -2;
	}

	if (col < 0) {
		return -3;
	}

	if (format == NULL) {
		return -4;
	}

	WaitForSingleObject(mutexHandle, INFINITE);
	if (row >= console->height) {
		ReleaseMutex(mutexHandle);
		return -5;
	}
	if (col >= console->width) {
		ReleaseMutex(mutexHandle);
		return -6;
	}

	if (!validateformatstringforsetstringformatted(format)) {
		return -7;
	}

	WaitForSingleObject(mutexHandle, INFINITE);
	unsigned int cursor = console->cursor;
	console->cursor = col + row * console->width;
	ReleaseMutex(mutexHandle);

	va_list args;
	va_start(args, format);
	int length = 0;

	// valid tokens sorted by length
	char* validtokens[] = {
		"%ull", "%ll", "%uh", "%ud",
		"%ul", "%c", "%h", "%d", "%l", "%s", "%%"
	};

	int tokenssize = sizeof(validtokens) / sizeof(validtokens[0]);

	while (*format) {

		// standard printf behaviour
		if (*format == '%') {
			format++;

			// floating point numbers printing
			char* precisionstr = malloc(20 * sizeof(char));
			int precisionstrsize = 10;

			if (*format == '.') {
				// precision detected
				// float match

				format++;

				int i = 0;
				while (isdigit(*format)) {
					precisionstr[i] = *format;
					i++;
					if (i == precisionstrsize) {
						precisionstrsize *= 2;
						if (!realloc(precisionstr, precisionstrsize)) {
							// error
							free(precisionstr);
							va_end(args);
							return -8;
						}
					}
					format++;
				}
				precisionstr[i] = '\0';
			}
			else {
				precisionstr[0] = '5';
				precisionstr[1] = '\0';
			}

			// lets handle that float
			// float
			if (strncmp(format, "f", 1) == 0) {
				format++;
				float f = va_arg(args, double);

				char* formatstr = malloc((precisionstrsize + 4) * sizeof(char));
				memset(formatstr, 0, (precisionstrsize + 4) * sizeof(char));
				formatstr[0] = '%';
				formatstr[1] = '.';
				strcat(formatstr, precisionstr);
				strcat(formatstr, "f\0");

				int size = snprintf(NULL, 0, formatstr, f);
				char* memory = malloc(size * sizeof(char));
				snprintf(memory, size, formatstr, f);
				setstring(console, memory);
				free(memory);
				length += size;
				free(formatstr);
			}
			// double
			if (strncmp(format, "lf", 2) == 0) {
				format += 2;
				double lf = va_arg(args, double);

				char* formatstr = malloc((precisionstrsize + 4) * sizeof(char));
				memset(formatstr, 0, (precisionstrsize + 4) * sizeof(char));
				formatstr[0] = '%';
				formatstr[1] = '.';
				strcat(formatstr, precisionstr);
				strcat(formatstr, "lf\0");

				int size = snprintf(NULL, 0, formatstr, lf);
				char* memory = malloc(size * sizeof(char));
				snprintf(memory, size, formatstr, lf);
				setstring(console, memory);
				free(memory);
				length += size;
				free(formatstr);
			}
			// long double
			if (strncmp(format, "llf", 3) == 0) {
				format += 3;
				long double llf = va_arg(args, long double);

				char* formatstr = malloc((precisionstrsize + 4) * sizeof(char));
				memset(formatstr, 0, (precisionstrsize + 4) * sizeof(char));
				formatstr[0] = '%';
				formatstr[1] = '.';
				strcat(formatstr, precisionstr);
				strcat(formatstr, "Lf\0");

				int size = snprintf(NULL, 0, formatstr, llf);
				char* memory = malloc(size * sizeof(char));
				snprintf(memory, size, formatstr, llf);
				setstring(console, memory);
				free(memory);
				length += size;
				free(formatstr);
			}
			free(precisionstr);

			// standard tokens
			for (int i = 0; i < tokenssize; i++) {
				char* token = validtokens[i] + 1;
				size_t tokensize = strlen(token);
				if (strncmp(format, token, tokensize) == 0) {
					// match
					format += tokensize;
					if (strcmp(token, "%") == 0) {
						setchar(console, '%');
						length++;
						break;
					}
					if (strcmp(token, "s") == 0) {
						char* s = va_arg(args, char*);
						setstring(console, s);
						length += (int)strlen(s);
						break;
					}
					if (strcmp(token, "l") == 0) {
						long l = va_arg(args, long);
						int len = snprintf(NULL, 0, "%ld", l);
						char* longstring = malloc((len+1) * sizeof(char));
						snprintf(longstring, len+1, "%ld", l);
						setstring(console, longstring);
						length += (int)strlen(longstring);
						free(longstring);
						break;
					}
					if (strcmp(token, "d") == 0) {
						int d = va_arg(args, int);
						int len = snprintf(NULL, 0, "%d", d);
						char* intstr = malloc((len+1) * sizeof(char));
						snprintf(intstr, len+1, "%d", d);
						setstring(console, intstr);
						length += (int)strlen(intstr);
						free(intstr);
						break;
					}
					if (strcmp(token, "h") == 0) {
						short h = va_arg(args, int);
						int len = snprintf(NULL, 0, "%hd", h);
						char* shortstr = malloc((len+1) * sizeof(char));
						snprintf(shortstr, len+1, "%hd", h);
						setstring(console, shortstr);
						length += (int)strlen(shortstr);
						free(shortstr);
						break;
					}
					if (strcmp(token, "c") == 0) {
						const char c = va_arg(args, int);
						setchar(console, c);
						length++;
						break;
					}
					if (strcmp(token, "ul") == 0) {
						unsigned long ul = va_arg(args, unsigned long);
						int len = snprintf(NULL, 0, "%lu", ul);
						char* unsignedlongstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedlongstr, len+1, "%lu", ul);
						setstring(console, unsignedlongstr);
						length += (int)strlen(unsignedlongstr);
						free(unsignedlongstr);
						break;
					}
					if (strcmp(token, "ud") == 0) {
						unsigned int ud = va_arg(args, unsigned int);
						int len = snprintf(NULL, 0, "%u", ud);
						char* unsignedlongstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedlongstr, len+1, "%u", ud);
						setstring(console, unsignedlongstr);
						length += (int)strlen(unsignedlongstr);
						free(unsignedlongstr);
						break;
					}
					if (strcmp(token, "uh") == 0) {
						unsigned short uh = va_arg(args, int);
						int len = snprintf(NULL, 0, "%hu", uh);
						char* unsignedshortstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedshortstr, len+1, "%hu", uh);
						setstring(console, unsignedshortstr);
						length += (int)strlen(unsignedshortstr);
						free(unsignedshortstr);
						break;
					}
					if (strcmp(token, "ll") == 0) {
						long long ll = va_arg(args, long long);
						int len = snprintf(NULL, 0, "%lld", ll);
						char* unsignedlongstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedlongstr, len+1, "%lld", ll);
						setstring(console, unsignedlongstr);
						length += (int)strlen(unsignedlongstr);
						free(unsignedlongstr);
						break;
					}
					if (strcmp(token, "ull") == 0) {
						unsigned long long ull = va_arg(args, unsigned long long );
						int len = snprintf(NULL, 0, "%llu", ull);
						char* unsignedlongstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedlongstr, len+1, "%llu", ull);
						setstring(console, unsignedlongstr);
						length += (int)strlen(unsignedlongstr);
						free(unsignedlongstr);
						break;
					}
				}
			}
			continue;
		}

		// font styles
		/*
		 * %ull unsigned long long
		 * %ll long long
		 * %uh unsigned short
		 * %ud unsigned int
		 * %ul unsigned long
		 * %c char
		 * %h short
		 * %d int
		 * %l long
		 * %s string
		 * %% '%' character
		 *
		 * Possible styles
		 * @<number>;<number>;<number>f foreground color
		 * @<number>;<number>;<number>b background color
		 * @b bold
		 * @rb remove bold
		 *
		 * @d dim
		 * @rd remove dim
		 *
		 * @i italic
		 * @ri remove italic
		 *
		 * @u underline
		 * @ru remove underline
		 *
		 * @l blinking
		 * @rl remove blinking
		 *
		 * @s strikethrough
		 * @rs remove strikethrough
		 *
		 * @uu doubleunderline
		 * @ruu remove doubleunderline
		 *
		 * @c clear all
		 *
		 * @@ at character
		*/
		if (*format == '@') {
			format++;

			if (*format == '@') {
				format++;
				setchar(console, '@');
				length++;
			}
			else if (*format == 'c') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->bold = FALSE;
				console->dim = FALSE;
				console->italic = FALSE;
				console->underline = FALSE;
				console->blinking = FALSE;
				console->strikethrough = FALSE;
				console->doubleunderline = FALSE;

				console->foregroundRed = console->defaultForegroundRed;
				console->foregroundGreen = console->defaultForegroundGreen;
				console->foregroundBlue = console->defaultForegroundBlue;

				ReleaseMutex(mutexHandle);
			}
			else if (*format == 'b') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->bold = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "rb", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->bold = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (*format == 'd') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->dim = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "rd", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->dim = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (*format == 'i') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->italic = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "ri", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->italic = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (*format == 'u') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->underline = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "ru", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->underline = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (*format == 'l') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->blinking = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "rl", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->blinking = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (*format == 's') {
				format++;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->strikethrough = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "rs", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->strikethrough = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "uu", 2) == 0) {
				format += 2;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->doubleunderline = TRUE;
				ReleaseMutex(mutexHandle);
			}
			else if (strncmp(format, "ruu", 3) == 0) {
				format += 3;
				WaitForSingleObject(mutexHandle, INFINITE);
				console->doubleunderline = FALSE;
				ReleaseMutex(mutexHandle);
			}
			else if (isdigit(*format)) {
				int r, g, b;
				r = g = b = 0;
				r = strtol(format, &format, 10);
				format++;
				g = strtol(format, &format, 10);
				format++;
				b = strtol(format, &format, 10);
				if (*format == 'f') {
					WaitForSingleObject(mutexHandle, INFINITE);
					console->foregroundRed = r;
					console->foregroundGreen = g;
					console->foregroundBlue = b;
					ReleaseMutex(mutexHandle);
				}
				else if (*format == 'b') {
					WaitForSingleObject(mutexHandle, INFINITE);
					console->backgroundRed = r;
					console->backgroundGreen = g;
					console->backgroundBlue = b;
					ReleaseMutex(mutexHandle);
				}
				format++;
			}

			continue;
		}

		// standard non-controlling character
		setchar(console, *format);
		length++;
		format++;
	}

	va_end(args);

	WaitForSingleObject(mutexHandle, INFINITE);
	console->cursor = cursor;
	ReleaseMutex(mutexHandle);

	return length;
}

int getstring(struct Console* console, char *buffer, size_t size) {
	if (console == NULL) {
		return -1;
	}

	if (buffer == NULL) {
		return -2;
	}

	WaitForSingleObject(mutexHandle, INFINITE);
	unsigned int timeout = console->blockTimeout;
	ReleaseMutex(mutexHandle);

	if (timeout <= 0) {
		int i = 0;
		while (i != size - 1) {
			WaitForSingleObject(mutexHandle, INFINITE);
			char c = dequeue(console->inputQueue);
			ReleaseMutex(mutexHandle);

			if (c == '\n' || c == '\r') {
				break;
			}
			if (c != 0 && isprint(c)) {
				buffer[i] = c;
				i++;
			}
		}
		buffer[i] = '\0';
		return i;
	}

	DWORD elapsedtimemiliseconds = 0;
	int i = 0;
	while (i != size - 1 && elapsedtimemiliseconds < timeout) {
		DWORD start = GetTickCount();

		struct GetPureCharThreadArgs args;
		args.console = console;
		args.readChar = 0;
		HANDLE thread = CreateThread(NULL, 0, puregetcharthread, &args, 0, NULL);

		DWORD result = WaitForSingleObject(thread, timeout - elapsedtimemiliseconds);
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
	return i;
}

int getstringbuffer(struct Console* console, char *buffer, size_t size) {
	if (console == NULL) {
		return -1;
	}

	if (buffer == NULL) {
		return -2;
	}

	WaitForSingleObject(mutexHandle, INFINITE);
	const unsigned int timeout = console->blockTimeout;
	ReleaseMutex(mutexHandle);

	if (timeout <= 0) {
		int i = 0;
		while (i != size - 1) {
			WaitForSingleObject(mutexHandle, INFINITE);
			char c = dequeue(console->inputQueue);
			ReleaseMutex(mutexHandle);

			if (c != 0 && isprint(c)) {
				buffer[i] = c;
				i++;
			}
		}
		buffer[i] = '\0';
		return i;
	}

	DWORD elapsedtimemiliseconds = 0;
	int i = 0;
	while (i != size - 1 && elapsedtimemiliseconds < timeout) {
		DWORD start = GetTickCount();

		struct GetPureCharThreadArgs args;
		args.console = console;
		args.readChar = 0;
		HANDLE thread = CreateThread(NULL, 0, puregetcharthread, &args, 0, NULL);

		const DWORD result = WaitForSingleObject(thread, timeout - elapsedtimemiliseconds);
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
	if (console == NULL) {
		return -1;
	}
	if (string == NULL) {
		return -2;
	}
	const size_t size = strlen(string);
	for (int i = 0; i < size; ++i) {
		setchar(console, string[i]);
	}
	return 0;
}

int setstringcursor(struct Console* console, char *string, int row, int col) {
	if (console == NULL) {
		return -1;
	}

	if (row < 0) {
		return -2;
	}

	if (col < 0) {
		return -3;
	}

	if (string == NULL) {
		return -4;
	}

	WaitForSingleObject(mutexHandle, INFINITE);
	if (row >= console->height) {
		ReleaseMutex(mutexHandle);
		return -5;
	}
	if (col > console->width) {
		ReleaseMutex(mutexHandle);
		return -6;
	}

	const unsigned int cursor = console->cursor;
	console->cursor = col + row * console->width;
	ReleaseMutex(mutexHandle);

	setstring(console, string);

	WaitForSingleObject(mutexHandle, INFINITE);
	console->cursor = cursor;
	ReleaseMutex(mutexHandle);

	return 0;
}

int clear(struct Console* console) {
	if (console == NULL) {
		return -1;
	}

	WaitForSingleObject(mutexHandle, INFINITE);

	for (int i = 0; i < console->height * console->width; ++i) {
		console->buffer[i].data = console->defaultchar;
		console->buffer[i].foregroundRed = console->defaultForegroundRed;
		console->buffer[i].foregroundGreen = console->defaultForegroundGreen;
		console->buffer[i].foregroundBlue = console->defaultForegroundBlue;
		console->buffer[i].backgroundRed = console->defaultBackgroundRed;
		console->buffer[i].backgroundGreen = console->defaultBackgroundGreen;
		console->buffer[i].backgroundBlue = console->defaultBackgroundBlue;
	}
	console->cursor = 0;
	ReleaseMutex(mutexHandle);
	return 0;
}

int fill(struct Console* console, char c, unsigned int foregroundRed, unsigned int foregroundGreen, unsigned int foregroundBlue, unsigned int backgroundRed, unsigned int backgroundGreen, unsigned int backgroundBlue) {
	if (console == NULL) {
		return -1;
	}

	if (foregroundRed > 255) {
		return -2;
	}

	if (foregroundGreen > 255) {
		return -3;
	}

	if (foregroundBlue > 255) {
		return -4;
	}

	if (backgroundRed > 255) {
		return -5;
	}

	if (backgroundGreen > 255) {
		return -6;
	}

	if (backgroundBlue > 255) {
		return -7;
	}

	WaitForSingleObject(mutexHandle, INFINITE);

	for (int i = 0; i < console->height * console->width; ++i) {
		console->buffer[i].data = c;
		console->buffer[i].foregroundRed = foregroundRed;
		console->buffer[i].foregroundGreen = foregroundGreen;
		console->buffer[i].foregroundBlue = foregroundBlue;
		console->buffer[i].backgroundRed = backgroundRed;
		console->buffer[i].backgroundGreen = backgroundGreen;
		console->buffer[i].backgroundBlue = backgroundBlue;
	}

	console->cursor = 0;

	ReleaseMutex(mutexHandle);

	return 0;
}

int fillchar(struct Console* console, char c) {
	if (console == NULL) {
		return -1;
	}

	WaitForSingleObject(mutexHandle, INFINITE);
	for (int i = 0; i < console->height * console->width; ++i) {
		console->buffer[i].data = c;
		console->buffer[i].foregroundRed = console->defaultForegroundRed;
		console->buffer[i].foregroundGreen = console->defaultForegroundGreen;
		console->buffer[i].foregroundBlue = console->defaultForegroundBlue;
		console->buffer[i].backgroundRed = console->defaultBackgroundRed;
		console->buffer[i].backgroundGreen = console->defaultBackgroundGreen;
		console->buffer[i].backgroundBlue = console->defaultBackgroundBlue;
	}
	console->cursor = 0;
	ReleaseMutex(mutexHandle);

	return 0;
}

int set2darray(struct Console* console, char* array, unsigned int row, unsigned int col, unsigned int width, unsigned int height) {
	if (console == NULL) {
		return -1;
	}

	if (array == NULL) {
		return -2;
	}

	if (height == 0) {
		return -3;
	}

	if (width == 0) {
		return -4;
	}

	WaitForSingleObject(mutexHandle, INFINITE);
	if (height + row > console->height) {
		ReleaseMutex(mutexHandle);
		return -5;
	}

	if (width + col > console->width) {
		ReleaseMutex(mutexHandle);
		return -6;
	}

	ReleaseMutex(mutexHandle);
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
	WaitForSingleObject(mutexHandle, INFINITE);
	if (row >= console->height) {
		ReleaseMutex(mutexHandle);
		return -2;
	}
	if (col >= console->width) {
		ReleaseMutex(mutexHandle);
		return -3;
	}

	console->cursor = col + row * console->width;
	ReleaseMutex(mutexHandle);
	return 0;
}

int getcursorposition(struct Console* console, unsigned int *row, unsigned int *col) {
	if (console == NULL) {
		return -1;
	}
	if (row == NULL) {
		return -2;
	}
	if (col == NULL) {
		return -3;
	}

	WaitForSingleObject(mutexHandle, INFINITE);
	*row = console->cursor / console->width;
	*col = console->cursor % console->width;
	ReleaseMutex(mutexHandle);
	return 0;
}

int refresh(struct Console* console) {
	// TODO remember about updating (specially buffer size)

	if (console == NULL) {
		return -1;
	}

	// init
	WaitForSingleObject(mutexHandle, INFINITE);
	/*
	 * console->width * console->height => characters
	 * (19 + 19 + 1) => colors
	 * (5 * 7) => font
	 * console->height => last row
	 * 6 => clear
	*/
	unsigned int bufferSize = console->width * console->height * (19 + 19 + 1) * (5 * 7) + console->height + 6;
	ReleaseMutex(mutexHandle);
	char *outputBuffer = malloc(bufferSize);
	if (outputBuffer == NULL) {
		return -2;
	}
	memset(outputBuffer, 0, bufferSize);
	int place = 0;

	// clear
	char buff[6];
	int add = sprintf(buff, "\x1B[1;1f");
	memcpy(&outputBuffer[place], buff, add);
	place += add;

	unsigned int foregroundRed = 255;
	unsigned int foregroundGreen = 255;
	unsigned int foregroundBlue = 255;
	unsigned int backgroundRed = 0;
	unsigned int backgroundGreen = 0;
	unsigned int backgroundBlue = 0;

	BOOL bold = FALSE;
	BOOL italic = FALSE;
	BOOL dim = FALSE;
	BOOL underline = FALSE;
	BOOL blinking = FALSE;
	BOOL strikethrough = FALSE;
	BOOL doubleunderline = FALSE;

	//char* position = "\x1B[%d;%dH";
	char position[100];



	WaitForSingleObject(mutexHandle, INFINITE);
	for (int i = 0; i < console->height * console->width; ++i) {
		if (i > 0 && i % console->width == 0) {
			outputBuffer[place] = '\n';
			place++;
		}

		const struct Cell cell = console->buffer[i];

		if (bold != cell.bold) {
			if (cell.bold == TRUE) {
				bold = TRUE;
				char boldstr[4] = "\x1B[1m";
				memcpy(&outputBuffer[place], boldstr, 4);
				place += 4;
			}
			else {
				bold = FALSE;
				char boldstr[5] = "\x1B[22m";
				memcpy(&outputBuffer[place], boldstr, 5);
				place += 5;
			}
		}

		if (dim != cell.dim) {
			if (cell.dim == TRUE) {
				dim = TRUE;
				char dimstr[4] = "\x1B[2m";
				memcpy(&outputBuffer[place], dimstr, 4);
				place += 4;
			}
			else {
				dim = FALSE;
				char dimstr[5] = "\x1B[22m";
				memcpy(&outputBuffer[place], dimstr, 5);
				place += 5;
			}
		}

		if (underline != cell.underline) {
			if (cell.underline == TRUE) {
				underline = TRUE;
				char underlinestr[4] = "\x1B[4m";
				memcpy(&outputBuffer[place], underlinestr, 4);
				place += 4;
			}
			else {
				underline = FALSE;
				char underlinestr[5] = "\x1B[24m";
				memcpy(&outputBuffer[place], underlinestr, 5);
				place += 5;
			}
		}

		if (blinking != cell.blinking) {
			if (cell.blinking == TRUE) {
				blinking = TRUE;
				char blinkingstr[4] = "\x1B[5m";
				memcpy(&outputBuffer[place], blinkingstr, 4);
				place += 4;
			}
			else {
				blinking = FALSE;
				char blinkingstr[5] = "\x1B[25m";
				memcpy(&outputBuffer[place], blinkingstr, 5);
				place += 5;
			}
		}

		if (strikethrough != cell.strikethrough) {
			if (cell.strikethrough == TRUE) {
				strikethrough = TRUE;
				char strikethroughstr[4] = "\x1B[9m";
				memcpy(&outputBuffer[place], strikethroughstr, 4);
				place += 4;
			}
			else {
				strikethrough = FALSE;
				char strikethroughstr[5] = "\x1B[29m";
				memcpy(&outputBuffer[place], strikethroughstr, 5);
				place += 5;
			}
		}

		if (doubleunderline != cell.doubleunderline) {
			if (cell.doubleunderline == TRUE) {
				doubleunderline = TRUE;
				char doubleunderlinestr[5] = "\x1B[21m";
				memcpy(&outputBuffer[place], doubleunderlinestr, 5);
				place += 5;
			}
			else {
				doubleunderline = FALSE;
				char doubleunderlinestr[5] = "\x1B[24m";
				memcpy(&outputBuffer[place], doubleunderlinestr, 5);
				place += 5;
			}
		}

		if (italic != cell.italic) {
			if (cell.italic == TRUE) {
				italic = TRUE;
				char italicstr[4] = "\x1B[3m";
				memcpy(&outputBuffer[place], italicstr, 4);
				place += 4;
			}
			else {
				italic = FALSE;
				char italicstr[5] = "\x1B[23m";
				memcpy(&outputBuffer[place], italicstr, 5);
				place += 5;
			}
		}

		if (foregroundRed != cell.foregroundRed || foregroundGreen != cell.foregroundGreen || foregroundBlue != cell.foregroundBlue) {
			foregroundRed = cell.foregroundRed;
			foregroundGreen = cell.foregroundGreen;
			foregroundBlue = cell.foregroundBlue;
			char colorbuff[19];
			add = sprintf(colorbuff, "\x1B[38;2;%d;%d;%dm", foregroundRed, foregroundGreen, foregroundBlue);
			memcpy(&outputBuffer[place], colorbuff, add);
			place += add;
		}
		if (backgroundRed != cell.backgroundRed || backgroundGreen != cell.backgroundGreen || backgroundBlue != cell.backgroundBlue) {
			backgroundRed = cell.backgroundRed;
			backgroundGreen = cell.backgroundGreen;
			backgroundBlue = cell.backgroundBlue;
			char colorbuff[19];
			add = sprintf(colorbuff, "\x1B[48;2;%d;%d;%dm", backgroundRed, backgroundGreen, backgroundBlue);
			memcpy(&outputBuffer[place], colorbuff, add);
			place += add;
		}
		outputBuffer[place] = cell.data;
		place++;
	}
	outputBuffer[place] = '\0';
	WriteConsoleA(console->outputHandle, outputBuffer, strlen(outputBuffer), NULL, NULL);

	unsigned int row = console->cursor / console->width + 1;
	unsigned int col = console->cursor % console->width + 1;
	sprintf(position, "\x1B[%d;%dH", row, col);
	WriteConsoleA(console->outputHandle, position, strlen(position), NULL, NULL);

	ReleaseMutex(mutexHandle);
	free(outputBuffer);
	return 0;
}
