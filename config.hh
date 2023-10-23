#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#ifndef CONFIG
#define CONFIG

#define ROW_SIZE 8192 //Bytes
#define N_BANKS 8
#define N_RANKS 1
#define MEM_SIZE 4  //Memory Size in GB.
#define N_ROWS  (MEM_SIZE*1024*1024*1024)/(N_RANKS*N_BANKS*ROW_SIZE)
#define COL_SIZE  8 //in bytes.
#define N_COLS_PER_ROW  (ROW_SIZE/COL_SIZE)
#define PAGE_SIZE  4096
#define N_HUGEPAGE 1024
#define HUGEPAGE_SIZE 2*1024*1024
#define OIO  0
#define IOI  1
#define HCOUNT  1000000
#define TIMESPEC_NSEC(ts) ((ts)->tv_sec * 1e9 + (ts)->tv_nsec)

typedef struct rowmap {
	uint64_t bank;
	uint64_t row;
	uint64_t p_addr;
	uint64_t v_addr;
} rowmap;

uint64_t virt_to_phys(uint64_t v_addr);
rowmap get_physical_mapping( uint64_t v_addr);

static inline __attribute__ ((always_inline))
uint64_t realtime_now()
{
	struct timespec now_ts;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now_ts);
	//CLOCK_MONOTONIC
	//CLOCK_PROCESS_CPUTIME_ID
	//CLOCK_THREAD_CPUTIME_ID
	return TIMESPEC_NSEC(&now_ts);
}

static inline __attribute__ ((always_inline))
void mfence()
{
	asm volatile ("mfence":::"memory");
}

static inline __attribute__ ((always_inline))
void clflushopt(volatile void *p)
{
	asm volatile ("clflush (%0)\n"::"r" (p):"memory");
}




#endif
