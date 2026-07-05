# Tut Week 1

## Operating Systems Intro

<details>
<summary>1. What are some differences between a processor running in privileged (kernel) mode and user mode? Why are the two modes needed?</summary>

> We need the 2 modes to make sure that user-level applications can't bypass or take control of the OS (or other applications).
>
> Kernel mode allows:
> - Access to all CPU instructions
> - Direct access to hardware (disk, I/O, etc)
> - Access to all address spaces (including the kernel's own)

</details>

<details>
<summary>2. What are the two main roles of an Operating System?</summary>

> 1. Provide an abstraction of the hardware to users
> 2. To divide/manage resources between multiple competing processes

</details>

<details>
<summary>3. How does a file system fulfill the two roles of an operating system?</summary>

> 1. Provide an abstraction of the hardware to users: the filesystem is just a wrapper to how files are stored on the disk.
> 2. To divide/manage resources between multiple competing processes: the filesystem needs to manage storage (a limited resource) between different competing processes while also enforcing limiting mechanisms, such as quotas.

</details>

<details>
<summary>4. Which of these should only be allowed in kernel mode?</summary>

> - disable interrupts
> - read time of day clock
> - set time of day clock
> - change memory map
> - write to disk controller register
> - Trigger the write of all buffered blocks associated with a file back to disk (fsync).

</details>

## OS System Call Interface

**5.** The following code uses typical UNIX process management system calls: `fork()`, `execl()`, `exit()` and `getpid()`.

```c
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define FORK_DEPTH 3

main()
{
  int i, r;
  pid_t my_pid;

  my_pid = getpid();

  for (i = 1; i <= FORK_DEPTH; i++) {

    r = fork();

    if (r > 0) {
      /* we're in the parent process after
         successfully forking a child */

      printf("Parent process %d forked child process %d\n", my_pid, r);

    } else if (r == 0) {

      /* We're in the child process, so update my_pid */
      my_pid = getpid();

      /* run /bin/echo if we are at maximum depth, otherwise continue loop */
      if (i == FORK_DEPTH) {
        r = execl("/bin/echo", "/bin/echo", "Hello World", NULL);

        /* we never expect to get here, just bail out */
        exit(1);
      }
    } else { /* r < 0 */
      /* Eek, not expecting to fail, just bail ungracefully */
      exit(1);
    }
  }
}
```

<details>
<summary>a. What is the value of i in the parent and child after fork?</summary>

> The child is a new independent process that is a copy of the parent. i in the child will have whatever the value was in the parent at the point of forking.

</details>

<details>
<summary>b. What is the value of my_pid in a parent after a child updates it?</summary>

> my_pid in a parent is not updated by any action of the child. The child and parent are independent after forking.

</details>

<details>
<summary>c. What is the process id of /bin/echo?</summary>

> execl replaces the contents of a running process with the specified executable. The process id does not change.

</details>

<details>
<summary>d. Why is the code after execl not expected to be reached in the normal case?</summary>

> A successful execl results in the current code being replaced. execl does not return if it succeeds, as there is no previous code to return to.

</details>

<details>
<summary>e. How many times is Hello World printed when FORK_DEPTH is 3?</summary>

> Hello World is printed 4 times if FORK_DEPTH is 3.

</details>

<details>
<summary>f. How many processes are created when running the code (including the first)?</summary>

> There are 8 processes involved in the execution of the code.

</details>

---

**6.** Look at this code snippet.

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char teststr[] = "The quick brown fox jumps over the lazy dog.\n";

main()
{
  int fd;
  int len;
  ssize_t r;

  fd = open("testfile", O_WRONLY | O_CREAT, 0600);
  if (fd < 0) {
    /* just ungracefully bail out */
    perror("File open failed");
    exit(1);
  }

  len = strlen(teststr);
  printf("Attempting to write %d bytes\n", len);

  r = write(fd, teststr, len);

  if (r < 0) {
    perror("File write failed");
    exit(1);
  }
  printf("Wrote %d bytes\n", (int) r);

  close(fd);
}
```

<details>
<summary>a. What does this code do?</summary>

> The code writes a string to a file. It will create a new file if needed (O_CREAT).

</details>

<details>
<summary>b. In addition to O_WRONLY, what are the other 2 ways to open a file?</summary>

> The other ways of opening a file are read-only (O_RDONLY) and read-write (O_RDWR).

</details>

<details>
<summary>c. What does open return in fd, and what is it used for?</summary>

> In the case of failure, fd is set to -1 to signify an error. In the case of success, fd is set to a file descriptor (an integer) that becomes a handle to the file. The file descriptor is used in the other file-related system calls to identify the file to operate on.

</details>

---

**7.** The following code is a variation of the previous code that writes twice.

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char teststr[] = "The quick brown fox jumps over the lazy dog.\n";

main()
{
  int fd;
  int len;
  ssize_t r;
  off_t off;

  fd = open("testfile2", O_WRONLY | O_CREAT, 0600);
  if (fd < 0) {
    /* just ungracefully bail out */
    perror("File open failed");
    exit(1);
  }

  len = strlen(teststr);
  printf("Attempting to write %d bytes\n", len);

  r = write(fd, teststr, len);

  if (r < 0) {
    perror("File write failed");
    exit(1);
  }
  printf("Wrote %d bytes\n", (int) r);

  off = lseek(fd, 5, SEEK_SET);
  if (off < 0) {
    perror("File lseek failed");
    exit(1);
  }

  r = write(fd, teststr, len);

  if (r < 0) {
    perror("File write failed");
    exit(1);
  }
  printf("Wrote %d bytes\n", (int) r);

  close(fd);
}
```

<details>
<summary>a. How big is the file (in bytes) after the two writes?</summary>

> 50 bytes. For each open file, the operating system keeps track of the current offset within the file. The current offset is where the next read or write will start from, and is usually at the location just past the end of the previous read or write. So one would expect the file size to be 90 bytes after two 45-byte writes, except for lseek's interference.

</details>

<details>
<summary>b. What is lseek() doing that affects the final file size?</summary>

> lseek sets the current offset to a specific location in the file. The lseek in the code moves the current offset from 45 bytes (after the initial write) to 5 bytes from the start of the file. The second write begins from offset 5 and writes 45 bytes, giving 50 bytes in total in the file.

</details>

<details>
<summary>c. What other options are there in addition to SEEK_SET?</summary>

> See the man page for details on SEEK_CUR and SEEK_END.

</details>

---

**8.** Compile either of the previous two code fragments on a UNIX/Linux machine and run `strace ./a.out` and observe the output.

<details>
<summary>a. What is strace doing?</summary>

> strace prints a trace of all system calls invoked by a process, together with the arguments to the system call. There are a lot of system calls at the beginning of a trace related to dynamically loading code libraries. Towards the end of the trace you will see the system calls you expect to see.

</details>

<details>
<summary>b. Without modifying the above code to print fd, what is the value of the file descriptor used to write to the open file?</summary>

> 3

</details>

<details>
<summary>c. printf does not appear in the system call trace. What is appearing in its place? What's happening here?</summary>

> printf is a library function that creates a buffer based on the string specification it is passed. The buffer is then written to the console using write() to file descriptor 1.

</details>

---

**9.** Compile and run the following code.

```c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

main()
{
  int r;
  r = chdir("..");
  if (r < 0) {
    perror("Eek!");
    exit(1);
  }

  r = execl("/bin/ls", "/bin/ls", NULL);
  perror("Double eek!");
}
```

<details>
<summary>a. What does this code do?</summary>

> The code sets the current working directory of the process to be the parent directory (one higher in the directory hierarchy), and then runs ls to list the directory.

</details>

<details>
<summary>b. After the program runs, the shell's working directory is the same. Why?</summary>

> The shell forks a child process that runs the code. Each process has its own current working directory, so the code above changes the current working directory of the child process; the current working directory of the parent process (the shell) remains the same.

</details>

<details>
<summary>c. In what directory does /bin/ls run? Why?</summary>

> exec replaces the contents of the child process with ls, not the environment the child process runs in. The current working directory is part of the environment that the OS manages on behalf of every process, so ls runs in the current working directory of the child process.

</details>

---

<details>
<summary>10. On UNIX, which of the following are considered system calls? Why? (read, printf, memcpy, open, strncpy)</summary>

>

</details>

## Processes and Threads

<details>
<summary>11. In the three-state process model, what do each of the three states signify? What transitions are possible between each of the states, and what causes a process (or thread) to undertake such a transition?</summary>

> The three states are:
> - Running: process has been scheduled and is running on the CPU
> - Ready: process is ready to be run but has not yet been dispatched
> - Blocked: process is waiting for some event to happen before execution
>
> Transitions:
> - Running to Ready: timeslice expired, yield, or higher priority process becomes ready.
> - Ready to Running: dispatcher chose the next thread to run.
> - Running to Blocked: a requested resource (file, disk block, printer, mutex) is unavailable, so the process is blocked waiting for the resource to become available.
> - Blocked to Ready: a resource has become available, so all processes blocked waiting for the resource now become ready to continue execution.

</details>

<details>
<summary>12. Given N threads in a uniprocessor system, how many threads can be running at the same point in time? How many can be ready? How many can be blocked?</summary>

> - Running threads = 0 or 1.
> - Blocked = N − Running − Ready
> - Ready = N − Running − Blocked

</details>

<details>
<summary>13. Compare reading a file using a single-threaded vs a multithreaded file server. It takes 15 msec to get a request and process it (block in cache). A disk operation is needed for 1/3 of requests, taking an additional 75 msec during which the thread sleeps. How many requests/sec for single-threaded? Multithreaded?</summary>

> In the single-threaded case, the cache hits take 15 msec and cache misses take 90 msec. The weighted average is 2/3 × 15 + 1/3 × 90. Thus the mean request takes 40 msec and the server can do 25 per second. For a multithreaded server, all the waiting for the disk is overlapped, so every request takes 15 msec, and the server can handle 66 2/3 requests per second.


</details>

## Critical Sections

<details>
<summary>14. The following fragment is a single line of code. How might a race condition occur if executed concurrently by multiple threads? Give an example of an incorrect result computed for x.

```c
x = x + 1;
```
</summary>

> The single code statement is compiled into multiple machine instructions. The memory location corresponding to x is loaded into a register, incremented, and then stored back to memory. During the interval between the load and store in the first thread, another thread may perform a load, increment, and store; and when control passes back to the first thread, the results of the second are then overwritten. Another outcome would be for the results of the first to be overwritten by the second (as the first thread loads, increments, then the second thread loads, increments, then the first thread stores, then the second thread stores).

</details>

<details>
<summary>15. The following function is called by multiple threads (potentially concurrently). Identify the critical section(s) that require mutual exclusion. Describe the race condition or why none exists.

```c
int i;

void foo()
{
    int j;
    /* random stuff */
    i = i + 1;
    j = j + 1;
    /* more random stuff */
}
```
</summary>

> There is no race condition on j, since it is a local variable per thread. However, i is a variable shared between threads. Thus i = i + 1 would form a critical section (assuming no random stuff is dependent on i).

</details>

<details>
<summary>16. Under what conditions would the following form a critical section?

```c
void inc_mem(int *iptr)
{
    *iptr = *iptr + 1;
}
```
</summary>

> Whether *iptr = *iptr + 1 forms a critical section depends on the scope of the pointer passed to inc_mem. If the pointer points to a local variable, then there is no race. If the pointer points to a shared global variable there is potential for a race, and thus the increment would become a critical section.

</details>