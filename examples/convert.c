#include "../h264.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>

void printHelpMessage() {
  printf("\n\n== MP4 to h264 bitstream\n");
  printf("Reads in an MP4 container converts it to an h264 bitstream\n");
  printf("Example: ./convert movie.mp4 output.264\n\n");
}

int main(int argc, char **argv) {

  if (argc < 3) {
    printHelpMessage();
    return -1;
  }

  //////
  MP4Container_t *container = new_mp4_container(argv[1]);
  AVCCAtom_t *avc = get_video_info(container);
  if (avc == NULL) {
    printf("FAILED: Didn't find the avcC atom\n");
    return -1;
  }

  unsigned char naluDeliminator[4] = {0, 0, 0, 1};
  const char *outfile = argv[2];
  FILE *fp = fopen(outfile, "w");
  if (fp == NULL) {
    printf("FAILED: Couldn't open file for writing\n");
    return -1;
  }

  MP4Atom_t *mdat = find_atom("mdat", container);

  int fd = fileno(container->file);

  unsigned char *memblock;
  memblock = mmap(NULL, container->fileSize, PROT_READ, MAP_SHARED, fd, 0);
  if (memblock == MAP_FAILED) {
    printf("mmap failed: %s\n", strerror(errno));
    return 0;
  }

  int readLength = avc->nalu_length_minus_one + 1;
  int bytesRead = 8;
  while (bytesRead < mdat->length) {

    unsigned char lengthData[readLength];
    memcpy(lengthData, memblock + mdat->position + bytesRead, readLength);
    bytesRead += readLength;

    unsigned int naluLength = to_host(lengthData);

    unsigned char nalu[naluLength];
    memcpy(nalu, memblock + mdat->position + bytesRead, naluLength);
    bytesRead += naluLength;

    uint8_t nal_type = (nalu[0] & 0x1f);
    if (nal_type == 5) {
      fwrite(naluDeliminator, 1, sizeof(naluDeliminator), fp);
      SPS_t *sps = avc->sps_array[0];
      fwrite(sps->bytes, 1, sps->size, fp);

      fwrite(naluDeliminator, 1, sizeof(naluDeliminator), fp);
      PPS_t *pps = avc->pps_array[0];
      fwrite(pps->bytes, 1, pps->size, fp);
    }

    fwrite(naluDeliminator, 1, sizeof(naluDeliminator), fp);
    fwrite(nalu, 1, naluLength, fp);
  }
  munmap(memblock, container->fileSize);

  close_mp4_container(container);
}
