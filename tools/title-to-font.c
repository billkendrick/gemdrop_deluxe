/*
  title-to-font.c
  (used for grabbing one or the other half
  of a 160x48 bitmap for the game's title screen;
  now also used for a regular character set (256x32 bitmap))
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]) {
  FILE * fi, * fo;
  int half, x, y, yy, b, c, bits, tot;
  char line[1024], fname[1024], cmd[1024];
  unsigned char data[256 * 32]; // for title, we only need 160 * 24
  int png_w, png_h, chopping, out_h, font_rows;

  if (argc != 4) {
    fprintf(stderr, "Usage: %s input.png {1|2} output.fnt\n", argv[0]);
    exit(1);
  }

  half = atoi(argv[2]);

  snprintf(cmd, sizeof(cmd), "/usr/bin/pngtopnm %s", argv[1]);
  fi = popen(cmd, "r");
  if (fi == NULL) {
    fprintf(stderr, "can't open input\n");
    exit(1);
  }

  strncpy(fname, argv[3], sizeof(fname));
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
  if (strcmp(line, "160 48\n") != 0 &&
      strcmp(line, "256 32\n") != 0) {
    fprintf(stderr, "Not 160x48 or 256x32\n");
    exit(1);
  }
  sscanf(line, "%d %d", &png_w, &png_h);
  if (png_w == 160 && png_h == 48) {
    chopping = 1;
    out_h = png_h / 2;
    font_rows = 3;
    printf("Chopping half #%d of %s\n", half, argv[1]);
  } else {
    chopping = 0;
    font_rows = 4;
    out_h = png_h;
  }
  

  fgets(line, sizeof(line), fi);
  if (strcmp(line, "255\n") != 0) {
    fprintf(stderr, "Not 8-bit\n");
    exit(1);
  }

  if (chopping) {
    if (half == 2) {
      for (y = 0; y < (png_h / 2); y++) {
        for (x = 0; x < png_w; x++) {
          fgetc(fi);
        }
      }
      printf("Skipped for %d rows of %d columns (%d bytes)\n",
             png_h / 2, png_w, (png_h / 2) * png_w);
    }
  }

  for (y = 0; y < out_h; y++) {
    for (x = 0; x < png_w; x++) {
      data[y * png_w + x] = fgetc(fi);
    }
  }
  printf("Collected %d rows of %d columns (%d bytes)\n",
         out_h, png_w, out_h * png_w);

  printf("Processing %d rows of %d characters (x8 = %d bytes)\n",
         font_rows, png_w / 8, font_rows * png_w);

  for (y = 0; y < font_rows; y++) {
    for (x = 0; x < (png_w / 8); x++) {
      for (yy = 0; yy < 8; yy++) {
        bits = 256;
        tot = 0;
        for (b = 0; b < 8; b++) {
          bits = bits / 2;
          c = data[(y * 8 + yy) * png_w + (x * 8) + b];
          if (c == 255) {
            tot = tot + bits;
          }
        }
        fputc(tot, fo);
      }
    }
  }

  if (chopping) {
    for (c = 480; c < 512; c++) {
      fputc(0, fo);
    }
  }

  pclose(fi);
  fclose(fo);

  return(0);
}

