/*
 *  Copyright (C) Sascha Montellese
 */

#ifndef CPP_CACHE_TTL_CACHE_H_
#define CPP_CACHE_TTL_CACHE_H_

#include "cache.h"
#include "policy/ttl.h"
#include "storage/map.h"

namespace cpp_cache
{
  template<class Key, class T, size_t MaxAgeMs, class StoragePolicy = storage::map<Key, T>, class LockingPolicy = no_locking>
  using ttl_cache = cpp_cache::cache<Key, T, policy::ttl<Key, MaxAgeMs>, StoragePolicy, LockingPolicy>;
}

#endif  // CPP_CACHE_TTL_CACHE_H_