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

#ifndef CPP_CACHE_POLICY_LIFO_H_
#define CPP_CACHE_POLICY_LIFO_H_

#include <algorithm>
#include <cstddef>
#include <deque>
#include <utility>
#include <vector>

#include "none.h"

namespace cpp_cache
{
namespace policy
{
  template<class Key, size_t MaxSize, class ChainedCachingPolicy = none<Key, size_t>>
  class lifo : public ChainedCachingPolicy
  {
  public:
    using key_type = Key;
    using size_type = size_t;

    lifo()
      : queue_()
    { }

    virtual ~lifo()
    {
      clear_keys();
    }

    inline virtual size_type max_size() const { return MaxSize; }

  protected:
    inline virtual size_type size() const override { return queue_.size(); }

    inline virtual bool empty() const override { return queue_.empty(); }

    inline virtual bool has_key(const key_type& key) const override
    {
      return find(key) != queue_.cend();
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
      expire_keys(expired_keys);

      // if we already have the key there's nothing to do
      if (has_key(key))
        return expired_keys;

      // check if we need to expire a key as well
      if (is_full())
      {
        // expire the first key in the queue
        expired_keys.push_back(queue_.front());
        queue_.pop_front();
      }

      // insert the new key at the beginning
      queue_.push_front(key);

      return expired_keys;
    }

    inline virtual bool erase_key(const key_type& key) override
    {
      auto it = find(key);
      if (it == queue_.cend())
        return false;

      ChainedCachingPolicy::erase_key(key);
      queue_.erase(it);

      return true;
    }

    inline virtual void clear_keys() override
    {
      ChainedCachingPolicy::clear_keys();
      queue_.clear();
    }

    virtual std::vector<key_type> expire_keys() const override
    {
      // expire on the chained policy
      std::vector<key_type> expired_keys = ChainedCachingPolicy::expire_keys();

      // remove all the keys that were expired in the chained policy
      expire_keys(expired_keys);

      // nothing else to do because we expire on insert
      return expired_keys;
    }

  private:
    using queue = std::deque<key_type>;

    inline bool is_full() const
    {
      return size() >= max_size();
    }

    inline typename queue::const_iterator find(const key_type& key) const
    {
      return std::find(queue_.cbegin(), queue_.cend(), key);
    }

    void expire_keys(const std::vector<key_type>& keys) const
    {
      for (const auto& key : keys)
        queue_.erase(find(key));
    }

    mutable queue queue_;
  };
}
}

#endif  // CPP_CACHE_POLICY_LIFO_H_
