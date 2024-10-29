# ft_malloc
My very own replica of the legendary triumvirate: `malloc`, `free` and `realloc`.

This project invites you to open up the hood of these very well-known libc functions and understand really deeply what is going on everytime you allocate and deallocate a pointer using these functions. By writing my own implementation of a memory allocator in C, I learned more abot virtual memory, memory alignment, internal bookkeeping strategies, dynamic libraries and even memory defragmentation (although I didn't implement it myself). I had a lot of fun!

## Notable takeaways
- *"a call to malloc claims memory from the system"* - That is an adage repeated over and over about `malloc` especially amongst beginners, but this isn't necessarily the truth. It suffices to conceive `malloc` under this light when you are a programmer using it, but you soon realise that this is exactly what `malloc` is designed to AVOID doing: constantly making requests to the kernel.
- *"a call to free returns memory back to the system"* - Again this is probably how most people are introduced to `free`, but it's not entirely true. Memory is supposed to be handed back to the system by the end of process execution in most cases.
- "the `main()` function is the entrypoint for any program written in C" - This is a perfectly good way of understanding what a program is and how it works for beginners, but eventually you realise there's workaraounds for that. `malloc` needs them in order to properly work.
- "dynamic libraries are shared between processes" - While it is true that dynamic libraries are not copied into binary files during compilation, making them easy to share across programs, the dynamic library takes a separate address space for each process.
- at the end of the day, memory is literally just a big blob of zeroes and ones. That's it. What really matters is how you interpret memory, and that is why typecasting is such a powerful feature, especially in low-level languages like C.

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