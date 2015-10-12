/*
  title-to-font.c
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
  FILE * fi, * fo;
  int half;
  char line[1024], fname[1024];

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

  printf("%s\n", fname);

  fgets(line, sizeof(line), fi);
  printf("%s\n", line);

  pclose(fi);
  fclose(fo);

  return(0);
}

