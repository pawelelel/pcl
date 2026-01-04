//
// Created by pawel on 14.05.2025.
//

#include "pcl.h"

#include <math.h>
#include <stdio.h>

#include "queue.h"


HANDLE mutexHandle;


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
					enqueue(console->inputQueue, &lpBuffer[0].Event.KeyEvent.uChar.AsciiChar);
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





				/*
				// TODO implemet resizing
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

				/*
				 * console->width * console->height => characters
				 * (19 + 19 + 1) => colors
				 * (5 * 7) => font
				 * console->height => last row
				 * 6 => clear
				*/



				/*
				console->bufferSize = width * height * (19 + 19 + 1) * (5 * 7) + height + 6;

				free(console->outputBuffer);
				console->outputBuffer = malloc(console->bufferSize);
				if (console->outputBuffer == NULL) {
					// error
				}

				console->width = width;
				console->height = height;
				console->cursor = 0;

*/








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

	console->height = info.dwSize.Y;
	console->width = info.dwSize.X;

	console->asciiScreens = malloc(sizeof(struct AsciiScreen*) * 1);
	console->asciiScreensIndex = 0;
	console->unicodeScreens = malloc(sizeof(struct UnicodeScreen*) * 1);
	console->unicodeScreensIndex = 0;

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

	// TODO free struct console properly
	// also do not foreget about screeens
	free(console);

	printf("\x1B[1;1f");
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

unsigned int gettimeout(struct Console *console) {
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

/**
 * pure ReadConsoleInput() wrapper without any non-blocking or timeout features
 *
 * @param console pointer to struct Console
 * @return char from console input buffer
 */
char puregetchar(struct Console* console) {
	while (1) {
		WaitForSingleObject(mutexHandle, INFINITE);

		const char c = (const char)dequeue(console->inputQueue);
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
		const char c = (const char)dequeue(console->inputQueue);
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
			char c = (char)dequeue(console->inputQueue);
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
			char c = (char)dequeue(console->inputQueue);
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
