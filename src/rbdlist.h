// vim: ft=c

#ifndef RBD_LIST_H
#define RBD_LIST_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "rbddef.h"

/* Generate the declarations for the list. */
#define RBD_LIST_GEN_DECL(List, Elem)\
\
  /*=================================================================================================================*/\
  /* List Iterator                                                                                                   */\
  /*=================================================================================================================*/\
\
  /* List iterator. */\
  typedef struct RBD(List, Iter) RBD(List, Iter);\
\
  /* Construct a new list iterator. */\
  RBD(List, Iter) RBD(List, Iter_cons)(RBD(List, Iter) iter, Elem *elem);\
\
  /* Advance the list iterator to the next element. */\
  RBD(List, Iter) RBD(List, Iter_next)(RBD(List, Iter) iter);\
\
  /* Get the element at the current position. */\
  Elem *RBD(List, Iter_at)(RBD(List, Iter) iter);\
\
  /* Check if two iterators point to the same element. */\
  bool RBD(List, Iter_equals)(RBD(List, Iter) a, RBD(List, Iter) b);\
\
  /* Print the underlying representation of the iterator with depth indentation. */\
  void RBD(List, Iter_debug)(RBD(List, Iter) iter, FILE *file, uint32_t depth);\
\
  /* Destruct the list iterator. */\
  RBD(List, Iter) RBD(List, Iter_des)(RBD(List, Iter) iter);\
\
  /*=================================================================================================================*/\
  /* List                                                                                                            */\
  /*=================================================================================================================*/\
\
  /* List. */\
  typedef struct List List;\
\
  /* Construct a new list with initial capacity. */\
  List *RBD(List, _cons)(List *list, size_t cap);\
\
  /* Get pointer to element at the index. */\
  Elem *RBD(List, _at)(List *list, size_t i);\
\
  /* Get the capacity of the list. */\
  size_t RBD(List, _cap)(List *list);\
\
  /* Get the length of the list. */\
  size_t RBD(List, _len)(List *list);\
\
  /* Check if the list is empty. */\
  bool RBD(List, _empty)(List *list);\
\
  /* Get the first element of the list. */\
  Elem *RBD(List, _front)(List *list);\
\
  /* Get the last element of the list. */\
  Elem *RBD(List, _back)(List *list);\
\
  /* Reserve at least the provided capacity. */\
  void RBD(List, _reserve)(List *list, size_t cap);\
\
  /* Resize the list to the provided length, appending default-constructed elements as needed. */\
  void RBD(List, _resize)(List *list, size_t len);\
\
  /* Insert element at the provided position. */\
  void RBD(List, _insert)(List *list, size_t i, Elem elem);\
\
  /* Push the provided element to the back of the list, resizing as needed. */\
  void RBD(List, _pushBack)(List *list, Elem elem);\
\
  /* Remove the element at the end of the list. */\
  void RBD(List, _popBack)(List *list);\
\
  /* Clear all elements and set length to zero, calling element destructor for each element. */\
  void RBD(List, _clear)(List *list);\
\
  /* Erase the provided element, calling element destructor. */\
  void RBD(List, _erase)(List *list, size_t i);\
\
  /* Return iterator starting at first element. */\
  RBD(List, Iter) RBD(List, _begin)(List *list);\
\
  /* Return iterator starting after last element. */\
  RBD(List, Iter) RBD(List, _end)(List *list);\
\
  /* Checks if two lists are equal, calling element equals for each element, if necessary. */\
  bool RBD(List, _equals)(List *a, List *b);\
\
  /* Print the underlying representation of the list, calling element debug for each element. */\
  void RBD(List, _debug)(List *list, FILE *file, uint32_t depth);\
\
  /* Destruct the list. */\
  List *RBD(List, _des)(List *list);

/* Generate the definitions for the list. */
#define RBD_LIST_GEN_DEF(List, Elem, Elem_default, Elem_equals, Elem_debug, Elem_des, Allocator_alloc, Allocator_realloc, Allocator_free)\
\
  /*=================================================================================================================*/\
  /* List Iterator                                                                                                   */\
  /*=================================================================================================================*/\
\
  struct RBD(List, Iter) {\
    Elem *elem;\
  };\
\
  RBD(List, Iter) RBD(List, Iter_cons)(RBD(List, Iter) iter, Elem *elem) {\
    iter.elem = elem;\
    return iter;\
  }\
\
  RBD(List, Iter) RBD(List, Iter_next)(RBD(List, Iter) iter) {\
    iter.elem++;\
    return iter;\
  }\
\
  Elem *RBD(List, Iter_at)(RBD(List, Iter) iter) {\
    return iter.elem;\
  }\
\
  bool RBD(List, Iter_equals)(RBD(List, Iter) a, RBD(List, Iter) b) {\
    return (a.elem == b.elem);\
  }\
\
  void RBD(List, Iter_debug)(RBD(List, Iter) iter, FILE *file, RBD_UNUSED uint32_t depth) {\
    fprintf(file, #List "Iter { elem: %p }", iter.elem);\
  }\
\
  RBD(List, Iter) RBD(List, Iter_des)(RBD(List, Iter) iter) {\
    return iter;\
  }\
\
  /*=================================================================================================================*/\
  /* List                                                                                                            */\
  /*=================================================================================================================*/\
\
  struct List {\
    Elem *elems;\
    size_t cap;\
    size_t len;\
  };\
\
  List *RBD(List, _cons)(List *list, size_t cap) {\
    list->elems = IF(Allocator_alloc)(Allocator_alloc, malloc)(cap * sizeof(Elem));\
    list->cap = cap;\
    list->len = 0;\
    return list;\
  }\
\
  Elem *RBD(List, _at)(List *list, size_t i) {\
    return &list->elems[i];\
  }\
\
  size_t RBD(List, _cap)(List *list) {\
    return list->cap;\
  }\
\
  size_t RBD(List, _len)(List *list) {\
    return list->len;\
  }\
\
  bool RBD(List, _empty)(List *list) {\
    return !list->len;\
  }\
\
  Elem *RBD(List, _front)(List *list) {\
    return &list->elems[0];\
  }\
\
  Elem *RBD(List, _back)(List *list) {\
    return &list->elems[list->len - 1];\
  }\
\
  /* Reserve at least the provided capacity, assuming capacity is larger than current. */\
  void RBD(List, _reserveUnchecked)(List *list, size_t cap) {\
    list->elems = IF(Allocator_realloc)(Allocator_realloc, realloc)(list->elems, cap * sizeof(Elem));\
    list->cap = cap;\
  }\
\
  void RBD(List, _reserve)(List *list, size_t cap) {\
    if (cap > list->cap) {\
      RBD(List, _reserveUnchecked)(list, cap);\
    }\
  }\
\
  void RBD(List, _resize)(List *list, size_t len) {\
    if (len > list->cap) {\
      RBD(List, _reserveUnchecked)(list, len);\
    }\
    for (size_t i = list->len; i < len; i++) {\
      IF(Elem_default)(list->elems[i] = Elem_default(list->elems[i]),);\
    }\
    list->len = len;\
  }\
\
  void RBD(List, _insert)(List *list, size_t i, Elem elem) {\
    if (list->len == list->cap) {\
      RBD(List, _reserveUnchecked)(list, list->cap * 2);\
    }\
    for (size_t j = i; j < list->len; j++) {\
      list->elems[j + 1] = list->elems[j];\
    }\
    list->elems[i] = elem;\
    list->len++;\
  }\
\
  void RBD(List, _pushBack)(List *list, Elem elem) {\
    if (list->len == list->cap) {\
      RBD(List, _reserveUnchecked)(list, list->cap * 2);\
    }\
    list->elems[list->len++] = elem;\
  }\
\
  void RBD(List, _popBack)(List *list) {\
    IF(Elem_des)(Elem_des(list->elems[--list->len]), --list->len);\
  }\
\
  void RBD(List, _clear)(List *list) {\
    for (size_t i = 0; i < list->len; i++) {\
      IF(Elem_des)(Elem_des(list->elems[i]),);\
    }\
    list->len = 0;\
  }\
\
  void RBD(List, _erase)(List *list, size_t i) {\
    IF(Elem_des)(Elem_des(list->elems[i]),);\
    for (size_t j = i; j < list->len - 1; j++) {\
      list->elems[j] = list->elems[j + 1];\
    }\
    --list->len;\
  }\
\
  RBD(List, Iter) RBD(List, _begin)(List *list) {\
    return RBD(List, Iter_cons)((RBD(List, Iter)) {0}, &list->elems[0]);\
  }\
\
  RBD(List, Iter) RBD(List, _end)(List *list) {\
    return RBD(List, Iter_cons)((RBD(List, Iter)) {0}, &list->elems[list->len]);\
  }\
\
  bool RBD(List, _equals)(List *a, List *b) {\
    if (a->len != b->len) {\
      return false;\
    }\
    for (size_t i = 0; i < a->len; i++) {\
      if (!IF(Elem_equals)(Elem_equals(a->elems[i], b->elems[i]), (a->elems[i] == b->elems[i]))) {\
        return false;\
      }\
    }\
    return true;\
  }\
\
  void RBD(List, _debug)(List *list, FILE *file, uint32_t depth) {\
    fprintf(file, #List " (%p) {\n", list);\
    RBD_INDENT(file, depth + 1); fprintf(file, "elems: (%p) [\n", list->elems);\
    for (size_t i = 0; i < list->len; i++) {\
      RBD_INDENT(file, depth + 2); IF(Elem_debug)(Elem_debug(list->elems[i], file, depth + 2), fprintf(file, #List "Elem { ? }")); fprintf(file, ",\n");\
    }\
    RBD_INDENT(file, depth + 1); fprintf(file, "],\n");\
    RBD_INDENT(file, depth + 1); fprintf(file, "cap: %lu,\n", list->cap);\
    RBD_INDENT(file, depth + 1); fprintf(file, "len: %lu,\n", list->len);\
    RBD_INDENT(file, depth); fprintf(file, "}");\
  }\
\
  List *RBD(List, _des)(List *list) {\
    for (size_t i = 0; i < list->len; i++) {\
      IF(Elem_des)(Elem_des(list->elems[i]),);\
    }\
    IF(Allocator_free)(Allocator_free, free)(list->elems);\
    return list;\
  }

#endif // RBD_LIST_H
