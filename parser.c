#include "h264.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void printHelpMessage() 
{
	printf("\n\n== MP4 Atom Parser\n");
	printf("An example program for printing out all the atoms in an MP4 file\n");
	printf("Example: ./parser movie.mp4\n\n");
}

void found_atom(char *type, unsigned int pos, unsigned int length){
	printf("%s - pos: %d - lngth: %d\n", type, pos, length);
}

int main(int argc, char **argv) 
{

	if (argc < 2) {
		printHelpMessage();
		return -1;
	}


	FILE *fp;
	fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		printf("\nFile not found\n");
		return -1;
	}

	parse_file(fp, found_atom);

}