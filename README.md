# cpp-cache #
![License](https://img.shields.io/github/license/Montellese/cpp-cache.svg) ![C++11](https://img.shields.io/badge/C%2B%2B-11-blue.svg)

cpp-cache is a header-only pure C++11 library providing an interface and some implementations for generic caching. Using it is as easy as
```cpp
#include <cpp-cache/fifo-cache.h>
...
cpp_cache::fifo_cache<int, std::string, 2> fifo_cache;

// insert a new element
fifo_cache.insert(1, "one");

// check if the element is cached
if (fifo_cache.has(1)) {
  std::cout << "Element 1 successfully cached" << std::endl;

  // get the element's value
  std::string one_value = fifo_cache.get(1);
  std::cout << "Element 1: " << fifo_cache.get(1) << std::endl;
}
else {
  std::cout << "Failed to cache element 1!" << std::endl;
  return;
}

// insert additional elements resulting in the first element to drop out of the cache
fifo_cache.insert(2, "two");
fifo_cache.insert(3, "three");

// check again if the first element is still cached
if (fifo_cache.has(1)) {
  std::cout << "Element 1 still cached" << std::endl;
}
else {
  std::cout << "Element 1 not cached anymore" << std::endl;
}
```
which will result in the following output on standard out:
```
Element 1 successfully cached
Element 1: one
Element 1 not cached anymore
```

----------
### Table of Contents ###
*   [Requirements](#requirements)
    *   [Buildsystem](#buildsystem)
*   [Features](#features)
    *   [Type-safety](#type-safety)
    *   [Caching policy](#caching-policy)
    *   [Storage policy](#storage-policy)
    *   [Threading policy](#threading-policy)
*   [The Future](#the-future)

### Requirements ###
The only thing required to use cpp-cache is a C++11 compliant compiler.

#### Buildsystem ####
To be able to run the tests provided with cpp-cache [CMake](https://cmake.org/) 3.1 or newer is required. Run the following commands to build the tests and execute them:
```bash
# mkdir build
# cd build
# cmake ..
# cmake --build
# ctest -C Debug
```

### Features ###
#### Type-safety ####
Thanks to C++11's templates cpp-cache calls are completely type-safe. It is not possible to store a value with a mismatching type in a cache.

#### Caching policy ####
When it comes to caching the most important decision is the caching policy. It defines and determines which values are kept in the cache and which ones are removed once the cache is full. Depending on the use case completely different caching policies are required. That's why cpp-cache provides a pre-defined set of caching policies but is not limited to them:
*   First In First Out (FIFO): `cpp_cache::fifo_cache<>`
*   Last In First Out (LIFO): `cpp_cache::lifo_cache<>`
*   Least Recently Used (LRU): `cpp_cache::lru_cache<>`
*   Most Recently Used (MRU): `cpp_cache::mru_cache<>`
*   Least Frequently Used (LFU): `cpp_cache::lfu_cache<>`
*   Time To Live (TTL): `cpp_cache::ttl_cache<>`
*   Random: `cpp_cache::random_cache<>`

It is also possible to implement and use custom caching policies. All that is required by any caching policy is to implement the following methods:
```cpp
size_type size();
bool empty();
bool has_key(const key_type& key);
bool touch_key(const key_type& key);
template<typename... Args> std::vector<key_type> insert_key(const key_type& key, Args&&... args);
bool erase_key(const key_type& key);
void clear_keys();
std::vector<key_type> expire_keys();
```

##### Chained caching #####
To be able to build even more complex caching policies each of the pre-defined caching policies supports chaining another caching policy. This way it would be possible to create a cache which combines the caching policies of an LRU and a TTL cache.
By default all pre-defined caching policies use the `cpp_cache::none<>` caching policy as the chained caching policy.

#### Storage policy ####
Performance is always an important topic when it comes to caching. To reach the best performance it is important to choose a proper storage policy which specifies how the cached items are stored and accessed. cpp-cache provides a pre-defined set of storage policies but is not limited to them:
*   Ordered / Unordered Map: `cpp_cache::map<>`

It is also possible to implement and use custom storage policies: All that is required by any storage policy is to implement the following methods:
```cpp
const stored_type& get_from_storage(const key_type& key);
void insert_into_storage(const key_type& key, const stored_type& value);
void erase_from_storage(const key_type& key);
void clear_storage();
```

#### Threading policy ####
When it comes to threading there are many different applications out there with different needs. Some applications run in a single thread and concurrency is not an issue. Other applications use multiple threads which can all potentially interact with the same cache and it is necessary to protect the cache's internal state. Because locking isn't free and has a negative impact on performance cpp-cache does not enforce locking but rather provides the possibility to choose the best fitting threading policy. This is achieved by specifying the threading policy in `cpp_cache::cache<Key, T, CachingPolicy, StoragePolicy, LockingPolicy>`. cpp-cache comes with the following threading policies:
*   No locking (default): `cpp_cache::no_locking`

It is also possible to implement and use custom threading policies. All that is required by any threading policy is to implement the following methods (matching `std::mutex`):
```cpp
void lock();
void unlock();
```

### The Future ###
I'm always open for new ideas and feedback.
