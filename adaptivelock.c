#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "adaptivelock.h"

void 
adaptiveinitlock(struct adaptivelock *lk, char *name){
	initlock(&lk->lk, "adaptive lock");
	lk->locked = 0;
	lk->name = name;
	lk->pid = 0;
	lk->adaptivecount = 0;
}

void
adaptiveacquire(struct adaptivelock *lk){
	int hold = adaptiveholding(lk);
	if (hold == 1){
		return;
	}
	if (lk->adaptivecount < 1024){ //sleep
		xchg(&lk->locked, 1);
		__sync_synchronize();
	} 
	else{ //spins
		acquire(&lk->lk); 
		while(lk->locked){
			sleep(lk, &lk->lk);
		}
		lk->locked = 1;
		lk->pid = myproc()->pid;
		release(&lk->lk);
	}
	lk->adaptivecount++;
}

void
adaptiverelease(struct adaptivelock *lk){
	int hold = adaptiveholding(lk);
	if (hold == 0){
		return;
	}
	acquire(&lk->lk); 
	lk->locked = 0;
	lk->pid = 0;
	wakeup(lk);
	release(&lk->lk);
}

int
adaptiveholding(struct adaptivelock *lk){
	if ((lk->locked == 1) && (lk->pid == myproc()->pid)){ 
		return 1;
	}else{
		return 0;
	}
}