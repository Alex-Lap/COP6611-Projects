#include "types.h"
#include "stat.h"
#include "user.h"

// Global shared variable to demonstrate shared address space
int global_counter = 0;
int NUM_THREADS = 4;
int ITERATIONS = 1000;

// Thread function
void thread_function(void *arg)
{
  int thread_id = (int)arg;
  int i;
  
  printf(1, "Thread %d: Starting\n", thread_id);
  
  // Increment the global counter to demonstrate shared memory
  for(i = 0; i < ITERATIONS; i++) {
    global_counter++;
  }
  
  printf(1, "Thread %d: Finished, global_counter = %d\n", thread_id, global_counter);
  
  // We don't have a proper thread exit, so just sleep for a bit
  // This avoids crashing before our test completes
  sleep(100);
  
  // This will exit the thread but might cause issues
  exit();
}

// Test memory sharing between threads
void test_shared_memory(void)
{
  void *stacks[8];
  int tids[8];
  int i;
  
  printf(1, "Starting shared memory test with %d threads\n", NUM_THREADS);
  printf(1, "Initial global_counter = %d\n", global_counter);
  
  // Create threads
  for(i = 0; i < NUM_THREADS; i++) {
    // Allocate stack for this thread
    stacks[i] = malloc(4096);
    if(stacks[i] == 0) {
      printf(1, "Error: could not allocate stack for thread %d\n", i);
      exit();
    }
    
    // Create the thread
    tids[i] = clone(thread_function, (void*)i, stacks[i]);
    if(tids[i] < 0) {
      printf(1, "Error: failed to create thread %d\n", i);
      exit();
    }
    
    printf(1, "Created thread %d with ID %d\n", i, tids[i]);
  }
  
  // Sleep to let threads run
  sleep(500);
  
  // Expected final counter value: NUM_THREADS * ITERATIONS
  printf(1, "All threads completed\n");
  printf(1, "Final global_counter = %d\n", global_counter);
  printf(1, "Expected value = %d\n", NUM_THREADS * ITERATIONS);
  
  if(global_counter == NUM_THREADS * ITERATIONS) {
    printf(1, "SHARED MEMORY TEST PASSED!\n");
  } else {
    printf(1, "SHARED MEMORY TEST FAILED!\n");
  }
}

// Test that each thread has its own stack
void stack_value_function(void *arg)
{
  int thread_id = (int)arg;
  int stack_var = thread_id * 100;  // Different value per thread
  
  printf(1, "Thread %d: stack_var = %d\n", thread_id, stack_var);
  sleep(100);
  printf(1, "Thread %d: stack_var still = %d\n", thread_id, stack_var);
  
  // We don't have a proper thread exit, so just sleep for a bit
  sleep(100);
  exit();
}

void test_separate_stacks(void)
{
  void *stacks[8];
  int tids[8];
  int i;
  
  printf(1, "\nStarting separate stacks test with %d threads\n", NUM_THREADS);
  
  // Create threads
  for(i = 0; i < NUM_THREADS; i++) {
    // Allocate stack for this thread
    stacks[i] = malloc(4096);
    if(stacks[i] == 0) {
      printf(1, "Error: could not allocate stack for thread %d\n", i);
      exit();
    }
    
    // Create the thread
    tids[i] = clone(stack_value_function, (void*)i, stacks[i]);
    if(tids[i] < 0) {
      printf(1, "Error: failed to create thread %d\n", i);
      exit();
    }
  }
  
  // Sleep to let threads run
  sleep(500);
  
  printf(1, "SEPARATE STACKS TEST COMPLETED\n");
}

int main(void)
{
  // Test that memory is shared between threads
  test_shared_memory();
  
  // Test that stacks are separate
  test_separate_stacks();
  
  // Sleep to let all test output finish
  sleep(200);
  
  exit();
}