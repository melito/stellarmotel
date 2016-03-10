#include "h264.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *lastAtom;

void printHelpMessage() {
  printf("\n\n== MP4 Atom Parser\n");
  printf("An example program for printing out all the atoms in an MP4 file\n");
  printf("Example: ./parser movie.mp4\n\n");
}

void found_atom(MP4Atom_t *atom) {
  printf("Atom %s @ %d of size: %d, ends @ %d\n", atom->type, atom->position,
         atom->length, atom->position + atom->length);
}

int main(int argc, char **argv) {

  if (argc < 2) {
    printHelpMessage();
    return -1;
  }

  MP4Container_t *container = new_mp4_container(argv[1]);
  print_atom_tree(container);
  close_mp4_container(container);
}
