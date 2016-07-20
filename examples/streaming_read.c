#include "../h264.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void printHelpMessage() {
  printf("\n\n== MP4 Streaming Reader\n");
  printf("Point this to an mp4 file that is being written to by another "
         "process.  Will return atoms.\n");
  printf("Example: ./streaming_read movie.mp4\n\n");
}

int main(int argc, char **argv) {

  if (argc < 2) {
    printHelpMessage();
    return -1;
  }

  __block int done = 0;
  char cursor[4];
  char *c = cursor;

  mp4_streaming_reader(argv[1], ^(char *data, ssize_t size) {

    printf("%ld\n", size);

    for (size_t i = 0; i < size; i++) {

      c[0] = c[1];
      c[1] = c[2];
      c[2] = c[3];
      c[3] = data[i];

      if (chunkIsAtom(c)) {
        printf("GOT ATOM! %s\n", c);

        // MP4Atom_t *atom;
        // atom = (MP4Atom_t *)malloc(sizeof(MP4Atom_t));
        //
        // atomPtr[4] = 0;
        // int startPos =
        //     fpPos - 8; // 4 bytes for identifier and 4 bytes for length
        // fseek(fp, startPos, SEEK_SET);
        //
        // unsigned char q[4];
        // fread(&q, sizeof(char), 4, fp);
        // int length = to_host(q);
        // fseek(fp, startPos + 8, SEEK_SET);
        //
        // strcpy(atom->type, atomPtr);
        // atom->position = startPos;
        // atom->length = length;
        //
        // atom->child = NULL;
        // atom->sibling = NULL;
        // atom->parent = NULL;
      }
    }

  });

  while (done != 1) {
  }
}
