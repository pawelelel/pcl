//
// Created by pawel on 14.05.2025.
//

#include <pcl.h>

struct Console* start(void) {
	struct Console* console = malloc(sizeof(struct Console));
	console->inputHandle = GetStdHandle(STD_INPUT_HANDLE);
	console->outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	console->errorHandle = GetStdHandle(STD_ERROR_HANDLE);
	console->windowHandle = GetCurrentProcess();
	return console;
}

void end(struct Console* console) {
	CloseHandle(console->inputHandle);
	CloseHandle(console->outputHandle);
	CloseHandle(console->errorHandle);
	free(console);
}

char* gettitle() {
	char* title = malloc(MAX_PATH * sizeof(char));
	GetConsoleTitleA(title, MAX_PATH);
	return title;
}

int settitle(const char* title) {
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
	if (console->outputHandle == INVALID_HANDLE_VALUE) {
		*width = 0;
		*height = 0;
		return -4;
	}

	CONSOLE_SCREEN_BUFFER_INFO info;
	if (GetConsoleScreenBufferInfo(console->outputHandle, &info) == 0) {
		*width = 0;
		*height = 0;
		return -5;
	}

	*width = info.dwSize.X;
	*height = info.dwSize.Y;
	return 0;
}

char getchar(void) {
}

void setchar(char c) {
}

void setcharc(char c, int row, int col) {
}

void scanf(char *format, ...) {
}

void setstringf(char *format, ...) {
}

void setstringfc(char *format, int row, int col, ...) {
}

void getstring(char *buffer) {
}

void getstrings(char *buffer, size_t size) {
}

void setstring(char *string) {
}

void setstringc(char *string, int row, int col) {
}

void clear() {
}

void setcursorposition(int row, int col) {
}
