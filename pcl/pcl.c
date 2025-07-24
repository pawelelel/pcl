//
// Created by pawel on 14.05.2025.
//

#include <pcl.h>
#include <stdio.h>
#include <time.h>

struct Console* start(void) {
	//TODO implement error handling
	//TODO implement saving console state to restore it back at end() function


	struct Console* console = malloc(sizeof(struct Console));
	console->inputHandle = GetStdHandle(STD_INPUT_HANDLE);

	console->currentOutput = 1;
	console->outputHandle1 = GetStdHandle(STD_OUTPUT_HANDLE);
	console->outputHandle2 = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	int width, height;
	getdimensions(console, &width, &height);
	CHAR_INFO* buffer = malloc(width * height * sizeof(CHAR_INFO));
	COORD coordsize;
	coordsize.X = (short)width;
	coordsize.Y = (short)height;
	COORD coordtop;
	coordtop.X = 0;
	coordtop.Y = 0;
	SMALL_RECT rect;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = (short)(width - 1);
	rect.Bottom = (short)(height - 1);
	BOOL result = ReadConsoleOutput(console->outputHandle1, buffer, coordsize, coordtop, &rect);
	if (!result) {
		return NULL;
	}
	BOOL result2 = WriteConsoleOutput(console->outputHandle2, buffer, coordsize, coordtop, &rect);
	if (!result2) {
		return NULL;
	}
	free(buffer);

	console->errorHandle = GetStdHandle(STD_ERROR_HANDLE);
	console->windowHandle = GetCurrentProcess();
	console->blockInput = TRUE;
	console->blockTimeout = 0;
	return console;
}

void end(struct Console* console) {
	//TODO implement error handling


	CloseHandle(console->inputHandle);
	CloseHandle(console->outputHandle1);
	CloseHandle(console->outputHandle2);
	CloseHandle(console->errorHandle);
	free(console);
}

int setinputblock(struct Console *console, BOOL blockinput) {
	if (console == NULL) {
		return -1;
	}
	console->blockInput = blockinput;
	return 0;
}

int getinputblock(const struct Console *console) {
	return console->blockInput;
}

int settimeout(struct Console *console, int timeout) {
	//TODO implement error handling in docs
	if (console == NULL) {
		return -1;
	}
	if (timeout < 0) {
		return -2;
	}
	console->blockTimeout = timeout;
	return 0;
}

int gettimeout(const struct Console *console) {
	//TODO implement error handling
	return console->blockTimeout;
}

char* gettitle(struct Console* console) {
	//TODO implement error handling


	char* title = malloc(MAX_PATH * sizeof(char));
	GetConsoleTitleA(title, MAX_PATH);
	return title;
}

int settitle(struct Console* console, const char* title) {
	//TODO implement error handling


	if(!SetConsoleTitleA(title))
	{
		return 0;
	}
	return 1;
}

int getdimensions(const struct Console* console, int* width, int* height) {
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
	if (console->outputHandle1 == INVALID_HANDLE_VALUE || console->outputHandle2 == INVALID_HANDLE_VALUE) {
		*width = 0;
		*height = 0;
		return -4;
	}

	HANDLE h = NULL;
	switch (console->currentOutput) {
		case 1: {
			h = console->outputHandle2;
			break;
		}
		case 2: {
			h = console->outputHandle1;
			break;
		}
		default: break;
	}

	CONSOLE_SCREEN_BUFFER_INFO info;
	if (GetConsoleScreenBufferInfo(h, &info) == 0) {
		*width = 0;
		*height = 0;
		return -5;
	}

	*width = info.dwSize.X;
	*height = info.dwSize.Y;
	return 1;
}

/**
 * pure ReadConsoleInput() wrapper without any non-blocking or timeout features
 * but TODO: checks console input buffer for other than keyboard events and fires them
 *
 * @param console pointer to struct Console
 * @return char from console input buffer
 */
char puregetchar(const struct Console* console) {
	DWORD read;
	INPUT_RECORD keyboard[1];

	while (1) {
		const int result = ReadConsoleInput(console->inputHandle, keyboard, 1, &read);

		if (result == 0 || read != 1) {
			// error ReadConsoleInput failed
			return -1;
		}
		if (keyboard[0].EventType == KEY_EVENT && keyboard[0].Event.KeyEvent.bKeyDown) {
			const char c = keyboard[0].Event.KeyEvent.uChar.AsciiChar;
			return c;
		}
	}
}

char getchr(const struct Console* console) {
	//TODO implement error handling
	//TODO add virtual key codes for arrows, special keys etc.
	// TODO add unicode
	// TODO "events" in struct console pointer to function when resize or click event occurs


	if (!console->blockInput) { // non-blocking input behaviour
		INPUT_RECORD lpBuffer[1];
		DWORD read;
		BOOL b = PeekConsoleInput(console->inputHandle, lpBuffer, 1, &read);
		if (b == 0) {
			return -1;
		}
		if (read == 1 && (lpBuffer[0].EventType != KEY_EVENT || !lpBuffer[0].Event.KeyEvent.bKeyDown)) {
			// peeked something but not a keybaord input we want
			// lets kick it out of buffer
			INPUT_RECORD buffer[1];
			ReadConsoleInput(console->inputHandle, buffer, 1, &read);
			return 0;
		}
		if (read == 0 || lpBuffer[0].EventType != KEY_EVENT || !lpBuffer[0].Event.KeyEvent.bKeyDown) {
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
}

void setchar(const struct Console* console, char c) {
	//TODO implement error handling

	HANDLE h = NULL;
	switch (console->currentOutput) {
		case 1: {
			h = console->outputHandle2;
			break;
		}
		case 2: {
			h = console->outputHandle1;
			break;
		}
		default: break;
	}
	WriteConsole(h, &c, 1, NULL, NULL);
}

void setcharcursor(const struct Console* console, char c, int row, int col) {
	//TODO implement error handling

	int nowrow, nowcol;
	getcursorposition(console, &nowrow, &nowcol);
	setcursorposition(console, row, col);

	HANDLE h = NULL;
	switch (console->currentOutput) {
		case 1: {
			h = console->outputHandle2;
			break;
		}
		case 2: {
			h = console->outputHandle1;
			break;
		}
		default: break;
	}
	WriteConsole(h, &c, 1, NULL, NULL);

	setcursorposition(console, nowrow, nowcol);
}

//TODO implement better error handling
int getcharvariable(const struct Console* console, char* c) {
	DWORD read;

	while (1) {
		//TODO: replace with puregetchar()
		INPUT_RECORD keyboard[1];
		const int result = ReadConsoleInput(console->inputHandle, keyboard, 1, &read);
		if (result == 0 || read != 1) {
			// error ReadConsoleInput failed
			return -1;
		}
		if (keyboard[0].EventType == KEY_EVENT && keyboard[0].Event.KeyEvent.bKeyDown) {
			*c = keyboard[0].Event.KeyEvent.uChar.AsciiChar;
			return 0;
		}
	}
}

char getsignedintkeyboardin(const struct Console* console, char** buffer, int* buffercount) {
	char ret = 0;
	INPUT_RECORD keyboard[1];
	DWORD read;

	if (*buffer == NULL) {
		*buffer = malloc(10 * sizeof(char));
		if (*buffer == NULL) {
			*buffercount = 0;
			return 0;
		}
	}
	*buffercount = 0;


	while (1) {
		//TODO: replace with puregetchar()
		const int result = ReadConsoleInput(console->inputHandle, keyboard, 1, &read);
		if (result == 0 || read != 1) {
			return -1;
		}
		if (keyboard[0].EventType == KEY_EVENT && keyboard[0].Event.KeyEvent.bKeyDown) {
			const char c = keyboard[0].Event.KeyEvent.uChar.AsciiChar;
			if (isdigit(c) || c == '-') {
				(*buffer)[(*buffercount)++] = c;
				ret = c;
				break;
			}
			ret = c;
			break;
		}
	}

	if (isdigit(ret) || ret == '-') {
		while (1) {
		//TODO: replace with puregetchar()
			if (!ReadConsoleInput(console->inputHandle, keyboard, 1, &read) || read != 1) {
				(*buffer)[*buffercount] = '\0';
				return ret;
			}
			if (keyboard[0].EventType == KEY_EVENT && keyboard[0].Event.KeyEvent.bKeyDown) {
				const char c = keyboard[0].Event.KeyEvent.uChar.AsciiChar;
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
	}

	(*buffer)[*buffercount] = '\0';
	return ret;
}

char getunsignedintkeyboardin(const struct Console* console, char** buffer, int* buffercount) {
	char ret = 0;
	DWORD read;

	if (*buffer == NULL) {
		*buffer = malloc(10 * sizeof(char));
		if (*buffer == NULL) {
			*buffercount = 0;
			return 0;
		}
	}
	*buffercount = 0;

	while (1) {
		//TODO: replace with puregetchar()
		INPUT_RECORD keyboard[1];
		if (!ReadConsoleInput(console->inputHandle, keyboard, 1, &read) || read != 1) {
			(*buffer)[*buffercount] = '\0';
			return ret;
		}
		if (keyboard[0].EventType == KEY_EVENT && keyboard[0].Event.KeyEvent.bKeyDown) {
			const char c = keyboard[0].Event.KeyEvent.uChar.AsciiChar;
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

char getshortvariable(const struct Console* console, short* h) {
	if (h == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	const char ret = getsignedintkeyboardin(console, &buffer, &buffercount);

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

char getunsignedshortvariable(const struct Console* console, unsigned short* uh) {
	if (uh == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	const char ret = getunsignedintkeyboardin(console, &buffer, &buffercount);

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

char getintvariable(const struct Console* console, int* d) {
	if (d == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	const char ret = getsignedintkeyboardin(console, &buffer, &buffercount);

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

char getunsignedintvariable(const struct Console* console, unsigned int* ud) {
	if (ud == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	const char ret = getunsignedintkeyboardin(console, &buffer, &buffercount);

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

char getlongvariable(const struct Console* console, long* ld) {
	if (ld == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	const char ret = getsignedintkeyboardin(console, &buffer, &buffercount);

	if (buffercount == 0) {
		free(buffer);
		return ret;
	}

	*ld = strtol(buffer, NULL, 10);

	free(buffer);
	return ret;
}

char getunsignedlongvariable(const struct Console* console, unsigned long* uld) {
	if (uld == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	const char ret = getunsignedintkeyboardin(console, &buffer, &buffercount);

	if (buffercount == 0) {
		free(buffer);
		return ret;
	}

	*uld = strtoul(buffer, NULL, 10);

	free(buffer);
	return ret;
}

char getlonglongvariable(const struct Console* console, long long* lld) {
	if (lld == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	const char ret = getsignedintkeyboardin(console, &buffer, &buffercount);

	if (buffercount == 0) {
		free(buffer);
		return ret;
	}

	*lld = strtoll(buffer, NULL, 10);

	free(buffer);
	return ret;
}

char getunsignedlonglongvariable(const struct Console* console, unsigned long long* ulld) {
	if (ulld == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;

	const char ret = getunsignedintkeyboardin(console, &buffer, &buffercount);

	if (buffercount == 0) {
		free(buffer);
		return ret;
	}

	*ulld = strtoull(buffer, NULL, 10);

	free(buffer);
	return ret;
}

char getfloatkeyboardin(const struct Console* console, char** buffer, int* buffercount) {
	char ret = 0;
	INPUT_RECORD keyboard[1];
	DWORD read;

	if (*buffer == NULL) {
		*buffer = malloc(10 * sizeof(char));
		if (*buffer == NULL) {
			*buffercount = 0;
			return 0;
		}
	}
	*buffercount = 0;


	while (1) {
		//TODO: replace with puregetchar()
		int result = ReadConsoleInput(console->inputHandle, keyboard, 1, &read);
		if (result == 0 || read != 1) {
			return -1;
		}
		if (keyboard[0].EventType == KEY_EVENT && keyboard[0].Event.KeyEvent.bKeyDown) {
			const char c = keyboard[0].Event.KeyEvent.uChar.AsciiChar;
			if (isdigit(c) || c == '-') {
				(*buffer)[(*buffercount)++] = c;
				ret = c;
				break;
			}
			ret = c;
			break;
		}
	}

	if (isdigit(ret) || ret == '-') {
		while (1) {
		//TODO: replace with puregetchar()
			if (!ReadConsoleInput(console->inputHandle, keyboard, 1, &read) || read != 1) {
				(*buffer)[*buffercount] = '\0';
				return ret;
			}
			if (keyboard[0].EventType == KEY_EVENT && keyboard[0].Event.KeyEvent.bKeyDown) {
				const char c = keyboard[0].Event.KeyEvent.uChar.AsciiChar;
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
	}

	(*buffer)[*buffercount] = '\0';
	return ret;
}

char getfloatvariable(const struct Console* console, float* f) {
	if (f == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;
	const char ret = getfloatkeyboardin(console, &buffer, &buffercount);
	*f = strtof(buffer, NULL);
	return ret;
}

char getdoublevariable(const struct Console* console, double* lf) {
	if (lf == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;
	const char ret = getfloatkeyboardin(console, &buffer, &buffercount);
	*lf = strtod(buffer, NULL);
	return ret;
}

char getlongdoublevariable(const struct Console* console, long double* llf) {
	if (llf == NULL) {
		return -1;
	}
	char* buffer = NULL;
	int buffercount = 0;
	const char ret = getfloatkeyboardin(console, &buffer, &buffercount);
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
int validateformatstring(const char* format, const char** validtokens, const int tokens) {
	while (*format) {
		if (*format == '%') {
			format++;
			int matched = 0;
			for (int i = 0; i < tokens; i++) {
				const char* token = validtokens[i] + 1;
				const size_t size = strlen(token);
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

int getvariables(const struct Console *console, char *format, ...) {
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
	const char* validtokens[] = {
		"%llf", "%ull", "%ll", "%uh", "%ud",
		"%lf", "%ul", "%c", "%h", "%d", "%l", "%f"
	};

	const int tokenssize = sizeof(validtokens) / sizeof(validtokens[0]);

	if (!validateformatstring(format, validtokens, tokenssize)) {
		// error
		// format string is not valid
		return -1;
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
				const char* token = validtokens[j] + 1;
				const size_t tokensize = strlen(token);
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

void setstringformatted(const struct Console* console, char *format, ...) {
	//TODO implement error handling


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
}

void setstringformattedcursor(const struct Console* console, char* format, int row, int col, ...) {
	//TODO implement error handling


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
}

typedef struct {
	const struct Console* console;
	char readChar;
} GetPureCharThreadArgs;

DWORD puregetcharthread(LPVOID lpParam) {
	GetPureCharThreadArgs* args = lpParam;

	args->readChar = puregetchar(args->console);
	return 0;
}

void getstring(const struct Console* console, char *buffer, size_t size) {
	//TODO implement error handling
	if (buffer == NULL) {
		return;
	}

	DWORD elapsedtimemiliseconds = 0;
	int i = 0;
	while (i != size - 1 && elapsedtimemiliseconds < console->blockTimeout) {
		DWORD start = GetTickCount();

		GetPureCharThreadArgs args;
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
}

void getstringbuffer(const struct Console* console, char *buffer, const size_t size) {
	//TODO implement error handling

	if (buffer == NULL) {
		return;
	}

	DWORD elapsedtimemiliseconds = 0;
	int i = 0;
	while (i != size - 1 && elapsedtimemiliseconds < console->blockTimeout) {
		DWORD start = GetTickCount();

		GetPureCharThreadArgs args;
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
}

void setstring(const struct Console* console, const char *string) {
	//TODO implement error handling

	if (console == NULL) {
		return;
	}
	if (string == NULL) {
		return;
	}
	size_t size = strlen(string);
	for (size_t i = 0; i < size; i++) {
		setchar(console, string[i]);
	}
}

void setstringcursor(const struct Console* console, const char *string, const int row, const int col) {
	//TODO implement error handling


	int nowrow, nowcol;
	getcursorposition(console, &nowrow, &nowcol);
	setcursorposition(console, row, col);

	setstring(console, string);

	setcursorposition(console, nowrow, nowcol);
}

void clear(const struct Console* console) {
	//TODO implement error handling

	fill(console, ' ');
}

void fill(const struct Console* console, const char c) {
	//TODO implement error handling

	int width = 0, height = 0;
	if (getdimensions(console, &width, &height) != 1) {
		//TODO error code
		return;
	}

	DWORD written;
	COORD topleft = {0, 0};
	HANDLE h = NULL;
	switch (console->currentOutput) {
		case 1: {
			h = console->outputHandle2;
			break;
		}
		case 2: {
			h = console->outputHandle1;
			break;
		}
		default: break;
	}
	FillConsoleOutputCharacter(h, c, width * height, topleft, &written);
	FillConsoleOutputAttribute(h, 7, width * height, topleft, &written);

	setcursorposition(console, 0, 0);
}

void set2darray(const struct Console* console, const char* array, const int row, const int col, const int width, const int height) {
	//TODO implement error handling

	HANDLE h = NULL;
	switch (console->currentOutput) {
		case 1: {
			h = console->outputHandle2;
			break;
		}
		case 2: {
			h = console->outputHandle1;
			break;
		}
		default: break;
	}

	DWORD written;

	for (int i = 0; i < width; ++i) {
		const COORD topleft = {(short)col, (short)(row + i)};
		WriteConsoleOutputCharacter(h, array, height, topleft, &written);
		array += height;
	}
}

void setcursorposition(const struct Console* console, int row, int col) {
	//TODO implement error handling


	int width = 0, height = 0;
	if (getdimensions(console, &width, &height) != 1) {
		//TODO error code
		return;
	}
	if (row >= height) {
		//TODO error code
		return;
	}
	if (col >= width) {
		//TODO error code
		return;
	}

	COORD coord;
	coord.X = (short)col;
	coord.Y = (short)row;
	HANDLE h = NULL;
	switch (console->currentOutput) {
		case 1: {
			h = console->outputHandle2;
			break;
		}
		case 2: {
			h = console->outputHandle1;
			break;
		}
		default: break;
	}
	if (SetConsoleCursorPosition(h, coord) == 0) {
		//TODO error code
		return;
	}
}

int getcursorposition(const struct Console* console, int *row, int *col) {
	if (console == NULL) {
		return -1;
	}
	if (row == NULL) {
		return -2;
	}
	if (col == NULL) {
		return -3;
	}


	HANDLE h = NULL;
	switch (console->currentOutput) {
		case 1: {
			h = console->outputHandle2;
			break;
		}
		case 2: {
			h = console->outputHandle1;
			break;
		}
		default: break;
	}
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(h, &info);

	*row = info.dwCursorPosition.Y;
	*col = info.dwCursorPosition.X;
	return 1;
}

void refresh(struct Console* console) {
	//TODO remember about updating
	//TODO implement error handling

	int row, col;
	getcursorposition(console, &row, &col);

	HANDLE h = NULL;
	int width, height;
	getdimensions(console, &width, &height);
	CHAR_INFO* buffer = malloc(width * height * sizeof(CHAR_INFO));
	COORD coordsize;
	coordsize.X = (short)width;
	coordsize.Y = (short)height;
	COORD coordtop;
	coordtop.X = 0;
	coordtop.Y = 0;
	SMALL_RECT rect;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = (short)(width - 1);
	rect.Bottom = (short)(height - 1);

	switch (console->currentOutput) {
		case 1: {
			h = console->outputHandle2;
			console->currentOutput = 2;

			BOOL result = ReadConsoleOutput(console->outputHandle2, buffer, coordsize, coordtop, &rect);
			if (!result) {
				return;
			}
			BOOL result2 = WriteConsoleOutput(console->outputHandle1, buffer, coordsize, coordtop, &rect);
			if (!result2) {
				return;
			}
			break;
		}
		case 2: {
			h = console->outputHandle1;
			console->currentOutput = 1;

			BOOL result = ReadConsoleOutput(console->outputHandle1, buffer, coordsize, coordtop, &rect);
			if (!result) {
				return;
			}
			BOOL result2 = WriteConsoleOutput(console->outputHandle2, buffer, coordsize, coordtop, &rect);
			if (!result2) {
				return;
			}
			break;
		}
		default: break;
	}

	free(buffer);

	SetConsoleActiveScreenBuffer(h);

	setcursorposition(console, row, col);
}
