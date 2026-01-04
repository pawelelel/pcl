#include "pcl.h"

int main(void) {
	struct Console* console = start();
	struct AsciiScreen* ascii = initascii(console);

	setfontunderlineascii(ascii);
	setstringascii(ascii, "Hello World!!!\n");
	refreshascii(console, ascii);
	end(console);
	return 0;
}
