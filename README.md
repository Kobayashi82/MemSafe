
<div align="center">

![System & Kernel](https://img.shields.io/badge/System-brown?style=for-the-badge)
![Memory & FD Management](https://img.shields.io/badge/Memory%20&%20FD-Management-blue?style=for-the-badge)
![Norminette-OK](https://img.shields.io/badge/Norminette-OK-green?style=for-the-badge)
![C Language](https://img.shields.io/badge/Language-C-red?style=for-the-badge)

*Wrapper functions for safe memory and system resource management*

</div>

<div align="center">
  <img src="/MemSafe.png">
</div>

# MemSafe

[README en Español](README_es.md)

`MemSafe` is a wrapper library that provides a safety layer for automatic memory and system resource management in C applications.

This project intercepts standard OS calls (`malloc`, `calloc`, `realloc`, `free`, `open`, `close`, `execve`, etc.) to automatically release resources and prevent memory `leaks` in case of errors or program termination.

## ✨ Features

- `Automatic memory management`: Tracks and frees all allocated memory.
- `File descriptor management`: Automatically handles open/close of file descriptors.
- `execve safety`: Safely duplicates arguments and environment before executing new processes.
- `Efficient hash table`: Uses a hash table for fast tracking of memory pointers.
- `Mass release`: Special function to free all resources with a single call.
- `Transparent usage`: Wrappers are transparent to user code.
- `POSIX compatible`: Standard implementation for Unix/Linux systems.
- `Norminette compliant`: Meets 42 Norminette standards.

## 📦 Components

### safe_mem.c
Wrapper for memory management functions:
- `__wrap_malloc()` - Intercepts malloc and registers the pointer in a hash table
- `__wrap_free()` - Intercepts free and removes the pointer from the hash table
- Special function: `free((void *)-42)` - Frees all registered memory

**Technical details:**
- Hash table with 1031 entries for optimal distribution
- Custom hash function to minimize collisions
- Collision handling via linked lists
- O(1) average insert/lookup
- Does not free unallocated memory (no error)
- Does not free already freed memory (no error)

### safe_xmem.c
Wrapper for extra memory management functions (requires `safe_mem.c`):
- `__wrap_calloc()` - Intercepts calloc and registers the pointer in a hash table
- `__wrap_realloc()` - Intercepts realloc and updates the pointer in the hash table

### safe_fd.c
Wrapper for file descriptor management:
- `__wrap_open()` - Intercepts open and registers the descriptor in a table
- `__wrap_close()` - Intercepts close and updates the descriptor table
- `__wrap_dup()` - Intercepts dup and registers the new descriptor
- `__wrap_dup2()` - Intercepts dup2 and manages both descriptors
- `__wrap_pipe()` - Intercepts pipe and registers both ends
- Special function: `close(-42)` - Closes all open file descriptors

**Technical details:**
- Static table of 1024 entries (0-1023)
- Supports `O_CREAT` flags with variadic mode
- Manages standard descriptors (stdin, stdout, stderr)

### safe_execve.c
Execution wrapper (requires `safe_mem.c`):
- `__wrap_execve()` - Intercepts `execve`, duplicates args/environment and frees resources automatically

**Technical details:**
- Deep copy of string arrays for `argv` and `envp`
- Restores stdin, stdout, stderr after freeing resources
- Safe memory handling on execve failure

### safe_exit.c
Termination wrapper (requires `safe_mem.c`):
- `__wrap_exit()` - Manages process exit via `exit`, both on normal completion and error conditions

**Technical details:**
- Uses `__attribute__((constructor(101)))` for early handler initialization
- Registers with `atexit()` to guarantee cleanup on normal termination
- Signal handlers for termination signals (SIGTERM, SIGINT, SIGQUIT, SIGHUP)

## 🔧 Installation

### Build the test project

```sh
make
```

This generates the `test` executable with all wrappers applied.

### Run the test

```sh
./test
```

The test program demonstrates:
- Multiple memory allocations
- File opening
- Pipe creation
- Descriptor duplication
- execve call
- Automatic resource cleanup

### Valgrind verification

The project includes the `leaks` script that runs Valgrind with appropriate options to detect memory leaks and FD leaks:

```sh
./leaks
```

This script runs:
- `--leak-check=full` - Full memory leak analysis
- `--show-leak-kinds=all` - Shows all leak kinds
- `--track-fds=yes` - Tracks open file descriptors
- `--trace-children=yes` - Follows child processes

### Integrate into your project

To use MemSafe in your project, add the linker flags and .c files:

```makefile
# Demo Makefile

NAME	= myprogram
CC		= clang -g

LDFLAGS = -Wl,--wrap=malloc  \
		  -Wl,--wrap=calloc  \
		  -Wl,--wrap=realloc \
		  -Wl,--wrap=free    \
		  -Wl,--wrap=open    \
		  -Wl,--wrap=close   \
		  -Wl,--wrap=dup     \
		  -Wl,--wrap=dup2    \
		  -Wl,--wrap=pipe    \
		  -Wl,--wrap=execve  \
		  -Wl,--wrap=exit

SRCS	= safe_mem.c         \
		  safe_xmem.c        \
		  safe_fd.c          \
		  safe_execve.c      \
		  safe_exit.c        \
		  myprogram.c

OBJS    = $(SRCS:%.c=obj/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(LDFLAGS) -o $(NAME) $(OBJS)

obj/%.o: %.c
	@mkdir -p $(@D)
	@$(CC) -o $@ -c $<

re: fclean all

clean:
	@rm -rf obj 2> /dev/null

fclean: clean
	@rm -f $(NAME) 2> /dev/null

.PHONY: all clean fclean re
```

### Usage in code

Your application code does not need changes. Just use standard functions:

```c
#include <stdlib.h>
#include <fcntl.h>

int main(void)
{
    char *ptr;
    int  fd;

	ptr = malloc(100);                // Automatically tracked
	fd = open("file.txt", O_RDONLY);  // Automatically tracked

    // rest of code...

    // Free all tracked memory
    free((void *)-42);

    // Close all open file descriptors
    close(-42);

    // On exit, everything is freed automatically, either via normal exit, exit() call, or a fatal signal
}
```

## ⚠️ Considerations

- `Memory overhead`: Each tracked pointer uses extra memory (16 bytes) for the list node.
- `Thread-safety`: This implementation is NOT thread-safe. Mutexes are required for multi-threaded use.
- `FD limit`: Supports up to 1024 file descriptors.
- `-42 usage`: The magic value `-42` is used for special commands. Avoid using this value in your code.
- `Performance`: Overhead is minimal for most applications, but can be significant in programs with millions of allocations.

---

## 📄 License

This project is licensed under the WTFPL – [Do What the Fuck You Want to Public License](http://www.wtfpl.net/about/).

---

<div align="center">

**🧮 Developed by Kobayashi82 🧮**

*"Memory leaks? Not on my watch!"*

</div>
