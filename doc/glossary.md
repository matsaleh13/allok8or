# allok8or Glossary

## Purpose

Common terms associated with memory allocation and management exist, but their usage is not consistent across the domain. This document establishes how these and other terms are used within this project.

## Glossary

- **allocator**: An abstraction that reserves a unique region of memory of a given size and alignment for exclusive use by calling code.
- **block**: A contiguous region of memory that has been allocated by an allocator. A single block is used by the client of the allocator to contain a single instance of given type.
- **block array**: A contiguous region of memory that has been allocated by an allocator, the size of which is a multiple of a fixed block size. A block array is used by the client of an allocator to contain an array of instances of the same type. 
- **pool**: An abstract container from which a client can retrieve blocks for use and return them after use.
- **arena**: A contiguous region of memory that is intended to be subdivided into blocks or block arrays.