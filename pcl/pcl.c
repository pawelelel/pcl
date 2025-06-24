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

char getchr(struct Console* console) {
	//TODO implement error handling

	INPUT_RECORD buffer[1];
	DWORD read;
	ReadConsoleInput(console->inputHandle, buffer, 1, &read);
	switch (buffer[0].EventType) {
		case KEY_EVENT: {
			return buffer[0].Event.KeyEvent.uChar.AsciiChar;
		}
		case MOUSE_EVENT: {
			//TODO think
			break;
		}
		case WINDOW_BUFFER_SIZE_EVENT: {
			//TODO implement
			break;
		}
		case FOCUS_EVENT: {
			//TODO implement
			break;
		}
		case MENU_EVENT: {
			// pass; according to docs
			//https://learn.microsoft.com/en-us/windows/console/input-record-str
			break;
		}
		default: ;
	}
	return -1;
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

void getstringformatted(struct Console* console, char *format, ...) {
	//TODO implement
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

void setstringformattedcursor(struct Console* console, char *format, int row, int col, ...) {
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

void getstring(struct Console* console, char *buffer) {
	//TODO implement
}

void getstringbuffer(struct Console* console, char *buffer, size_t size) {
	//TODO implement
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
	//TODO implement error handling


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
}
