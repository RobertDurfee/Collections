#ifndef RBDDEF_H
#define RBDDEF_H

#define UNUSED __attribute__((unused))

#define CAT(a, b) a ## b

#define IIF(c) CAT(IIF_, c)
#define IIF_0(t, f) f
#define IIF_1(t, f) t

#define COMPL(b) CAT(COMPL_, b)
#define COMPL_0 1
#define COMPL_1 0

#define CHECK_N(x, n, ...) n
#define CHECK(...) CHECK_N(__VA_ARGS__, 0,)
#define PROBE(x) x, 1,

#define NOT(x) CHECK(CAT(NOT_, x))
#define NOT_ PROBE(~)

#define BOOL(x) COMPL(NOT(x))

#define IF(c) IIF(BOOL(c))

#endif // RBDDEF_H
