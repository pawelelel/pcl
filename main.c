#include <stdio.h>

#include "pcl.h"

int main(void) {
	// test 1
	printf("\x1b[38;2;255;165;0mHello, True Color!\x1b[0m\n");
	system("pause");

	// test 2
	struct Console* console = start();
	setforegroundcolor(console, 255, 165, 0);
	setstring(console, "test Ala ma kota bla bla bla\n");
	refresh(console);
	system("pause");
	end(console);
/*
	struct Console* console = start();
	for (int r = 0; r <= 255; r += 10) {
		setforegroundcolor(console, r, 0, 0);
		setstring(console, "Test color\n");
		refresh(console);
	}
	system("pause");
	end(console);
*/
	return 0;
}