#include "../h264.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void printHelpMessage() {
  printf("\n\n== MP4 SPS/PPS Parser\n");
  printf(
      "Reads in an MP4 container and parses the SPS/PPS from the avc1 atom\n");
  printf("Example: ./parser movie.mp4\n\n");
}

int main(int argc, char **argv) {

  if (argc < 2) {
    printHelpMessage();
    return -1;
  }

  MP4Container_t *container = new_mp4_container(argv[1]);

  MP4Atom_t *avccAtom = find_atom("avcC", container);
  if (avccAtom == NULL) {
    printf("FAILED: Didn't find avcC atom\n");
    return -1;
  }

  printf("SUCCESS! Got the avcC atom\n");
  print_atom(avccAtom, 0);

  close_mp4_container(container);
}
