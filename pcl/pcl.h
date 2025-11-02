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
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int end(struct Console* console);

/**
 * Sets cursor style
 *
 * @param console pointer to struct Console
 * @param style one of 6 predefined styles, can be BLINKING_BLOCK, STEADY_BLOCK, BLINKING_UNDERLINE, STEADY_UNDERLINE, BLINKING_BAR or STEADY_BAR
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) style parameter is unknown
 */
int setcursorstyle(struct Console* console, int style);

/**
 * Sets cursor style to default (BLINKING_BAR)
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int unsetcursorstyle(struct Console* console);

/**
 * Shows cursor in console window
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
*/
int showcursor(struct Console* console);

/**
 * Hides cursor in console window
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
*/
int hidecursor(struct Console* console);

/**
 * Sets font bold property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int setfontbold(struct Console* console);

/**
 * Unsets font bold property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int unsetfontbold(struct Console* console);

/**
 * Returns font bold property
 *
 * @param console pointer to struct Console
 * @return bold property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int getfontbold(struct Console* console);

/**
 * Sets font dim property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int setfontdim(struct Console* console);

/**
 * Unsets font dim property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int unsetfontdim(struct Console* console);

/**
 * Returns font dim property
 *
 * @param console pointer to struct Console
 * @return dim property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int getfontdim(struct Console* console);

/**
 * Sets font italic property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int setfontitalic(struct Console* console);

/**
 * Unsets font italic property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int unsetfontitalic(struct Console* console);

/**
 * Returns font italic property
 *
 * @param console pointer to struct Console
 * @return italic property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int getfontitalic(struct Console* console);

/**
 * Sets font underline property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int setfontunderline(struct Console* console);

/**
 * Unsets font underline property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int unsetfontunderline(struct Console* console);

/**
 * Returns font underline property
 *
 * @param console pointer to struct Console
 * @return underline property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int getfontunderline(struct Console* console);

/**
 * Sets font blinking property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int setfontblinking(struct Console* console);

/**
 * Unsets font blinking property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int unsetfontblinking(struct Console* console);

/**
 * Returns font blinking property
 *
 * @param console pointer to struct Console
 * @return blinking property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int getfontblinking(struct Console* console);

/**
 * Sets font strikethrough property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int setfontstrikethrough(struct Console* console);

/**
 * Unsets font strikethrough property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int unsetfontstrikethrough(struct Console* console);

/**
 * Returns font strikethrough property
 *
 * @param console pointer to struct Console
 * @return strikethrough property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int getfontstrikethrough(struct Console* console);

/**
 * Sets font doubleunderline property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int setfontdoubleunderline(struct Console* console);

/**
 * Unsets font doubleunderline property
 *
 * @param console pointer to struct Console
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int unsetfontdoubleunderline(struct Console* console);

/**
 * Returns font doubleunderline property
 *
 * @param console pointer to struct Console
 * @return doubleunderline property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int getfontdoubleunderline(struct Console* console);

/**
 * Sets foreground color to specified value
 *
 * @param console pointer to struct Console
 * @param red red
 * @param green green
 * @param blue blue
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 * (-2) red parameter is greater than 255
 * (-3) green parameter is greater than 255
 * (-4) blue parameter is greater than 255
 */
int setforegroundcolor(struct Console *console, unsigned int red, unsigned int green, unsigned int blue);

/**
 * Sets background color to specified value
 *
 * @param console pointer to struct Console
 * @param red red
 * @param green green
 * @param blue blue
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 * (-2) red parameter is greater than 255
 * (-3) green parameter is greater than 255
 * (-4) blue parameter is greater than 255
 */
int setbackgroundcolor(struct Console *console, int red, int green, int blue);

/**
 * Sets foreground color to default value
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int clearforegroundcolor(struct Console *console);

/**
 * Sets background color to default value
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
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
int setinputblock(struct Console* console, BOOL blockinput);

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
 * @param FocusEvent pointer to event handling function
 * @return  0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 * (-1) FocusEvent parameter is NULL
 */
int setfocusevent(struct Console* console, void(*FocusEvent)(struct Console* , int));

/**
 * Unsets focus event function
 * 
 * @param console pointer to struct Console
 * @return  0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int unsetfocusevent(struct Console* console);

/**
 *
 * @param console pointer to struct Console
 * @param KeyEvent pointer to event handling function which receives  FOCUSED or UNFOCUSED arguments
 * @return  0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 * (-2) KeyEvent parameter is NULL
 */
int setkeyevent(struct Console* console, void(*KeyEvent)(struct Console*, char, int));

/**
 * 
 * @param console pointer to struct Console
 * @return  0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int unsetkeyevent(struct Console* console);

/**
 * 
 * @param console pointer to struct Console
 * @param MouseEvent pointer to event handling function
 * @return  0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 * (-2) MouseEvent parameter is NULL
 */
int setmouseevent(struct Console* console, void(*MouseEvent)(struct Console*, int, int, int, int, int));

/**
 * 
 * @param console pointer to struct Console
 * @return  0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int unsetmouseevent(struct Console* console);

/**
 * 
 * @param console pointer to struct Console
 * @param ResizeEvent pointer to event handling function
 * @return  0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 * (-2) ResizeEvent parameter is NULL
 */
int setresizeevent(struct Console* console, void(*ResizeEvent)(struct Console*, unsigned int, unsigned int));

/**
 * 
 * @param console pointer to struct Console
 * @return  0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int unsetresizeevent(struct Console* console);

/**
 * Sets timeout
 * See also gettimeout()
 *
 * @param console pointer to struct Console
 * @param timeout should be >= 0
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int settimeout(struct Console* console, unsigned int timeout);

/**
 * Returns set timeout
 * See also settimeout()
 *
 * @param console pointer to struct Console
 * @return current value of timeout or negative if error occurs:
 * (-1) console parameter is NULL
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
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) title parameter is NULL<br>
 * (-3) internal error occured
 */
int settitle(struct Console* console, char* title);

/**
 * Returns console dimensions
 *
 * @param console pointer to structure console
 * @param width console width
 * @param height console height
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) width parameter is NULL<br>
 * (-2) height parameter is NULL<br>
 * (-3) console parameter is NULL
 */
int getdimensions(struct Console* console, unsigned int* width, unsigned int* height);

/**
 * Gets first char from stdin<br>
 * <b>Warning!</b> input blocking is "stronger" than timeout
 *
 * @param console pointer to struct Console
 *
 * @return char if everything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) function waited too much for input
 */
char getchr(struct Console* console);

/**
 * Sets char on cursor position. Changes cursor position
 *
 * @param console pointer to structure console
 * @param c char to set
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int setchar(struct Console* console, char c);

/**
 * Sets char on specified position. Do not changes cursor position
 *
 * @param console pointer to struct Console
 * @param c char to set
 * @param row row number
 * @param col column number
 *
 * @return 0 if everything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) row is greater than console height
 * (-3) col is greater than console width
 */
int setcharcursor(struct Console* console, char c, unsigned int row, unsigned int col);

/**
 * Works as standard scanf
 *
 * @param console pointer to struct Console
 * @param format format string. Now available are given tokens:<br>
 * %s string<br>
 * %c char<br>
 * %h short<br>
 * %uh unsigned short<br>
 * %d int<br>
 * %ud unsigned int<br>
 * %l long<br>
 * %ul unsigned long<br>
 * %ll long long<br>
 * %ull unsigned long long<br>
 * %f float<br>
 * %lf double<br>
 * %llf long double
 * @param ... scanned variables
 *
 * @return number of assigned variables if everything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) format parameter is NULL<br>
 * (-3) format is not valid format string<br>
 * (-4) float variable processing error<br>
 * (-5) long variable processing error<br>
 * (-6) int variable processing error<br>
 * (-7) short variable processing error<br>
 * (-8) char variable processing error<br>
 * (-9) unsigned long variable processing error<br>
 * (-10) double variable processing error<br>
 * (-11) unsigned int variable processing error<br>
 * (-12) unsigned short variable processing error<br>
 * (-13) long long variable processing error<br>
 * (-14) unsigned long long variable processing error<br>
 * (-15) long double variable processing error<br>
 * (-16) general format processing error
 */
int getvariables(struct Console* console, char* format, ...);

/**
 * Prints formatted string on cursor position. Changes cursor position
 *
 * @param console pointer to struct Console
 * @param format format string
 * @param ... variables to format
 *
 * @return size of written string if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) format parameter is NULL
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
 *
 * @return size of written string if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) row parameter is lower than zero <br>
 * (-3) col parameter is lower than zero <br>
 * (-4) format parameter is NULL<br>
 * (-5) row parameter is greater than console height <br>
 * (-6) col parameter is greater than console width
 */
int setstringformattedcursor(struct Console* console, int row, int col, char* format, ...);

/**
 * Gets chars from stdin till detects '\n' or '\r' or reaches buffer size or execution time reaches timeout time
 * Always adds '\0' at last place
 *
 * @param console pointer to struct Console
 * @param buffer input buffer
 * @param size
 *
 * @return number of chars gotten if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) buffer parameter is NULL
 */
int getstring(struct Console* console, char* buffer, size_t size);

/**
 * Gets chars from stdin till reaches buffer size or execution time reaches timeout time
 * Always adds '\0' at last place
 *
 * @param console pointer to struct Console
 * @param buffer input buffer
 * @param size buffer size
 *
 * @return number of chars gotten if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) buffer parameter is NULL
 */
int getstringbuffer(struct Console* console, char* buffer, size_t size);

/**
 * Prints string on cursor position. Changes cursor position
 *
 * @param console pointer to struct Console
 * @param string input
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 * (-1) string parameter is NULL
 */
int setstring(struct Console* console, char* string);

/**
 * Prints string on cursor position. Do not changes cursor position
 *
 * @param console pointer to struct Console
 * @param string input
 * @param row row number
 * @param col column number
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) row parameter is lower than zero <br>
 * (-3) col parameter is lower than zero <br>
 * (-4) string parameter is NULL<br>
 * (-5) row parameter is greater than console height <br>
 * (-6) col parameter is greater than console width
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
 * @param foregroundRed foreground color red value
 * @param foregroundGreen foreground color green value
 * @param foregroundBlue foreground color blue value
 * @param backgroundRed backround color red value
 * @param backgroundGreen backround color green value
 * @param backgroundBlue backround color blue value
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) foreground red parameter is greater than 255<br>
 * (-3) foreground green parameter is greater than 255<br>
 * (-4) foreground blue parameter is greater than 255<br>
 * (-5) background red parameter is greater than 255<br>
 * (-6) background green parameter is greater than 255<br>
 * (-7) background blue parameter is greater than 255
 */
int fill(struct Console* console, char c, unsigned int foregroundRed, unsigned int foregroundGreen, unsigned int foregroundBlue, unsigned int backgroundRed, unsigned int backgroundGreen, unsigned int backgroundBlue);

/**
 * Fills console with specified char
 *
 * @param console pointer to struct Console
 * @param c char to be filled
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int fillchar(struct Console* console, char c);

/**
 * Sets 2d array on specified location. Do not changes cursor position
 *
 * @param console pointer to struct Console
 * @param array array to write
 * @param row top-left array position of array in console
 * @param col top-left array position of array in console
 * @param width array height
 * @param height array wwidth
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) array parameter is NULL<br>
 * (-3) height parameter equals 0<br>
 * (-4) width parameter equals 0<br>
 * (-5) height parameter equals 0<br>
 * (-6) width parameter equals 0<br>
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
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) row parameter is NULL<br>
 * (-3) col parameter is NULL<br>
 */
int getcursorposition(struct Console* console, unsigned int *row, unsigned int *col);

/**
 * Prints console window
 *
 * @param console pointer to struct Console
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) internal function error
 */
int refresh(struct Console* console);

#endif //PCL_H
