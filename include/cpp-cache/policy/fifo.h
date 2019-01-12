/*
 *  Copyright (C) Sascha Montellese
 */

#ifndef CPP_CACHE_POLICY_FIFO_H_
#define CPP_CACHE_POLICY_FIFO_H_

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
  class fifo : public ChainedCachingPolicy
  {
  public:
    using key_type = Key;
    using size_type = size_t;

    fifo()
      : queue_()
    { }

    virtual ~fifo()
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
        // expire the last key in the queue
        expired_keys.push_back(queue_.back());
        queue_.pop_back();
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

#endif  // CPP_CACHE_POLICY_FIFO_H_
