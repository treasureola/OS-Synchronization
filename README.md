# Synchronization

## User-Level Interface

The main lock API is somewhat similar to the `pthread`s API, and focuses on being relatively simple.

```
#define MAX_NUM_LOCKS 16
#define LOCK_ADAPTIVE_SPIN 1024

typedef enum {
	LOCK_SPIN,
	LOCK_BLOCK,
	LOCK_ADAPTIVE,
} lock_type_t;

int  lock_create(lock_type_t type);
int  lock_take(int lockid);
int  lock_release(int lockid);
void lock_delete(int lockid);
```

You should place these in `lock.h` which can be included both in user-level code, and in the kernel.
If your implementation of those functions at user-level needs code beyond the system call stubs, you should implement it in `ulib.c`.
Each lock is identified by a unique integer (`lockid`).

- `lock_create` returns the `lockid` of the created lock that is a number `>= 0` on success.
	This process now has access to this lock, and can (without error) use the rest of the functions in the API.
	It also takes the *type* of the lock that is being created.
	You'll support spin-based locks (`LOCK_SPIN`), block-based locks (`LOCK_BLOCK`), and adaptive locks that combine both (`LOCK_ADAPTIVE`).
	If an error is encountered (i.e. the system already has `MAX_NUM_LOCKS`), it returns `-1`.
- `lock_take` takes as its argument a lock that was previously created by `lock_create` within this process, or within a parent of this process.
	Once this function returns without error (`0`), the current thread has a critical section.
	This function can return an error (`-1`) if the calling thread *already holds the lock*, or doesn't have access to that lock!
- `lock_release` releases the critical section it has previously taken, returning `0`.
	Return `-1` for an error if we do not already hold the lock, or if this process doesn't have access to the lock.
- `lock_delete` deallocates the kernel part of the lock, and makes it inaccessible to the current process (i.e. its lock id can no longer be used to access that lock).
	If the lock is currently held by the thread doing the `lock_delete`, this also releases it.
	This operation does nothing if we pass in a `lockid` we don't have access to.

Note that the *implementation* of these locks must be in the kernel, so this API can essentially define a set of new system calls you need to implement.

## Lock Implementation Details

Locks are implemented in the kernel by you, and they have three main components.

1. The lock id which is an integer identifier that the user and kernel use as a shared means to identify the lock.
1. The kernel's data-structure associated with each process that tracks the locks that process has access to.
1. The kernel's data-structure which tracks *all* of the system's locks (i.e. `MAX_NUM_LOCKS` locks), their state (i.e. if they are held, and by which process).

**Lock ids, and process lock tables.**
Lock ids, should be used at the system call layer to identify which lock user-level is trying to operate on.
Lock ids are simple integers that identify one of a process's locks.
Each process has a "lock table" which is just an array of the locks that it has open (very much like the open file array in processes).
For simplicity, you might as well make the lock id be the lock's offset into this table.

**Lock type.**
The `type` of a lock determines how it is implemented in the kernel.
If it is a `LOCK_SPIN`, then it will use a loop to spin actively (and not block), awaiting another process to release the lock.
You should not spin with interrupts disabled (recall that `pushcli/cli` and `popcli/sti` disable and enable interrupts, respectively).
If the type is `LOCK_BLOCK`, then the lock implementation will block the thread when there is critical section contention.
For `LOCK_ADAPTIVE`, if the critical section is contended, the implementation will spin `LOCK_ADAPTIVE_SPIN` times, and if the lock is still contended, it will block awaiting the release of the critical section.

Remember that your lock implementation must be itself implemented in a manner that avoids race conditions.
Note, that you must *never* execute user-level code with interrupts disabled.
For spin-based implementations, if you need an atomic instruction, you can use `xv6`'s `xchg` or the compare-and-swap (`cas`) implementation I provide in `cas.h`.
I highly recommend that you re-use `xv6` abstractions for implementing each of your implementations, where possible.
If your spin-based implementations seem to spin forever, especially when only a single cpu is enabled, consider 1. that you may be spinning without interrupts enabled, or 2. virtualization might be messing with the timer interrupts.
The former is much more likely.

**Testing different lock type behavior.**
It is important to understand in which situation, each of these implementations will be most useful as this will guide your testing of the implementations.
One of the main discriminating factors is the number of cores active in the system, and the other is how many contending threads there are.

- If you only use a single cpu?
- If the number of contending threads is much greater than `CPUS`?
- If the number of contending threads is less than or equal to `CPUS`?

Note that you can execute `qemu` with varying number of cpus using `CPUS=1 make qemu` for one, and `CPUS=4 make qemu` for four.
It is likely not worthwhile to run `qemu` with more cores than your physical machine has.

**Interaction with `fork`.**
When a process forks, you *must* copy the lock table of the parent into the child so that both the child and parent have access to the lock.
This is the only way for multiple processes to share a lock.
This means that if a parent has open locks when they fork, the child should be able to access those locks.
When any of the processes `exit`s, or call `lock_delete`, they will remove the lock from their process's lock table, but only when the *last process* that has a reference to the lock `exit`s or calls `lock_delete` is the actual lock in the kernel reclaimed for future lock allocations.

## Example

Locks provide critical sections for shared resources, but I'm not assuming for this assignment that your thread implementation is complete.
So the natural question is what is the "shared resource" if it isn't something in global, shared memory?
We are going to use `pipe`s.
These are message passing channels that are asynchronous, but with a bounded size (`512` in `xv6`).
We are going to have a number of child processes that are all separately writing a single byte at a time to a shared pipe.
Our "critical section" is to ensure that each of the child processes must write in batches of three bytes to the pipe.
Without locks, the children will interleave their `write`s to the pipe, and possibly preempt each other's access to the pipe, thus interleaving their access to the pipe at granularities less than three.

Please see `lock_example.c` for an example of how critical sections are used to protect multiple groups of writes to a shared pipe between child processes.
Note that locks are not enabled in this example as you haven't yet implemented them!

## Kernel Implementation

Each lock will need a kernel data-structure to track its state.
As `xv6` discourages dynamic allocation, we are going to statically allocate an array of locks for all processes in the system to use (much like there is a static array of processes).
This array has `MAX_NUM_LOCKS` locks in it, thus the system should be able to have up to and including that number of locks active (created) at any point in time.
As discussed above, you'll also need to track for each process which locks it has access to.

# Suggested Reading

**Locks.**
I suggest that you understand *all* of the existing lock interfaces (as discussed in lab, this includes `spinlock`s and `sleeplock`s), and the `wakeup`/`sleep` interface (which is similar to condition variables) in `xv6`.
You may not need to use all of these APIs, but understanding them will better prepare you to implement the specification.
You might be able to use some of these APIs in your implementation, and there is a relatively simple way to implement the blocking functionality in the kernel, if you understand these APIs well.

**Lock tables.**
To understand how to implement the per-process lock tables, and properly interact with `fork` and `exit`, you can look at the file table implementation for existing `xv6` processes.
These are the list of open files for each process.
Just as with our locks, open file descriptors (similar to lock ids) are inherited across `fork`s, and only when the last reference to a file is removed (via `exit` or `close`), will we drop our access to the file.

**Global lock table.**
You have the single global array of locks that are used to allocate (`lock_create`) locks in your system.
This is very similar to how processes exist in a big array until we need to allocate them (`fork`).
