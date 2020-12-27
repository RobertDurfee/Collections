#ifndef RBDIO_H
#define RBDIO_H

#include <stdio.h>

#define findent(file, depth)                                                                                           \
  if (depth) {                                                                                                         \
    fprintf(file, "%*c", (depth) * 2, ' ');                                                                            \
  }

#endif // RBDIO_H
