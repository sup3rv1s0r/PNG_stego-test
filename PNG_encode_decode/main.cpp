#include "PNG_file.h"

void main(int argc, char *argv[])
{
	PNG_file link = PNG_file("big.png");
	link.encode("1.txt");
	link.outputPNG("bigen.png");
	PNG_file link2 = PNG_file("bigen.png");
	link2.decode("2.txt");
}