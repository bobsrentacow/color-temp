#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "color_temp.h"
#include "color_temp_table.h"

void usage(
    char * argv0
)
{
  fprintf(stderr, "usage: %s kelvin\n", argv0);
  fprintf(stderr, "  kelvin -- desired color temp data in kelvin\n");
  exit(-1);
}

int
main(
    int argc,
    char *argv[]
)
{
  if (argc != 2)
    usage(argv[0]);

  color_temp_t ct;
  char *endPtr = argv[1] + strlen(argv[1]);
  ct.kelvin = strtod(argv[1], &endPtr);
  if (endPtr == argv[1]) {
    fprintf(stderr, "invalid kelvin input: %s\n", argv[1]);
    usage(argv[0]);
  }

  interp_color_temp(&ct);

  printf("kelvin    : %f\nintensity : %f\nnorm_red  : %f\nnorm_green: %f\nnorm_blue : %f\nbyte_red  : %d\nbyte_green: %d\nbyte_blue : %d\n",
         ct.kelvin, ct.intensity, ct.norm_red, ct.norm_green, ct.norm_blue, ct.byte_red, ct.byte_green, ct.byte_blue);

  return 0;
}
