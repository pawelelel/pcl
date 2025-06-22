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
	coordsize.X = width;
	coordsize.Y = height;
	COORD coordtop;
	coordtop.X = 0;
	coordtop.Y = 0;
	SMALL_RECT rect;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = width - 1;
	rect.Bottom = height - 1;
	BOOL result = ReadConsoleOutput(console->outputHandle1, buffer, coordsize, coordtop, &rect);
	BOOL result2 = WriteConsoleOutput(console->outputHandle2, buffer, coordsize, coordtop, &rect);
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

char* gettitle() {
	//TODO implement error handling


	char* title = malloc(MAX_PATH * sizeof(char));
	GetConsoleTitleA(title, MAX_PATH);
	return title;
}

int settitle(const char* title) {
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

char getchr(void) {
	//TODO implement
}

void setchar(const struct Console* console, char c) {
	//TODO implement error handling


	//putchar(c);
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
	}
	WriteConsole(h, &c, 1, NULL, NULL);
}

void setcharcursor(struct Console* console, char c, int row, int col) {
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
	}
	WriteConsole(h, &c, 1, NULL, NULL);

	setcursorposition(console, nowrow, nowcol - 1);
}

void getstringf(char *format, ...) {
	//TODO implement
}

void setstringf(char *format, ...) {
	//TODO implement
}

void setstringfc(char *format, int row, int col, ...) {
	//TODO implement
}

void getstring(char *buffer) {
	//TODO implement
}

void getstrings(char *buffer, size_t size) {
	//TODO implement
}

void setstring(char *string) {
	//TODO implement
}

void setstringc(char *string, int row, int col) {
	//TODO implement
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
	}
	FillConsoleOutputCharacter(h, ' ', width * height, topleft, &written);
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
	coord.X = col;
	coord.Y = row;
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
	}
	if (SetConsoleCursorPosition(h, coord) == 0) {
		//TODO error code
		return;
	}

	//TODO good code
	return;
}

int getcursorposition(const struct Console* console, int *row, int *col) {
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
	}
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(h, &info);

	*row = info.dwCursorPosition.Y;
	*col = info.dwCursorPosition.X;
	return 1;
}

void refresh(struct Console *console) {
	//TODO implement error handling


	HANDLE h = NULL;
	int width, height;
	getdimensions(console, &width, &height);
	CHAR_INFO* buffer = malloc(width * height * sizeof(CHAR_INFO));
	COORD coordsize;
	coordsize.X = width;
	coordsize.Y = height;
	COORD coordtop;
	coordtop.X = 0;
	coordtop.Y = 0;
	SMALL_RECT rect;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = width - 1;
	rect.Bottom = height - 1;

	switch (console->currentOutput) {
		case 1: {
			h = console->outputHandle2;
			console->currentOutput = 2;

			BOOL result = ReadConsoleOutput(console->outputHandle2, buffer, coordsize, coordtop, &rect);
			BOOL result2 = WriteConsoleOutput(console->outputHandle1, buffer, coordsize, coordtop, &rect);
			break;
		}
		case 2: {
			h = console->outputHandle1;
			console->currentOutput = 1;

			BOOL result = ReadConsoleOutput(console->outputHandle1, buffer, coordsize, coordtop, &rect);
			BOOL result2 = WriteConsoleOutput(console->outputHandle2, buffer, coordsize, coordtop, &rect);
			break;
		}
		default: break;
	}

	free(buffer);

	SetConsoleActiveScreenBuffer(h);
}
