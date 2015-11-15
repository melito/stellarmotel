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

	int x = 0;
	char buf;
	char cursor[4];

	while(fread(&buf, sizeof(char), 1, fp) > 0) {
		x++;

		cursor[0] = cursor[1];
		cursor[1] = cursor[2];
		cursor[2] = cursor[3];
		cursor[3] = buf;

		char *atomPtr = cursor;
		if (chunkIsAtom(atomPtr)){
			atomPtr[4]=0;

			int startPos = x - 8; // 4 bytes for identifier and 4 bytes for length

			fseek(fp, startPos, SEEK_SET);

            unsigned char q[4];
			fread(&q, sizeof(char), 4, fp);
			int length = to_host(q);

			fseek(fp, startPos+8, SEEK_SET);

			printf("%s - pos: %d - lngth: %d\n", atomPtr, startPos, length);
		}

	}
	fclose(fp);

}