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

MP4Atom_t CreateMP4Atom(char *x)
{

}