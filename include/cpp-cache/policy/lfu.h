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

#ifndef CPP_CACHE_POLICY_LRU_H_
#define CPP_CACHE_POLICY_LRU_H_

#include <cstddef>
#include <list>
#include <unordered_map>
#include <vector>

#include "none.h"

namespace cpp_cache
{
namespace policy
{
  template<class Key, size_t MaxSize, class ChainedCachingPolicy = none<Key, size_t>>
  class lfu : protected ChainedCachingPolicy
  {
  public:
    using key_type = Key;
    using size_type = size_t;

    lfu()
      : list_()
      , map_()
    { }

    virtual ~lfu()
    {
      clear_keys();
    }

    inline virtual size_type max_size() const { return MaxSize; }

  protected:
    inline virtual size_type size() const override { return map_.size(); }

    inline virtual bool empty() const override { return map_.empty(); }

    inline virtual bool has_key(const key_type& key) const override
    {
      return map_.find(key) != map_.cend();
    }

    inline virtual void get_key(const key_type& key) const override
    {
      // first pass the action on to the chained policy
      ChainedCachingPolicy::get_key(key);

      // now move the (recently) used key to the front
      auto list_it = map_.at(key);
      list_.splice(list_.begin(), list_, list_it);
    }

    inline virtual std::vector<key_type> insert_key(const key_type& key) override
    {
      // insert the key into the chained policy
      std::vector<key_type> expired_keys = ChainedCachingPolicy::insert_key(key);

      // remove all the keys that were expired in the chained policy
      erase_keys(expired_keys);

      // add the key to the front of the list (most recently used)
      list_.push_front(key);

      // if we already have the key we need to remove it from the list because
      // we just added it to the front
      auto it = map_.find(key);
      if (it != map_.cend())
        list_.erase(it->second);

      // add / update the key in the map
      map_[key] = list_.begin();

      // check if we need to expire a key as well
      if (is_too_full())
      {
        // expire the last key in the list
        key_type last_key = list_.back();
        map_.erase(last_key);
        list_.pop_back();

        expired_keys.push_back(last_key);
      }

      return expired_keys;
    }

    inline virtual bool erase_key(const key_type& key) override
    {
      auto it = map_.find(key);
      if (it == map_.cend())
        return false;

      ChainedCachingPolicy::erase_key(key);

      list_.erase(it->second);
      map_.erase(it);

      return true;
    }

    inline virtual void clear_keys() override
    {
      ChainedCachingPolicy::clear_keys();

      map_.clear();
      list_.clear();
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
    using list = std::list<key_type>;
    using list_iterator = typename list::iterator;
    using map = std::unordered_map<key_type, list_iterator>;

    inline bool is_too_full() const
    {
      return size() > max_size();
    }

    void erase_keys(const std::vector<key_type>& keys) const
    {
      for (const auto& key : keys)
      {
        auto it = map_.find(key);
        if (it == map_.cend())
          continue;

        list_.erase(it->second);
        map_.erase(it);
      }
    }

    mutable list list_;
    mutable map map_;
  };
}
}

#endif  // CPP_CACHE_POLICY_LRU_H_
