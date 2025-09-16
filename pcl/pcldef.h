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


// events

#define LEFT_MOUSE_BUTTON_PRESSED 1
#define RIGHT_MOUSE_BUTTON_PRESSED 2
#define WHEEL_MOUSE_BUTTON_PRESSED 4

#define MOUSE_PRESSED_OR_RELEASED 0

#define FOCUSED 1
#define UNFOCUSED 1

#define PRESSED 1
#define RELEASED 0


// foreground colors

#define F_RED FOREGROUND_RED
#define F_GREEN FOREGROUND_GREEN
#define F_BLUE FOREGROUND_BLUE
#define F_GOLD (FOREGROUND_GREEN | FOREGROUND_RED)
#define F_MAGENTA (FOREGROUND_BLUE | FOREGROUND_RED)
#define F_LIGHT_BLUE (FOREGROUND_BLUE | FOREGROUND_GREEN)
#define F_LIGHT_GRAY (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN
#define F_LIGHT_RED (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define F_LIGHT_GREEN (FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define F_ROYAL_BLUE (FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define F_YELLOW (FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY)
#define F_VIOLET (FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY)
#define F_TURQOISE (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define F_WHITE (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define F_BLACK 0
#define F_GRAY FOREGROUND_INTENSITY


// background colors

#define B_RED BACKGROUND_RED
#define B_GREEN BACKGROUND_GREEN
#define B_BLUE BACKGROUND_BLUE
#define B_GOLD (BACKGROUND_GREEN | BACKGROUND_RED)
#define B_MAGENTA (BACKGROUND_BLUE | BACKGROUND_RED)
#define B_LIGHT_BLUE (BACKGROUND_BLUE | BACKGROUND_GREEN)
#define B_LIGHT_GRAY (BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN
#define B_LIGHT_RED (BACKGROUND_RED | BACKGROUND_INTENSITY)
#define B_LIGHT_GREEN (BACKGROUND_GREEN | BACKGROUND_INTENSITY)
#define B_ROYAL_BLUE (BACKGROUND_BLUE | BACKGROUND_INTENSITY)
#define B_YELLOW (BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY)
#define B_VIOLET (BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY)
#define B_TURQOISE (BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY)
#define B_WHITE (BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY
#define B_BLACK 0
#define B_GRAY BACKGROUND_INTENSITY

#endif //PCLDEF_H
