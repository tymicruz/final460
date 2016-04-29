#include "ucode.c"

main(int argc, char *argv[ ])
{
  int i;
  printf("PLEASE WORK\n");

  printf("targc=%d\n", argc);

  for (i=0; i<argc; i++){
    printf("targv[%d]=%s\n", i, argv[i]);
  }

  printf("the end\n");
}

