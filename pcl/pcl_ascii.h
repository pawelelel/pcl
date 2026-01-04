//
// Created by pawci on 03.01.2026.
//

#ifndef PCL_PCL_ASCII_H
#define PCL_PCL_ASCII_H

#include "pcldef.h"

/**
 * TODO add docs
 * @param console
 * @return
 */
struct AsciiScreen* initascii(struct Console *console);

/**
 * Sets cursor style
 *
 * @param ascii pointer to struct ScreenAscii
 * @param style one of 6 predefined styles, can be BLINKING_BLOCK, STEADY_BLOCK, BLINKING_UNDERLINE, STEADY_UNDERLINE, BLINKING_BAR or STEADY_BAR
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL<br>
 * (-2) style parameter is unknown
 */
int setcursorstyleascii(struct AsciiScreen *ascii, int style);

/**
 * Sets cursor style to default (BLINKING_BAR)
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int unsetcursorstyleascii(struct AsciiScreen *ascii);

/**
 * Shows cursor in ascii window
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
*/
int showcursorascii(struct AsciiScreen *ascii);

/**
 * Hides cursor in ascii window
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
*/
int hidecursorascii(struct AsciiScreen *ascii);

/**
 * Sets font bold property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int setfontboldascii(struct AsciiScreen *ascii);

/**
 * Unsets font bold property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int unsetfontboldascii(struct AsciiScreen *ascii);

/**
 * Returns font bold property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return bold property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int getfontboldascii(struct AsciiScreen *ascii);

/**
 * Sets font dim property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int setfontdimascii(struct AsciiScreen *ascii);

/**
 * Unsets font dim property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int unsetfontdimascii(struct AsciiScreen *ascii);

/**
 * Returns font dim property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return dim property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int getfontdimascii(struct AsciiScreen *ascii);

/**
 * Sets font italic property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int setfontitalicascii(struct AsciiScreen *ascii);

/**
 * Unsets font italic property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int unsetfontitalicascii(struct AsciiScreen *ascii);

/**
 * Returns font italic property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return italic property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int getfontitalicascii(struct AsciiScreen *ascii);

/**
 * Sets font underline property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int setfontunderlineascii(struct AsciiScreen *ascii);

/**
 * Unsets font underline property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int unsetfontunderlineascii(struct AsciiScreen *ascii);

/**
 * Returns font underline property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return underline property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int getfontunderlineascii(struct AsciiScreen *ascii);

/**
 * Sets font blinking property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int setfontblinkingascii(struct AsciiScreen *ascii);

/**
 * Unsets font blinking property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int unsetfontblinkingascii(struct AsciiScreen *ascii);

/**
 * Returns font blinking property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return blinking property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int getfontblinkingascii(struct AsciiScreen *ascii);

/**
 * Sets font strikethrough property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int setfontstrikethroughascii(struct AsciiScreen *ascii);

/**
 * Unsets font strikethrough property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int unsetfontstrikethroughascii(struct AsciiScreen *ascii);

/**
 * Returns font strikethrough property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return strikethrough property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int getfontstrikethroughascii(struct AsciiScreen *ascii);

/**
 * Sets font doubleunderline property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int setfontdoubleunderlineascii(struct AsciiScreen *ascii);

/**
 * Unsets font doubleunderline property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int unsetfontdoubleunderlineascii(struct AsciiScreen *ascii);

/**
 * Returns font doubleunderline property
 *
 * @param ascii pointer to struct ScreenAscii
 * @return doubleunderline property (TRUE or FALSE) if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int getfontdoubleunderlineascii(struct AsciiScreen *ascii);

/**
 * Sets foreground color to specified value
 *
 * @param ascii pointer to struct ScreenAscii
 * @param red red
 * @param green green
 * @param blue blue
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 * (-2) red parameter is greater than 255
 * (-3) green parameter is greater than 255
 * (-4) blue parameter is greater than 255
 */
int setforegroundcolorascii(struct AsciiScreen *ascii, unsigned int red, unsigned int green, unsigned int blue);

/**
 * Sets background color to specified value
 *
 * @param ascii pointer to struct ScreenAscii
 * @param red red
 * @param green green
 * @param blue blue
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 * (-2) red parameter is greater than 255
 * (-3) green parameter is greater than 255
 * (-4) blue parameter is greater than 255
 */
int setbackgroundcolorascii(struct AsciiScreen *ascii, int red, int green, int blue);

/**
 * Sets foreground color to default value
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int clearforegroundcolorascii(struct AsciiScreen *ascii);

/**
 * Sets background color to default value
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int clearbackgroundcolorascii(struct AsciiScreen *ascii);

/**
 * Prints string on cursor position. Changes cursor position
 *
 * @param ascii pointer to struct Ascii
 * @param string input
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 * (-1) string parameter is NULL
 */
int setstringascii(struct AsciiScreen *ascii, char* string);

/**
 * Prints string on cursor position. Do not changes cursor position
 *
 * @param ascii pointer to struct Ascii
 * @param string input
 * @param row row number
 * @param col column number
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL<br>
 * (-2) row parameter is lower than zero <br>
 * (-3) col parameter is lower than zero <br>
 * (-4) string parameter is NULL<br>
 * (-5) row parameter is greater than ascii height <br>
 * (-6) col parameter is greater than ascii width
 */
int setstringcursorascii(struct AsciiScreen *ascii, char* string, int row, int col);

/**
 * Clears ascii
 *
 * @param ascii pointer to struct Ascii
 */
int clearascii(struct AsciiScreen *ascii);

/**
 * Fills ascii with specified char
 *
 * @param ascii pointer to struct Ascii
 * @param c char to be filled
 * @param foregroundRed foreground color red value
 * @param foregroundGreen foreground color green value
 * @param foregroundBlue foreground color blue value
 * @param backgroundRed backround color red value
 * @param backgroundGreen backround color green value
 * @param backgroundBlue backround color blue value
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL<br>
 * (-2) foreground red parameter is greater than 255<br>
 * (-3) foreground green parameter is greater than 255<br>
 * (-4) foreground blue parameter is greater than 255<br>
 * (-5) background red parameter is greater than 255<br>
 * (-6) background green parameter is greater than 255<br>
 * (-7) background blue parameter is greater than 255
 */
int fillascii(struct AsciiScreen *ascii, char c, unsigned int foregroundRed, unsigned int foregroundGreen, unsigned int foregroundBlue, unsigned int backgroundRed, unsigned int backgroundGreen, unsigned int backgroundBlue);

/**
 * Fills ascii with specified char
 *
 * @param ascii pointer to struct Ascii
 * @param c char to be filled
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL
 */
int fillcharascii(struct AsciiScreen *ascii, char c);

/**
 * Sets 2d array on specified location. Do not changes cursor position
 *
 * @param ascii pointer to struct Ascii
 * @param array array to write
 * @param row top-left array position of array in ascii
 * @param col top-left array position of array in ascii
 * @param width array height
 * @param height array wwidth
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL<br>
 * (-2) array parameter is NULL<br>
 * (-3) height parameter equals 0<br>
 * (-4) width parameter equals 0<br>
 * (-5) height parameter equals 0<br>
 * (-6) width parameter equals 0<br>
 */
int set2darrayascii(struct AsciiScreen *ascii, char* array, unsigned int row, unsigned int col, unsigned int width, unsigned int height);

/**
 * Sets cursor position
 *
 * @param ascii pointer to struct Ascii
 * @param row row number
 * @param col column number
 */
int setcursorpositionascii(struct AsciiScreen *ascii, unsigned int row, unsigned int col);

/**
 * Returns cursor position
 *
 * @param ascii pointer to struct Ascii
 * @param row row number
 * @param col column number
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL<br>
 * (-2) row parameter is NULL<br>
 * (-3) col parameter is NULL<br>
 */
int getcursorpositionascii(struct AsciiScreen *ascii, unsigned int *row, unsigned int *col);

/**
 * Prints ascii window
 *
 * @param ascii pointer to struct Ascii
 *
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) ascii parameter is NULL<br>
 * (-2) internal function error
 */
int refreshascii(struct Console* console, struct AsciiScreen* ascii);

/**
 * Returns ascii dimensions
 *
 * @param ascii pointer to structure ascii
 * @param width ascii width
 * @param height ascii height
 * @return 0 if eveything went well otherwise returns negative:<br>
 * (-1) width parameter is NULL<br>
 * (-2) height parameter is NULL<br>
 * (-3) ascii parameter is NULL
 */
int getdimensionsascii(struct AsciiScreen *ascii, unsigned int* width, unsigned int* height);


#endif //PCL_PCL_ASCII_H