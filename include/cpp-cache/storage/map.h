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
