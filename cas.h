/*
 * Stolen this from the parsec library @ https://github.com/gwsystems/ps
 * Maintains the BSD 3 clause license.
 *
 * Return values:
 * 0 on failure due to contention (*target != old)
 * 1 otherwise (*target == old -> *target = updated)
 */
static inline int
cas(unsigned long *target, unsigned long old, unsigned long updated)
{
        char z;
        __asm__ __volatile__("lock cmpxchgl %2, %0; setz %1"
                             : "+m" (*target), "=a" (z)
                             : "q"  (updated), "a"  (old)
                             : "memory", "cc");
        return (int)z;
}
