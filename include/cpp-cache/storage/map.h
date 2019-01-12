/*
 *  Copyright (C) Sascha Montellese
 */

#ifndef CPP_CACHE_STORAGE_MAP_H_
#define CPP_CACHE_STORAGE_MAP_H_

#include <unordered_map>

namespace cpp_cache
{
namespace storage
{
  template<class Key, class T, class Map = std::unordered_map<Key, T>>
  class map
  {
  public:
    using key_type = Key;
    using stored_type = T;
    using map_type = Map;

    map() = default;

    ~map()
    {
      clear_storage();
    }

  protected:
    const stored_type& get_from_storage(const key_type& key) const
    {
      return map_.at(key);
    }

    void insert_into_storage(const key_type& key, const stored_type& value)
    {
      // check if we already know the key
      auto it = map_.find(key);

      // if not add the key and value to the map
      if (it == map_.end())
        map_.insert(std::make_pair(key, value));
      // otherwise update the value
      else
        map_[key] = value;
    }

    void erase_from_storage(const key_type& key) const
    {
      map_.erase(key);
    }

    inline void clear_storage()
    {
      map_.clear();
    }

  private:
    mutable Map map_;
  };
}
}

#endif  // CPP_CACHE_STORAGE_MAP_H_
