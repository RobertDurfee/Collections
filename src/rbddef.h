// vim: ft=c

#ifndef RBD_DEF_H
#define RBD_DEF_H

#include <stdio.h>

#define RBD(a, b) a ## b

#define RBD_IIF(c) RBD(RBD_IIF_, c)
#define RBD_IIF_0(t, f) f
#define RBD_IIF_1(t, f) t

#define RBD_COMPL(b) RBD(RBD_COMPL_, b)
#define RBD_COMPL_0 1
#define RBD_COMPL_1 0

#define RBD_CHECK_N(x, n, ...) n
#define RBD_CHECK(...) RBD_CHECK_N(__VA_ARGS__, 0,)
#define RBD_PROBE(x) x, 1,

#define RBD_NOT(x) RBD_CHECK(RBD(RBD_NOT_, x))
#define RBD_NOT_ RBD_PROBE(~)

#define RBD_BOOL(x) RBD_COMPL(RBD_NOT(x))

#define RBD_IF(c) RBD_IIF(RBD_BOOL(c))

#define RBD_UNUSED __attribute__((unused))

#define RBD_INDENT(file, depth)\
  if (depth) {\
    fprintf(file, "%*c", (depth) * 2, ' ');\
  }

#endif // RBD_DEF_H
