#include <stdio.h>
#include <stdlib.h>

#include "calculator_scanner.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s file\n", argv[0]);
    exit(1);
  }

  scan_file(argv[1]);
}
