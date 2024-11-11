#include "types.h"
#include "user.h"
#include "lock.h"

int
main(void){

	// int create1 = lock_create(1);
	int create2 = lock_create(2);
	// int take2 = 2;
	int S_take2 = 2;

	// for (int i=0; i<1024; i++){
		if (fork() == 0){
			printf(1, "TAKE LOCK 1 AGAIN - SHOULD RETURN -1\n");
			S_take2 = lock_take(create2);  //try to acquire the lock again
			if (S_take2 == -1){
				printf(1, "%s %d\n", "S_take2 = ", S_take2); //should return -1 becuase the lock has been acquired
				printf(1, "TAKE LOCK 1 AGAIN SUCCESSFUL\n");
			} else{
				printf(1, "TAKE LOCK 1 AGAIN UNSUCCESSFUL\n");
			}
			printf(1, "------------------------------\n");
			printf(1, "\n");
	// }
	}
	wait();
	exit();
}