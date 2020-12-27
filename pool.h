#ifndef POOL_H
#define POOL_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "rbddef.h"
#include "rbdio.h"

#define POOL_GEN_DECL(Pool, Elem)                                                                                      \
                                                                                                                       \
  void Pool##_cons(size_t cap);                                                                                        \
                                                                                                                       \
  Elem *Pool##_alloc();                                                                                                \
                                                                                                                       \
  void Pool##_free(Elem *elem);                                                                                        \
                                                                                                                       \
  void Pool##_debug(FILE *file, uint32_t depth);                                                                       \
                                                                                                                       \
  void Pool##_des();

#define POOL_GEN_DEF(Pool, Elem, Allocator)                                                                            \
                                                                                                                       \
  typedef struct Pool##Free Pool##Free;                                                                                \
                                                                                                                       \
  struct Pool##Free {                                                                                                  \
    Pool##Free *next;                                                                                                  \
  };                                                                                                                   \
                                                                                                                       \
  Pool##Free *Pool##Free_cons(Pool##Free *free, Pool##Free *next) {                                                    \
    free->next = next;                                                                                                 \
    return free;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  void Pool##Free_debug(Pool##Free *free, FILE *file, uint32_t depth) {                                                \
    fprintf(file, #Pool "Free (%p) { next: %p }", free, free->next);                                                   \
  }                                                                                                                    \
                                                                                                                       \
  typedef struct Pool##Slab Pool##Slab;                                                                                \
                                                                                                                       \
  struct Pool##Slab {                                                                                                  \
    Pool##Slab *next;                                                                                                  \
    Elem elems[];                                                                                                      \
  };                                                                                                                   \
                                                                                                                       \
  Pool##Slab *Pool##Slab_cons(Pool##Slab *slab, Pool##Slab *next) {                                                    \
    slab->next = next;                                                                                                 \
    return slab;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  void Pool##Slab_debug(Pool##Slab *slab, FILE *file, uint32_t depth) {                                                \
    fprintf(file, #Pool "Slab (%p) {\n", slab);                                                                        \
    findent(file, depth + 1); fprintf(file, "next: %p,\n", slab->next);                                                \
    findent(file, depth + 1); fprintf(file, "elems: %p,\n", slab->elems);                                              \
    findent(file, depth); fprintf(file, "}");                                                                          \
  }                                                                                                                    \
                                                                                                                       \
  struct {                                                                                                             \
    Pool##Slab *slabs;                                                                                                 \
    size_t cap;                                                                                                        \
    size_t len;                                                                                                        \
    Pool##Free *frees;                                                                                                 \
  } Pool;                                                                                                              \
                                                                                                                       \
  void Pool##_cons(size_t cap) {                                                                                       \
    Pool.slabs = Pool##Slab_cons(Allocator##_alloc(sizeof(Pool##Slab) + cap * sizeof(Elem)), NULL);                    \
    Pool.cap = cap;                                                                                                    \
    Pool.len = 0;                                                                                                      \
    Pool.frees = NULL;                                                                                                 \
  }                                                                                                                    \
                                                                                                                       \
  Elem *Pool##_alloc() {                                                                                               \
    if (Pool.frees != NULL) {                                                                                          \
      Elem *elem = (Elem *)Pool.frees;                                                                                 \
      Pool.frees = Pool.frees->next;                                                                                   \
      return elem;                                                                                                     \
    }                                                                                                                  \
    if (Pool.len == Pool.cap) {                                                                                        \
      Pool.cap *= 2;                                                                                                   \
      Pool.len = 0;                                                                                                    \
      Pool.slabs = Pool##Slab_cons(Allocator##_alloc(sizeof(Pool##Slab) + Pool.cap * sizeof(Elem)), Pool.slabs);       \
    }                                                                                                                  \
    return &Pool.slabs->elems[Pool.len++];                                                                             \
  }                                                                                                                    \
                                                                                                                       \
  void Pool##_free(Elem *elem) {                                                                                       \
    Pool.frees = Pool##Free_cons((Pool##Free *)elem, Pool.frees);                                                      \
  }                                                                                                                    \
                                                                                                                       \
  void Pool##_debug(FILE *file, uint32_t depth) {                                                                      \
    fprintf(file, #Pool " (%p) {\n", Pool);                                                                            \
    findent(file, depth + 1); fprintf(file, "slabs: [\n");                                                             \
    Pool##Slab *slab = Pool.slabs;                                                                                     \
    while (slab) {                                                                                                     \
      findent(file, depth + 2); Pool##Slab_debug(slab, file, depth + 2); fprintf(file, ",\n");                         \
      slab = slab->next;                                                                                               \
    }                                                                                                                  \
    findent(file, depth + 1); fprintf(file, "],\n");                                                                   \
    findent(file, depth + 1); fprintf(file, "cap: %lu,\n", Pool.cap);                                                  \
    findent(file, depth + 1); fprintf(file, "len: %lu,\n", Pool.len);                                                  \
    if (Pool.frees) {                                                                                                  \
      findent(file, depth + 1); fprintf(file, "frees: [\n");                                                           \
      Pool##Free *free = Pool.frees;                                                                                   \
      while (free) {                                                                                                   \
        findent(file, depth + 2); Pool##Free_debug(free, file, depth + 2); fprintf(file, ",\n");                       \
        free = free->next;                                                                                             \
      }                                                                                                                \
      findent(file, depth + 1); fprintf(file, "],\n");                                                                 \
    } else {                                                                                                           \
      findent(file, depth + 1); fprintf(file, "frees: [ ],\n");                                                        \
    }                                                                                                                  \
    findent(file, depth); fprintf(file, "}");                                                                          \
  }                                                                                                                    \
                                                                                                                       \
  void Pool##_des() {                                                                                                  \
    Pool##Slab *curr = Pool.slabs, *next;                                                                              \
    while (curr) {                                                                                                     \
      next = curr->next;                                                                                               \
      Allocator##_free(curr);                                                                                          \
      curr = next;                                                                                                     \
    }                                                                                                                  \
  }

#endif // POOL_H
