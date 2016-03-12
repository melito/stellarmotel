#include "h264.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>

#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif

const char *atomTypes[43] = {
    "ftyp", "wide", "free", "mdat", "moov", "moof",    "mvhd", "trak", "tkhd",
    "edts", "elst", "mdia", "mdhd", "hdlr", "minf",    "vmhd", "dinf", "dref",
    "url ", "stbl", "stsd", "avc1", "avcC", "colr",    "stts", "ctts", "stss",
    "sdtp", "stsc", "stsz", "stco", "mvex", "trex",    "mfhd", "traf", "tfhd",
    "trun", "utda", "meta", "ilst", "data", "\xa9too", "udta"};

int chunkIsAtom(char *x) {
  for (int i = 0; i < sizeof(atomTypes) / sizeof(atomTypes[0]); i++) {
    if (strncmp(atomTypes[i], x, 4) == 0) {
      return 1;
    }
  }
  return 0;
}

unsigned int to_host(unsigned char *p) {
  return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3];
};

int is_sibling(MP4Atom_t *a, MP4Atom_t *b) {
  if (b->position == a->position + a->length) {
    return 1;
  }
  return 0;
}

int is_child(MP4Atom_t *parent, MP4Atom_t *child) {

  if ((child->position < parent->position + parent->length) &&
      child->position > parent->position) {
    return 1;
  }
  return 0;
}

MP4Atom_t *read_atom_from(char *atomPtr, FILE *fp, int fpPos) {
  MP4Atom_t *atom;
  atom = (MP4Atom_t *)malloc(sizeof(MP4Atom_t));

  atomPtr[4] = 0;
  int startPos = fpPos - 8; // 4 bytes for identifier and 4 bytes for length
  fseek(fp, startPos, SEEK_SET);

  unsigned char q[4];
  fread(&q, sizeof(char), 4, fp);
  int length = to_host(q);
  fseek(fp, startPos + 8, SEEK_SET);

  strcpy(atom->type, atomPtr);
  atom->position = startPos;
  atom->length = length;

  atom->child = NULL;
  atom->sibling = NULL;
  atom->parent = NULL;

  return atom;
}

void parse_file(MP4Container_t *container, found_atom_callback_t *callback) {

  int x = 0;
  char buf;
  char cursor[4];

  MP4Atom_t *prevAtom = NULL;
  while (fread(&buf, sizeof(char), 1, container->file) > 0) {
    x++;

    cursor[0] = cursor[1];
    cursor[1] = cursor[2];
    cursor[2] = cursor[3];
    cursor[3] = buf;

    char *atomPtr = cursor;
    if (chunkIsAtom(atomPtr)) {
      MP4Atom_t *atom = read_atom_from(atomPtr, container->file, x);

      atom->container = container;

      // No root?  Set one.
      if (container->root == NULL) {
        container->root = atom;
      }

      if (prevAtom != NULL) {
        if (strncmp("ftyp", prevAtom->type, 4) == 0) {
          if (is_child(prevAtom, atom)) {
            continue;
          }
        }

        if (is_sibling(prevAtom, atom)) {
          prevAtom->sibling = atom;
        } else if (is_child(prevAtom, atom)) {
          prevAtom->child = atom;
          atom->parent = prevAtom;
        }
      }

      // Update previous atom for next loop
      prevAtom = atom;
    } // if
  }   // while
} // parse_file

void atomFound(MP4Atom_t *atom, MP4Atom_t *prevAtom) {}

MP4Container_t *new_mp4_container(char *file) {
  MP4Container_t *container;
  container = (MP4Container_t *)malloc(sizeof(MP4Container_t));
  container->root = NULL;

  FILE *fp;
  fp = fopen(file, "r");
  if (fp == NULL) {
    printf("%s", file);
    printf("\nFile not found\n");
    return NULL;
  }

  // Add *FILE to container
  container->file = fp;

  // Add file size to container
  int fd = fileno(fp);
  struct stat st;
  fstat(fd, &st);
  container->fileSize = st.st_size;

  parse_file(container, atomFound);

  return container;
}

void print_atom_tree(MP4Container_t *container) {
  print_atom(container->root, 0);
  print_siblings(container->root, 0);
}

void print_siblings(MP4Atom_t *atom, int depth) {
  MP4Atom_t *conductor = NULL;
  conductor = atom->sibling;
  while (conductor != NULL) {
    print_atom(conductor, depth);
    print_children(conductor, depth + 1);
    conductor = conductor->sibling;
  }
}

void print_children(MP4Atom_t *atom, int depth) {
  MP4Atom_t *conductor = NULL;
  conductor = atom->child;
  while (conductor != NULL) {
    print_atom(conductor, depth);
    print_siblings(conductor, depth + 1);
    conductor = conductor->child;
  }
}

void print_atom(MP4Atom_t *atom, int depth) {
  printf("%*s"
         "Atom %s @ %d of size: %d, ends @ %d\n",
         depth * 3, "", atom->type, atom->position, atom->length,
         atom->position + atom->length);
}

MP4Atom_t *find_atom(char *type, MP4Container_t *container) {
  return search_siblings(type, container->root);
}

MP4Atom_t *search_siblings(char *type, MP4Atom_t *atom) {
  MP4Atom_t *conductor = NULL;
  MP4Atom_t *result = NULL;
  conductor = atom->sibling;
  while (conductor != NULL) {
    if (strcmp(type, conductor->type) == 0) {
      result = conductor;
      return result;
    }
    result = search_children(type, conductor);
    conductor = conductor->sibling;
  }

  return result;
}

MP4Atom_t *search_children(char *type, MP4Atom_t *atom) {
  MP4Atom_t *conductor = NULL;
  MP4Atom_t *result = NULL;

  conductor = atom->child;
  while (conductor != NULL) {
    if (strcmp(type, conductor->type) == 0) {
      result = conductor;
      return result;
    }
    result = search_siblings(type, conductor);
    conductor = conductor->child;
  }

  return result;
}

AVCCAtom_t *get_video_info(MP4Container_t *container) {
  AVCCAtom_t *result = NULL;

  MP4Atom_t *avccAtom = find_atom("avcC", container);
  if (avccAtom == NULL) {
    return result;
  }

  result = parse_AVCC_nalu(avccAtom);

  return result;
}

AVCCAtom_t *parse_AVCC_nalu(MP4Atom_t *atom) {
  AVCCAtom_t *result = NULL;

  unsigned char data[atom->length];
  if (read_atom_data(atom, data)) {

    result = (AVCCAtom_t *)malloc(sizeof(AVCCAtom_t));
    result->position = atom->position;
    result->length = atom->length;

    result->size = atom->length;
    result->bytes = malloc(sizeof(data));
    memcpy(result->bytes, data, sizeof(data));

    uint8_t atomDataIdx = 8;
    result->version = data[atomDataIdx]; // skip atom size and type

    atomDataIdx += 1;
    result->profile = data[atomDataIdx];

    atomDataIdx += 1;
    result->compatibility = data[atomDataIdx];

    atomDataIdx += 1;
    result->level = data[atomDataIdx];

    atomDataIdx += 1;
    result->reserved = data[atomDataIdx];
    result->nalu_length_minus_one = data[atomDataIdx] & ((1 << 2) - 1);

    result->reserved_after_length_minus_one = data[atomDataIdx++];
    result->number_of_sps_nalus = data[atomDataIdx];

    // Get the SPS, yo
    for (size_t i = 0; i < result->number_of_sps_nalus; i++) {

      SPS_t *sps = (SPS_t *)malloc(sizeof(SPS_t));

      atomDataIdx += 2;
      sps->size = data[atomDataIdx];

      int byteOffset = atomDataIdx;
      sps->bytes = malloc(sps->size);
      memcpy(sps->bytes, data + byteOffset, sps->size);

      atomDataIdx += sps->size;
      result->sps_array[i] = sps;
    }

    atomDataIdx += 1;
    result->number_of_pps_nalus = data[atomDataIdx];
    // Get all PPS data
    for (size_t i = 0; i < result->number_of_pps_nalus; i++) {

      PPS_t *pps = (PPS_t *)malloc(sizeof(PPS_t));

      atomDataIdx += 2;
      pps->size = data[atomDataIdx];

      int byteOffset = atomDataIdx;
      pps->bytes = malloc(pps->size);
      memcpy(pps->bytes, data + byteOffset, pps->size);

      result->pps_array[i] = pps;
    }

    printf("\n");
  }

  return result;
}

int read_atom_data(MP4Atom_t *atom, unsigned char *buf) {
  unsigned char *memblock;

  if (atom->container != NULL) {
    int fd = fileno(atom->container->file);

    memblock =
        mmap(NULL, atom->container->fileSize, PROT_READ, MAP_SHARED, fd, 0);
    memcpy(buf, memblock + atom->position, atom->length);

    if (memblock == MAP_FAILED) {
      printf("mmap failed: %s\n", strerror(errno));
      return 0;
    }

    munmap(memblock, atom->container->fileSize);
  } else {
    return 0;
  }

  return 1;
}

void close_mp4_container(MP4Container_t *c) { fclose(c->file); }

/////////
const char *bit_rep[16] = {
        [0] = "0000",  [1] = "0001",  [2] = "0010",  [3] = "0011",
        [4] = "0100",  [5] = "0101",  [6] = "0110",  [7] = "0111",
        [8] = "1000",  [9] = "1001",  [10] = "1010", [11] = "1011",
        [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
};

void print_byte(uint8_t byte) {
  printf("%s%s", bit_rep[byte >> 4], bit_rep[byte & 0x0F]);
}

void print_buffer_data(uint8_t *buf, int length) {
  /// Print buffer data
  printf("\n ");
  for (int i = 0; i < length; i++) {
    // printf("%d\n", i);
    printf("%02X ", buf[i]);
    if (((i + 1) % 16 == 0)) {
      printf("\n");
    }

    if (((i + 1) % 8 == 0)) {
      printf(" ");
    }
  }
  printf("\n");
}
