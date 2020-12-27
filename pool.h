#ifndef POOL_H
#define POOL_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "rbddef.h"

#define POOL_GEN_DECL(Pool, Elem)                                                                                      \
                                                                                                                       \
  typedef struct Pool##Free Pool##Free;                                                                                \
                                                                                                                       \
  struct Pool##Free {                                                                                                  \
    Pool##Free *next;                                                                                                  \
  };                                                                                                                   \
                                                                                                                       \
  Pool##Free *Pool##Free_cons(Pool##Free *free, Pool##Free *next);                                                     \
                                                                                                                       \
  void Pool##Free_debug(FILE *file, Pool##Free *free, uint32_t depth);                                                 \
                                                                                                                       \
  typedef struct Pool##Slab Pool##Slab;                                                                                \
                                                                                                                       \
  struct Pool##Slab {                                                                                                  \
    Pool##Slab *next;                                                                                                  \
    Elem elems[];                                                                                                      \
  };                                                                                                                   \
                                                                                                                       \
  Pool##Slab *Pool##Slab_cons(Pool##Slab *slab, Pool##Slab*next);                                                      \
                                                                                                                       \
  void Pool##Slab_debug(FILE *file, Pool##Slab *slab, uint32_t depth);                                                 \
                                                                                                                       \
  typedef struct Pool Pool;                                                                                            \
                                                                                                                       \
  struct Pool {                                                                                                        \
    Pool##Slab *slabs;                                                                                                 \
    size_t cap;                                                                                                        \
    size_t len;                                                                                                        \
    Pool##Free *frees;                                                                                                 \
  };                                                                                                                   \
                                                                                                                       \
  Pool *Pool##_cons(Pool *pool, size_t cap);                                                                           \
                                                                                                                       \
  Elem *Pool##_alloc(Pool *pool);                                                                                      \
                                                                                                                       \
  void Pool##_free(Pool *pool, Elem *elem);                                                                            \
                                                                                                                       \
  bool Pool##_equals(Pool *a, Pool *b);                                                                                \
                                                                                                                       \
  void Pool##_debug(FILE *file, Pool *pool, uint32_t depth);                                                           \
                                                                                                                       \
  Pool *Pool##_des(Pool *pool);

#define POOL_GEN_DEF(Pool, Elem)                                                                                       \
                                                                                                                       \
  Pool##Free *Pool##Free_cons(Pool##Free *free, Pool##Free *next) {                                                    \
    free->next = next;                                                                                                 \
    return free;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  void Pool##Free_debug(FILE *file, Pool##Free *free, uint32_t depth) {                                                \
    fprintf(file, #Pool "Free (%p) { next: %p }", free, free->next);                                                   \
  }                                                                                                                    \
                                                                                                                       \
  Pool##Slab *Pool##Slab_cons(Pool##Slab *slab, Pool##Slab *next) {                                                    \
    slab->next = next;                                                                                                 \
    return slab;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  void Pool##Slab_debug(FILE *file, Pool##Slab *slab, uint32_t depth) {                                                \
    fprintf(file, #Pool "Slab (%p) {\n", slab);                                                                        \
    FINDENT(file, depth + 1); fprintf(file, "next: %p,\n", slab->next);                                                \
    FINDENT(file, depth + 1); fprintf(file, "elems: %p,\n", slab->elems);                                              \
    FINDENT(file, depth); fprintf(file, "}");                                                                          \
  }                                                                                                                    \
                                                                                                                       \
  Pool *Pool##_cons(Pool *pool, size_t cap) {                                                                          \
    pool->slabs = Pool##Slab_cons(malloc(sizeof(Pool##Slab) + cap * sizeof(Elem)), NULL);                              \
    pool->cap = cap;                                                                                                   \
    pool->len = 0;                                                                                                     \
    pool->frees = NULL;                                                                                                \
    return pool;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  Elem *Pool##_alloc(Pool *pool) {                                                                                     \
    if (pool->frees != NULL) {                                                                                         \
      Elem *elem = (Elem *)pool->frees;                                                                                \
      pool->frees = pool->frees->next;                                                                                 \
      return elem;                                                                                                     \
    }                                                                                                                  \
    if (pool->len == pool->cap) {                                                                                      \
      pool->cap *= 2;                                                                                                  \
      pool->len = 0;                                                                                                   \
      pool->slabs = Pool##Slab_cons(malloc(sizeof(Pool##Slab) + pool->cap * sizeof(Elem)), pool->slabs);               \
    }                                                                                                                  \
    return &pool->slabs->elems[pool->len++];                                                                           \
  }                                                                                                                    \
                                                                                                                       \
  void Pool##_free(Pool *pool, Elem *elem) {                                                                           \
    pool->frees = Pool##Free_cons((Pool##Free *)elem, pool->frees);                                                    \
  }                                                                                                                    \
                                                                                                                       \
  bool Pool##_equals(Pool *a, Pool *b) {                                                                               \
    return (a == b);                                                                                                   \
  }                                                                                                                    \
                                                                                                                       \
  void Pool##_debug(FILE *file, Pool *pool, uint32_t depth) {                                                          \
    fprintf(file, #Pool " (%p) {\n", pool);                                                                            \
    FINDENT(file, depth + 1); fprintf(file, "slabs: [\n");                                                             \
    Pool##Slab *slab = pool->slabs;                                                                                    \
    while (slab) {                                                                                                     \
      FINDENT(file, depth + 2); Pool##Slab_debug(file, slab, depth + 2); fprintf(file, ",\n");                         \
      slab = slab->next;                                                                                               \
    }                                                                                                                  \
    FINDENT(file, depth + 1); fprintf(file, "],\n");                                                                   \
    FINDENT(file, depth + 1); fprintf(file, "cap: %lu,\n", pool->cap);                                                 \
    FINDENT(file, depth + 1); fprintf(file, "len: %lu,\n", pool->len);                                                 \
    if (pool->frees) {                                                                                                 \
      FINDENT(file, depth + 1); fprintf(file, "frees: [\n");                                                           \
      Pool##Free *free = pool->frees;                                                                                  \
      while (free) {                                                                                                   \
        FINDENT(file, depth + 2); Pool##Free_debug(file, free, depth + 2); fprintf(file, ",\n");                       \
        free = free->next;                                                                                             \
      }                                                                                                                \
      FINDENT(file, depth + 1); fprintf(file, "],\n");                                                                 \
    } else {                                                                                                           \
      FINDENT(file, depth + 1); fprintf(file, "frees: [ ],\n");                                                        \
    }                                                                                                                  \
    FINDENT(file, depth); fprintf(file, "}");                                                                          \
  }                                                                                                                    \
                                                                                                                       \
  Pool *Pool##_des(Pool *pool) {                                                                                       \
    Pool##Slab *curr = pool->slabs, *next;                                                                             \
    while (curr) {                                                                                                     \
      next = curr->next;                                                                                               \
      free(curr);                                                                                                      \
      curr = next;                                                                                                     \
    }                                                                                                                  \
    return pool;                                                                                                       \
  }

#endif // POOL_H
