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

  container->file = fp;
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

void close_mp4_container(MP4Container_t *c) { fclose(c->file); }
