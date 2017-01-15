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

#ifndef CPP_CACHE_POLICY_RANDOM_H_
#define CPP_CACHE_POLICY_RANDOM_H_

#include <cstddef>
#include <random>
#include <unordered_set>
#include <utility>
#include <vector>

#include "none.h"

namespace cpp_cache
{
namespace policy
{
  template<class Key, size_t MaxSize, class ChainedCachingPolicy = none<Key, size_t>>
  class random : public ChainedCachingPolicy
  {
  public:
    using key_type = Key;
    using size_type = size_t;

    random()
      : set_()
      , rand_(rd_())
    {
      set_.reserve(MaxSize);
    }

    virtual ~random()
    {
      clear_keys();
    }

    inline virtual size_type max_size() const { return MaxSize; }

  protected:
    inline virtual size_type size() const override { return set_.size(); }

    inline virtual bool empty() const override { return set_.empty(); }

    inline virtual bool has_key(const key_type& key) const override
    {
      return set_.find(key) != set_.cend();
    }

    inline virtual bool touch_key(const key_type& key) const override
    {
      // pass the touch on to the chained policy
      if (!ChainedCachingPolicy::touch_key(key))
        return false;

      return has_key(key);
    }

    template<typename... Args>
    inline std::vector<key_type> insert_key(const key_type& key, Args&&... args)
    {
      // insert the key into the chained policy
      std::vector<key_type> expired_keys = ChainedCachingPolicy::insert_key(key, std::forward<Args>(args)...);

      // remove all the keys that were expired in the chained policy
      erase_keys(expired_keys);

      // if we already have the key there's nothing to do
      if (has_key(key))
        return expired_keys;

      // check if we need to expire a key as well
      if (is_full())
      {
        // expire a random key in the set
        std::uniform_int_distribution<size_type> dist(0, size() - 1);
        auto it = set_.cbegin();
        std::advance(it, dist(rand_));

        expired_keys.push_back(*it);

        set_.erase(it);
      }

      // insert the new key at the beginning
      set_.insert(key);

      return expired_keys;
    }

    inline virtual bool erase_key(const key_type& key) override
    {
      auto it = set_.find(key);
      if (it == set_.cend())
        return false;

      ChainedCachingPolicy::erase_key(key);
      set_.erase(it);

      return true;
    }

    inline virtual void clear_keys() override
    {
      ChainedCachingPolicy::clear_keys();
      set_.clear();
    }

    virtual std::vector<key_type> expire_keys() const override
    {
      // expire on the chained policy
      std::vector<key_type> expired_keys = ChainedCachingPolicy::expire_keys();

      // remove all the keys that were expired in the chained policy
      erase_keys(expired_keys);

      // nothing else to do because we expire on insert
      return expired_keys;
    }

  private:
    using set = std::unordered_set<key_type>;

    inline bool is_full() const
    {
      return size() >= max_size();
    }

    void erase_keys(const std::vector<key_type>& keys) const
    {
      for (const auto& key : keys)
        set_.erase(key);
    }

    mutable set set_;
    std::random_device rd_;
    std::default_random_engine rand_;
  };
}
}

#endif  // CPP_CACHE_POLICY_LRU_H_
