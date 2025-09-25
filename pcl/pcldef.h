//
// Created by pawel	on 24.07.2025.
//
// This file contains constants ad struct definitions for pcl library
//

#ifndef PCLDEF_H
#define PCLDEF_H

#include <windows.h>





// Queue
// TODO make separate library for this
struct Queue {
	struct Node* head;
	struct Node* tail;
};

struct Cell {
	char data;
	unsigned int fr, fg, fb;
	unsigned int br, bg, bb;
};

struct Console {
	// input
	HANDLE inputHandle;
	HANDLE mutexHandle;
	HANDLE threadHandle;
	HANDLE threadExitEvent;
	struct Queue* inputQueue;

	// output
	int currentOutput;
	HANDLE outputHandle;
	struct Cell* buffer;
	unsigned int cursor;
	unsigned int width, height;
	int fr, fg, fb;
	int br, bg, bb;

	// error
	HANDLE errorHandle;

	// settings
	int blockInput;
	int blockTimeout;

	/**
	 * That event is raised when console window is focused or unfocused
	 *
	 * @param int current window state can be FOCUSED and UNFOCUSED
	 */
	void(*FocusEvent)(struct Console*, int);

	/**
	 * That event is raised when console window is focused and keyboard key is clicked
	 * TODO Event is run in main thread ???
	 *
	 * @param char key
	 * @param int keystate can be PRESSED or RELEASED
	 */
	void(*KeyEvent)(struct Console*, char, int);

	/**
	 * That event is raised when console window is docused or unfocused
	 *
	 *
	 * @param int height - cursor position
	 * @param int width - cursor position
	 * @param int button - state of mouse buttons. Can be NONE_BUTTON_PRESSED, LEFT_MOUSE_BUTTON_PRESSED, RIGHT_MOUSE_BUTTON_PRESSED, WHEEL_MOUSE_UP, WHEEL_MOUSE_DOWN or WHEEL_MOUSE_BUTTON_PRESSED
	 * @param int keystate - state of control keys. Can be CONTROL_KEYS_ALT_LEFT, CONTROL_KEYS_ALT_RIGHT or CONTROL_KEYS_CONTROL
	 * @param int action - specifies what happend. Can be CLICK, MOVE, DOUBLE_CLICK, WHEEL or HORIZONTAL_WHEEL
	 */
	void(*MouseEvent)(struct Console*, int, int, int, int, int);

	/**
	 * That event is raised when console window is resized
	 *
	 * @param int new height
	 * @param int new width
	 */
	void(*ResizeEvent)(struct Console*, int, int);
};


// events

#define NONE_BUTTON_PRESSED 0
#define LEFT_MOUSE_BUTTON_PRESSED 1
#define RIGHT_MOUSE_BUTTON_PRESSED 2
#define WHEEL_MOUSE_BUTTON_PRESSED 4
#define WHEEL_MOUSE_UP 8388608
#define WHEEL_MOUSE_DOWN -8388608

#define CONTROL_KEYS_ALT_LEFT 2
#define CONTROL_KEYS_ALT_RIGHT 10
#define CONTROL_KEYS_CONTROL 8

#define CLICK 0
#define MOVE 1
#define DOUBLE_CLICK 2
#define WHEEL 4
#define HORIZONTAL_WHEEL 8

#define FOCUSED 1
#define UNFOCUSED 1

#define PRESSED 1
#define RELEASED 0


#endif //PCLDEF_H
