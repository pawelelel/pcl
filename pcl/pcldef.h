//
// Created by pawci on 24.07.2025.
//
// This file contains constants ad struct definitions for pcl library
//

#ifndef PCLDEF_H
#define PCLDEF_H

#include <windows.h>

struct Console {
	HANDLE inputHandle;
	int currentOutput;
	HANDLE outputHandle1;
	HANDLE outputHandle2;
	HANDLE errorHandle;
	HANDLE windowHandle;
	int blockInput;
	int blockTimeout;

	void(*FocusEvent)(int);
	void(*KeyEvent)(char);
	void(*MenuEvent)(void*);
	void(*MouseEvent)(void*);
	void(*ResizeEvent)(void*);
};

#define FOCUSED 1
#define UNFOCUSED 1

#endif //PCLDEF_H
