# Fast hash table implementation

-----

## Requiremtns 

Required to impelement hash table with decreased amount of dynamic memory allocations (`new` operator calls). Implemented hash table must provide API to overriding comparator and hash function.


## Implementation description

### Main idea

Main idea to decrease new calls count is to use hash table with open addressing instead classic hash table with chaining. In this repo implemented variation of hash table published in [article on habr](https://habr.com/ru/company/mailru/blog/323242) with internal table size equal to power of 2 for fast `mod` computation. Another option - use prime numbers for table size can decrease table's memory footprint is harder to implement and leads to longer `mod` computation.

### Details

To use this hash map user need to include [header](HashTable.hpp) file with `custom_map` implementation. 

`custom_map` provides following base methods:
- `insert`
- `erase`
- `search` which returnz `Iterator` on found object (or `end` otherwise)

Also, it's possible to provide own custom hash function and comparator as template args like in `std::unordered_map`.

## Testing

### Суть

Added only simple tests for integer keys.
 
### Launching

Build:

```bash
g++ -std=c++17 -pedantic -Wall test.cpp -o run
```

Run:

```bash
./run
```

Work done and tested on Linux OS.


-----

**Made by Maksim Bronnikov**

