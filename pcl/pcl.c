//
// Created by pawel on 14.05.2025.
//

#include <pcl.h>
#include <stdio.h>
#include <windows.h>

struct Console* Start(void) {
	struct Console* console = malloc(sizeof(struct Console));
	console->inputHandle = GetStdHandle(STD_INPUT_HANDLE);
	console->outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	console->errorHandle = GetStdHandle(STD_ERROR_HANDLE);
	console->windowHandle = GetCurrentProcess();
	return console;
}

/**
 * Ends a console window
 *
 * @param console pointer to struct Console
 */
void End(struct Console* console) {
	CloseHandle(console->inputHandle);
	CloseHandle(console->outputHandle);
	CloseHandle(console->errorHandle);
	free(console);
}

/**
 * @returns char* a pointer to name of console
 */
char* GetTitle() {
	char* title = malloc(MAX_PATH * sizeof(char));
	GetConsoleTitleA(title, MAX_PATH);
	return title;
}

/**
 * Sets a new title to console window
 *
 * @param title new console title
 * @return 1 if eveything went well otherwise returns 0
 */
int SetTitle(const char* title) {
	if(!SetConsoleTitleA(title))
	{
		return 0;
	}
	return 1;
}

/**
 * @param console pointer to structure console
 * @param width console width
 * @param height console height
 * @return 1 if eveything went well otherwise returns negative
 */
int GetConsoleDimentions(const struct Console* console, int* width, int* height) {
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
