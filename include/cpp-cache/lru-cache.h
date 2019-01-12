/*
 *  Copyright (C) Sascha Montellese
 */

#ifndef CPP_CACHE_LRU_CACHE_H_
#define CPP_CACHE_LRU_CACHE_H_

#include "cache.h"
#include "policy/lru.h"
#include "storage/map.h"

namespace cpp_cache
{
  template<class Key, class T, size_t MaxSize, class StoragePolicy = storage::map<Key, T>, class LockingPolicy = no_locking>
  using lru_cache = cpp_cache::cache<Key, T, policy::lru<Key, MaxSize>, StoragePolicy, LockingPolicy>;
}

#endif  // CPP_CACHE_LRU_CACHE_H_