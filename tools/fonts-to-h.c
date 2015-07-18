#include <stdio.h>
#include <stdlib.h>

void convert(FILE * fi, FILE * fo, int first) {
  int i;
  unsigned char c;

  for (i = 0; i < 1024; i++) {
    c = fgetc(fi);

    fprintf(fo, "0x%02x", c);
    if (i < 1023 || first) {
      fprintf(fo, ", ");
    }
    if (i % 16 == 15) {
      fprintf(fo, "\n");
    }
  }
}

int main(int argc, char * argv[]) {
  FILE * fi;
  FILE * fo;

  fo = fopen("../gemdrop-font.h", "w");
  if (fo == NULL) {
    perror("../gemdrop-font.h");
    return(1);
  }

  fprintf(fo, "#ifndef GEMDROP_FONT_H\n");
  fprintf(fo, "#define GEMDROP_FONT_H\n");
  fprintf(fo, "/* gemerated by tools/fonts-to-h */\n");
  fprintf(fo, "#define FONT_LEN 2048\n");
  fprintf(fo, "static unsigned char font[] = {\n");


  fi = fopen("../orig-src/GEMDROP1.FNT", "rb");
  if (fi == NULL) {
    perror("../orig-src/GEMDROP1.FNT");
    return(1);
  }

  convert(fi, fo, 1);

  fclose(fi);


  fi = fopen("../orig-src/GEMDROP2.FNT", "rb");
  if (fi == NULL) {
    perror("../orig-src/GEMDROP2.FNT");
    return(1);
  }

  convert(fi, fo, 0);

  fclose(fi);

  fprintf(fo, "};\n");
  fprintf(fo, "#endif /* GEMDROP_FONT_H */\n");

  fclose(fo);

  return(0);
}
