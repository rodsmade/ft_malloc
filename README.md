# ft_malloc
My very own replica of the legendary triumvirate: `malloc`, `free` and `realloc`.

This project invites you to look under the hood of these widely known `libc` functions and gain a deeper understanding of what really happens each time you allocate or deallocate a pointer using them. By creating my own implementation of a memory allocator in C, I've learned a lot about virtual memory, memory alignment, internal bookkeeping strategies, dynamic libraries, and even memory defragmentation (though I didn't implement it myself). I had a lot of fun!

## Notable takeaways
- *"A call to malloc claims memory from the system."* This adage, common among beginners, isn't quite accurate. While it's fine to think of `malloc` this way when you're a programmer using it, it's actually designed to AVOID making constant requests to the kernel.

- *"A call to free returns memory back to the system."* Again, this is how `free` is often explained to beginners, but it's not the full story. Memory is typically only handed back to the system at the end of a process, not with every call to `free`.

- *"The `main()` function is the entry point for any program written in C."* This is generally true, but eventually, you discover workarounds. `malloc` in fact relies on these mechanisms to work properly.

- *"Dynamic libraries are shared between processes."* While it is true that dynamic libraries aren't copied into binaries during compilation, thus allowing for them to be shared across programs, they still take up a unique address space when loaded into each process.

- At the end of the day, memory is simply a massive blob of zeroes and ones — nothing more. What truly matters is *how* you interpret memory, and this is why typecasting is such a powerful tool, especially in low-level languages like C.

## Project Features
- Compiler attributes (`__atribute__((constructor|destructor))`)
- Internal bookkeeping
- Hex dump of memory layout for debugging purposes
- Thread-safety with mutexes
- Unit tests

## Run this project locally
First clone this git repository and get in the repository:
```
git clone https://github.com/rodsmade/ft_malloc.git
cd ft_malloc
```

Then running the following commands:

```make``` - to compile the dynamic library

```make run``` - to run the `main.c` file present in the repository

```make tests``` - to run the unit tests