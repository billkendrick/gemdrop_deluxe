/*
  title-to-font.c
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
  FILE * fi, * fo;
  int half, x, y, yy, b, c, bits, tot;
  char line[1024], fname[1024];
  unsigned char data[160 * 24];

  if (argc != 2) {
    fprintf(stderr, "Usage: %s {1|2}\n", argv[0]);
    exit(1);
  }

  half = atoi(argv[1]);

  fi = popen("/usr/bin/pngtopnm data/source/gemdrop_deluxe.png", "r");
  if (fi == NULL) {
    fprintf(stderr, "can't open input\n");
    exit(1);
  }

  snprintf(fname, sizeof(fname), "data/generated/title%d.fnt", half);
  fo = fopen(fname, "w");
  if (fo == NULL) {
    perror(fname);
    pclose(fi);
    exit(1);
  }

  fgets(line, sizeof(line), fi);
  if (strcmp(line, "P5\n") != 0) {
    fprintf(stderr, "Not a P5\n");
    exit(1);
  }

  fgets(line, sizeof(line), fi);
  if (strcmp(line, "160 48\n") != 0) {
    fprintf(stderr, "Not 160x48\n");
    exit(1);
  }

  fgets(line, sizeof(line), fi);
  if (strcmp(line, "255\n") != 0) {
    fprintf(stderr, "Not 8-bit\n");
    exit(1);
  }

  for (y = 0; y < 24 * (half - 1); y++) {
    for (x = 0; x < 160; x++) {
      fgetc(fi);
    }
  }

  for (y = 0; y < 24; y++) {
    for (x = 0; x < 160; x++) {
      data[y * 160 + x] = fgetc(fi);
    }
  }

  for (y = 0; y < 3; y++) {
    for (x = 0; x < 20; x++) {
      for (yy = 0; yy < 8; yy++) {
        bits = 256;
        tot = 0;
        for (b = 0; b < 8; b++) {
          bits = bits / 2;
          c = data[(y * 8 + yy) * 160 + (x * 8) + b];
          if (c == 255) {
            tot = tot + bits;
          }
        }
        fputc(tot, fo);
      }
    }
  }

  for (c = 480; c < 512; c++) {
    fputc(0, fo);
  }

  pclose(fi);
  fclose(fo);

  return(0);
}

