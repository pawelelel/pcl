#include "pcl.h"

int main(void) {
	struct Console* console = start();
	setstring(console, "Hello World!!!\n");
	refresh(console);
	end(console);

	return 0;
}