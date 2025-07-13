//
// Created by pawel on 14.05.2025.
//

#include <pcl.h>
#include <stdio.h>

struct Console* start(void) {
	//TODO implement error handling


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
	console->blockInput = FALSE;
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

char getchr(const struct Console* console) {
	//TODO implement error handling

	if (console->blockInput) {
		INPUT_RECORD lpBuffer[1];
		DWORD word;
		BOOL b = PeekConsoleInput(console->inputHandle, lpBuffer, 1, &word);
		if (word == 0 || lpBuffer[0].EventType != KEY_EVENT || !lpBuffer[0].Event.KeyEvent.bKeyDown) {
			return 0;
		}
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



void getcharvariable(const struct Console* console, char* c) {
	INPUT_RECORD keyboard[1];
	DWORD read;

	while (1) {
		ReadConsoleInput(console->inputHandle, keyboard, 1, &read);
		if (keyboard[0].EventType == KEY_EVENT && keyboard[0].Event.KeyEvent.bKeyDown) {
			*c = keyboard[0].Event.KeyEvent.uChar.AsciiChar;
			return;
		}
	}
}

char getsignedintvariable(const struct Console* console, char** buffer, int* buffercount) {
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
		ReadConsoleInput(console->inputHandle, keyboard, 1, &read);
		if (keyboard[0].EventType == KEY_EVENT && keyboard[0].Event.KeyEvent.bKeyDown) {
			char c = keyboard[0].Event.KeyEvent.uChar.AsciiChar;
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
			if (!ReadConsoleInput(console->inputHandle, keyboard, 1, &read) || read == 0) {
				(*buffer)[*buffercount] = '\0';
				return ret;
			}
			if (keyboard[0].EventType == KEY_EVENT && keyboard[0].Event.KeyEvent.bKeyDown) {
				char c = keyboard[0].Event.KeyEvent.uChar.AsciiChar;
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

char getshortvariable(const struct Console* console, short* h) {
	char* buffer = NULL;
	int buffercount = 0;

	char ret = getsignedintvariable(console, &buffer, &buffercount);

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

void getunsignedshortvariable(const struct Console* console, unsigned short* h) {

}

void getintvariable(const struct Console* console, int* d) {

}

void getunsignedintvariable(const struct Console* console, unsigned int *ud) {

}

void getlongvariable(const struct Console* console, long* h) {

}

void getunsignedlongvariable(const struct Console* console, unsigned long* h) {

}

void getlonglongvariable(const struct Console* console, long long* h) {

}

void getunsignedlonglongvariable(const struct Console* console, unsigned long long* h) {

}

void getfloatvariable(const struct Console* console, float* h) {

}

void getdoublevariable(const struct Console* console, double* h) {

}

int getvariables(const struct Console *console, char *format, ...) {
	//TODO implement

	short h;
	char c = getshortvariable(console, &h);

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
	 *
	 * %[int]s string
	 */

	va_list args;
	va_start(args, format);
	const size_t size = strlen(format);

	for (int i = 0; i < size; ++i) {

	}

	va_end(args);

	return 0;
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

void getstring(const struct Console* console, char *buffer, size_t size) {
	//TODO implement error handling

	if (buffer == NULL) {
		return;
	}

	int i = 0;
	while (i != size - 1) {
		INPUT_RECORD buff[1];
		DWORD read;
		ReadConsoleInput(console->inputHandle, buff, 1, &read);

		if (buff[0].EventType == KEY_EVENT) {
			if (buff[0].Event.KeyEvent.bKeyDown) {
				char c = buff[0].Event.KeyEvent.uChar.AsciiChar;
				if (c == '\n' || c == '\r') {
					break;
				}
				if (c != 0 && isprint(c)) {
					buffer[i] = c;
					i++;
				}
			}
		}
	}
	buffer[i] = '\0';
}

void getstringbuffer(const struct Console* console, char *buffer, const size_t size) {
	//TODO implement error handling

	if (buffer == NULL) {
		return;
	}

	int i = 0;
	while (i != size - 1) {
		INPUT_RECORD buff[1];
		DWORD read;
		ReadConsoleInput(console->inputHandle, buff, 1, &read);

		if (buff[0].EventType == KEY_EVENT) {
			if (buff[0].Event.KeyEvent.bKeyDown) {
				char c = buff[0].Event.KeyEvent.uChar.AsciiChar;
				if (c != 0 && isprint(c)) {
					buffer[i] = c;
					i++;
				}
			}
		}
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
	FillConsoleOutputCharacter(h, ' ', width * height, topleft, &written);
	FillConsoleOutputAttribute(h, 7, width * height, topleft, &written);

	setcursorposition(console, 0, 0);
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
