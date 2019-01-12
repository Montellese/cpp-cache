/*
 *  Copyright (C) Sascha Montellese
 */

#ifndef CPP_CACHE_POLICY_TTL_H_
#define CPP_CACHE_POLICY_TTL_H_

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <ratio>
#include <unordered_map>
#include <utility>
#include <vector>

#include "none.h"

namespace cpp_cache
{
namespace policy
{
  template<class Key, size_t MaxAgeMs, class ChainedCachingPolicy = none<Key, size_t>>
  class ttl : public ChainedCachingPolicy
  {
  private:
    using time = std::chrono::time_point<std::chrono::system_clock>;
    using duration = std::chrono::milliseconds;
    struct ttl_key
    {
      time start_;
      duration duration_;
      time end_;
    };

  public:
    using key_type = Key;
    using age_type = size_t;
    using duration_type = duration;
    using size_type = size_t;

    ttl()
      : map_()
    { }

    virtual ~ttl()
    {
      clear_keys();
    }

    inline virtual age_type default_max_age() const { return MaxAgeMs; }

  protected:
    inline virtual size_type size() const override { return map_.size(); }

    inline virtual bool empty() const override { return map_.empty(); }

    inline virtual bool has_key(const key_type& key) const override
    {
      return map_.find(key) != map_.cend();
    }

    inline virtual bool touch_key(const key_type& key) const override
    {
      // pass the touch on to the chained policy
      if (!ChainedCachingPolicy::touch_key(key))
        return false;

      // try to find the key
      auto it = map_.find(key);
      if (it == map_.cend())
        return false;

      // update the end time of the key because it was just used
      it->second.end_ = time::clock::now() + it->second.duration_;

      return true;
    }

    template<typename... Args>
    inline std::vector<key_type> insert_key(const key_type& key, Args&&... args)
    {
      // insert the key into the chained policy
      auto expired_keys = ChainedCachingPolicy::insert_key(key, std::forward<Args>(args)...);

      // remove all the keys that were expired in the chained policy
      erase_keys(expired_keys);

      // pass the call to the proper internal handler relying on SFINAE
      insert_key_internal(key, std::forward<Args>(args)...);

      return expired_keys;
    }

    inline virtual bool erase_key(const key_type& key) override
    {
      auto it = map_.find(key);
      if (it == map_.cend())
        return false;

      ChainedCachingPolicy::erase_key(key);
      map_.erase(it);

      return true;
    }

    inline virtual void clear_keys() override
    {
      ChainedCachingPolicy::clear_keys();
      map_.clear();
    }

    virtual std::vector<key_type> expire_keys() const override
    {
      // expire on the chained policy
      std::vector<key_type> expired_keys = ChainedCachingPolicy::expire_keys();

      // remove all the keys that were expired in the chained policy
      erase_keys(expired_keys);

      // get the current time
      time now = time::clock::now();

      // find and remove all keys that have expired their time-to-live
      for (auto it = map_.cbegin(); it != map_.cend(); )
      {
        if (it->second.end_ < now)
        {
          expired_keys.push_back(it->first);
          map_.erase(it++);
        }
        else
          ++it;
      }

      return expired_keys;
    }

  private:
    using map = std::unordered_map<key_type, ttl_key>;

    template<typename... Args>
    inline void insert_key_internal(const key_type& key, Args&&... args)
    {
      return insert_key_internal(key, default_max_age(), std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void insert_key_internal(const key_type& key, age_type max_age_ms, Args&&... args)
    {
      return insert_key_internal(key, duration_type(max_age_ms), std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void insert_key_internal(const key_type& key, duration_type max_age_duration, Args&&... args)
    {
      time now = time::clock::now();

      // try to find the key
      auto it = map_.find(key);
      // if it doesn't exist yet insert it and determine the end time
      if (it == map_.cend())
        map_.insert(std::make_pair(key, ttl_key { now, max_age_duration, now + max_age_duration }));
      else
      {
        // update the duration and the end time of the key because it was just used
        it->second.duration_ = max_age_duration;
        it->second.end_ = now + it->second.duration_;
      }
    }

    void erase_keys(const std::vector<key_type>& keys) const
    {
      for (const auto& key : keys)
        map_.erase(key);
    }

    mutable map map_;
  };
}
}

#endif  // CPP_CACHE_POLICY_TTL_H_
