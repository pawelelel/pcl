//
// Created by pawci on 14.05.2025.
//

#include <bemapiset.h>
#include <windows.h>
#ifndef PCL_H

struct Console {
	HANDLE inputHandle;
	HANDLE outputHandle;
	HANDLE errorHandle;
	HANDLE windowHandle;
};

struct Console* Start(void);

void End(struct Console* console);

char* GetTitle(struct Console* console);

int SetTitle(struct Console* console, const char* title);

int GetConsoleDimentions(const struct Console* console, int* width, int* height);

#define PCL_H

#endif //PCL_H
