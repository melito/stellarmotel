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

  mp4_streaming_reader(argv[1], ^(char *data, ssize_t size) {
    printf("%ld\n", size);
  });

  while (done != 1) {
  }
}
