# alloc
## Copyright
Public domain, 2024

github.com/SirJonthe

## About
`alloc` is a minimalist C++11 library that allows heap memory management within a pre-defined memory region, allowing for tighter control over memory.

## Design
`alloc` only provides a single class, `allocator`. The `allocator` can only query some size statistics, and allocate a given number of bytes or deallocate memory associated with a given pointer.

An `allocator` object does not own its own data, meaning the user provides the data and creates an `allocator` object to manage the memory region. As such, the `allocator` object does not allocate its own main memory region, nor does it free such data should it have been dynamically allocated.

`allocator` can safely be copied. Allocating from a copy of an `allocator` object affects all copies.

All metadata regarding the management of the internal heap is stored in the heap itself, meaning the `allocator` class does not dynamically allocate any memory outside of the internal heap. This comes at the cost of consuming more memory inside the predefined memory region.

## Building
No special adjustments need to be made to build `alloc` except enabling C++11 compatibility or above. Simply include the relevant headers in your code and make sure the headers and source files are available in your compiler search paths. Using `g++` as an example, building is no harder than:

```
g++ -std=c++11 code.cpp alloc/alloc.cpp
```

...where `code.cpp` is an example source file containing the user-defined code, such as program entry point.

## Examples
### Create an `allocator` object
```
#include "alloc/alloc.h"

int main()
{
	const int SIZE = 1024;
	char memory[SIZE]; // Allocate memory to use as a heap on stack.

	cc0::allocator alloc(memory, SIZE); // Set the stack memory to become the memory region from which to allocate memory.

	return 0;
}
```
* Note: There is a third parameter to the `allocator` constructor which is the memory alignment requirement (all allocations are aligned to the given byte boundry).

### Allocate memory
```
#include "alloc/alloc.h"

int main()
{
	const int SIZE = 1024;
	char memory[SIZE]; // Allocate memory to use as a heap on stack.
	cc0::allocator alloc(memory, SIZE); // Set the stack memory to become the memory region from which to allocate memory.

	void *ptr = alloc.alloc(64); // Allocates 64 bytes of memory (NULL on failure).

	return 0;
}
```
* Note: Allocations may be adjusted to be larger than the requested memory due to memory alignment and/or if there is no possibility to store additional blocks of memory after the current one. Unline C and C++ standard allocations, requesting an allocation of 0 size yields a NULL pointer and does not result in any modification of the internal memory at all.

### Deallocate memory
```
#include "alloc/alloc.h"

int main()
{
	const int SIZE = 1024;
	char memory[SIZE]; // Allocate memory to use as a heap on stack.
	cc0::allocator alloc(memory, SIZE); // Set the stack memory to become the memory region from which to allocate memory.
	void *ptr = alloc.alloc(64); // Allocates 64 bytes of memory (NULL on failure).
	
	alloc.free(ptr); // `ptr` is now freed, and the memory can be reused for subsequent allocations.

	return 0;
}
```

### Checking size statistics
```
#include "alloc/alloc.h"

int main()
{
	const int SIZE = 1024;
	char memory[SIZE]; // Allocate memory to use as a heap on stack.
	cc0::allocator alloc(memory, SIZE); // Set the stack memory to become the memory region from which to allocate memory.
	void *ptr = alloc.alloc(64); // Allocates 64 bytes of memory (NULL on failure).
	
	int occupied = alloc.occupied_bytes();
	int total = alloc.total_bytes();

	return 0;
}
```
