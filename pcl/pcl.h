//
// Created by pawel on 14.05.2025.
//

#ifndef PCL_H
#define PCL_H

#include <windows.h>
#include "pcldef.h"

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
int end(struct Console* console);

/**
 * Sets foreground color to specified value
 *
 * @param r red
 * @param g green
 * @param b blue
 * @return error code
 */
int setforegroundcolor(struct Console *console, int r, int g, int b);

/**
 * Sets background color to specified value
 *
 * @param r red
 * @param g green
 * @param b blue
 * @return error code
 */
int setbackgroundcolor(struct Console *console, int r, int g, int b);

/**
 * Sets foreground color to default value
 *
 * @return error code
 */
int clearforegroundcolor(struct Console *console);

/**
 * Sets background color to default value
 *
 * @return error code
 */
int clearbackgroundcolor(struct Console *console);

/**
 * Sets or unsets getchr() input blocking behaviour
 * See also getinputblock()
 *
 * @param console pointer to struct Console
 * @param blockinput should be TRUE or FALSE
 * @return 1 if eveything went well otherwise returns 0
 */
int setinputblock(struct Console* console, int blockinput);

/**
 * Returns getchr() input blocking behaviour
 * See also setinputblock()
 *
 * @param console pointer to struct Console
 * @return current state of input block. TRUE or FALSE
 */
int getinputblock(struct Console* console);

/**
 * Sets focus event function
 *
 * @param console pointer to struct Console
 * @param FocusEvent pointer to event handling function receives FOCUSED or UNFOCUSED argument
 * @return error code
 */
int setfocusevent(struct Console* console, void(*FocusEvent)(struct Console* , int));

/**
 * Unsets focus event function
 * 
 * @param console pointer to struct Console
 * @return error code
 */
int unsetfocusevent(struct Console* console);

/**
 *
 * @param console pointer to struct Console
 * @param KeyEvent pointer to event handling function
 * @return error code
 */
int setkeyevent(struct Console* console, void(*KeyEvent)(struct Console*, char, int));

/**
 * 
 * @param console pointer to struct Console
 * @return error code
 */
int unsetkeyevent(struct Console* console);

/**
 * 
 * @param console pointer to struct Console
 * @param MouseEvent pointer to event handling function
 * @return error code
 */
int setmouseevent(struct Console* console, void(*MouseEvent)(struct Console*, int, int, int, int, int));

/**
 * 
 * @param console pointer to struct Console
 * @return error code
 */
int unsetmouseevent(struct Console* console);

/**
 * 
 * @param console pointer to struct Console
 * @param ResizeEvent pointer to event handling function
 * @return error code
 */
int setresizeevent(struct Console* console, void(*ResizeEvent)(struct Console*, int, int));

/**
 * 
 * @param console pointer to struct Console
 * @return error code
 */
int unsetresizeevent(struct Console* console);

/**
 * Sets timeout
 * See also gettimeout()
 *
 * @param console pointer to struct Console
 * @param timeout should be >= 0
 * @return 1 if eveything went well otherwise returns 0
 */
int settimeout(struct Console* console, int timeout);

/**
 * Returns set timeout
 * See also settimeout()
 *
 * @param console pointer to struct Console
 * @return current value of timeout
 */
int gettimeout(struct Console* console);

/**
 * Gets a console title
 *
 * @returns char* a pointer to name of console
 */
int gettitle(struct Console* console, char** title);

/**
 * Sets a title of console window
 *
 * @param console pointer to struct Console
 * @param title new console title
 * @return Error code:\n
	- 0 if eveything went well
	- -1 if console is NULL\n
	- -2 if title is NULL\n
	- -3 if internal error occured
 */
int settitle(struct Console* console, char* title);

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
int getdimensions(struct Console* console, int* width, int* height);

/**
 * Gets first char from stdin
 * Warning! input blocking is "stronger" than timeout
 *
 * @param console pointer to struct Console
 *
 * @return char
 */
char getchr(struct Console* console);

/**
 * Sets char on cursor position. Changes cursor position
 *
 * @param console pointer to structure console
 * @param c char to set
 * @return error code:\n
 */
int setchar(struct Console* console, char c);

/**
 * Sets char on specified position. Do not changes cursor position
 *
 * @param console pointer to struct Console
 * @param c char to set
 * @param row row number
 * @param col column number
 */
int setcharcursor(struct Console* console, char c, unsigned int row, unsigned int col);

/**
 * Works as standard scanf
 *
 * @param console pointer to struct Console
 * @param format format string
 * @param ... scanned variables
 */
int getvariables(struct Console* console, char* format, ...);

/**
 * Prints formatted string on cursor position. Changes cursor position
 *
 * @param console pointer to struct Console
 * @param format format string
 * @param ... variables to format
 */
int setstringformatted(struct Console* console, char* format, ...);

/**
 * Prints formatted string on cursor position. Do not changes cursor position
 *
 * @param console pointer to struct Console
 * @param format format string
 * @param row row number
 * @param col column number
 * @param ... variables to format
 */
int setstringformattedcursor(struct Console* console, int row, int col, char* format, ...);

/**
 * Gets chars from stdin till detects '\n' or '\r' or reaches buffer size or execution time reaches timeout time
 * Always adds '\0' at last place
 *
 * @param console pointer to struct Console
 * @param buffer input buffer
 * @param size
 */
int getstring(struct Console* console, char* buffer, size_t size);

/**
 * Gets chars from stdin till reaches buffer size or execution time reaches timeout time
 * Always adds '\0' at last place
 *
 * @param console pointer to struct Console
 * @param buffer input buffer
 * @param size buffer size
 */
int getstringbuffer(struct Console* console, char* buffer, size_t size);

/**
 * Prints string on cursor position. Changes cursor position
 *
 * @param console pointer to struct Console
 * @param string input
 */
int setstring(struct Console* console, char* string);

/**
 * Prints string on cursor position. Do not changes cursor position
 *
 * @param console pointer to struct Console
 * @param string input
 * @param row row number
 * @param col column number
 */
int setstringcursor(struct Console* console, char* string, int row, int col);

/**
 * Clears console
 *
 * @param console pointer to struct Console
 */
int clear(struct Console* console);

/**
 * Fills console with specified char
 *
 * @param console pointer to struct Console
 * @param c char to be filled
 */
int fill(struct Console* console, char c, unsigned int fr, unsigned int fg, unsigned int fb, unsigned int br, unsigned int bg, unsigned int bb);

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
int set2darray(struct Console* console, char* array, unsigned int row, unsigned int col, unsigned int width, unsigned int height);

/**
 * Sets cursor position
 *
 * @param console pointer to struct Console
 * @param row row number
 * @param col column number
 */
int setcursorposition(struct Console* console, unsigned int row, unsigned int col);

/**
 * Returns cursor position
 *
 * @param console pointer to struct console
 * @param row row number
 * @param col column number
 * @return 1 if eveything went well otherwise returns negative:<br>
 */
int getcursorposition(struct Console* console, unsigned int *row, unsigned int *col);

/**
 * Prints console window
 *
 * @param console pointer to struct Console
 */
int refresh(struct Console* console);

#endif //PCL_H
