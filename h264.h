const char *atomTypes[42];

int chunkIsAtom(char *x);

unsigned int to_host(unsigned char *p);

typedef struct MP4Atom {

	char type[4];
	unsigned int pos;
	unsigned int length;
	struct MP4Atom *next;

} MP4Atom_t;


MP4Atom_t* CreateMP4Atom(char *x);