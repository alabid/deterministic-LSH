# deterministic-LSH
Deterministic Locality Sensitive Hashing

**Not ready for use at all**

Dependencies
------------
You need to install the following
(possibly through macports or brew if you are on a mac):
* `FLANN`
* `LZ4`

Use
---
You need to modify the following variables in the `Makefile`:
* `FLANN_INCLUDES`: Points to `flann` header files
* `FLANN_LINKS`: Points to `flann` static or dynamic libraries
* `LZ4_LIB`: Points to `lz4` library files

To compile, run `make`. Then run main binary produced in current directory. `*.o` files and other secondary
binary files are stored in `bin/`.

Rough Plan
----------
### Stage 0
* Setup `Makefile`

### Stage 1: Core Implementation
* Implement deterministic LSH with one basic test.
* Implement classical LSH algorithm(s) with one basic test.
* [Remember to update implementation section in final project report.]

### Stage 2: Evaluation
* Evaluate determistic LSH while varying parameters.
* Evaluate classical LSH algorithm(s) while varying parameters.
* [Remember to update evaluation section in final project report.]

### Stage 3: Final Report
Final project due Dec 14

Todo List
----------
1. ~~vector\<bool> for linear scan~~
2. ~~deterministic LSH~~
3. ~~success prob for randomized LSH~~
4. measure time for all algorithms
5. ~~measure recall~~
6. experiment for success probability
7. impact of varing parameters c, k in randomized LSH
8. presentation slides
9. report
