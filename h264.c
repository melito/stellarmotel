#include "h264.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

int is_child(MP4Atom_t *a, MP4Atom_t *b) {
  if (b->position < a->position + a->length) {
    if (b->position > a->position) {
      return 1;
    }
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

  return atom;
}

void parse_file(FILE *fp, found_atom_callback_t *callback) {

  MP4Container_t *container;
  container = (MP4Container_t *)malloc(sizeof(MP4Container_t));

  int x = 0;
  char buf;
  char cursor[4];

  MP4Atom_t *prevAtom;
  while (fread(&buf, sizeof(char), 1, fp) > 0) {
    x++;

    cursor[0] = cursor[1];
    cursor[1] = cursor[2];
    cursor[2] = cursor[3];
    cursor[3] = buf;

    char *atomPtr = cursor;
    if (chunkIsAtom(atomPtr)) {
      MP4Atom_t *atom = read_atom_from(atomPtr, fp, x);
      if (strncmp("ftyp", prevAtom->type, 4) != 0) {
        callback(atom);
      }
      prevAtom = atom;
    }
  }
  fclose(fp);
}
