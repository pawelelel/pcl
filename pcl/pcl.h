//
// Created by pawel on 14.05.2025.
//

#ifndef PCL_H
#define PCL_H

#include <windows.h>

struct Console {
	HANDLE inputHandle;
	int currentOutput;
	HANDLE outputHandle1;
	HANDLE outputHandle2;
	HANDLE errorHandle;
	HANDLE windowHandle;
	//TODO implement
	int blockInput;
	int blockTimeout;
};

/**
 * Starts library
 *
 * @return pointer to struct Console
 */
struct Console* start(void);

/**
 * Ends a console window
 *
 * @param console pointer to struct Console
 */
void end(struct Console* console);

/**
 * Gets a console title
 *
 * @returns char* a pointer to name of console
 */
char* gettitle(struct Console* console);

/**
 * Sets a new title to console window
 *
 * @param title new console title
 * @return 1 if eveything went well otherwise returns 0
 */
int settitle(struct Console* console, const char* title);

/**
 * Returns console dimensions
 *
 * @param console pointer to structure console
 * @param width console width
 * @param height console height
 * @return 1 if eveything went well otherwise returns negative:<br>
 * (-1) width parameter is NULL <br>
 * (-2) height parameter is NULL <br>
 * (-3) console parameter is NULL <br>
 * (-4) library has not been initialized properly <br>
 * (-5) win api internal error <br>
 */
int getdimensions(const struct Console* console, int* width, int* height);

/**
 * Gets first char from stdin
 *
 * @return char
 */
char getchr(const struct Console* console);

/**
 * Sets char on cursor position. Changes cursor position
 *
 * @param console pointer to structure console
 * @param c char to set
 */
void setchar(const struct Console* console, char c);

/**
 * Sets char on specified position. Do not changes cursor position
 *
 * @param c char to set
 * @param row row number
 * @param col column number
 */
void setcharcursor(const struct Console* console, char c, int row, int col);

/**
 * Works as standard scanf
 *
 * @param console pointer to struct Console
 * @param format format string
 * @param ... scanned variables
 */
int getvariables(const struct Console* console, char* format, ...);

/**
 * Prints formatted string on cursor position. Changes cursor position
 *
 * @param format format string
 * @param ... variables to format
 */
void setstringformatted(const struct Console* console, char* format, ...);

/**
 * Prints formatted string on cursor position. Do not changes cursor position
 *
 * @param format format string
 * @param row row number
 * @param col column number
 * @param ... variables to format
 */
void setstringformattedcursor(const struct Console* console, char* format, int row, int col, ...);

/**
 * Gets chars from stdin till detects '\n' or '\r' or reaches buffer size
 *
 * @param buffer input buffer
 */
void getstring(const struct Console* console, char* buffer, size_t size);

/**
 * Gets chars from stdin till reaches buffer size
 *
 * @param console pointer to struct Console
 * @param buffer input buffer
 * @param size buffer size
 */
void getstringbuffer(const struct Console* console, char* buffer, size_t size);

/**
 * Prints string on cursor position. Changes cursor position
 *
 * @param console pointer to struct Console
 * @param string input
 */
void setstring(const struct Console* console, const char* string);

/**
 * Prints string on cursor position. Do not changes cursor position
 *
 * @param string input
 * @param row row number
 * @param col column number
 */
void setstringcursor(const struct Console* console, const char* string, int row, int col);

/**
 * Clears console
 *
 * @param console pointer to struct Console
 */
void clear(const struct Console* console);

/**
 * Fills console with specified char
 *
 * @param console pointer to struct Console
 * @param c char to be filled
 */
void fill(const struct Console* console, const char c);

/**
 * Sets 2d array on specified location. Do not changes cursor position
 *
 * @param console pointer to struct Console
 * @param array array to write
 * @param row top-left array position of array in console
 * @param col top-left array position of array in console
 * @param width array height
 * @param height array wwidth
 */
void set2darray(const struct Console* console, const char* array, const int row, const int col, const int width, const int height);

/**
 * Sets cursor position
 *
 * @param console pointer to struct Console
 * @param row row number
 * @param col column number
 */
void setcursorposition(const struct Console* console, int row, int col);

/**
 * Returns cursor position
 *
 * @param console pointer to struct console
 * @param row row number
 * @param col column number
 * @return 1 if eveything went well otherwise returns negative:<br>
 */
int getcursorposition(const struct Console* console, int* row, int* col);

/**
 * Prints console window
 *
 * @param console pointer to struct Console
 */
void refresh(struct Console* console);

#endif //PCL_H
