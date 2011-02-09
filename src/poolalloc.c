/* $Id: poolalloc.c 42272 2010-04-26 23:14:55Z espreon $ */
#ifndef DISABLE_POOL_ALLOC
/*
   Copyright (C) 2008 by David White <dave@whitevine.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/*
This file defines a custom allocator that is optimized for doing memory
allocations for Wesnoth. Its primary consideration is space, though it
should be pretty fast too.

The largest consideration is meta-data: Wesnoth allocates many small chunks,
and so we want to minimize per-chunk meta-data. Typical general-purpose
allocators have a per-chunk overhead of one or two pointers. This allocator
has no per-chunk overhead, just memory overhead of less than 2%.

The allocator is designed to handle small chunks. We include dlmalloc's source,
and allocations that are not considered small are simply punted to dlmalloc
to allocate.

Some basic terminology:

- chunk: a single allocation, allocated with malloc.
- block: a block of memory from which we allocate chunks. A block has a header
and then its data section. A block should be a multiple of the page size.
A given block is dedicated to allocating chunks of a specific size. All blocks
are the same size (4096 bytes by default, which should be the minimum).
- superblock: we allocate one huge block from which all blocks are allocated.
*/

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* dlmalloc(size_t size);
void* dlcalloc(size_t count, size_t size);
void* dlvalloc(size_t size);
void* dlmemalign(size_t alignment, size_t size);
void* dlrealloc(void* ptr, size_t size);
void dlfree(void* ptr);

#define BLOCK_SIZE (4096)

#define MAX_CHUNK_SIZE (256)
#define CHUNK_SIZE_STEP (sizeof(void*))
#define NUM_POOLS ((MAX_CHUNK_SIZE/CHUNK_SIZE_STEP) + 1)

// find the index of the pool that a chunk of size n will be allocated from.
#define GET_POOL_INDEX(n) ((n)/CHUNK_SIZE_STEP)
#define ROUNDUP_SIZE(n) (((n)%CHUNK_SIZE_STEP) ? ((n) + CHUNK_SIZE_STEP - ((n)%CHUNK_SIZE_STEP)) : (n))

#define CUSTOM_MEMORY_SIZE (1024*1024*40)
uint8_t* begin_superblock_range = NULL;
uint8_t* begin_superblock = NULL;
uint8_t* end_superblock = NULL;
#define IS_OUR_PTR(ptr) ((uint8_t*)(ptr) >= begin_superblock_range && (uint8_t*)(ptr) < end_superblock)

pthread_t main_thread;

void init_custom_malloc()
{
	main_thread = pthread_self();

	// allocate the memory -- allocate an extra block at the end, so that
	// if the address we get back isn't block-aligned, we can advance
	// the pointer until it is.
	void* alloc = dlmalloc(CUSTOM_MEMORY_SIZE + BLOCK_SIZE);
	assert(alloc);
	begin_superblock = (uint8_t*)alloc;
	while(((uintptr_t)begin_superblock)%BLOCK_SIZE) {
		++begin_superblock;
	}

	end_superblock = begin_superblock + CUSTOM_MEMORY_SIZE;
	begin_superblock_range = begin_superblock;
}

typedef struct BlockHeader {
	uint32_t check_a;
	struct Block* next;
	struct Block* prev;
	uint8_t* uninit;
	void* free_list;
	uint32_t chunk_size;
	uint32_t allocated_chunks;
	uint32_t check_b;
} BlockHeader;

typedef struct Block {
	BlockHeader header;
	char data[BLOCK_SIZE - sizeof(BlockHeader)];
} Block;

#define BLOCK_EMPTY(b) ((b)->header.allocated_chunks == 0)
#define BLOCK_FULL(b) ((b)->header.uninit == NULL && (b)->header.free_list == NULL)

void* allocate_chunk_from_block(Block* b)
{
	b->header.allocated_chunks++;
	if(b->header.uninit) {
		void* result = b->header.uninit;
		b->header.uninit += b->header.chunk_size;

		//check if we've run out of uninitialized elements.
		if(b->header.uninit + b->header.chunk_size > (uint8_t*)(b+1)) {
			b->header.uninit = NULL;
		}

		return result;
	}

	assert(b->header.free_list);
	void* result = b->header.free_list;
	b->header.free_list = *(void**)result;
	return result;
}

// inline causes linker errors in debug builds with gcc 4.3.2(-std=c99 and -O0)
#ifdef DEBUG
#define inline
#endif
inline Block* get_block_from_chunk(void* chunk)
#ifdef DEBUG
#undef inline
#endif
{
	int8_t* block_ptr = ((int8_t*)chunk) - ((uintptr_t)chunk)%BLOCK_SIZE;
	return (Block*)block_ptr;
}

Block* free_chunk_from_block(void* chunk)
{
	Block* block = get_block_from_chunk(chunk);
	block->header.allocated_chunks--;
	*(void**)chunk = block->header.free_list;
	block->header.free_list = chunk;

	return block;
}

Block* format_block(Block* ptr, int chunk_size)
{
	BlockHeader* block = &ptr->header;

	block->check_a = 0xFFFFFFFF;
	block->check_b = 0xFFFFFFFF;
	
	block->next = NULL;
	block->prev = NULL;
	block->uninit = (uint8_t*)(block+1);
	block->free_list = NULL;
	block->chunk_size = chunk_size;
	block->allocated_chunks = 0;
	return ptr;
}

Block* block_free_list = NULL;

Block* allocate_new_block(uint32_t chunk_size)
{
	if(block_free_list == NULL && begin_superblock >= end_superblock) {
		return NULL;
	}

	Block* block;

	if(block_free_list != NULL) {
		block = block_free_list;
		block_free_list = block->header.next;
	} else {
		block = (Block*)begin_superblock;
		begin_superblock += sizeof(Block);
	}

	return format_block(block, chunk_size);
}

void return_block_to_free_list(Block* block)
{
	block->header.next = block_free_list;
	block_free_list = block;
}

Block* block_pools[NUM_POOLS];

#define IS_BLOCK_ORPHAN(block) ((block)->header.next == NULL && (block)->header.prev == NULL && block_pools[GET_POOL_INDEX(block->header.chunk_size)] != (block))

void add_block_to_pool(Block* block)
{
	assert(block->header.chunk_size > 0 && block->header.chunk_size <= MAX_CHUNK_SIZE);
	Block** target = &block_pools[GET_POOL_INDEX(block->header.chunk_size)];
	block->header.next = *target;
	block->header.prev = NULL;
	if(*target) {
		(*target)->header.prev = block;
	}
	*target = block;
}

void make_block_orphan(Block* block)
{
	BlockHeader* header = &block->header;
	if(block_pools[GET_POOL_INDEX(header->chunk_size)] == block) {
		block_pools[GET_POOL_INDEX(header->chunk_size)] = header->next;
	}

	if(header->prev) {
		header->prev->header.next = header->next;
	}

	if(header->next) {
		header->next->header.prev = header->prev;
	}

	header->prev = NULL;
	header->next = NULL;
}

// A list of the chunks that were allocated in the main thread, but free()
// was called in another thread. We can't deallocate them from another thread,
// so we put them in this array. The main thread will free all these chunks,
// whenever it can't immediately allocate memory.
void** free_chunks;
size_t nfree_chunks, capacity_free_chunks;
pthread_mutex_t free_chunks_mutex = PTHREAD_MUTEX_INITIALIZER;

//mutex to protect all calls to dlmalloc.
pthread_mutex_t dlmalloc_mutex = PTHREAD_MUTEX_INITIALIZER;

void free_memory(void* ptr);

void collect_memory_from_other_threads()
{
	pthread_mutex_lock(&free_chunks_mutex);
	int n;
	for(n = 0; n != nfree_chunks; ++n) {
		free_memory(free_chunks[n]);
	}

	nfree_chunks = 0;
	pthread_mutex_unlock(&free_chunks_mutex);
}

void free_memory_from_other_thread(void* ptr)
{
	pthread_mutex_lock(&free_chunks_mutex);
	
	if(nfree_chunks == capacity_free_chunks) {
		capacity_free_chunks *= 2;
		if(capacity_free_chunks < 16) {
			capacity_free_chunks = 16;
		}

		pthread_mutex_lock(&dlmalloc_mutex);
		void** new_free_chunks = (void**)dlrealloc(free_chunks, sizeof(void*)*capacity_free_chunks);
		pthread_mutex_unlock(&dlmalloc_mutex);
		if(!new_free_chunks) {
			pthread_mutex_unlock(&free_chunks_mutex);
			fprintf(stderr, "DLREALLOC FAILED!\n");
			return;
		}

		free_chunks = new_free_chunks;
	}

	free_chunks[nfree_chunks++] = ptr;
	pthread_mutex_unlock(&free_chunks_mutex);
}

Block* get_block(uint32_t chunk_size)
{
	const int index = GET_POOL_INDEX(chunk_size);
	assert(index >= 0 && index < sizeof(block_pools)/sizeof(*block_pools));
	if(block_pools[index]) {
		return block_pools[index];
	}

	// free memory from other threads and then try again. This requires a mutex
	// lock, but this code should be rarely reached.
	collect_memory_from_other_threads();

	if(block_pools[index]) {
		return block_pools[index];
	}

	Block* block = allocate_new_block(chunk_size);
	if(block == NULL) {
		return block;
	}
	add_block_to_pool(block);
	return block;
}

void* allocate_memory(int32_t size)
{
	Block* block = get_block(size);
	if(block == NULL) {
		return NULL;
	}

	void* result = allocate_chunk_from_block(block);
	if(BLOCK_FULL(block)) {
		make_block_orphan(block);
	}

	return result;
}

void free_memory(void* ptr)
{
	Block* block = free_chunk_from_block(ptr);
	if(IS_BLOCK_ORPHAN(block)) {
		add_block_to_pool(block);
	} else if(BLOCK_EMPTY(block)) {
		//since the block is empty, return it to the global free list of
		//blocks, so it can be moved to a different pool.
		make_block_orphan(block);
		return_block_to_free_list(block);
	}
}

void* malloc(size_t size)
{
	if(pthread_equal(pthread_self(), main_thread) && size > 0 && size <= MAX_CHUNK_SIZE) {
		size = ROUNDUP_SIZE(size);
		void* result = allocate_memory(size);
		if(result != NULL) {
			return result;
		}
	}

	pthread_mutex_lock(&dlmalloc_mutex);
	void* result = dlmalloc(size);
	pthread_mutex_unlock(&dlmalloc_mutex);
	return result;
}

void* calloc(size_t count, size_t size)
{
	pthread_mutex_lock(&dlmalloc_mutex);
	void* result = dlcalloc(count, size);
	pthread_mutex_unlock(&dlmalloc_mutex);
	return result;
}

void* valloc(size_t size)
{
	pthread_mutex_lock(&dlmalloc_mutex);
	void* result = dlvalloc(size);
	pthread_mutex_unlock(&dlmalloc_mutex);
	return result;
}

void* memalign(size_t alignment, size_t size)
{
	pthread_mutex_lock(&dlmalloc_mutex);
	void* result = dlmemalign(alignment, size);
	pthread_mutex_unlock(&dlmalloc_mutex);
	return result;
}

// Note this function might not be entirely POSIX compatible, but it seems to
// work.
int posix_memalign(void **memptr, size_t alignment, size_t size)
{
	pthread_mutex_lock(&dlmalloc_mutex);
	*memptr = dlmemalign(alignment, size);
	pthread_mutex_unlock(&dlmalloc_mutex);
	return errno;
}

void* realloc(void* ptr, size_t size)
{
	if(IS_OUR_PTR(ptr)) {
		if(size == 0) {
			free(ptr);
			return NULL;
		}

		void* new_memory = malloc(size);
		if(new_memory == NULL) {
			return NULL;
		}

		const int old_size = get_block_from_chunk(ptr)->header.chunk_size;
		const size_t nbytes = size < old_size ? size : old_size;
		memcpy(new_memory, ptr, nbytes);
		free(ptr);
		return new_memory;
	}

	pthread_mutex_lock(&dlmalloc_mutex);
	void* result = dlrealloc(ptr, size);
	pthread_mutex_unlock(&dlmalloc_mutex);
	return result;
}

void free(void* ptr)
{
	if(IS_OUR_PTR(ptr)) {
		if(!pthread_equal(pthread_self(), main_thread)) {
			//this will queue up the free to be performed later in the
			//main thread when it wants more memory.
			free_memory_from_other_thread(ptr);
			return;
		}

		free_memory(ptr);
		return;
	}
	pthread_mutex_lock(&dlmalloc_mutex);
	dlfree(ptr);
	pthread_mutex_unlock(&dlmalloc_mutex);
}

#ifdef TEST_POOLED_ALLOC
int main()
{
	init_custom_malloc();

	void** items = NULL;
	int nitems = 0;

	while(nitems < 100000) {
		if(nitems) {
			int clear = rand()%nitems;
			while(--clear >= 0) {
				int len = rand()%1000;
				free(items[clear]);
				items[clear] = malloc(len);
				assert(items[clear]);
				memset(items[clear], 10, len);
			}
		}

		int i = nitems;
		nitems += rand()%100;
		items = realloc(items, sizeof(*items)*nitems);
		while(i != nitems) {
			int len = rand()%1000;
			items[i] = malloc(len);
			assert(items[i]);
			memset(items[i], 10, len);
			++i;
		}
	}

	while(nitems--) {
		free(items[nitems]);
	}

	return 0;
}
#endif
#endif
