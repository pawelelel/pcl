//
// Created by pawel on 14.05.2025.
//
#ifndef PCL_H
#define PCL_H

#include <windows.h>

struct Console {
	HANDLE inputHandle;
	HANDLE outputHandle;
	HANDLE errorHandle;
	HANDLE windowHandle;
};

struct Console* Start(void);

void End(struct Console* console);

char* GetTitle();

int SetTitle(const char* title);

int GetConsoleDimentions(const struct Console* console, int* width, int* height);

char getchar(void);

void setchar(char c);

void setchar(char c, int row, int col);

void scanf(char* format, ...);

void printf(char* format, ...);

void printf(char* format, int row, int col, ...);

void gets(char* buffer);

void gets(char* buffer, size_t size);

void puts(char* buffer);

void puts(char* buffer, int row, int col);

#endif //PCL_H
