// vim: ft=c

#ifndef RBD_SORT_H
#define RBD_SORT_H

#include <stddef.h>
#include <stdint.h>

#include "rbddef.h"

/* Generate the declaration for the counting sort. */
#define RBD_CSORT_GEN_DECL(Sorter, ElemIter, Elem)\
\
  /* Perform counting sort on the provided iterator using i_digit of elements and return in out_elems. */\
  void RBD(Sorter, _csort)(ElemIter in_elems_begin, ElemIter in_elems_end, Elem *out_elems, size_t i_digit);

/* Generate the definition for the counting sort. */
#define RBD_CSORT_GEN_DEF(Sorter, ElemIter, ElemIter_equals, ElemIter_elem, ElemIter_next, Elem, Elem_digitMax, Elem_digitAt)\
\
  void RBD(Sorter, _csort)(ElemIter in_elems_begin, ElemIter in_elems_end, Elem *out_elems, size_t i_digit) {\
    /* Counts for each digit. */\
    size_t counts[RBD_IF(Elem_digitMax)(Elem_digitMax, UINT8_MAX) + 1] = { 0 };\
\
    /* Bucket offsets. */\
    size_t offsets[RBD_IF(Elem_digitMax)(Elem_digitMax, UINT8_MAX) + 1];\
\
    /* Set counts for current digit. */\
    for (ElemIter i_elem = in_elems_begin; !RBD_IF(ElemIter_equals)(ElemIter_equals(i_elem, in_elems_end), (i_elem == in_elems_end)); i_elem = RBD_IF(ElemIter_next)(ElemIter_next(i_elem), i_elem + 1)) {\
      Elem elem = *RBD_IF(ElemIter_elem)(ElemIter_elem(i_elem), i_elem);\
      size_t digit = RBD_IF(Elem_digitAt)(Elem_digitAt(elem, i_digit), ((uint64_t)elem >> (i_digit << 3)) & 0xFFlu);\
      counts[digit]++;\
    }\
\
    /* Set bucket offsets for current digit. */\
    size_t i_elem = 0;\
    for (size_t i_count = 0; i_count < RBD_IF(Elem_digitMax)(Elem_digitMax, UINT8_MAX) + 1; i_count++) {\
      offsets[i_count] = i_elem;\
      i_elem += counts[i_count];\
    }\
\
    /* Copy elems from input iterator to output array at corresponding offset. */\
    for (ElemIter i_elem = in_elems_begin; !RBD_IF(ElemIter_equals)(ElemIter_equals(i_elem, in_elems_end), (i_elem == in_elems_end)); i_elem = RBD_IF(ElemIter_next)(ElemIter_next(i_elem), i_elem + 1)) {\
      Elem elem = *RBD_IF(ElemIter_elem)(ElemIter_elem(i_elem), i_elem);\
      size_t digit = RBD_IF(Elem_digitAt)(Elem_digitAt(elem, i_digit), ((uint64_t)elem >> (i_digit << 3)) & 0xFFlu);\
      out_elems[offsets[digit]++] = elem;\
    }\
  }

/* Generate the declaration for the radix sort. */
#define RBD_RSORT_GEN_DECL(Sorter, ElemIter, Elem)\
\
  /* Perform LSD radix sort on the provided iterator and return in out_elems. */\
  void RBD(Sorter, _rsort)(ElemIter in_elems_begin, ElemIter in_elems_end, size_t n_elems, Elem *out_elems);

/* Generate the definition for the radix sort. */
#define RBD_RSORT_GEN_DEF(Sorter, ElemIter, ElemIter_equals, ElemIter_elem, ElemIter_next, Elem, Elem_digitMax, Elem_digitLen, Elem_digitAt)\
\
  /* Generate counting sort for iterator over elements. */\
  RBD_CSORT_GEN_DEF(RBD(Sorter, Iter), ElemIter, ElemIter_equals, ElemIter_elem, ElemIter_next, Elem, Elem_digitMax, Elem_digitAt)\
\
  /* Generate counting sort for array of elements. */\
  RBD_CSORT_GEN_DEF(RBD(Sorter, Array), Elem *, , , , Elem, Elem_digitMax, Elem_digitAt)\
\
  void RBD(Sorter, _rsort)(ElemIter in_elems_begin, ElemIter in_elems_end, size_t n_elems, Elem *out_elems) {\
    /* Determine number of rounds of radix sort (one for each digit). */\
    Elem elem = *RBD_IF(ElemIter_elem)(ElemIter_elem(in_elems_begin), in_elems_begin);\
    size_t n_digits = RBD_IF(Elem_digitLen)(Elem_digitLen(elem), RBD_CEIL_DIV(64 - __builtin_clzl((uint64_t)elem), 8));\
    for (ElemIter i_elem = RBD_IF(ElemIter_next)(ElemIter_next(in_elems_begin), in_elems_begin + 1); !RBD_IF(ElemIter_equals)(ElemIter_equals(i_elem, in_elems_end), (i_elem == in_elems_end)); i_elem = RBD_IF(ElemIter_next)(ElemIter_next(i_elem), i_elem + 1)) {\
      Elem elem = *RBD_IF(ElemIter_elem)(ElemIter_elem(i_elem), i_elem);\
      n_digits = RBD_MAX(n_digits, RBD_IF(Elem_digitLen)(Elem_digitLen(elem), RBD_CEIL_DIV(64 - __builtin_clzl((uint64_t)elem), 8)));\
    }\
\
    /* Temporary element array. */\
    Elem tmp_elems[n_elems];\
\
    /* Initialize destination array. */\
    Elem *dst_elems = (n_digits % 2) ? out_elems : tmp_elems;\
\
    /* Perform counting sort for least-significant digit. */\
    RBD(Sorter, Iter_csort)(in_elems_begin, in_elems_end, dst_elems, 0);\
\
    /* Swap source and destination arrays. */\
    Elem *src_elems = dst_elems;\
    dst_elems = (n_digits % 2) ? tmp_elems : out_elems;\
\
    /* Repeat for each remaining digit. */\
    for (size_t i_digit = 1; i_digit < n_digits; i_digit++) {\
      /* Perform counting sort for current digit. */\
      RBD(Sorter, Array_csort)(&src_elems[0], &src_elems[n_elems], dst_elems, i_digit);\
\
      /* Swap source and destination arrays. */\
      RBD_SWAP(Elem *, src_elems, dst_elems);\
    }\
  }

/* Generate the declaration for the merge sort. */
#define RBD_MSORT_GEN_DECL(Sorter, ElemIter, Elem)\
\
  void RBD(Sorter, _msort)(ElemIter in_elems, size_t n_elems, Elem *out_elems);

/* Generate the definition for the merge sort. */
#define RBD_MSORT_GEN_DEF(Sorter, ElemIter, ElemIter_elem, ElemIter_next, Elem, Elem_cmp)\
\
  void RBD(Sorter, _msort)(ElemIter in_elems, size_t n_elems, Elem *out_elems) {\
  }

/* Generate the declaration for the quick sort. */
#define RBD_QSORT_GEN_DECL(Sorter, ElemIter, Elem)\
\
  void RBD(Sorter, _qsort)(ElemIter in_elems, size_t n_elems, Elem *out_elems);

/* Generate the definition for the quick sort. */
#define RBD_QSORT_GEN_DEF(Sorter, ElemIter, ElemIter_elem, ElemIter_next, Elem, Elem_cmp)\
\
  void RBD(Sorter, _qsort)(ElemIter in_elems, size_t n_elems, Elem *out_elems) {\
  }

#endif // RBD_SORT_H
