#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <ctype.h>

void convert(FILE * fi, FILE * fo, int not_last) {
  int i;
  unsigned char c;

  for (i = 0; i < 1024; i++) {
    c = fgetc(fi);

    fprintf(fo, "0x%02x", c);
    if (i < 1023 || not_last) {
      fprintf(fo, ", ");
    }
    if (i % 16 == 15) {
      fprintf(fo, "\n");
    }
  }
}

void merge(FILE * fi1, FILE * fi2, FILE * fo, int not_last) {
  int i;
  unsigned char c1, c2, c;
  unsigned char toggle, toggle2, bit, bitval;

  toggle = toggle2 = 0;
  for (i = 0; i < 1024; i++) {
    c1 = fgetc(fi1);
    c2 = fgetc(fi2);

    /* FIXME: Dither c1 & c2 into c */
    toggle2 = !toggle2;
    if (toggle2) {
      toggle = !toggle;
    }
    bitval = 128 + 64;
    c = 0;
    for (bit = 0; bit < 4; bit++) {
      if (bit % 2 == toggle) {
        c = c | (c1 & bitval);
      } else {
        c = c | (c2 & bitval);
      }
      bitval = bitval >> 2;
    }

    fprintf(fo, "0x%02x", c);
    if (i < 1023 || not_last) {
      fprintf(fo, ", ");
    }
    if (i % 16 == 15) {
      fprintf(fo, "\n");
    }
  }
}

int main(int argc, char * argv[]) {
  FILE * fi, * fi2, * fo;
  char h_name[256];
  int i;

  if (argc < 4) {
    fprintf(stderr, "Usage: %s font1.fnt font2.fnt output.h\n", argv[0]);
    exit(1);
  }

  fo = fopen(argv[3], "w");
  if (fo == NULL) {
    perror(argv[3]);
    exit(1);
  }

  snprintf(h_name, sizeof(h_name), "%s", basename(argv[3]));
  for (i = 0; h_name[i] != '\0'; i++) {
    if (isalnum(h_name[i])) {
      h_name[i] = toupper(h_name[i]);
    } else {
      h_name[i] = '_';
    }
  }

  fprintf(fo, "#ifndef %s\n", h_name);
  fprintf(fo, "#define %s\n", h_name);
  fprintf(fo, "/* gemerated by tools/fonts-to-h */\n");
  fprintf(fo, "#define FONT_LEN 3072\n");
  fprintf(fo, "static unsigned char font[] = {\n");


  fi = fopen(argv[1], "rb");
  if (fi == NULL) {
    perror(argv[1]);
    exit(1);
  }
  fprintf(fo, "/* %s */\n", argv[1]);
  convert(fi, fo, 1);
  fclose(fi);

  fi = fopen(argv[2], "rb");
  if (fi == NULL) {
    perror(argv[2]);
    exit(1);
  }
  fprintf(fo, "/* %s */\n", argv[2]);
  convert(fi, fo, 1);
  fclose(fi);

  fi = fopen(argv[1], "rb");
  if (fi == NULL) {
    perror(argv[1]);
    exit(1);
  }
  fi2 = fopen(argv[2], "rb");
  if (fi2 == NULL) {
    perror(argv[2]);
    exit(1);
  }
  fprintf(fo, "/* %s + %s combined */\n", argv[1], argv[2]);
  merge(fi, fi2, fo, 0);
  fclose(fi);
  fclose(fi2);


  fprintf(fo, "};\n");
  fprintf(fo, "#endif /* %s */\n", h_name);

  fclose(fo);

  return(0);
}
