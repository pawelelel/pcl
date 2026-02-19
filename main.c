#include "pcl.h"

int main(void) {
	struct Console* console = start();
	struct AsciiScreen* ascii = initascii(console);

	setfontunderlineascii(ascii);
	setstringascii(ascii, "Hello World!!!\n");
	refreshascii(console, ascii);

	getcharacter(console);

	end(console);

	system("pause");
	return 0;
}
