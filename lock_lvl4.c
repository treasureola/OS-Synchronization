#include "types.h"
#include "stat.h"
#include "user.h"
#include "lock.h"

int main() {
	printf(1, "Test 4 Begin\n");

    int lock_parent = lock_create(LOCK_SPIN); // Create a lock in parent process
    int lock_child = -1;
    int pid = fork();

    if (pid == 0) {
        // Child process
        lock_child = lock_create(LOCK_SPIN); // Create a lock in child process

        // Child tries to access parent's lock
        if (lock_take(lock_parent) == 0) {
            printf(1, "Child: Successfully acquired parent's lock\n");
            lock_release(lock_parent); // Release parent's lock
        } else {
            printf(1, "Child: Failed to acquire parent's lock\n");
			exit();
        }

        // Child tries to access its own lock
        if (lock_take(lock_child) == 0) {
            printf(1, "Child: Successfully acquired own lock\n");
            lock_release(lock_child); // Release own lock
        } else {
            printf(1, "Child: Failed to acquire own lock\n");
			exit();
        }
        lock_delete(lock_parent);
        lock_delete(lock_child);
        exit();
    } else {
        // Parent process

        // Parent tries to access its own lock
        if (lock_take(lock_parent) == 0) {
            printf(1, "Parent: Successfully acquired own lock\n");
            lock_release(lock_parent); // Release own lock
        } else {
            printf(1, "Parent: Failed to acquire own lock\n");
        }

		wait();

		// Parent tries to access child's lock
        if (lock_take(lock_child) == 0) {
            printf(1, "Parent: Failed to fail acquiring child's lock\n");
            lock_release(lock_child); // Release child's lock
			exit();
        } else {
            printf(1, "Parent: Succeeded to fail acquiring child's lock\n");
        } 
    }

    lock_delete(lock_parent); // Delete parent's lock
	//lock_delete(lock_child);  // Delete child's lock

	printf(1, "Test 4 passed\n");

    exit();
}




















// #include "types.h"
// #include "user.h"
// #include "lock.h"


// int
// main(void){
// 	int create1 = lock_create(1);
// 	printf(1, "%s %d\n", "Lockid = ", create1); 
// 	printf(1, "------------------------------\n");


// 	int take1 = lock_take(create1);
// 	// printf(1, take1); 
// 	printf(1, "%s %d\n", "SHOULD RETURN 0", take1);
// 	printf(1, "------------------------------\n");

// 	int take2 = 0;

// 	printf(1, "HERE\n");
// 	if (fork() == 0){
// 		printf(1, "HERE\n");
// 		take2 = lock_take(create1);
// 		printf(1, "%s %d\n", "SHOULD RETURN -1", take2);
// 		printf(1, "------------------------------\n");

// 	}
// }