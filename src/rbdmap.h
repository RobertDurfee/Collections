#ifndef RBD_MAP_H
#define RBD_MAP_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "rbddef.h"

#define RBD_MAP_ELEM_UNUSED 0
#define RBD_MAP_ELEM_OCCUPIED 1
#define RBD_MAP_ELEM_ERASED 2

#define RBD_MAP_GEN_DECL(Map, Key, Val)\
\
  /*=================================================================================================================*/\
  /* Map Element                                                                                                     */\
  /*=================================================================================================================*/\
\
  /* Map element. */\
  typedef struct RBD(Map, Elem) RBD(Map, Elem);\
\
  /*=================================================================================================================*/\
  /* Map Iterator                                                                                                    */\
  /*=================================================================================================================*/\
\
  /* Map iterator. */\
  typedef struct RBD(Map, Iter) RBD(Map, Iter);\
\
  /* Construct a new map iterator. */\
  RBD(Map, Iter) RBD(Map, Iter_cons)(RBD(Map, Iter) iter, RBD(Map, Elem) *elem);\
\
  /* Advance the map iterator to the next element. */\
  RBD(Map, Iter) RBD(Map, Iter_next)(RBD(Map, Iter) iter);\
\
  /* Get the key at the current position. */\
  Key *RBD(Map, Iter_key)(RBD(Map, Iter) iter);\
\
  /* Get the value at the current position. */\
  Val *RBD(Map, Iter_val)(RBD(Map, Iter) iter);\
\
  /* Check if two iterators point to the same element. */\
  bool RBD(Map, Iter_equals)(RBD(Map, Iter) a, RBD(Map, Iter) b);\
\
  /* Print the underlying representation of the iterator with depth indentation. */\
  void RBD(Map, Iter_debug)(RBD(Map, Iter) iter, FILE *file, uint32_t depth);\
\
  /* Destruct the map iterator. */\
  RBD(Map, Iter) RBD(Map, Iter_des)(RBD(Map, Iter) iter);\
\
  /*=================================================================================================================*/\
  /* Map                                                                                                             */\
  /*=================================================================================================================*/\
\
  /* Map. */\
  typedef struct Map Map;\
\
  /* Construct a new map with initial capacity. */\
  Map *RBD(Map, _cons)(Map *map, size_t cap);\
\
  /* Check if the map is empty. */\
  bool RBD(Map, _empty)(Map *map);\
\
  /* Get the capacity of the map. */\
  size_t RBD(Map, _cap)(Map *map);\
\
  /* Get the length of the map. */\
  size_t RBD(Map, _len)(Map *map);\
\
  /* Reserve at least the provided capacity. */\
  void RBD(Map, _reserve)(Map *map, size_t cap);\
\
  /* Clear all elements and set length to zero, calling element destructor for each element. */\
  void RBD(Map, _clear)(Map *map);\
\
  /* Insert a new element into the map (must not exist). */\
  void RBD(Map, _insert)(Map *map, Key key, Val val);\
\
  /* Replace an existing element in the map (must exist). */\
  void RBD(Map, _replace)(Map *map, Key key, Val val);\
\
  /* Get the value of the provided key (must exist). */\
  Val *RBD(Map, _at)(Map *map, Key key);\
\
  /* Get the element of the provided key. */\
  RBD(Map, Iter) RBD(Map, _find)(Map *map, Key key);\
\
  /* Check if the key exists in the map. */\
  bool RBD(Map, _contains)(Map *map, Key key);\
\
  /* Erase the provided element (must exist). */\
  void RBD(Map, _erase)(Map *map, Key key);\
\
  /* Return iterator starting at first element. */\
  RBD(Map, Iter) RBD(Map, _begin)(Map *map);\
\
  /* Return iterator starting after last element. */\
  RBD(Map, Iter) RBD(Map, _end)(Map *map);\
\
  /* Check if two maps are equal, calling element equals for each element, if necessary. */\
  bool RBD(Map, _equals)(Map *a, Map *b);\
\
  /* Print the underlying representation of the map, calling element debug for each element. */\
  void RBD(Map, _debug)(Map *map, FILE *file, uint32_t depth);\
\
  /* Destruct the map. */\
  Map *RBD(Map, _des)(Map *map);

#define RBD_MAP_GEN_DEF(Map, Key, Key_hash, Key_equals, Key_debug, Key_des, Val, Val_equals, Val_debug, Val_des, Allocator_alloc, Allocator_free)\
\
  /*=================================================================================================================*/\
  /* Map Element                                                                                                     */\
  /*=================================================================================================================*/\
\
  /* Map element. */\
  struct RBD(Map, Elem) {\
    uint8_t typ;\
    size_t hash;\
    Key key;\
    Val val;\
  };\
\
  /* Construct a new unused map element. */\
  RBD(Map, Elem) *RBD(Map, Elem_consUnused)(RBD(Map, Elem) *elem) {\
    elem->typ = RBD_MAP_ELEM_UNUSED;\
    return elem;\
  }\
\
  /* Construct a new occupied map element. */\
  RBD(Map, Elem) *RBD(Map, Elem_consOccupied)(RBD(Map, Elem) *elem, size_t hash, Key key, Val val) {\
    elem->typ = RBD_MAP_ELEM_OCCUPIED;\
    elem->hash = hash;\
    elem->key = key;\
    elem->val = val;\
    return elem;\
  }\
\
  /* Construct a new erased map element. */\
  RBD(Map, Elem) *RBD(Map, Elem_consErased)(RBD(Map, Elem) *elem) {\
    elem->typ = RBD_MAP_ELEM_ERASED;\
    return elem;\
  }\
\
  /* Check if two map elements are equal. */\
  bool RBD(Map, Elem_equals)(RBD(Map, Elem) *a, RBD(Map, Elem) *b) {\
    if (a->typ != b->typ) {\
      return false;\
    }\
    if (a->typ == RBD_MAP_ELEM_OCCUPIED) {\
      if (a->hash != b->hash) {\
        return false;\
      }\
      if (!RBD_IF(Key_equals)(Key_equals(a->key, b->key), (a->key == b->key))) {\
        return false;\
      }\
      if (!RBD_IF(Val_equals)(Val_equals(a->val, b->val), (a->val == b->val))) {\
        return false;\
      }\
    }\
    return true;\
  }\
\
  /* Print the underlying representation of the map element with depth indentation. */\
  void RBD(Map, Elem_debug)(RBD(Map, Elem) *elem, FILE *file, uint32_t depth) {\
    switch (elem->typ) {\
      case RBD_MAP_ELEM_UNUSED:\
        fprintf(file, #Map "Elem (%p) { typ: RBD_MAP_ELEM_UNUSED }", elem);\
        break;\
      case RBD_MAP_ELEM_OCCUPIED:\
        fprintf(file, #Map "Elem (%p) {\n", elem);\
        RBD_INDENT(file, depth + 1); fprintf(file, "typ: RBD_MAP_ELEM_OCCUPIED,\n");\
        RBD_INDENT(file, depth + 1); fprintf(file, "hash: %lu,\n", elem->hash);\
        RBD_INDENT(file, depth + 1); fprintf(file, "key: "); RBD_IF(Key_debug)(Key_debug(elem->key, file, depth + 1), fprintf(file, #Map "Key { ? }")); fprintf(file, ",\n");\
        RBD_INDENT(file, depth + 1); fprintf(file, "val: "); RBD_IF(Val_debug)(Val_debug(elem->val, file, depth + 1), fprintf(file, #Map "Val { ? }")); fprintf(file, ",\n");\
        RBD_INDENT(file, depth); fprintf(file, "}");\
        break;\
      case RBD_MAP_ELEM_ERASED:\
        fprintf(file, #Map "Elem (%p) { typ: RBD_MAP_ELEM_ERASED }", elem);\
        break;\
      default:\
        __builtin_unreachable();\
    }\
  }\
\
  /* Destruct the map element, calling key and value destructors, if necessary. */\
  RBD(Map, Elem) *RBD(Map, Elem_des)(RBD(Map, Elem) *elem) {\
    if (elem->typ == RBD_MAP_ELEM_OCCUPIED) {\
      RBD_IF(Key_des)(Key_des(elem->key),);\
      RBD_IF(Val_des)(Val_des(elem->val),);\
    }\
    return elem;\
  }\
\
  /*=================================================================================================================*/\
  /* Map Iterator                                                                                                    */\
  /*=================================================================================================================*/\
\
  struct RBD(Map, Iter) {\
    RBD(Map, Elem) *elem;\
  };\
\
  RBD(Map, Iter) RBD(Map, Iter_cons)(RBD(Map, Iter) iter, RBD(Map, Elem) *elem) {\
    iter.elem = elem;\
    return iter;\
  }\
\
  RBD(Map, Iter) RBD(Map, Iter_next)(RBD(Map, Iter) iter) {\
    do {\
      iter.elem++;\
    } while (iter.elem->typ != RBD_MAP_ELEM_OCCUPIED);\
    return iter;\
  }\
\
  Key *RBD(Map, Iter_key)(RBD(Map, Iter) iter) {\
    return &iter.elem->key;\
  }\
\
  Val *RBD(Map, Iter_val)(RBD(Map, Iter) iter) {\
    return &iter.elem->val;\
  }\
\
  bool RBD(Map, Iter_equals)(RBD(Map, Iter) a, RBD(Map, Iter) b) {\
    return (a.elem == b.elem);\
  }\
\
  void RBD(Map, Iter_debug)(RBD(Map, Iter) iter, FILE *file, RBD_UNUSED uint32_t depth) {\
    fprintf(file, #Map "Iter { elem: %p }", iter.elem);\
  }\
\
  RBD(Map, Iter) RBD(Map, Iter_des)(RBD(Map, Iter) iter) {\
    return iter;\
  }\
\
  /*=================================================================================================================*/\
  /* Map                                                                                                             */\
  /*=================================================================================================================*/\
\
  struct Map {\
    RBD(Map, Elem) *elems;\
    size_t cap;\
    size_t len;\
  };\
\
  Map *RBD(Map, _cons)(Map *map, size_t cap) {\
    map->elems = RBD_IF(Allocator_alloc)(Allocator_alloc, malloc)((cap + 1) * sizeof(RBD(Map, Elem)));\
    memset(map->elems, 0, cap * sizeof(RBD(Map, Elem)));\
    map->elems[cap].typ = RBD_MAP_ELEM_OCCUPIED;\
    map->cap = cap;\
    map->len = 0;\
    return map;\
  }\
\
  bool RBD(Map, _empty)(Map *map) {\
    return !map->len;\
  }\
\
  size_t RBD(Map, _cap)(Map *map) {\
    return map->cap;\
  }\
\
  size_t RBD(Map, _len)(Map *map) {\
    return map->len;\
  }\
\
  /* Reserve provided capacity and rehash, assuming larger capacity than current. */\
  void RBD(Map, _reserveUnchecked)(Map *map, size_t cap) {\
    RBD(Map, Elem) *elems = RBD_IF(Allocator_alloc)(Allocator_alloc, malloc)((cap + 1) * sizeof(RBD(Map, Elem)));\
    memset(elems, 0, cap * sizeof(RBD(Map, Elem)));\
    elems[cap].typ = RBD_MAP_ELEM_OCCUPIED;\
    for (size_t i = 0; i < map->cap; i++) {\
      if (map->elems[i].typ == RBD_MAP_ELEM_OCCUPIED) {\
        for (size_t j = 0, k = map->elems[i].hash % cap; j < cap; j++, k = (k + 1) % cap) {\
          if (elems[k].typ != RBD_MAP_ELEM_OCCUPIED) {\
            elems[k] = map->elems[i];\
            break;\
          }\
        }\
      }\
    }\
    RBD_IF(Allocator_free)(Allocator_free, free)(map->elems);\
    map->elems = elems;\
    map->cap = cap;\
  }\
\
  void RBD(Map, _reserve)(Map *map, size_t cap) {\
    if (3 * cap > 2 * map->cap) {\
      RBD(Map, _reserveUnchecked)(map, cap);\
    }\
  }\
\
  void RBD(Map, _clear)(Map *map) {\
    for (size_t i = 0; i < map->cap; i++) {\
      RBD(Map, Elem_des)(&map->elems[i]);\
      RBD(Map, Elem_consUnused)(&map->elems[i]);\
    }\
    map->len = 0;\
  }\
\
  void RBD(Map, _insert)(Map *map, Key key, Val val) {\
    size_t hash = RBD_IF(Key_hash)(Key_hash(key), 0);\
    for (size_t i = 0, j = hash % map->cap; i < map->cap; i++, j = (j + 1) % map->cap) {\
      if (map->elems[j].typ != RBD_MAP_ELEM_OCCUPIED) {\
        RBD(Map, Elem_consOccupied)(&map->elems[j], hash, key, val);\
        map->len++;\
        break;\
      }\
    }\
    if (3 * map->len > 2 * map->cap) {\
      RBD(Map, _reserveUnchecked)(map, map->cap * 2);\
    }\
  }\
\
  void RBD(Map, _replace)(Map *map, Key key, Val val) {\
    size_t hash = RBD_IF(Key_hash)(Key_hash(key), 0);\
    for (size_t i = 0, j = hash % map->cap; i < map->cap; i++, j = (j + 1) % map->cap) {\
      if (map->elems[j].typ == RBD_MAP_ELEM_OCCUPIED && RBD_IF(Key_equals)(Key_equals(map->elems[j].key, key), map->elems[j].key == key)) {\
        RBD_IF(Val_des)(Val_des(map->elems[j].val),);\
        map->elems[j].val = val;\
        return;\
      }\
    }\
    __builtin_unreachable();\
  }\
\
  Val *RBD(Map, _at)(Map *map, Key key) {\
    size_t hash = RBD_IF(Key_hash)(Key_hash(key), 0);\
    for (size_t i = 0, j = hash % map->cap; i < map->cap; i++, j = (j + 1) % map->cap) {\
      if (map->elems[j].typ == RBD_MAP_ELEM_OCCUPIED && RBD_IF(Key_equals)(Key_equals(map->elems[j].key, key), map->elems[j].key == key)) {\
        return &map->elems[j].val;\
      }\
    }\
    __builtin_unreachable();\
  }\
\
  RBD(Map, Iter) RBD(Map, _find)(Map *map, Key key) {\
    size_t hash = RBD_IF(Key_hash)(Key_hash(key), 0);\
    for (size_t i = 0, j = hash % map->cap; i < map->cap; i++, j = (j + 1) % map->cap) {\
      if (map->elems[j].typ == RBD_MAP_ELEM_OCCUPIED && RBD_IF(Key_equals)(Key_equals(map->elems[j].key, key), map->elems[j].key == key)) {\
        return RBD(Map, Iter_cons)((RBD(Map, Iter)) {0}, &map->elems[j]);\
      } else if (map->elems[j].typ == RBD_MAP_ELEM_UNUSED) {\
        return RBD(Map, Iter_cons)((RBD(Map, Iter)) {0}, &map->elems[map->cap]);\
      }\
    }\
    return RBD(Map, Iter_cons)((RBD(Map, Iter)) {0}, &map->elems[map->cap]);\
  }\
\
  bool RBD(Map, _contains)(Map *map, Key key) {\
    size_t hash = RBD_IF(Key_hash)(Key_hash(key), 0);\
    for (size_t i = 0, j = hash % map->cap; i < map->cap; i++, j = (j + 1) % map->cap) {\
      if (map->elems[j].typ == RBD_MAP_ELEM_OCCUPIED && RBD_IF(Key_equals)(Key_equals(map->elems[j].key, key), map->elems[j].key == key)) {\
        return true;\
      } else if (map->elems[j].typ == RBD_MAP_ELEM_UNUSED) {\
        return false;\
      }\
    }\
    return false;\
  }\
\
  void RBD(Map, _erase)(Map *map, Key key) {\
    size_t hash = RBD_IF(Key_hash)(Key_hash(key), 0);\
    for (size_t i = 0, j = hash % map->cap; i < map->cap; i++, j = (j + 1) % map->cap) {\
      if (map->elems[j].typ == RBD_MAP_ELEM_OCCUPIED && RBD_IF(Key_equals)(Key_equals(map->elems[j].key, key), map->elems[j].key == key)) {\
        RBD(Map, Elem_des)(&map->elems[j]);\
        RBD(Map, Elem_consErased)(&map->elems[j]);\
        return;\
      }\
    }\
    __builtin_unreachable();\
  }\
\
  RBD(Map, Iter) RBD(Map, _begin)(Map *map) {\
    RBD(Map, Elem) *elem = map->elems;\
    while (elem->typ != RBD_MAP_ELEM_OCCUPIED) {\
      elem++;\
    }\
    return RBD(Map, Iter_cons)((RBD(Map, Iter)) {0}, elem);\
  }\
\
  RBD(Map, Iter) RBD(Map, _end)(Map *map) {\
    return RBD(Map, Iter_cons)((RBD(Map, Iter)) {0}, &map->elems[map->cap]);\
  }\
\
  bool RBD(Map, _equals)(Map *a, Map *b) {\
    if (a->len != b->len) {\
      return false;\
    }\
    for (size_t i = 0; i < a->cap; i++) {\
      if (a->elems[i].typ == RBD_MAP_ELEM_OCCUPIED) {\
        if (!RBD(Map, _contains)(b, a->elems[i].key)) {\
          return false;\
        }\
        if (!RBD_IF(Val_equals)(Val_equals(a->elems[i].val, *RBD(Map, _at)(b, a->elems[i].key)), (a->elems[i].val == *RBD(Map, _at)(b, a->elems[i].key)))) {\
          return false;\
        }\
      }\
    }\
    return true;\
  }\
\
  void RBD(Map, _debug)(Map *map, FILE *file, uint32_t depth) {\
    fprintf(file, #Map " (%p) {\n", map);\
    RBD_INDENT(file, depth + 1); fprintf(file, "elems: (%p) [\n", map->elems);\
    for (size_t i = 0; i < map->cap; i++) {\
      RBD_INDENT(file, depth + 2); RBD(Map, Elem_debug)(&map->elems[i], file, depth + 2); fprintf(file, ",\n");\
    }\
    RBD_INDENT(file, depth + 1); fprintf(file, "],\n");\
    RBD_INDENT(file, depth + 1); fprintf(file, "cap: %lu,\n", map->cap);\
    RBD_INDENT(file, depth + 1); fprintf(file, "len: %lu,\n", map->len);\
    RBD_INDENT(file, depth); fprintf(file, "}");\
  }\
\
  Map *RBD(Map, _des)(Map *map) {\
    for (size_t i = 0; i < map->cap; i++) {\
      RBD(Map, Elem_des)(&map->elems[i]);\
    }\
    RBD_IF(Allocator_free)(Allocator_free, free)(map->elems);\
    return map;\
  }

#endif // RBD_MAP_H
