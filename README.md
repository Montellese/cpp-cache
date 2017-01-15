# cpp-cache #
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