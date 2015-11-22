#include "h264.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char *atomTypes[42] = {"ftyp", "wide", "free", "mdat", "moov", "moof", "mvhd",
                             "trak", "tkhd", "edts", "elst", "mdia", "mdhd", "hdlr",
                             "minf", "vmhd", "dinf", "dref", "url ", "stbl", "stsd",
                             "avc1", "avcC", "colr", "stts", "ctts", "stss", "sdtp",
                             "stsc", "stsz", "stco", "mvex", "trex", "mfhd", "traf",
                             "tfhd", "trun", "utda", "meta", "ilst", "data", "©too"};

int chunkIsAtom(char *x) {
	for (int i = 0; i < sizeof(atomTypes)/sizeof(atomTypes[0]); i++)
	{
		if (strncmp(atomTypes[i], x, 4) == 0) {
			return 1;
		}
	}
	return 0;
}

unsigned int to_host(unsigned char *p)
{
    return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3];
};

void parse_file(FILE *fp, found_atom_callback_t *callback)
{
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

			callback(atomPtr, startPos, length);
		}

	}
	fclose(fp);

}