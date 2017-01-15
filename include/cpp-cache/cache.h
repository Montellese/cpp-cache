/*
 *  Copyright (C) Sascha Montellese
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with cpp-signals; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CPP_CACHE_CACHE_H_
#define CPP_CACHE_CACHE_H_

#include <mutex>
#include <stdexcept>
#include <utility>

namespace cpp_cache
{
  class no_locking
  {
  public:
    no_locking() = default;
    ~no_locking() = default;

    inline void lock() { }
    inline void unlock() { }
  };

  template<class Key, class T, class CachingPolicy, class StoragePolicy, class LockingPolicy = no_locking>
  class cache : public CachingPolicy, protected StoragePolicy
  {
  public:
    using key_type = Key;
    using cached_type = T;
    using caching_policy = CachingPolicy;
    using storage_policy = StoragePolicy;
    using locking_policy = LockingPolicy;

    explicit cache() = default;

    ~cache()
    {
      clear();
    }

    typename caching_policy::size_type size() const
    {
      std::lock_guard<locking_policy> lock(lock_);

      // first expire elements if necessary
      expire();

      return caching_policy::size();
    }

    bool empty() const
    {
      std::lock_guard<locking_policy> lock(lock_);

      // first expire elements if necessary
      expire();

      return caching_policy::empty();
    }

    bool has(const key_type& key) const
    {
      std::lock_guard<locking_policy> lock(lock_);

      // first expire elements if necessary
      expire();

      return has_internal(key);
    }

    const cached_type& get(const key_type& key) const
    {
      std::lock_guard<locking_policy> lock(lock_);

      return get_internal(key);
    }

    bool try_get(const key_type& key, cached_type& value) const
    {
      std::lock_guard<locking_policy> lock(lock_);

      try
      {
        value = get_internal(key);
      }
      catch (std::out_of_range&)
      {
        return false;
      }

      return true;
    }

    bool touch(const key_type& key)
    {
      std::lock_guard<locking_policy> lock(lock_);

      // first expire elements if necessary
      expire();

      return caching_policy::touch_key(key);
    }

    template<typename... CachingPolicyArgs>
    void insert(const key_type& key, const cached_type& value, CachingPolicyArgs&&... args)
    {
      std::lock_guard<locking_policy> lock(lock_);

      // first expire elements if necessary
      expire();

      // insert the key into the caching policy and get any expired keys
      const auto expired_keys = caching_policy::insert_key(key, std::forward<CachingPolicyArgs>(args)...);

      // remove the expired keys from the storage policy
      expire_from_storage(expired_keys);

      // insert the item into the storage policy
      storage_policy::insert_into_storage(key, value);
    }

    void erase(const key_type& key)
    {
      std::lock_guard<locking_policy> lock(lock_);

      // first expire elements if necessary
      expire();

      // erase the key from the caching policy
      if (!caching_policy::erase_key(key))
        return;

      // erase the item from the storage policy
      storage_policy::erase_from_storage(key);
    }

    void clear()
    {
      std::lock_guard<locking_policy> lock(lock_);
      storage_policy::clear_storage();
      caching_policy::clear_keys();
    }

  private:
    inline bool has_internal(const key_type& key) const
    {
      // TODO: what if it is present in the caching policy but not in the storage policy???
      return caching_policy::has_key(key);
    }

    const cached_type& get_internal(const key_type& key) const
    {
      // first expire elements if necessary
      expire();

      // check if the key is cached and if so touch it
      if (!has_internal(key) || !caching_policy::touch_key(key))
        throw std::out_of_range("element not present in the cache");

      return storage_policy::get_from_storage(key);
    }

    void expire() const
    {
      // get all the expired keys from the caching policy
      const auto expired_keys = caching_policy::expire_keys();

      // remove the expired keys from the storage policy
      expire_from_storage(expired_keys);
    }

    void expire_from_storage(const std::vector<key_type>& keys) const
    {
      for (const auto& key : keys)
        storage_policy::erase_from_storage(key);
    }

    mutable locking_policy lock_;
  };
}

#endif  // CPP_CACHE_CACHE_H_
