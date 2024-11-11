#include "types.h"
#include "user.h"
#include "lock.h"

int main(void)
{
	printf(1, "Stating test SPIN\n");
	int S_take1 = 2;
	int S_take2 = 2;
	int release1 = -2;
	printf(1, "CREATE LOCK 1\n");
	int S_create1 = lock_create(0); //create first lock
	printf(1, "%s %d\n", "Lockid = ", S_create1);
	printf(1, "CREATE LOCK 1 SUCCESSFUL\n");
	printf(1, "------------------------------\n");
	printf(1, "\n");

	printf(1, "TAKE LOCK 1\n");
	S_take1 = lock_take(S_create1); //acquire the first lock
	if (S_take1 == 0){
		printf(1, "%s %d\n", "S_take1 = ", S_take1); //should return 0
		printf(1, "TAKE LOCK 1 SUCCESSFUL\n");
	} else{
		printf(1, "TAKE LOCK 1 UNSUCCESSFUL\n");
	}
	printf(1, "------------------------------\n");
	printf(1, "\n");

	if (fork() == 0){
		printf(1, "TAKE LOCK 1 AGAIN - SHOULD RETURN -1\n");
		S_take2 = lock_take(S_create1);  //try to acquire the lock again
		if (fork()  == 0){
			release1 = lock_release(S_create1);
			printf(1, "%s %d\n", "release1 = ", release1);
		}
		if (S_take2 == -1){
			printf(1, "%s %d\n", "S_take2 = ", S_take2); //should return -1 becuase the lock has been acquired
			printf(1, "TAKE LOCK 1 AGAIN SUCCESSFUL\n");
		} else{
			printf(1, "TAKE LOCK 1 AGAIN UNSUCCESSFUL\n");
		}
		printf(1, "------------------------------\n");
		printf(1, "\n");
	}


	wait();
	exit();
}