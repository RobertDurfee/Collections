#ifndef RBDMAP_H
#define RBDMAP_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "rbddef.h"
#include "rbdio.h"

#define MAP_ELEM_UNUSED 0
#define MAP_ELEM_OCCUPIED 1
#define MAP_ELEM_ERASED 2

#define MAP_GEN_DECL(Map, Key, Val)\
\
  /*=================================================================================================================*/\
  /* Map Element                                                                                                     */\
  /*=================================================================================================================*/\
\
  /* Map element. */\
  typedef struct Map##Elem Map##Elem;\
\
  /*=================================================================================================================*/\
  /* Map Iterator                                                                                                    */\
  /*=================================================================================================================*/\
\
  /* Map iterator. */\
  typedef struct Map##Iter Map##Iter;\
\
  /* Construct a new map iterator. */\
  Map##Iter Map##Iter_cons(Map##Iter iter, Map##Elem *elem);\
\
  /* Advance the map iterator to the next element. */\
  Map##Iter Map##Iter_next(Map##Iter iter);\
\
  /* Get the key at the current position. */\
  Key *Map##Iter_key(Map##Iter iter);\
\
  /* Get the value at the current position. */\
  Val *Map##Iter_val(Map##Iter iter);\
\
  /* Check if two iterators point to the same element. */\
  bool Map##Iter_equals(Map##Iter a, Map##Iter b);\
\
  /* Print the underlying representation of the iterator with depth indentation. */\
  void Map##Iter_debug(Map##Iter iter, FILE *file, uint32_t depth);\
\
  /* Destruct the map iterator. */\
  Map##Iter Map##Iter_des(Map##Iter iter);\
\
  /*=================================================================================================================*/\
  /* Map                                                                                                             */\
  /*=================================================================================================================*/\
\
  /* Map. */\
  typedef struct Map Map;\
\
  /* Construct a new map with initial capacity. */\
  Map *Map##_cons(Map *map, size_t cap);\
\
  /* Check if the map is empty. */\
  bool Map##_empty(Map *map);\
\
  /* Get the capacity of the map. */\
  size_t Map##_cap(Map *map);\
\
  /* Get the length of the map. */\
  size_t Map##_len(Map *map);\
\
  /* Reserve at least the provided capacity. */\
  void Map##_reserve(Map *map, size_t cap);\
\
  /* Clear all elements and set length to zero, calling element destructor for each element. */\
  void Map##_clear(Map *map);\
\
  /* Insert a new element into the map (must not exist). */\
  void Map##_insert(Map *map, Key key, Val val);\
\
  /* Replace an existing element in the map (must exist). */\
  void Map##_replace(Map *map, Key key, Val val);\
\
  /* Get the value of the provided key (must exist). */\
  Val *Map##_at(Map *map, Key key);\
\
  /* Get the element of the provided key. */\
  Map##Iter Map##_find(Map *map, Key key);\
\
  /* Check if the key exists in the map. */\
  bool Map##_contains(Map *map, Key key);\
\
  /* Erase the provided element (must exist). */\
  void Map##_erase(Map *map, Key key);\
\
  /* Return iterator starting at first element. */\
  Map##Iter Map##_begin(Map *map);\
\
  /* Return iterator starting after last element. */\
  Map##Iter Map##_end(Map *map);\
\
  /* Check if two maps are equal, calling element equals for each element, if necessary. */\
  bool Map##_equals(Map *a, Map *b);\
\
  /* Print the underlying representation of the map, calling element debug for each element. */\
  void Map##_debug(Map *map, FILE *file, uint32_t depth);\
\
  /* Destruct the map. */\
  Map *Map##_des(Map *map);

#define MAP_GEN_DEF(Map, Key, Key_hash, Key_equals, Key_debug, Key_des, Val, Val_equals, Val_debug, Val_des, Allocator_alloc, Allocator_free)\
\
  /*=================================================================================================================*/\
  /* Map Element                                                                                                     */\
  /*=================================================================================================================*/\
\
  /* Map element. */\
  struct Map##Elem {\
    uint8_t typ;\
    size_t hash;\
    Key key;\
    Val val;\
  };\
\
  /* Construct a new unused map element. */\
  Map##Elem *Map##Elem_consUnused(Map##Elem *elem) {\
    elem->typ = MAP_ELEM_UNUSED;\
    return elem;\
  }\
\
  /* Construct a new occupied map element. */\
  Map##Elem *Map##Elem_consOccupied(Map##Elem *elem, size_t hash, Key key, Val val) {\
    elem->typ = MAP_ELEM_OCCUPIED;\
    elem->hash = hash;\
    elem->key = key;\
    elem->val = val;\
    return elem;\
  }\
\
  /* Construct a new erased map element. */\
  Map##Elem *Map##Elem_consErased(Map##Elem *elem) {\
    elem->typ = MAP_ELEM_ERASED;\
    return elem;\
  }\
\
  /* Check if two map elements are equal. */\
  bool Map##Elem_equals(Map##Elem *a, Map##Elem *b) {\
    if (a->typ != b->typ) {\
      return false;\
    }\
    if (a->typ == MAP_ELEM_OCCUPIED) {\
      if (a->hash != b->hash) {\
        return false;\
      }\
      if (!IF(Key_equals)(Key_equals(a->key, b->key), (a->key == b->key))) {\
        return false;\
      }\
      if (!IF(Val_equals)(Val_equals(a->val, b->val), (a->val == b->val))) {\
        return false;\
      }\
    }\
    return true;\
  }\
\
  /* Print the underlying representation of the map element with depth indentation. */\
  void Map##Elem_debug(Map##Elem *elem, FILE *file, uint32_t depth) {\
    switch (elem->typ) {\
      case MAP_ELEM_UNUSED:\
        fprintf(file, #Map "Elem (%p) { typ: MAP_ELEM_UNUSED }", elem);\
        break;\
      case MAP_ELEM_OCCUPIED:\
        fprintf(file, #Map "Elem (%p) {\n", elem);\
        findent(file, depth + 1); fprintf(file, "typ: MAP_ELEM_OCCUPIED,\n");\
        findent(file, depth + 1); fprintf(file, "hash: %lu,\n", elem->hash);\
        findent(file, depth + 1); fprintf(file, "key: "); IF(Key_debug)(Key_debug(elem->key, file, depth + 1), fprintf(file, #Map "Key { ? }")); fprintf(file, ",\n");\
        findent(file, depth + 1); fprintf(file, "val: "); IF(Val_debug)(Val_debug(elem->val, file, depth + 1), fprintf(file, #Map "Val { ? }")); fprintf(file, ",\n");\
        findent(file, depth); fprintf(file, "}");\
        break;\
      case MAP_ELEM_ERASED:\
        fprintf(file, #Map "Elem (%p) { typ: MAP_ELEM_ERASED }", elem);\
        break;\
      default:\
        __builtin_unreachable();\
    }\
  }\
\
  /* Destruct the map element, calling key and value destructors, if necessary. */\
  Map##Elem *Map##Elem_des(Map##Elem *elem) {\
    if (elem->typ == MAP_ELEM_OCCUPIED) {\
      IF(Key_des)(Key_des(elem->key),);\
      IF(Val_des)(Val_des(elem->val),);\
    }\
    return elem;\
  }\
\
  /*=================================================================================================================*/\
  /* Map Iterator                                                                                                    */\
  /*=================================================================================================================*/\
\
  struct Map##Iter {\
    Map##Elem *elem;\
  };\
\
  Map##Iter Map##Iter_cons(Map##Iter iter, Map##Elem *elem) {\
    iter.elem = elem;\
    return iter;\
  }\
\
  Map##Iter Map##Iter_next(Map##Iter iter) {\
    do {\
      iter.elem++;\
    } while (iter.elem->typ != MAP_ELEM_OCCUPIED);\
    return iter;\
  }\
\
  Key *Map##Iter_key(Map##Iter iter) {\
    return &iter.elem->key;\
  }\
\
  Val *Map##Iter_val(Map##Iter iter) {\
    return &iter.elem->val;\
  }\
\
  bool Map##Iter_equals(Map##Iter a, Map##Iter b) {\
    return (a.elem == b.elem);\
  }\
\
  void Map##Iter_debug(Map##Iter iter, FILE *file, UNUSED uint32_t depth) {\
    fprintf(file, #Map "Iter { elem: %p }", iter.elem);\
  }\
\
  Map##Iter Map##Iter_des(Map##Iter iter) {\
    return iter;\
  }\
\
  /*=================================================================================================================*/\
  /* Map                                                                                                             */\
  /*=================================================================================================================*/\
\
  struct Map {\
    Map##Elem *elems;\
    size_t cap;\
    size_t len;\
  };\
\
  Map *Map##_cons(Map *map, size_t cap) {\
    map->elems = IF(Allocator_alloc)(Allocator_alloc, malloc)((cap + 1) * sizeof(Map##Elem));\
    memset(map->elems, 0, cap * sizeof(Map##Elem));\
    map->elems[cap].typ = MAP_ELEM_OCCUPIED;\
    map->cap = cap;\
    map->len = 0;\
    return map;\
  }\
\
  bool Map##_empty(Map *map) {\
    return !map->len;\
  }\
\
  size_t Map##_cap(Map *map) {\
    return map->cap;\
  }\
\
  size_t Map##_len(Map *map) {\
    return map->len;\
  }\
\
  /* Reserve provided capacity and rehash, assuming larger capacity than current. */\
  void Map##_reserveUnchecked(Map *map, size_t cap) {\
    Map##Elem *elems = IF(Allocator_alloc)(Allocator_alloc, malloc)((cap + 1) * sizeof(Map##Elem));\
    memset(elems, 0, cap * sizeof(Map##Elem));\
    elems[cap].typ = MAP_ELEM_OCCUPIED;\
    for (size_t i = 0; i < map->cap; i++) {\
      if (map->elems[i].typ == MAP_ELEM_OCCUPIED) {\
        for (size_t j = 0, k = map->elems[i].hash % cap; j < cap; j++, k = (k + 1) % cap) {\
          if (elems[k].typ != MAP_ELEM_OCCUPIED) {\
            elems[k] = map->elems[i];\
            break;\
          }\
        }\
      }\
    }\
    IF(Allocator_free)(Allocator_free, free)(map->elems);\
    map->elems = elems;\
    map->cap = cap;\
  }\
\
  void Map##_reserve(Map *map, size_t cap) {\
    if (3 * cap > 2 * map->cap) {\
      Map##_reserveUnchecked(map, cap);\
    }\
  }\
\
  void Map##_clear(Map *map) {\
    for (size_t i = 0; i < map->cap; i++) {\
      Map##Elem_des(&map->elems[i]);\
      Map##Elem_consUnused(&map->elems[i]);\
    }\
    map->len = 0;\
  }\
\
  void Map##_insert(Map *map, Key key, Val val) {\
    size_t hash = IF(Key_hash)(Key_hash(key), 0);\
    for (size_t i = 0, j = hash % map->cap; i < map->cap; i++, j = (j + 1) % map->cap) {\
      if (map->elems[j].typ != MAP_ELEM_OCCUPIED) {\
        Map##Elem_consOccupied(&map->elems[j], hash, key, val);\
        map->len++;\
        break;\
      }\
    }\
    if (3 * map->len > 2 * map->cap) {\
      Map##_reserveUnchecked(map, map->cap * 2);\
    }\
  }\
\
  void Map##_replace(Map *map, Key key, Val val) {\
    size_t hash = IF(Key_hash)(Key_hash(key), 0);\
    for (size_t i = 0, j = hash % map->cap; i < map->cap; i++, j = (j + 1) % map->cap) {\
      if (map->elems[j].typ == MAP_ELEM_OCCUPIED && IF(Key_equals)(Key_equals(map->elems[j].key, key), map->elems[j].key == key)) {\
        IF(Val_des)(Val_des(map->elems[j].val),);\
        map->elems[j].val = val;\
        return;\
      }\
    }\
    __builtin_unreachable();\
  }\
\
  Val *Map##_at(Map *map, Key key) {\
    size_t hash = IF(Key_hash)(Key_hash(key), 0);\
    for (size_t i = 0, j = hash % map->cap; i < map->cap; i++, j = (j + 1) % map->cap) {\
      if (map->elems[j].typ == MAP_ELEM_OCCUPIED && IF(Key_equals)(Key_equals(map->elems[j].key, key), map->elems[j].key == key)) {\
        return &map->elems[j].val;\
      }\
    }\
    __builtin_unreachable();\
  }\
\
  Map##Iter Map##_find(Map *map, Key key) {\
    size_t hash = IF(Key_hash)(Key_hash(key), 0);\
    for (size_t i = 0, j = hash % map->cap; i < map->cap; i++, j = (j + 1) % map->cap) {\
      if (map->elems[j].typ == MAP_ELEM_OCCUPIED && IF(Key_equals)(Key_equals(map->elems[j].key, key), map->elems[j].key == key)) {\
        return Map##Iter_cons((Map##Iter) {0}, &map->elems[j]);\
      } else if (map->elems[j].typ == MAP_ELEM_UNUSED) {\
        return Map##Iter_cons((Map##Iter) {0}, &map->elems[map->cap]);\
      }\
    }\
    return Map##Iter_cons((Map##Iter) {0}, &map->elems[map->cap]);\
  }\
\
  bool Map##_contains(Map *map, Key key) {\
    size_t hash = IF(Key_hash)(Key_hash(key), 0);\
    for (size_t i = 0, j = hash % map->cap; i < map->cap; i++, j = (j + 1) % map->cap) {\
      if (map->elems[j].typ == MAP_ELEM_OCCUPIED && IF(Key_equals)(Key_equals(map->elems[j].key, key), map->elems[j].key == key)) {\
        return true;\
      } else if (map->elems[j].typ == MAP_ELEM_UNUSED) {\
        return false;\
      }\
    }\
    return false;\
  }\
\
  void Map##_erase(Map *map, Key key) {\
    size_t hash = IF(Key_hash)(Key_hash(key), 0);\
    for (size_t i = 0, j = hash % map->cap; i < map->cap; i++, j = (j + 1) % map->cap) {\
      if (map->elems[j].typ == MAP_ELEM_OCCUPIED && IF(Key_equals)(Key_equals(map->elems[j].key, key), map->elems[j].key == key)) {\
        Map##Elem_des(&map->elems[j]);\
        Map##Elem_consErased(&map->elems[j]);\
        return;\
      }\
    }\
    __builtin_unreachable();\
  }\
\
  Map##Iter Map##_begin(Map *map) {\
    Map##Elem *elem = map->elems;\
    while (elem->typ != MAP_ELEM_OCCUPIED) {\
      elem++;\
    }\
    return Map##Iter_cons((Map##Iter) {0}, elem);\
  }\
\
  Map##Iter Map##_end(Map *map) {\
    return Map##Iter_cons((Map##Iter) {0}, &map->elems[map->cap]);\
  }\
\
  bool Map##_equals(Map *a, Map *b) {\
    if (a->len != b->len) {\
      return false;\
    }\
    for (size_t i = 0; i < a->cap; i++) {\
      if (a->elems[i].typ == MAP_ELEM_OCCUPIED) {\
        if (!Map##_contains(b, a->elems[i].key)) {\
          return false;\
        }\
        if (!IF(Val_equals)(Val_equals(a->elems[i].val, *Map##_at(b, a->elems[i].key)), (a->elems[i].val == *Map##_at(b, a->elems[i].key)))) {\
          return false;\
        }\
      }\
    }\
    return true;\
  }\
\
  void Map##_debug(Map *map, FILE *file, uint32_t depth) {\
    fprintf(file, #Map " (%p) {\n", map);\
    findent(file, depth + 1); fprintf(file, "elems: (%p) [\n", map->elems);\
    for (size_t i = 0; i < map->cap; i++) {\
      findent(file, depth + 2); Map##Elem_debug(&map->elems[i], file, depth + 2); fprintf(file, ",\n");\
    }\
    findent(file, depth + 1); fprintf(file, "],\n");\
    findent(file, depth + 1); fprintf(file, "cap: %lu,\n", map->cap);\
    findent(file, depth + 1); fprintf(file, "len: %lu,\n", map->len);\
    findent(file, depth); fprintf(file, "}");\
  }\
\
  Map *Map##_des(Map *map) {\
    for (size_t i = 0; i < map->cap; i++) {\
      Map##Elem_des(&map->elems[i]);\
    }\
    IF(Allocator_free)(Allocator_free, free)(map->elems);\
    return map;\
  }

#endif // RBDMAP_H
