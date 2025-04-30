// Add this to a new file uthread.c

#include "types.h"
#include "stat.h"
#include "user.h"

#define THREAD_STACK_SIZE 4096
#define MAX_THREADS 8

int thread_create(void (*fn)(void*), void *arg) {
  void *stack;
  int tid;
  
  // Allocate stack for the thread
  stack = malloc(THREAD_STACK_SIZE);
  if(!stack)
    return -1;
  
  // Call clone system call
  tid = clone(fn, arg, stack);
  if(tid < 0) {
    free(stack);
    return -1;
  }
  
  return tid;
}

// Add these to user.h
int thread_create(void (*fn)(void*), void *arg);