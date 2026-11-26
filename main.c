#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct header {
  size_t size;
  struct header *next;
  bool free;
} header;

#define MAX_SIZE sizeof(header)

void *globalBase = NULL;

header *findFreeBlock(header **last, size_t size) {
  header *current = globalBase;
  while (current && !(current->size >= size && current->free)) {
    *last = current;
    current = current->next;
  }
  return current;
}

header *requestMemory(header *last, size_t size) {
  header *block = sbrk(0);
  void *request = sbrk(size + MAX_SIZE);

  assert(request == block);
  if (request == (void *)-1) {
    return NULL;
  }
  if (last) {
    last->next = block;
  }

  block->size = size;
  block->free = false;
  block->next = NULL;
  return block;
}

void *my_malloc(size_t size) {
  header *block;
  if (size <= 0) {
    return NULL;
  }
  if (!globalBase) {
    block = requestMemory(NULL, size);
    if (!block) return NULL;
    globalBase = block;
  } else {
    header *last = globalBase;
    block = findFreeBlock(&last, size);
    if (!block) {
      block = requestMemory(last, size);
      if (!block)
        return NULL;
    } else {
      block->free = false;
    }
  }
  return (block + 1);
}

void my_free(void *ptr) {
  if (!ptr)
    return;
  header *block = (header *)ptr - 1;
  if(block->next && block->next->free) {
    block->size += block->next->size;
    block->next = block->next->next;
  }
  block->free = true;
}

/*
void printAllMem() {
  header *current = globalBase;
  while(current) {
    printf("%lu\n", current->size);
    current = current->next;
  }
  printf("\n");
}

void coalesceTest() {
  char *text = my_malloc(13);
  char *text2 = my_malloc(16);
  printAllMem();
  my_free(text2);
  my_free(text);
  char *text3 = my_malloc(26);
  printAllMem();
}

*/

int main(void) {
  /*
  coalesceTest();
  */

  return 0;
}
