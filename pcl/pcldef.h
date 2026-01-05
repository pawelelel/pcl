//
// Created by pawel	on 24.07.2025.
//
// This file contains constants ad struct definitions for pcl library
//

#ifndef PCLDEF_H
#define PCLDEF_H

#include <windows.h>

struct AsciiCell {
	char data;
	unsigned int foregroundRed, foregroundGreen, foregroundBlue;
	unsigned int backgroundRed, backgroundGreen, backgroundBlue;

	// specific character decorations
	BOOL bold;
	BOOL dim;
	BOOL italic;
	BOOL underline;
	BOOL blinking;
	BOOL strikethrough;
	BOOL doubleunderline;
};

struct UnicodeCell {
	wchar_t data1;
	wchar_t data2;
	unsigned int foregroundRed, foregroundGreen, foregroundBlue;
	unsigned int backgroundRed, backgroundGreen, backgroundBlue;

	// specific character decorations
	BOOL bold;
	BOOL dim;
	BOOL italic;
	BOOL underline;
	BOOL blinking;
	BOOL strikethrough;
	BOOL doubleunderline;
};

struct UnicodeScreen {
	struct UnicodeCell* buffer;
	unsigned int bufferSize;
	wchar_t* outputBuffer;
	unsigned int width, height;
	unsigned int foregroundRed, foregroundGreen, foregroundBlue;
	unsigned int backgroundRed, backgroundGreen, backgroundBlue;

	// cursor
	int cursorVisible;
	int cursorstyle;
	unsigned int cursor;

	// font decorations
	BOOL bold;
	BOOL dim;
	BOOL italic;
	BOOL underline;
	BOOL blinking;
	BOOL strikethrough;
	BOOL doubleunderline;

	// default values
	wchar_t data1;
	wchar_t data2;
	unsigned int defaultForegroundRed, defaultForegroundGreen, defaultForegroundBlue;
	unsigned int defaultBackgroundRed, defaultBackgroundGreen, defaultBackgroundBlue;
};

struct AsciiScreen {
	struct AsciiCell* buffer; // TODO buffer needs to be thread safe due to resizing event
	unsigned int bufferSize;
	char* outputBuffer;
	unsigned int width, height;
	unsigned int foregroundRed, foregroundGreen, foregroundBlue;
	unsigned int backgroundRed, backgroundGreen, backgroundBlue;

	// cursor
	int cursorVisible;
	int cursorstyle;
	unsigned int cursor;

	// font decorations
	BOOL bold;
	BOOL dim;
	BOOL italic;
	BOOL underline;
	BOOL blinking;
	BOOL strikethrough;
	BOOL doubleunderline;

	// default values
	char defaultchar;
	unsigned int defaultForegroundRed, defaultForegroundGreen, defaultForegroundBlue;
	unsigned int defaultBackgroundRed, defaultBackgroundGreen, defaultBackgroundBlue;
};

struct Console {
	// input
	HANDLE inputHandle;
	HANDLE threadHandle;
	HANDLE threadExitEvent;
	struct Queue* inputQueue;

	// output
	unsigned int width, height;
	HANDLE outputHandle;
	struct AsciiScreen** asciiScreens;
	int asciiScreensIndex;
	struct UnicodeScreen** unicodeScreens;
	int unicodeScreensIndex;

	// error
	HANDLE errorHandle;

	// settings
	int blockInput;
	unsigned int blockTimeout; // works only with getchr function

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
	void(*ResizeEvent)(struct Console*, unsigned int, unsigned int);
};


// events

#define NONE_BUTTON_PRESSED 0
#define LEFT_MOUSE_BUTTON_PRESSED 1
#define RIGHT_MOUSE_BUTTON_PRESSED 2
#define WHEEL_MOUSE_BUTTON_PRESSED 4
#define WHEEL_MOUSE_UP 8388608
#define WHEEL_MOUSE_DOWN (-8388608)

#define CONTROL_KEYS_ALT_LEFT 2
#define CONTROL_KEYS_ALT_RIGHT 10
#define CONTROL_KEYS_CONTROL 8

#define MOUSE_CLICK 0
#define MOUSE_MOVE 1
#define MOUSE_DOUBLE_CLICK 2
#define MOUSE_WHEEL 4
#define MOUSE_HORIZONTAL_WHEEL 8

#define FOCUSED 1
#define UNFOCUSED 1

#define PRESSED 1
#define RELEASED 0


// cursor types

#define BLINKING_BLOCK 1
#define STEADY_BLOCK 2
#define BLINKING_UNDERLINE 3
#define STEADY_UNDERLINE 4
#define BLINKING_BAR 5
#define STEADY_BAR 6

#endif //PCLDEF_H
