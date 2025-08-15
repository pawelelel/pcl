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
	 * That event is raised when console window is focused or unfocused
	 *
	 * @param int current window state can be FOCUSED and UNFOCUSED
	 */
	void(*FocusEvent)(int);

	/**
	 * That event is raised when console window is focused and keyboard key is clicked
	 *
	 * @param char key
	 * @param int keystate can be PRESSED or RELEASED
	 */
	void(*KeyEvent)(char, int);

	/**
	 * That event is raised when console window is docused or unfocused
	 *
	 * @param int height cursor position
	 * @param int width cursor position
	 * @param int button if mouse button was clicked than this variable is set
	 * @param int keystate if additional keys are clicked
	 * @param int action specifies mouse behaviour
	 */
	void(*MouseEvent)(int, int, int, int, int);

	/**
	 * That event is raised when console window is docused or unfocused
	 *
	 * @param int height
	 * @param int width
	 */
	void(*ResizeEvent)(int, int);
};

#define LEFT_MOUSE_BUTTON_PRESSED 1
#define RIGHT_MOUSE_BUTTON_PRESSED 2
#define WHEEL_MOUSE_BUTTON_PRESSED 4

#define MOUSE_PRESSED_OR_RELEASED 0

#define FOCUSED 1
#define UNFOCUSED 1

#define PRESSED 1
#define RELEASED 0

#endif //PCLDEF_H
