#ifndef RBDDEF_H
#define RBDDEF_H

#include <stdio.h>

#define J(x, y)              x ## y
#define FINDENT(file, depth) if (depth) fprintf(file, "%*c", (depth) * 2, ' ')

#endif // RBDDEF_H
