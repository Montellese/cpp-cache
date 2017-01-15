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

#ifndef CPP_CACHE_POLICY_NONE_H_
#define CPP_CACHE_POLICY_NONE_H_

namespace cpp_cache
{
namespace policy
{
  template<class Key, typename SizeType>
  class none
  {
  public:
    using key_type = Key;
    using size_type = SizeType;

    none() = default;
    virtual ~none() = default;

  private:
    // helper for unused parameters
    struct sink
    {
      template<typename ...Args>
      explicit sink(Args&& ... ) { }
    };

  protected:
    inline virtual size_type size() const { return 0; }

    inline virtual bool empty() const { return true; }

    inline virtual bool has_key(const key_type& key) const { sink { key }; return true; }

    inline virtual bool touch_key(const key_type& key) const { sink { key }; return true; }

    template<typename... Args>
    inline std::vector<key_type> insert_key(const key_type& key, Args&&... args) { sink { key, args... }; return {}; }

    inline virtual bool erase_key(const key_type& key) { sink { key }; return true; }

    inline virtual void clear_keys() { }

    virtual std::vector<key_type> expire_keys() const { return {}; }
  };
}
}

#endif  // CPP_CACHE_POLICY_NONE_H_
