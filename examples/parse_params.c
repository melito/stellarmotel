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

  AVCCNalu_t *avc = get_video_info(container);
  if (avc == NULL) {
    printf("FAILED: Didn't find the avcC atom\n");
    return -1;
  }

  printf("Success!  Gpt the avcC atom");

  close_mp4_container(container);
}
