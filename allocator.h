#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "rbddef.h"

#define ALLOCATOR_GEN_DECL(Allocator)                                                                                  \
                                                                                                                       \
  void Allocator##_cons();                                                                                             \
                                                                                                                       \
  void *Allocator##_alloc(size_t len);                                                                                 \
                                                                                                                       \
  void *Allocator##_realloc(void *ptr, size_t len);                                                                    \
                                                                                                                       \
  void Allocator##_free(void *ptr);                                                                                    \
                                                                                                                       \
  void Allocator##_debug(FILE *file, uint32_t depth);                                                                  \
                                                                                                                       \
  void Allocator##_des();

#define ALLOCATOR_GEN_DEF(Allocator)                                                                                   \
                                                                                                                       \
  void Allocator##_cons() { }                                                                                          \
                                                                                                                       \
  void *Allocator##_alloc(size_t len) {                                                                                \
    return malloc(len);                                                                                                \
  }                                                                                                                    \
                                                                                                                       \
  void *Allocator##_realloc(void *ptr, size_t len) {                                                                   \
    return realloc(ptr, len);                                                                                          \
  }                                                                                                                    \
                                                                                                                       \
  void Allocator##_free(void *ptr) {                                                                                   \
    free(ptr);                                                                                                         \
  }                                                                                                                    \
                                                                                                                       \
  void Allocator##_debug(FILE *file, UNUSED uint32_t depth) {                                                          \
    fprintf(file, #Allocator " { }");                                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  void Allocator##_des() { }

#endif // ALLOCATOR_H
