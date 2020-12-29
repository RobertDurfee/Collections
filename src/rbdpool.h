// vim: ft=c

#ifndef RBD_POOL_H
#define RBD_POOL_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "rbddef.h"

/* Generate the declarations for the object pool. */
#define RBD_POOL_GEN_DECL(Pool, Elem)\
\
  /*=================================================================================================================*/\
  /* Pool                                                                                                            */\
  /*=================================================================================================================*/\
\
  /* Construct the object pool. */\
  void RBD(Pool, _cons)(size_t cap);\
\
  /* Allocate an object from the pool. */\
  Elem *RBD(Pool, _alloc)();\
\
  /* Free an object from the pool. */\
  void RBD(Pool, _free)(Elem *elem);\
\
  /* Print the underlying representation of the object pool with depth indentation. */\
  void RBD(Pool, _debug)(FILE *file, uint32_t depth);\
\
  /* Destruct the object pool. */\
  void RBD(Pool, _des)();

/* Generate the definitions for the object pool. */
#define RBD_POOL_GEN_DEF(Pool, Elem, Allocator_alloc, Allocator_free)\
\
  /*=================================================================================================================*/\
  /* Pool Free Node                                                                                                  */\
  /*=================================================================================================================*/\
\
  /* Pool free node. */\
  typedef struct RBD(Pool, Free) RBD(Pool, Free);\
\
  /* Pool free node. */\
  struct RBD(Pool, Free) {\
    RBD(Pool, Free) *next;\
  };\
\
  /* Construct a pool free node. */\
  RBD(Pool, Free) *RBD(Pool, Free_cons)(RBD(Pool, Free) *free, RBD(Pool, Free) *next) {\
    free->next = next;\
    return free;\
  }\
\
  /* Print the underlying representation of the pool free node with depth indentation. */\
  void RBD(Pool, Free_debug)(RBD(Pool, Free) *free, FILE *file, RBD_UNUSED uint32_t depth) {\
    fprintf(file, #Pool "Free (%p) { next: %p }", free, free->next);\
  }\
\
  /*=================================================================================================================*/\
  /* Pool Slab Node                                                                                                  */\
  /*=================================================================================================================*/\
\
  /* Pool slab node. */\
  typedef struct RBD(Pool, Slab) RBD(Pool, Slab);\
\
  /* Pool slab node. */\
  struct RBD(Pool, Slab) {\
    RBD(Pool, Slab) *next;\
    Elem elems[];\
  };\
\
  /* Construct a pool slab node. */\
  RBD(Pool, Slab) *RBD(Pool, Slab_cons)(RBD(Pool, Slab) *slab, RBD(Pool, Slab) *next) {\
    slab->next = next;\
    return slab;\
  }\
\
  /* Print the underlying representation of the pool slab node with depth indentation. */\
  void RBD(Pool, Slab_debug)(RBD(Pool, Slab) *slab, FILE *file, uint32_t depth) {\
    fprintf(file, #Pool "Slab (%p) {\n", slab);\
    RBD_INDENT(file, depth + 1); fprintf(file, "next: %p,\n", slab->next);\
    RBD_INDENT(file, depth + 1); fprintf(file, "elems: %p,\n", slab->elems);\
    RBD_INDENT(file, depth); fprintf(file, "}");\
  }\
\
  /*=================================================================================================================*/\
  /* Pool                                                                                                            */\
  /*=================================================================================================================*/\
\
  struct {\
    RBD(Pool, Slab) *slabs;\
    size_t cap;\
    size_t len;\
    RBD(Pool, Free) *frees;\
  } Pool;\
\
  void RBD(Pool, _cons)(size_t cap) {\
    Pool.slabs = RBD(Pool, Slab_cons)(RBD_IF(Allocator_alloc)(Allocator_alloc, malloc)(sizeof(RBD(Pool, Slab)) + cap * sizeof(Elem)), NULL);\
    Pool.cap = cap;\
    Pool.len = 0;\
    Pool.frees = NULL;\
  }\
\
  Elem *RBD(Pool, _alloc)() {\
    if (Pool.frees != NULL) {\
      Elem *elem = (Elem *)Pool.frees;\
      Pool.frees = Pool.frees->next;\
      return elem;\
    }\
    if (Pool.len == Pool.cap) {\
      Pool.cap *= 2;\
      Pool.len = 0;\
      Pool.slabs = RBD(Pool, Slab_cons)(RBD_IF(Allocator_alloc)(Allocator_alloc, malloc)(sizeof(RBD(Pool, Slab)) + Pool.cap * sizeof(Elem)), Pool.slabs);\
    }\
    return &Pool.slabs->elems[Pool.len++];\
  }\
\
  void RBD(Pool, _free)(Elem *elem) {\
    Pool.frees = RBD(Pool, Free_cons)((RBD(Pool, Free) *)elem, Pool.frees);\
  }\
\
  void RBD(Pool, _debug)(FILE *file, uint32_t depth) {\
    fprintf(file, #Pool " (%p) {\n", Pool);\
    RBD_INDENT(file, depth + 1); fprintf(file, "slabs: [\n");\
    RBD(Pool, Slab) *slab = Pool.slabs;\
    while (slab) {\
      RBD_INDENT(file, depth + 2); RBD(Pool, Slab_debug)(slab, file, depth + 2); fprintf(file, ",\n");\
      slab = slab->next;\
    }\
    RBD_INDENT(file, depth + 1); fprintf(file, "],\n");\
    RBD_INDENT(file, depth + 1); fprintf(file, "cap: %lu,\n", Pool.cap);\
    RBD_INDENT(file, depth + 1); fprintf(file, "len: %lu,\n", Pool.len);\
    RBD_INDENT(file, depth + 1); fprintf(file, "frees: [\n");\
    RBD(Pool, Free) *free = Pool.frees;\
    while (free) {\
      RBD_INDENT(file, depth + 2); RBD(Pool, Free_debug)(free, file, depth + 2); fprintf(file, ",\n");\
      free = free->next;\
    }\
    RBD_INDENT(file, depth + 1); fprintf(file, "],\n");\
    RBD_INDENT(file, depth); fprintf(file, "}");\
  }\
\
  void RBD(Pool, _des)() {\
    RBD(Pool, Slab) *curr = Pool.slabs, *next;\
    while (curr) {\
      next = curr->next;\
      RBD_IF(Allocator_free)(Allocator_free, free)(curr);\
      curr = next;\
    }\
  }

#endif // RBD_POOL_H
