//
// Created by pawel on 03.01.2026.
//

#include "pcl_ascii.h"

#include <stdio.h>
#include <windows.h>

#include "pcl.h"

struct AsciiScreen* initascii(struct Console *console) {
	struct AsciiScreen *ascii = malloc(sizeof(struct AsciiScreen));
	if (ascii == NULL) {
		return NULL;
	}

	ascii->cursorVisible = TRUE;
	ascii->cursorstyle = BLINKING_BAR;
	ascii->cursor = 0;

	ascii->decoration.bold = FALSE;
	ascii->decoration.dim = FALSE;
	ascii->decoration.italic = FALSE;
	ascii->decoration.underline = FALSE;
	ascii->decoration.blinking = FALSE;
	ascii->decoration.strikethrough = FALSE;
	ascii->decoration.doubleunderline = FALSE;

	ascii->defaultchar = ' ';
	ascii->defaultForegroundRed = 255;
	ascii->defaultForegroundGreen = 255;
	ascii->defaultForegroundBlue = 255;
	ascii->defaultBackgroundRed = 0;
	ascii->defaultBackgroundGreen = 0;
	ascii->defaultBackgroundBlue = 0;

	ascii->foregroundRed = ascii->defaultForegroundRed;
	ascii->foregroundGreen = ascii->defaultForegroundGreen;
	ascii->foregroundBlue = ascii->defaultForegroundBlue;

	ascii->backgroundRed = ascii->defaultBackgroundRed;
	ascii->backgroundGreen = ascii->defaultBackgroundGreen;
	ascii->backgroundBlue = ascii->defaultBackgroundBlue;

	WaitForSingleObject(pclMutexHandle, INFINITE);
	ascii->width = console->width;
	ascii->height = console->height;
	ReleaseMutex(pclMutexHandle);

	/*
	 * ascii->width * ascii->height => characters
	 * (19 + 19 + 1) => colors
	 * (5 * 7) => font
	 * ascii->height => last row
	 * 4 => clear
	 */
	ascii->bufferSize = ascii->width * ascii->height * (19 + 19 + 1) * (5 * 7) + ascii->height + 4;
	ascii->buffer = malloc(sizeof (struct AsciiCell) * ascii->bufferSize);
	if (ascii->buffer == NULL) {
		free(ascii);
		return NULL;
	}

	for (int i = 0; i < ascii->bufferSize; i++) {
		ascii->buffer[i].data = ' ';
		ascii->buffer[i].foregroundRed = ascii->defaultForegroundRed;
		ascii->buffer[i].foregroundGreen = ascii->defaultForegroundGreen;
		ascii->buffer[i].foregroundBlue = ascii->defaultForegroundBlue;
		ascii->buffer[i].backgroundRed = ascii->defaultBackgroundRed;
		ascii->buffer[i].backgroundGreen = ascii->defaultBackgroundGreen;
		ascii->buffer[i].backgroundBlue = ascii->defaultBackgroundBlue;

		ascii->decoration.bold = FALSE;
		ascii->decoration.dim = FALSE;
		ascii->decoration.italic = FALSE;
		ascii->decoration.underline = FALSE;
		ascii->decoration.blinking = FALSE;
		ascii->decoration.strikethrough = FALSE;
		ascii->decoration.doubleunderline = FALSE;
	}

	ascii->outputBuffer = malloc(sizeof (char) * ascii->bufferSize);
	if (ascii->outputBuffer == NULL) {
		free(ascii->buffer);
		free(ascii);
		return NULL;
	}

	WaitForSingleObject(pclMutexHandle, INFINITE);
	console->asciiScreens[console->asciiScreensIndex] = ascii;
	console->asciiScreensIndex++;

	void* temp = realloc(console->asciiScreens,sizeof (struct AsciiScreen*) * console->asciiScreensIndex + 1);

	if (temp == NULL) {
		ReleaseMutex(pclMutexHandle);
		free(ascii->buffer);
		free(ascii);
		return NULL;
	}
	console->asciiScreens = temp;
	ReleaseMutex(pclMutexHandle);

	return ascii;
}

int setcursorstyleascii(struct AsciiScreen *ascii, int style) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->cursorstyle = style;
	return 0;
}

int unsetcursorstyleascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->cursorstyle = BLINKING_BAR;
	return 0;
}

int showcursorascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->cursorVisible = TRUE;
	return 0;
}

int hidecursorascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->cursorVisible = FALSE;
	return 0;
}

int setfontboldascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.bold = TRUE;
	return 0;
}

int unsetfontboldascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.bold = FALSE;
	return 0;
}

int getfontboldascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	return ascii->decoration.bold;
}

int setfontdimascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.dim = TRUE;
	return 0;
}

int unsetfontdimascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.dim = FALSE;
	return 0;
}

int getfontdimascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	return ascii->decoration.dim;
}

int setfontitalicascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.italic = TRUE;
	return 0;
}

int unsetfontitalicascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.italic = FALSE;
	return 0;
}

int getfontitalicascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	return ascii->decoration.italic;
}

int setfontunderlineascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.underline = TRUE;
	return 0;
}

int unsetfontunderlineascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.underline = FALSE;
	return 0;
}

int getfontunderlineascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	return ascii->decoration.underline;
}

int setfontblinkingascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.blinking = TRUE;
	return 0;
}

int unsetfontblinkingascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.blinking = FALSE;
	return 0;
}

int getfontblinkingascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	return ascii->decoration.blinking;
}

int setfontstrikethroughascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.strikethrough = TRUE;
	return 0;
}

int unsetfontstrikethroughascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.strikethrough = FALSE;
	return 0;
}

int getfontstrikethroughascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	return ascii->decoration.strikethrough;
}

int setfontdoubleunderlineascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.doubleunderline = TRUE;
	return 0;
}

int unsetfontdoubleunderlineascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->decoration.doubleunderline = FALSE;
	return 0;
}

int getfontdoubleunderlineascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	return ascii->decoration.doubleunderline;
}

int setforegroundcolorascii(struct AsciiScreen *ascii, unsigned int red, unsigned int green, unsigned int blue) {
	if (ascii == NULL) {
		return -1;
	}
	if (red > 255) {
		return -2;
	}
	if (green > 255) {
		return -3;
	}
	if (blue > 255) {
		return -4;
	}
	ascii->foregroundRed = red;
	ascii->foregroundGreen = green;
	ascii->foregroundBlue = blue;
	return 0;
}

int setbackgroundcolorascii(struct AsciiScreen *ascii, int red, int green, int blue) {
	if (ascii == NULL) {
		return -1;
	}
	if (red > 255) {
		return -2;
	}
	if (green > 255) {
		return -3;
	}
	if (blue > 255) {
		return -4;
	}
	ascii->backgroundRed = red;
	ascii->backgroundGreen = green;
	ascii->backgroundBlue = blue;
	return 0;
}

int clearforegroundcolorascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->foregroundRed = ascii->defaultForegroundRed;
	ascii->foregroundGreen = ascii->defaultForegroundGreen;
	ascii->foregroundBlue = ascii->defaultForegroundBlue;
	return 0;
}

int clearbackgroundcolorascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}
	ascii->backgroundRed = ascii->defaultBackgroundRed;
	ascii->backgroundGreen = ascii->defaultBackgroundGreen;
	ascii->backgroundBlue = ascii->defaultBackgroundBlue;
	return 0;
}

/*
 * Sets char on cursor position. Changes cursor position.
 * Dangerous! Do not waits for mutex. Partially validates data.
*/
// todo make all setchar depend on this/ make more customizable like formatted versions
void setcharasciinotsafe(struct AsciiScreen *ascii, char c) {
	switch (c) {
		case '\n': {
			ascii->cursor += ascii->width;
			ascii->cursor -= ascii->cursor % ascii->width;
			if (ascii->cursor >= ascii->width * ascii->height) {
				ascii->cursor = 0;
			}
			return;
		}
		case '\a': {
			// not supported
			c = ascii->defaultchar;
			break;
		}
		case '\b': {
			if (ascii->cursor == 0) {
				return;
			}
			ascii->cursor--;

			ascii->buffer[ascii->cursor].data = ascii->defaultchar;
			ascii->buffer[ascii->cursor].foregroundRed = ascii->foregroundRed;
			ascii->buffer[ascii->cursor].foregroundGreen = ascii->foregroundGreen;
			ascii->buffer[ascii->cursor].foregroundBlue = ascii->foregroundBlue;
			ascii->buffer[ascii->cursor].backgroundRed = ascii->backgroundRed;
			ascii->buffer[ascii->cursor].backgroundGreen = ascii->backgroundGreen;
			ascii->buffer[ascii->cursor].backgroundBlue = ascii->backgroundBlue;
			return;
		}
		case '\v': {
			ascii->cursor += ascii->width;
			if (ascii->cursor >= ascii->width * ascii->height) {
				ascii->cursor = 0;
			}
			return;
		}
		case '\r': {
			ascii->cursor -= ascii->cursor % ascii->width;
			return;
		}
		case '\f': {
			ascii->cursor = 0;
			return;
		}
		default: break;
	}

	// \n - new line
	// \a - not supported
	// \b - backspace
	// \v - vertical enter
	// \r - carrige return
	// \f - set cursor 0 0
	// \t - tab

	ascii->buffer[ascii->cursor].data = c;
	ascii->buffer[ascii->cursor].foregroundRed = ascii->foregroundRed;
	ascii->buffer[ascii->cursor].foregroundGreen = ascii->foregroundGreen;
	ascii->buffer[ascii->cursor].foregroundBlue = ascii->foregroundBlue;
	ascii->buffer[ascii->cursor].backgroundRed = ascii->backgroundRed;
	ascii->buffer[ascii->cursor].backgroundGreen = ascii->backgroundGreen;
	ascii->buffer[ascii->cursor].backgroundBlue = ascii->backgroundBlue;

	ascii->buffer[ascii->cursor].decoration = ascii->decoration;
	if (ascii->cursor < ascii->width * ascii->height - 1) {
		ascii->cursor++;
	}
}

int setcharascii(struct AsciiScreen *ascii, char c) {
	if (ascii == NULL) {
		return -1;
	}

	WaitForSingleObject(pclMutexHandle, INFINITE);

	setcharasciinotsafe(ascii, c);

	ReleaseMutex(pclMutexHandle);
	return 0;
}

int setcharformattedascii(struct AsciiScreen *ascii, char c, unsigned int foregroundRed, unsigned int foregroundGreen, unsigned int foregroundBlue,
						unsigned int backgroundRed, unsigned int backgroundGreen, unsigned int backgroundBlue,
						BOOL bold, BOOL dim, BOOL italic, BOOL underline, BOOL blinking, BOOL strikethrough, BOOL doubleunderline) {
	if (ascii == NULL) {
		return -1;
	}

	if (foregroundRed > 255) {
		return -2;
	}

	if (foregroundGreen > 255) {
		return -3;
	}

	if (foregroundBlue > 255) {
		return -4;
	}

	if (backgroundRed > 255) {
		return -5;
	}

	if (backgroundGreen > 255) {
		return -6;
	}

	if (backgroundBlue > 255) {
		return -7;
	}

	if (bold != TRUE && bold != FALSE) {
		return -8;
	}

	if (dim != TRUE && dim != FALSE) {
		return -9;
	}

	if (italic != TRUE && italic != FALSE) {
		return -10;
	}

	if (underline != TRUE && underline != FALSE) {
		return -11;
	}

	if (blinking != TRUE && blinking != FALSE) {
		return -12;
	}

	if (strikethrough != TRUE && strikethrough != FALSE) {
		return -13;
	}

	if (doubleunderline != TRUE && doubleunderline != FALSE) {
		return -14;
	}

	WaitForSingleObject(pclMutexHandle, INFINITE);

	switch (c) {
		case '\n': {
			ascii->cursor += ascii->width;
			ascii->cursor -= ascii->cursor % ascii->width;
			if (ascii->cursor >= ascii->width * ascii->height) {
				ascii->cursor = 0;
			}
			ReleaseMutex(pclMutexHandle);
			return 0;
		}
		case '\a': {
			// not supported
			c = ascii->defaultchar;
			break;
		}
		case '\b': {
			if (ascii->cursor == 0) {
				ReleaseMutex(pclMutexHandle);
				return 0;
			}
			ascii->cursor--;

			ascii->buffer[ascii->cursor].data = ascii->defaultchar;
			ascii->buffer[ascii->cursor].foregroundRed = ascii->foregroundRed;
			ascii->buffer[ascii->cursor].foregroundGreen = ascii->foregroundGreen;
			ascii->buffer[ascii->cursor].foregroundBlue = ascii->foregroundBlue;
			ascii->buffer[ascii->cursor].backgroundRed = ascii->backgroundRed;
			ascii->buffer[ascii->cursor].backgroundGreen = ascii->backgroundGreen;
			ascii->buffer[ascii->cursor].backgroundBlue = ascii->backgroundBlue;
			ReleaseMutex(pclMutexHandle);
			return 0;
		}
		case '\v': {
			ascii->cursor += ascii->width;
			if (ascii->cursor >= ascii->width * ascii->height) {
				ascii->cursor = 0;
			}
			ReleaseMutex(pclMutexHandle);
			return 0;
		}
		case '\r': {
			ascii->cursor -= ascii->cursor % ascii->width;
			ReleaseMutex(pclMutexHandle);
			return 0;
		}
		case '\f': {
			ascii->cursor = 0;
			ReleaseMutex(pclMutexHandle);
			return 0;
		}
		default: break;
	}

	// v \n - new line
	// v \a - not supported
	// v \b - backspace
	// v \v - vertical enter
	// v \r - carrige return
	// v \f - set cursor 0 0
	// v \t - tab

	ascii->buffer[ascii->cursor].data = c;
	ascii->buffer[ascii->cursor].foregroundRed = foregroundRed;
	ascii->buffer[ascii->cursor].foregroundGreen = foregroundGreen;
	ascii->buffer[ascii->cursor].foregroundBlue = foregroundBlue;
	ascii->buffer[ascii->cursor].backgroundRed = backgroundRed;
	ascii->buffer[ascii->cursor].backgroundGreen = backgroundGreen;
	ascii->buffer[ascii->cursor].backgroundBlue = backgroundBlue;

	ascii->buffer[ascii->cursor].decoration.bold = bold;
	ascii->buffer[ascii->cursor].decoration.dim = dim;
	ascii->buffer[ascii->cursor].decoration.italic = italic;
	ascii->buffer[ascii->cursor].decoration.underline = underline;
	ascii->buffer[ascii->cursor].decoration.blinking = blinking;
	ascii->buffer[ascii->cursor].decoration.strikethrough = strikethrough;
	ascii->buffer[ascii->cursor].decoration.doubleunderline = doubleunderline;

	if (ascii->cursor != ascii->width * ascii->height) {
		ascii->cursor++;
	}
	ReleaseMutex(pclMutexHandle);
	return 0;
}

int setcharformattedcursorascii(struct AsciiScreen *ascii, unsigned int row, unsigned int col, char c, unsigned int foregroundRed, unsigned int foregroundGreen, unsigned int foregroundBlue,
						unsigned int backgroundRed, unsigned int backgroundGreen, unsigned int backgroundBlue,
						BOOL bold, BOOL dim, BOOL italic, BOOL underline, BOOL blinking, BOOL strikethrough, BOOL doubleunderline) {
	if (ascii == NULL) {
		return -1;
	}

	if (row >= ascii->height) {
		return -2;
	}

	if (col >= ascii->width) {
		return -3;
	}

	if (foregroundRed > 255) {
		return -4;
	}

	if (foregroundGreen > 255) {
		return -5;
	}

	if (foregroundBlue > 255) {
		return -6;
	}

	if (backgroundRed > 255) {
		return -7;
	}

	if (backgroundGreen > 255) {
		return -8;
	}

	if (backgroundBlue > 255) {
		return -9;
	}

	if (bold != TRUE && bold != FALSE) {
		return -10;
	}

	if (dim != TRUE && dim != FALSE) {
		return -11;
	}

	if (italic != TRUE && italic != FALSE) {
		return -12;
	}

	if (underline != TRUE && underline != FALSE) {
		return -13;
	}

	if (blinking != TRUE && blinking != FALSE) {
		return -14;
	}

	if (strikethrough != TRUE && strikethrough != FALSE) {
		return -15;
	}

	if (doubleunderline != TRUE && doubleunderline != FALSE) {
		return -16;
	}

	switch (c) {
		case '\n':
		case '\a':
		case '\b':
		case '\v':
		case '\r':
		case '\f': {
			// all special characters are not supported in that functon variant
			return 0;
		}
		default: break;
	}

	// \n - new line
	// \a - not supported (alert/bell)
	// \b - backspace
	// \v - vertical enter
	// \r - carrige return
	// \f - set cursor 0 0
	// \t - tab

	WaitForSingleObject(pclMutexHandle, INFINITE);
	unsigned int cursorpos = ascii->cursor;
	ascii->cursor = col + row * ascii->width;

	ascii->buffer[ascii->cursor].data = c;
	ascii->buffer[ascii->cursor].foregroundRed = foregroundRed;
	ascii->buffer[ascii->cursor].foregroundGreen = foregroundGreen;
	ascii->buffer[ascii->cursor].foregroundBlue = foregroundBlue;
	ascii->buffer[ascii->cursor].backgroundRed = backgroundRed;
	ascii->buffer[ascii->cursor].backgroundGreen = backgroundGreen;
	ascii->buffer[ascii->cursor].backgroundBlue = backgroundBlue;

	ascii->buffer[ascii->cursor].decoration.bold = bold;
	ascii->buffer[ascii->cursor].decoration.dim = dim;
	ascii->buffer[ascii->cursor].decoration.italic = italic;
	ascii->buffer[ascii->cursor].decoration.underline = underline;
	ascii->buffer[ascii->cursor].decoration.blinking = blinking;
	ascii->buffer[ascii->cursor].decoration.strikethrough = strikethrough;
	ascii->buffer[ascii->cursor].decoration.doubleunderline = doubleunderline;
	ascii->cursor = cursorpos;
	ReleaseMutex(pclMutexHandle);
	return 0;
}

BOOL validateformatstringforsetstringformattedascii(char *format) {
	if (format == NULL) {
		return FALSE;
	}

	const size_t length = strlen(format);

	BOOL openToken = FALSE;
	for (int i = 0; i < length; ++i) {
		char token = format[i];

		// variable check
		if (token == '%') {
			// take next token
			i++;
			token = format[i];
			openToken = TRUE;
			switch (token) {
				case '%':
				case 's':
				case 'd':
				case 'h':
				case 'c':
				case 'f': {
					// standard one letter tokens
					openToken = FALSE;
					break;
				}
				case 'l': {
					openToken = FALSE; // %l
					if (i + 1 < length) {
						switch (format[i + 1]) {
							case 'l': {
								i++; // %ll
								if (i + 1 < length && format[i + 1] == 'f') {
									i++; // %llf
									break;
								}
								break;
							}
							case 'f': {
								i++; // %lf
								break;
							}
							default: ;
						}
					}
					break;
				}
				case 'u': { // unsigned
					if (i + 1 < length) {
						switch (format[i + 1]) {
							case 'l': {
								openToken = FALSE; // %ul
								i++;
								if (i + 1 < length && format[i + 1] == 'l') {
									i++; // %ull
								}
								break;
							}
							case 'd':
							case 'h': {
								openToken = FALSE;
								i++; // %ud and %uh
								break;
							}
							default: ;
						}
					}
					break;
				}
				case '.': { // floating point with precision
					while (TRUE) {
						if (i + 1 < length && isdigit(format[i + 1])) {
							i++; // precision
						}
						else {
							break;
						}
					}
					if (i + 1 < length) {
						switch (format[i + 1]) {
							case 'f': {
								openToken = FALSE; // %.<int>f
								i++;
								break;
							}
							case 'l': {
								i++; // %.<int>lf
								if (i + 1 < length && format[i + 1] == 'l') {
									openToken = FALSE; // %.<int>llf
									i++;
								}
								break;
							}
							default: break;
						}
					}
					break;
				}
				default: ;
			}
		}
		else if (token == '@') {
			// style check
			i++;
			token = format[i];
			openToken = TRUE;

			// font tokens
			switch (token) {
				case 'b':
				case 'd':
				case 'i':
				case 'l':
				case 's':
				case 'c':
				case '@': {
					// standard one letter tokens
					openToken = FALSE;
					break;
				}
				case 'u': {
					openToken = FALSE;
					if (i + 1 < length && format[i + 1] == 'u') {
						i++;
					}
					break;
				}
				case 'r': {
					if (i + 1 < length) {
						i++;
						switch (format[i]) {
							case 'b':
							case 'd':
							case 'i':
							case 'l':
							case 's':{
								// standard one letter tokens remover
								openToken = FALSE;
								break;
							}
							case 'u': {
								openToken = FALSE;
								if (i + 1 < length && format[i + 1] == 'u') {
									i++;
								}
								break;
							}
						}
					}
				}
				default: break;
			}

			// color tokens
			int semicolons = 0;
			while (TRUE) {
				if (i + 1 < length) {
					i++;
					if (format[i] == ';') {
						semicolons++;
						continue;
					}
					if (!isdigit(format[i])) {
						break;
					}
				}
				else {
					break;
				}
			}

			if (semicolons == 2) {
				if (format[i] == 'f' || format[i] == 'b') {
					openToken = FALSE;
					i++;
				}
				else {
					return FALSE;
				}
			}
			else {
				return FALSE;
			}
		}
		// all other tokens are good
	}
	if (openToken) {
		// format string suddenly ends with unfinished token
		return FALSE;
	}

	// format string is valid
	return TRUE;
}

/*
 * Sets char on specified cursor position.Do not changes cursor position.
 * Dangerous! Do not waits for mutex. Partially validates data.
*/
int setcharcursorasciinotsafe(struct AsciiScreen *ascii, char c, unsigned int row, unsigned int col) {
	unsigned int cursorpos = ascii->cursor;
	ascii->cursor = col + row * ascii->width;

	setcharasciinotsafe(ascii, c);

	ascii->cursor = cursorpos;
	return 0;
}

int setcharcursorascii(struct AsciiScreen *ascii, char c, unsigned int row, unsigned int col) {
	if (ascii == NULL) {
		return -1;
	}

	if (row >= ascii->height) {
		return -2;
	}

	if (col >= ascii->width) {
		return -3;
	}

	WaitForSingleObject(pclMutexHandle, INFINITE);

	setcharcursorasciinotsafe(ascii, c, row, col);

	ReleaseMutex(pclMutexHandle);

	return 0;
}

/*
 * Actual printf like logic. Does not provide any data validation
*/
int setstringformattedasciinotsafe(struct AsciiScreen *ascii, char *format, va_list args) {
	// valid tokens sorted by length
	char* validtokens[] = {
		"%ull", "%ll", "%uh", "%ud",
		"%ul", "%c", "%h", "%d", "%l", "%s", "%%"
	};

	int tokenssize = sizeof(validtokens) / sizeof(validtokens[0]);

	int length = 0;

	while (*format) {

		// standard printf behaviour
		if (*format == '%') {
			format++;

			// floating point numbers printing
			char* precisionstr = malloc(20 * sizeof(char));
			int precisionstrsize = 10;

			if (*format == '.') {
				// precision detected
				// float match

				format++;

				int i = 0;
				while (isdigit(*format)) {
					precisionstr[i] = *format;
					i++;
					if (i == precisionstrsize) {
						precisionstrsize *= 2;
						if (!realloc(precisionstr, precisionstrsize)) {
							// error
							free(precisionstr);
							va_end(args);
							return -4;
						}
					}
					format++;
				}
				precisionstr[i] = '\0';
			}
			else {
				precisionstr[0] = '5';
				precisionstr[1] = '\0';
			}

			// lets handle that float
			// float
			if (strncmp(format, "f", 1) == 0) {
				format++;
				float f = va_arg(args, double);

				char* formatstr = malloc((precisionstrsize + 4) * sizeof(char));
				memset(formatstr, 0, (precisionstrsize + 4) * sizeof(char));
				formatstr[0] = '%';
				formatstr[1] = '.';
				strcat(formatstr, precisionstr);
				strcat(formatstr, "f\0");

				int size = snprintf(NULL, 0, formatstr, f);
				char* memory = malloc(size * sizeof(char));
				snprintf(memory, size, formatstr, f);
				setstringascii(ascii, memory);
				free(memory);
				length += size;
				free(formatstr);
			}
			// double
			if (strncmp(format, "lf", 2) == 0) {
				format += 2;
				double lf = va_arg(args, double);

				char* formatstr = malloc((precisionstrsize + 4) * sizeof(char));
				memset(formatstr, 0, (precisionstrsize + 4) * sizeof(char));
				formatstr[0] = '%';
				formatstr[1] = '.';
				strcat(formatstr, precisionstr);
				strcat(formatstr, "lf\0");

				int size = snprintf(NULL, 0, formatstr, lf);
				char* memory = malloc(size * sizeof(char));
				snprintf(memory, size, formatstr, lf);
				setstringascii(ascii, memory);
				free(memory);
				length += size;
				free(formatstr);
			}
			// long double
			if (strncmp(format, "llf", 3) == 0) {
				format += 3;
				long double llf = va_arg(args, long double);

				char* formatstr = malloc((precisionstrsize + 4) * sizeof(char));
				memset(formatstr, 0, (precisionstrsize + 4) * sizeof(char));
				formatstr[0] = '%';
				formatstr[1] = '.';
				strcat(formatstr, precisionstr);
				strcat(formatstr, "Lf\0");

				int size = snprintf(NULL, 0, formatstr, llf);
				char* memory = malloc(size * sizeof(char));
				snprintf(memory, size, formatstr, llf);
				setstringascii(ascii, memory);
				free(memory);
				length += size;
				free(formatstr);
			}
			free(precisionstr);

			// standard tokens
			for (int i = 0; i < tokenssize; i++) {
				char* token = validtokens[i] + 1;
				size_t tokensize = strlen(token);
				if (strncmp(format, token, tokensize) == 0) {
					// match
					format += tokensize;
					if (strcmp(token, "%") == 0) {
						setcharasciinotsafe(ascii, '%');
						length++;
						break;
					}
					if (strcmp(token, "s") == 0) {
						char* s = va_arg(args, char*);
						setstringascii(ascii, s);
						length += (int)strlen(s);
						break;
					}
					if (strcmp(token, "l") == 0) {
						long l = va_arg(args, long);
						int len = snprintf(NULL, 0, "%ld", l);
						char* longstring = malloc((len+1) * sizeof(char));
						snprintf(longstring, len+1, "%ld", l);
						setstringascii(ascii, longstring);
						length += (int)strlen(longstring);
						free(longstring);
						break;
					}
					if (strcmp(token, "d") == 0) {
						int d = va_arg(args, int);
						int len = snprintf(NULL, 0, "%d", d);
						char* intstr = malloc((len+1) * sizeof(char));
						snprintf(intstr, len+1, "%d", d);
						setstringascii(ascii, intstr);
						length += (int)strlen(intstr);
						free(intstr);
						break;
					}
					if (strcmp(token, "h") == 0) {
						short h = va_arg(args, int);
						int len = snprintf(NULL, 0, "%hd", h);
						char* shortstr = malloc((len+1) * sizeof(char));
						snprintf(shortstr, len+1, "%hd", h);
						setstringascii(ascii, shortstr);
						length += (int)strlen(shortstr);
						free(shortstr);
						break;
					}
					if (strcmp(token, "c") == 0) {
						const char c = va_arg(args, int);
						setcharasciinotsafe(ascii, c);
						length++;
						break;
					}
					if (strcmp(token, "ul") == 0) {
						unsigned long ul = va_arg(args, unsigned long);
						int len = snprintf(NULL, 0, "%lu", ul);
						char* unsignedlongstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedlongstr, len+1, "%lu", ul);
						setstringascii(ascii, unsignedlongstr);
						length += (int)strlen(unsignedlongstr);
						free(unsignedlongstr);
						break;
					}
					if (strcmp(token, "ud") == 0) {
						unsigned int ud = va_arg(args, unsigned int);
						int len = snprintf(NULL, 0, "%u", ud);
						char* unsignedlongstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedlongstr, len+1, "%u", ud);
						setstringascii(ascii, unsignedlongstr);
						length += (int)strlen(unsignedlongstr);
						free(unsignedlongstr);
						break;
					}
					if (strcmp(token, "uh") == 0) {
						unsigned short uh = va_arg(args, int);
						int len = snprintf(NULL, 0, "%hu", uh);
						char* unsignedshortstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedshortstr, len+1, "%hu", uh);
						setstringascii(ascii, unsignedshortstr);
						length += (int)strlen(unsignedshortstr);
						free(unsignedshortstr);
						break;
					}
					if (strcmp(token, "ll") == 0) {
						long long ll = va_arg(args, long long);
						int len = snprintf(NULL, 0, "%lld", ll);
						char* unsignedlongstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedlongstr, len+1, "%lld", ll);
						setstringascii(ascii, unsignedlongstr);
						length += (int)strlen(unsignedlongstr);
						free(unsignedlongstr);
						break;
					}
					if (strcmp(token, "ull") == 0) {
						unsigned long long ull = va_arg(args, unsigned long long );
						int len = snprintf(NULL, 0, "%llu", ull);
						char* unsignedlongstr = malloc((len+1) * sizeof(char));
						snprintf(unsignedlongstr, len+1, "%llu", ull);
						setstringascii(ascii, unsignedlongstr);
						length += (int)strlen(unsignedlongstr);
						free(unsignedlongstr);
						break;
					}
				}
			}
			continue;
		}

		// font styles
		/*
		 * %ull unsigned long long
		 * %ll long long
		 * %uh unsigned short
		 * %ud unsigned int
		 * %ul unsigned long
		 * %c char
		 * %h short
		 * %d int
		 * %l long
		 * %s string
		 * %% '%' character
		 *
		 * Possible styles
		 * @<number>;<number>;<number>f foreground color
		 * @<number>;<number>;<number>b background color
		 * @b bold
		 * @rb remove bold
		 *
		 * @d dim
		 * @rd remove dim
		 *
		 * @i italic
		 * @ri remove italic
		 *
		 * @u underline
		 * @ru remove underline
		 *
		 * @l blinking
		 * @rl remove blinking
		 *
		 * @s strikethrough
		 * @rs remove strikethrough
		 *
		 * @uu doubleunderline
		 * @ruu remove doubleunderline
		 *
		 * @c clear all
		 *
		 * @@ at character
		*/
		if (*format == '@') {
			format++;

			if (*format == '@') {
				format++;
				setcharasciinotsafe(ascii, '@');
				length++;
			}
			else if (*format == 'c') {
				format++;

				ascii->buffer[ascii->cursor].decoration.bold = FALSE;
				ascii->buffer[ascii->cursor].decoration.dim = FALSE;
				ascii->buffer[ascii->cursor].decoration.italic = FALSE;
				ascii->buffer[ascii->cursor].decoration.underline = FALSE;
				ascii->buffer[ascii->cursor].decoration.blinking = FALSE;
				ascii->buffer[ascii->cursor].decoration.strikethrough = FALSE;
				ascii->buffer[ascii->cursor].decoration.doubleunderline = FALSE;

				ascii->foregroundRed = ascii->defaultForegroundRed;
				ascii->foregroundGreen = ascii->defaultForegroundGreen;
				ascii->foregroundBlue = ascii->defaultForegroundBlue;
			}
			else if (*format == 'b') {
				format++;
				ascii->decoration.bold = TRUE;
			}
			else if (strncmp(format, "rb", 2) == 0) {
				format += 2;
				ascii->decoration.bold = FALSE;
			}
			else if (*format == 'd') {
				format++;
				ascii->decoration.dim = TRUE;
			}
			else if (strncmp(format, "rd", 2) == 0) {
				format += 2;
				ascii->decoration.dim = FALSE;
			}
			else if (*format == 'i') {
				format++;
				ascii->decoration.italic = TRUE;
			}
			else if (strncmp(format, "ri", 2) == 0) {
				format += 2;
				ascii->decoration.italic = FALSE;
			}
			else if (*format == 'u') {
				format++;
				ascii->decoration.underline = TRUE;
			}
			else if (strncmp(format, "ru", 2) == 0) {
				format += 2;
				ascii->decoration.underline = FALSE;
			}
			else if (*format == 'l') {
				format++;
				ascii->decoration.blinking = TRUE;
			}
			else if (strncmp(format, "rl", 2) == 0) {
				format += 2;
				ascii->decoration.blinking = FALSE;
			}
			else if (*format == 's') {
				format++;
				ascii->decoration.strikethrough = TRUE;
			}
			else if (strncmp(format, "rs", 2) == 0) {
				format += 2;
				ascii->decoration.strikethrough = FALSE;
			}
			else if (strncmp(format, "uu", 2) == 0) {
				format += 2;
				ascii->decoration.doubleunderline = TRUE;
			}
			else if (strncmp(format, "ruu", 3) == 0) {
				format += 3;
				ascii->decoration.doubleunderline = FALSE;
			}
			else if (isdigit(*format)) {
				int r, g, b;
				r = g = b = 0;
				r = strtol(format, &format, 10);
				format++;
				g = strtol(format, &format, 10);
				format++;
				b = strtol(format, &format, 10);
				if (*format == 'f') {

					ascii->foregroundRed = r;
					ascii->foregroundGreen = g;
					ascii->foregroundBlue = b;

				}
				else if (*format == 'b') {

					ascii->backgroundRed = r;
					ascii->backgroundGreen = g;
					ascii->backgroundBlue = b;

				}
				format++;
			}

			continue;
		}

		// standard non-controlling character
		setcharasciinotsafe(ascii, *format);
		length++;
		format++;
	}

	return length;
}

int setstringformattedascii(struct AsciiScreen *ascii, char *format, ...) {
	if (ascii == NULL) {
		return -1;
	}

	if (format == NULL) {
		return -2;
	}

	if (!validateformatstringforsetstringformattedascii(format)) {
		return -3;
	}

	va_list args;
	va_start(args, format);
	int length = setstringformattedasciinotsafe(ascii, format, args);

	if (length < 0) {
		va_end(args);
		return length;
	}

	va_end(args);
	return length;
}

int setstringformattedcursorascii(struct AsciiScreen *ascii, int row, int col, char* format, ...) {
	if (ascii == NULL) {
		return -1;
	}

	if (row < 0) {
		return -2;
	}

	if (col < 0) {
		return -3;
	}

	if (format == NULL) {
		return -4;
	}

	if (row >= ascii->height) {

		return -5;
	}
	if (col >= ascii->width) {

		return -6;
	}

	if (!validateformatstringforsetstringformattedascii(format)) {
		return -7;
	}

	unsigned int cursor = ascii->cursor;
	ascii->cursor = col + row * ascii->width;

	va_list args;
	va_start(args, format);
	int length = setstringformattedasciinotsafe(ascii, format, args);

	if (length < 0) {
		va_end(args);
		return length;
	}

	va_end(args);

	ascii->cursor = cursor;

	return length;
}

int setstringcursorascii(struct AsciiScreen *ascii, char *string, int row, int col) {
	if (ascii == NULL) {
		return -1;
	}

	if (row < 0) {
		return -2;
	}

	if (col < 0) {
		return -3;
	}

	if (string == NULL) {
		return -4;
	}

	if (row >= ascii->height) {

		return -5;
	}
	if (col > ascii->width) {

		return -6;
	}

	const unsigned int cursor = ascii->cursor;
	ascii->cursor = col + row * ascii->width;

	setstringascii(ascii, string);

	ascii->cursor = cursor;

	return 0;
}

int clearascii(struct AsciiScreen *ascii) {
	if (ascii == NULL) {
		return -1;
	}

	WaitForSingleObject(pclMutexHandle, INFINITE);
	for (int i = 0; i < ascii->height * ascii->width; ++i) {
		ascii->buffer[i].data = ascii->defaultchar;
		ascii->buffer[i].foregroundRed = ascii->defaultForegroundRed;
		ascii->buffer[i].foregroundGreen = ascii->defaultForegroundGreen;
		ascii->buffer[i].foregroundBlue = ascii->defaultForegroundBlue;
		ascii->buffer[i].backgroundRed = ascii->defaultBackgroundRed;
		ascii->buffer[i].backgroundGreen = ascii->defaultBackgroundGreen;
		ascii->buffer[i].backgroundBlue = ascii->defaultBackgroundBlue;


		ascii->buffer[i].decoration.bold = FALSE;
		ascii->buffer[i].decoration.dim = FALSE;
		ascii->buffer[i].decoration.italic = FALSE;
		ascii->buffer[i].decoration.underline = FALSE;
		ascii->buffer[i].decoration.blinking = FALSE;
		ascii->buffer[i].decoration.strikethrough = FALSE;
		ascii->buffer[i].decoration.doubleunderline = FALSE;
	}
	ascii->cursor = 0;
	ReleaseMutex(pclMutexHandle);

	return 0;
}

int fillascii(struct AsciiScreen *ascii, char c, unsigned int foregroundRed, unsigned int foregroundGreen,
	unsigned int foregroundBlue, unsigned int backgroundRed, unsigned int backgroundGreen, unsigned int backgroundBlue,
	BOOL bold, BOOL dim, BOOL italic, BOOL underline, BOOL blinking, BOOL strikethrough, BOOL doubleunderline) {
	if (ascii == NULL) {
		return -1;
	}

	if (foregroundRed > 255) {
		return -2;
	}

	if (foregroundGreen > 255) {
		return -3;
	}

	if (foregroundBlue > 255) {
		return -4;
	}

	if (backgroundRed > 255) {
		return -5;
	}

	if (backgroundGreen > 255) {
		return -6;
	}

	if (backgroundBlue > 255) {
		return -7;
	}

	if (bold != TRUE && bold != FALSE) {
		return -8;
	}

	if (dim != TRUE && dim != FALSE) {
		return -9;
	}

	if (italic != TRUE && italic != FALSE) {
		return -10;
	}

	if (underline != TRUE && underline != FALSE) {
		return -11;
	}

	if (blinking != TRUE && blinking != FALSE) {
		return -12;
	}

	if (strikethrough != TRUE && strikethrough != FALSE) {
		return -13;
	}

	if (doubleunderline != TRUE && doubleunderline != FALSE) {
		return -14;
	}

	WaitForSingleObject(pclMutexHandle, INFINITE);
	for (int i = 0; i < ascii->height * ascii->width; ++i) {
		ascii->buffer[i].data = c;
		ascii->buffer[i].foregroundRed = foregroundRed;
		ascii->buffer[i].foregroundGreen = foregroundGreen;
		ascii->buffer[i].foregroundBlue = foregroundBlue;
		ascii->buffer[i].backgroundRed = backgroundRed;
		ascii->buffer[i].backgroundGreen = backgroundGreen;
		ascii->buffer[i].backgroundBlue = backgroundBlue;

		ascii->buffer[i].decoration.bold = bold;
		ascii->buffer[i].decoration.dim = dim;
		ascii->buffer[i].decoration.italic = italic;
		ascii->buffer[i].decoration.underline = underline;
		ascii->buffer[i].decoration.blinking = blinking;
		ascii->buffer[i].decoration.strikethrough = strikethrough;
		ascii->buffer[i].decoration.doubleunderline = doubleunderline;
	}
	ascii->cursor = 0;
	ReleaseMutex(pclMutexHandle);

	return 0;
}

int fillcharascii(struct AsciiScreen *ascii, char c) {
	if (ascii == NULL) {
		return -1;
	}

	WaitForSingleObject(pclMutexHandle, INFINITE);
	for (int i = 0; i < ascii->height * ascii->width; ++i) {
		ascii->buffer[i].data = c;
		ascii->buffer[i].foregroundRed = ascii->foregroundRed;
		ascii->buffer[i].foregroundGreen = ascii->foregroundGreen;
		ascii->buffer[i].foregroundBlue = ascii->foregroundBlue;
		ascii->buffer[i].backgroundRed = ascii->backgroundRed;
		ascii->buffer[i].backgroundGreen = ascii->backgroundGreen;
		ascii->buffer[i].backgroundBlue = ascii->backgroundBlue;

		ascii->buffer[i].decoration = ascii->decoration;
	}
	ReleaseMutex(pclMutexHandle);
	ascii->cursor = 0;

	return 0;
}

int set2darrayascii(struct AsciiScreen *ascii, char* array, unsigned int row, unsigned int col, unsigned int width, unsigned int height) {
	if (ascii == NULL) {
		return -1;
	}

	if (array == NULL) {
		return -2;
	}

	if (height == 0) {
		return -3;
	}

	if (width == 0) {
		return -4;
	}

	/*
	// if we want to show only arrays that can be showed entirely
	if (height + row > ascii->height) {
		return -5;
	}

	if (width + col > ascii->width) {
		return -6;
	}*/

	WaitForSingleObject(pclMutexHandle, INFINITE);
	for (int i = 0; i < height * width; ++i) {
		unsigned int r = row + i / width;
		unsigned int c = col + i % width;
		if (r < ascii->height && c < ascii->width) {
			setcharcursorasciinotsafe(ascii, array[i], r, c);
		}
	}
	ReleaseMutex(pclMutexHandle);
	return 0;
}

int setcursorpositionascii(struct AsciiScreen *ascii, unsigned int row, unsigned int col) {
	if (ascii == NULL) {
		return -1;
	}

	if (row >= ascii->height) {

		return -2;
	}
	if (col >= ascii->width) {

		return -3;
	}

	ascii->cursor = col + row * ascii->width;

	return 0;
}

int getcursorpositionascii(struct AsciiScreen *ascii, unsigned int *row, unsigned int *col) {
	if (ascii == NULL) {
		return -1;
	}
	if (row == NULL) {
		return -2;
	}
	if (col == NULL) {
		return -3;
	}
	*row = ascii->cursor / ascii->width;
	*col = ascii->cursor % ascii->width;

	return 0;
}

int refreshascii(struct Console* console, struct AsciiScreen* ascii) {
	if (ascii == NULL) {
		return -1;
	}

	// init
	int place = 0;

	// set cursor position to top left
	char buff[4];
	int add = sprintf(buff, "\x1B[H");

	// init
	WaitForSingleObject(pclMutexHandle, INFINITE);
	memset(ascii->outputBuffer, 0, ascii->bufferSize);
	memcpy(&ascii->outputBuffer[place], buff, add);

	place += add;

	unsigned int foregroundRed = 255;
	unsigned int foregroundGreen = 255;
	unsigned int foregroundBlue = 255;
	unsigned int backgroundRed = 0;
	unsigned int backgroundGreen = 0;
	unsigned int backgroundBlue = 0;

	struct Decoration current;
	current.bold = FALSE;
	current.italic = FALSE;
	current.dim = FALSE;
	current.underline = FALSE;
	current.blinking = FALSE;
	current.strikethrough = FALSE;
	current.doubleunderline = FALSE;

	for (int i = 0; i < ascii->height * ascii->width; ++i) {
		if (i != 0 && i % ascii->width == 0) {
			ascii->outputBuffer[place] = '\n';
			place++;
		}

		struct AsciiCell cell = ascii->buffer[i];

		if (current.bold != cell.decoration.bold) {
			if (cell.decoration.bold == TRUE) {
				current.bold = TRUE;
				char boldstr[4] = "\x1B[1m";
				memcpy(&ascii->outputBuffer[place], boldstr, 4);
				place += 4;
			}
			else {
				current.bold = FALSE;
				char boldstr[5] = "\x1B[22m";
				memcpy(&ascii->outputBuffer[place], boldstr, 5);
				place += 5;
			}
		}

		if (current.dim != cell.decoration.dim) {
			if (cell.decoration.dim == TRUE) {
				current.dim = TRUE;
				char dimstr[4] = "\x1B[2m";
				memcpy(&ascii->outputBuffer[place], dimstr, 4);
				place += 4;
			}
			else {
				current.dim = FALSE;
				char dimstr[5] = "\x1B[22m";
				memcpy(&ascii->outputBuffer[place], dimstr, 5);
				place += 5;
			}
		}

		if (current.italic != cell.decoration.italic) {
			if (cell.decoration.italic == TRUE) {
				current.italic = TRUE;
				char italicstr[4] = "\x1B[3m";
				memcpy(&ascii->outputBuffer[place], italicstr, 4);
				place += 4;
			}
			else {
				current.italic = FALSE;
				char italicstr[5] = "\x1B[23m";
				memcpy(&ascii->outputBuffer[place], italicstr, 5);
				place += 5;
			}
		}

		if (current.underline != cell.decoration.underline) {
			if (cell.decoration.underline == TRUE) {
				current.underline = TRUE;
				char underlinestr[4] = "\x1B[4m";
				memcpy(&ascii->outputBuffer[place], underlinestr, 4);
				place += 4;
			}
			else {
				current.underline = FALSE;
				char underlinestr[5] = "\x1B[24m";
				memcpy(&ascii->outputBuffer[place], underlinestr, 5);
				place += 5;
			}
		}

		if (current.blinking != cell.decoration.blinking) {
			if (cell.decoration.blinking == TRUE) {
				current.blinking = TRUE;
				char blinkingstr[4] = "\x1B[5m";
				memcpy(&ascii->outputBuffer[place], blinkingstr, 4);
				place += 4;
			}
			else {
				current.blinking = FALSE;
				char blinkingstr[5] = "\x1B[25m";
				memcpy(&ascii->outputBuffer[place], blinkingstr, 5);
				place += 5;
			}
		}

		if (current.strikethrough != cell.decoration.strikethrough) {
			if (cell.decoration.strikethrough == TRUE) {
				current.strikethrough = TRUE;
				char strikethroughstr[4] = "\x1B[9m";
				memcpy(&ascii->outputBuffer[place], strikethroughstr, 4);
				place += 4;
			}
			else {
				current.strikethrough = FALSE;
				char strikethroughstr[5] = "\x1B[29m";
				memcpy(&ascii->outputBuffer[place], strikethroughstr, 5);
				place += 5;
			}
		}

		if (current.doubleunderline != cell.decoration.doubleunderline) {
			if (cell.decoration.doubleunderline == TRUE) {
				current.doubleunderline = TRUE;
				char doubleunderlinestr[5] = "\x1B[21m";
				memcpy(&ascii->outputBuffer[place], doubleunderlinestr, 5);
				place += 5;
			}
			else {
				current.doubleunderline = FALSE;
				char doubleunderlinestr[5] = "\x1B[24m";
				memcpy(&ascii->outputBuffer[place], doubleunderlinestr, 5);
				place += 5;
			}
		}

		if (foregroundRed != cell.foregroundRed || foregroundGreen != cell.foregroundGreen || foregroundBlue != cell.foregroundBlue) {
			foregroundRed = cell.foregroundRed;
			foregroundGreen = cell.foregroundGreen;
			foregroundBlue = cell.foregroundBlue;
			char colorbuff[19];
			add = sprintf(colorbuff, "\x1B[38;2;%d;%d;%dm", foregroundRed, foregroundGreen, foregroundBlue);
			memcpy(&ascii->outputBuffer[place], colorbuff, add);
			place += add;
		}
		if (backgroundRed != cell.backgroundRed || backgroundGreen != cell.backgroundGreen || backgroundBlue != cell.backgroundBlue) {
			backgroundRed = cell.backgroundRed;
			backgroundGreen = cell.backgroundGreen;
			backgroundBlue = cell.backgroundBlue;
			char colorbuff[19];
			add = sprintf(colorbuff, "\x1B[48;2;%d;%d;%dm", backgroundRed, backgroundGreen, backgroundBlue);
			memcpy(&ascii->outputBuffer[place], colorbuff, add);
			place += add;
		}
		ascii->outputBuffer[place] = cell.data;
		place++;
	}

	ascii->outputBuffer[place] = '\0';
	WriteConsoleA(console->outputHandle, ascii->outputBuffer, strlen(ascii->outputBuffer), NULL, NULL);

	// setting curosor position
	unsigned int row = ascii->cursor / ascii->width + 1;
	unsigned int col = ascii->cursor % ascii->width + 1;
	char position[30];
	sprintf(position, "\x1B[%d;%dH", row, col);
	WriteConsoleA(console->outputHandle, position, strlen(position), NULL, NULL);

	char* cursorstyle;
	switch (ascii->cursorstyle) {
		case BLINKING_BLOCK: {
			cursorstyle = "\x1B[1 q";
			break;
		}
		case STEADY_BLOCK: {
			cursorstyle = "\x1B[2 q";
			break;
		}
		case BLINKING_UNDERLINE: {
			cursorstyle = "\x1B[3 q";
			break;
		}
		case STEADY_UNDERLINE: {
			cursorstyle = "\x1B[4 q";
			break;
		}
		default:
		case BLINKING_BAR: {
			cursorstyle = "\x1B[5 q";
			break;
		}
		case STEADY_BAR: {
			cursorstyle = "\x1B[6 q";
			break;
		}
	}

	WriteConsoleA(console->outputHandle, cursorstyle, strlen(cursorstyle), NULL, NULL);

	if (ascii->cursorVisible) {
		char* show = "\x1B[?25h";
		WriteConsoleA(console->outputHandle, show, strlen(show), NULL, NULL);
	}
	else {
		char* hide = "\x1B[?25l";
		WriteConsoleA(console->outputHandle, hide, strlen(hide), NULL, NULL);
	}

	ReleaseMutex(pclMutexHandle);

	return 0;
}

int setstringascii(struct AsciiScreen *ascii, char *string) {
	if (ascii == NULL) {
		return -1;
	}
	if (string == NULL) {
		return -2;
	}
	const size_t size = strlen(string);
	WaitForSingleObject(pclMutexHandle, INFINITE);
	for (int i = 0; i < size; ++i) {
		setcharasciinotsafe(ascii, string[i]);
	}
	ReleaseMutex(pclMutexHandle);
	return 0;
}

int getdimensionsascii(struct AsciiScreen *ascii, unsigned int* width, unsigned int* height) {
	if (width == NULL) {
		return -1;
	}
	if (height == NULL) {
		return -2;
	}
	if (ascii == NULL) {
		*width = 0;
		*height = 0;
		return -3;
	}

	WaitForSingleObject(pclMutexHandle, INFINITE);
	*width = ascii->width;
	*height = ascii->height;
	ReleaseMutex(pclMutexHandle);

	return 0;
}
