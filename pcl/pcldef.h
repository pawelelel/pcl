//
// Created by pawel	on 24.07.2025.
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

	/**
	 *
	 *
	 * @param int current window state defined by FOCUSED and UNFOCUSED macros
	 */
	void(*FocusEvent)(int);
	/**
	 *
	 */
	void(*KeyEvent)(char);
	/**
	 *
	 */
	void(*MouseEvent)(int, int, int, int, int);
	/**
	 *
	 */
	void(*ResizeEvent)(int, int);
};

#define LEFT_MOUSE_BUTTON_PRESSED 1
#define RIGHT_MOUSE_BUTTON_PRESSED 2
#define WHEEL_MOUSE_BUTTON_PRESSED 3



#define FOCUSED 1
#define UNFOCUSED 1

#endif //PCLDEF_H
