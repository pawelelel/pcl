//
// Created by pawel on 14.05.2025.
//

#ifndef PCL_H
#define PCL_H

#include <windows.h>
#include "pcldef.h"
#include "pcl_ascii.h"

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
unsigned int gettimeout(struct Console* console);

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
 * Sets char on cursor position. Changes cursor position
 *
 * @param console pointer to structure console
 * @param c char to set
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL
 */
int setcharsetcharformatted(struct Console* console, char c);

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
 * Prints formatted string on cursor position. Changes cursor position.
 *
 * @param console pointer to struct Console
 * @param format format string
 * @param ... variables to format
 *
 * @return size of written string if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) format parameter is NULL<br>
 * (-3) format parameter is not a valid format string<br>
 * (-4) internal error<br>
 *
 * Possible standard tokens:<br>
 * %ull unsigned long long<br>
 * %ll long long<br>
 * %uh unsigned short<br>
 * %ud unsigned int<br>
 * %ul unsigned long<br>
 * %c char<br>
 * %h short<br>
 * %d int<br>
 * %l long<br>
 * %s string<br>
 * %% '%' character<br>
 * Possible styles:<br>
 * @<number>;<number>;<number>f foreground color<br>
 * @<number>;<number>;<number>b background color<br>
 * @b bold<br>
 * @rb remove bold<br>
 * @d dim<br>
 * @rd remove dim<br>
 * @i italic<br>
 * @ri remove italic<br>
 * @u underline<br>
 * @ru remove underline<br>
 * @l blinking<br>
 * @rl remove blinking<br>
 * @s strikethrough<br>
 * @rs remove strikethrough<br>
 * @uu doubleunderline<br>
 * @ruu remove doubleunderline<br>
 * @c clear all<br>
 * @@ at character
 */
int setstringformatted(struct Console* console, char* format, ...);

/**
 * Prints formatted string on cursor position. Do not changes cursor position. See setstringformatted() for more information.
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
 * (-6) col parameter is greater than console width<br>
 * (-7) format parameter is not a valid format string<br>
 * (-8) internal error
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
 * Prints console window
 *
 * @param console pointer to struct Console
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) console parameter is NULL<br>
 * (-2) internal function error
 */
int refreshunicode(struct Console* console, struct UnicodeScreen* ascii);

#endif //PCL_H
