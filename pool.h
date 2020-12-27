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
  typedef struct J(Pool, Free) J(Pool, Free);                                                                          \
                                                                                                                       \
  struct J(Pool, Free) {                                                                                               \
    J(Pool, Free) *next;                                                                                               \
  };                                                                                                                   \
                                                                                                                       \
  J(Pool, Free) *J(Pool, Free_cons)(J(Pool, Free) *free, J(Pool, Free) *next);                                         \
                                                                                                                       \
  void J(Pool, Free_debug)(FILE *file, J(Pool, Free) *free, uint32_t depth);                                           \
                                                                                                                       \
  typedef struct J(Pool, Slab) J(Pool, Slab);                                                                          \
                                                                                                                       \
  struct J(Pool, Slab) {                                                                                               \
    J(Pool, Slab) *next;                                                                                               \
    Elem elems[];                                                                                                      \
  };                                                                                                                   \
                                                                                                                       \
  J(Pool, Slab) *J(Pool, Slab_cons)(J(Pool, Slab) *slab, J(Pool, Slab) *next);                                         \
                                                                                                                       \
  void J(Pool, Slab_debug)(FILE *file, J(Pool, Slab) *slab, uint32_t depth);                                           \
                                                                                                                       \
  typedef struct Pool Pool;                                                                                            \
                                                                                                                       \
  struct Pool {                                                                                                        \
    J(Pool, Slab) *slabs;                                                                                              \
    size_t cap;                                                                                                        \
    size_t len;                                                                                                        \
    J(Pool, Free) *frees;                                                                                              \
  };                                                                                                                   \
                                                                                                                       \
  Pool *J(Pool, _cons)(Pool *pool, size_t cap);                                                                        \
                                                                                                                       \
  Elem *J(Pool, _alloc)(Pool *pool);                                                                                   \
                                                                                                                       \
  void J(Pool, _free)(Pool *pool, Elem *elem);                                                                         \
                                                                                                                       \
  bool J(Pool, _equals)(Pool *a, Pool *b);                                                                             \
                                                                                                                       \
  void J(Pool, _debug)(FILE *file, Pool *pool, uint32_t depth);                                                        \
                                                                                                                       \
  Pool *J(Pool, _des)(Pool *pool);

#define POOL_GEN_DEF(Pool, Elem)                                                                                       \
                                                                                                                       \
  J(Pool, Free) *J(Pool, Free_cons)(J(Pool, Free) *free, J(Pool, Free) *next) {                                        \
    free->next = next;                                                                                                 \
    return free;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  void J(Pool, Free_debug)(FILE *file, J(Pool, Free) *free, uint32_t depth) {                                          \
    fprintf(file, #Pool "Free (%p) { next: %p }", free, free->next);                                                   \
  }                                                                                                                    \
                                                                                                                       \
  J(Pool, Slab) *J(Pool, Slab_cons)(J(Pool, Slab) *slab, J(Pool, Slab) *next) {                                        \
    slab->next = next;                                                                                                 \
    return slab;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  void J(Pool, Slab_debug)(FILE *file, J(Pool, Slab) *slab, uint32_t depth) {                                          \
    fprintf(file, #Pool "Slab (%p) {\n", slab);                                                                        \
    FINDENT(file, depth + 1); fprintf(file, "next: %p,\n", slab->next);                                                \
    FINDENT(file, depth + 1); fprintf(file, "elems: %p,\n", slab->elems);                                              \
    FINDENT(file, depth); fprintf(file, "}");                                                                          \
  }                                                                                                                    \
                                                                                                                       \
  Pool *J(Pool, _cons)(Pool *pool, size_t cap) {                                                                       \
    pool->slabs = J(Pool, Slab_cons)(malloc(sizeof(J(Pool, Slab)) + cap * sizeof(Elem)), NULL);                        \
    pool->cap = cap;                                                                                                   \
    pool->len = 0;                                                                                                     \
    pool->frees = NULL;                                                                                                \
    return pool;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  Elem *J(Pool, _alloc)(Pool *pool) {                                                                                  \
    if (pool->frees != NULL) {                                                                                         \
      Elem *elem = (Elem *)pool->frees;                                                                                \
      pool->frees = pool->frees->next;                                                                                 \
      return elem;                                                                                                     \
    }                                                                                                                  \
    if (pool->len == pool->cap) {                                                                                      \
      pool->cap *= 2;                                                                                                  \
      pool->len = 0;                                                                                                   \
      pool->slabs = J(Pool, Slab_cons)(malloc(sizeof(J(Pool, Slab)) + pool->cap * sizeof(Elem)), pool->slabs);         \
    }                                                                                                                  \
    return &pool->slabs->elems[pool->len++];                                                                           \
  }                                                                                                                    \
                                                                                                                       \
  void J(Pool, _free)(Pool *pool, Elem *elem) {                                                                        \
    pool->frees = J(Pool, Free_cons)((J(Pool, Free) *)elem, pool->frees);                                              \
  }                                                                                                                    \
                                                                                                                       \
  bool J(Pool, _equals)(Pool *a, Pool *b) {                                                                            \
    return (a == b);                                                                                                   \
  }                                                                                                                    \
                                                                                                                       \
  void J(Pool, _debug)(FILE *file, Pool *pool, uint32_t depth) {                                                       \
    fprintf(file, #Pool " (%p) {\n", pool);                                                                            \
    FINDENT(file, depth + 1); fprintf(file, "slabs: [\n");                                                             \
    J(Pool, Slab) *slab = pool->slabs;                                                                                 \
    while (slab) {                                                                                                     \
      FINDENT(file, depth + 2); J(Pool, Slab_debug)(file, slab, depth + 2); fprintf(file, ",\n");                      \
      slab = slab->next;                                                                                               \
    }                                                                                                                  \
    FINDENT(file, depth + 1); fprintf(file, "],\n");                                                                   \
    FINDENT(file, depth + 1); fprintf(file, "cap: %lu,\n", pool->cap);                                                 \
    FINDENT(file, depth + 1); fprintf(file, "len: %lu,\n", pool->len);                                                 \
    if (pool->frees) {                                                                                                 \
      FINDENT(file, depth + 1); fprintf(file, "frees: [\n");                                                           \
      J(Pool, Free) *free = pool->frees;                                                                               \
      while (free) {                                                                                                   \
        FINDENT(file, depth + 2); J(Pool, Free_debug)(file, free, depth + 2); fprintf(file, ",\n");                    \
        free = free->next;                                                                                             \
      }                                                                                                                \
      FINDENT(file, depth + 1); fprintf(file, "],\n");                                                                 \
    } else {                                                                                                           \
      FINDENT(file, depth + 1); fprintf(file, "frees: [ ],\n");                                                        \
    }                                                                                                                  \
    FINDENT(file, depth); fprintf(file, "}");                                                                          \
  }                                                                                                                    \
                                                                                                                       \
  Pool *J(Pool, _des)(Pool *pool) {                                                                                    \
    J(Pool, Slab) *curr = pool->slabs, *next;                                                                          \
    while (curr) {                                                                                                     \
      next = curr->next;                                                                                               \
      free(curr);                                                                                                      \
      curr = next;                                                                                                     \
    }                                                                                                                  \
    return pool;                                                                                                       \
  }

#endif // POOL_H
