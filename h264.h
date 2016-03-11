#include <stdio.h>
#include "avcc.h"

/* Array of atom names we care about */
const char *atomTypes[43];

/* Simple test to see if a char cursor is reading an atom name */
int chunkIsAtom(char *x);

/* Bit twiddling */
unsigned int to_host(unsigned char *p);

/* Tree structure for mapping out an MP4 container */
typedef struct MP4Atom {
  char type[5];
  unsigned int position;
  unsigned int length;
  struct MP4Container *container;
  struct MP4Atom *parent;
  struct MP4Atom *child;
  struct MP4Atom *sibling;

} MP4Atom_t;

typedef struct MP4Container {
  FILE *file;
  int fileSize;
  struct MP4Atom *root;
} MP4Container_t;

/* Callback fired when parsing a file */
typedef void(found_atom_callback_t)(MP4Atom_t *atom, MP4Atom_t *prevAtom);

/* Parse an MP4 file */
void parse_file(MP4Container_t *container, found_atom_callback_t *callback);

/* Create an MP4Container_t from a file */
MP4Container_t *new_mp4_container(char *file);

/* Prints all atoms in container */
void print_atom_tree(MP4Container_t *container);
void print_children(MP4Atom_t *atom, int depth);
void print_siblings(MP4Atom_t *atom, int depth);
void print_atom(MP4Atom_t *atom, int depth);

/* Finds atoms in a container */
MP4Atom_t *find_atom(char *type, MP4Container_t *container);
MP4Atom_t *search_siblings(char *type, MP4Atom_t *atom);
MP4Atom_t *search_children(char *type, MP4Atom_t *atom);

/* Gets AAVC info */
AVCCNalu_t *get_video_info(MP4Container_t *container);
AVCCNalu_t *parse_AVCC_nalu(MP4Atom_t *atom);
void read_atom_data(MP4Atom_t *atom, unsigned char *buf);

/* Close the mp4 file */
void close_mp4_container(MP4Container_t *container);
