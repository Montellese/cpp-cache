/*
 *  Copyright (C) Sascha Montellese
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
