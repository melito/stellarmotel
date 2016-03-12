#include "../h264.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void printHelpMessage() {
  printf("\n\n== MP4 SPS/PPS Parser\n");
  printf(
      "Reads in an MP4 container and parses the SPS/PPS from the avcC atom\n");
  printf("Example: ./parser movie.mp4\n\n");
}

int main(int argc, char **argv) {

  if (argc < 2) {
    printHelpMessage();
    return -1;
  }

  MP4Container_t *container = new_mp4_container(argv[1]);

  AVCCAtom_t *avc = get_video_info(container);
  if (avc == NULL) {
    printf("FAILED: Didn't find the avcC atom\n");
    return -1;
  }

  printf("\n avcC atom @ %d - length: %d", avc->position, avc->length);
  print_buffer_data(avc->bytes, avc->size);
  printf("\n");

  for (size_t i = 0; i < avc->number_of_sps_nalus; i++) {
    SPS_t *sps = avc->sps_array[i];
    printf(" SPS #%ld - size: %d", i + 1, sps->size);
    print_buffer_data(sps->bytes, sps->size);
    printf("\n");
  }

  for (size_t i = 0; i < avc->number_of_pps_nalus; i++) {
    PPS_t *pps = avc->pps_array[i];
    printf(" PPS #%ld - size %d", i + 1, pps->size);
    print_buffer_data(pps->bytes, pps->size);
    printf("\n");
  }

  printf(" version: %d\n", avc->version);
  printf(" profile: %d\n", avc->profile);
  printf(" compatibility: %d\n", avc->compatibility);
  printf(" level: %d\n", avc->level);

  printf(" reserved: %d\n           ", avc->reserved);
  print_byte(avc->reserved);
  printf("\n");

  printf(" nalu_length_minus_one: %d\n", avc->nalu_length_minus_one);

  printf(" reserved: ");
  print_byte(avc->reserved_after_length_minus_one);
  printf("\n");

  printf(" number_of_sps_nalus: %d\n", avc->number_of_sps_nalus);
  printf(" number_of_pps_nalus: %d\n", avc->number_of_pps_nalus);

  close_mp4_container(container);
}
