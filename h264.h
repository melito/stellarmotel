#include <stdio.h>

/* Array of atom names we care about */
const char *atomTypes[42];

/* Simple test to see if a char cursor is reading an atom name */
int chunkIsAtom(char *x);

/* Bit twiddling */
unsigned int to_host(unsigned char *p);

/* Parse an MP4 file */

typedef void (found_atom_callback_t) (char *type, unsigned int post, unsigned int length);

void parse_file(FILE *fp, found_atom_callback_t *callback);

void foundAtomCallback();



/* Tree structure for mapping out an MP4 container */
typedef struct MP4Atom {

	char type[4];
	unsigned int pos;
	unsigned int length;
	struct MP4Atom *children;
	struct MP4Atom *siblings;

} MP4Atom_t;
