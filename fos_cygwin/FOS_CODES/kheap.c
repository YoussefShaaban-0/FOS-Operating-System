#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

//==================================================================//
//==================================================================//
//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)//
//==================================================================//
//==================================================================//

void initialize_dyn_block_system()
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] initialize_dyn_block_system
	// your code is here, remove the panic and write your code
	//kpanic_into_prompt("initialize_dyn_block_system() is not implemented yet...!!");

	//[1] Initialize two lists (AllocMemBlocksList & FreeMemBlocksList) [Hint: use LIST_INIT()]
	LIST_INIT(&AllocMemBlocksList);
	LIST_INIT(&FreeMemBlocksList);
#if STATIC_MEMBLOCK_ALLOC
	//DO NOTHING
#else
	MAX_MEM_BLOCK_CNT=NUM_OF_KHEAP_PAGES;
	//uint32 size=sizeof(struct MemBlock)*MAX_MEM_BLOCK_CNT;
	//size=ROUNDUP(size,PAGE_SIZE);
	allocate_chunk(ptr_page_directory,KERNEL_HEAP_START,ROUNDUP(sizeof(struct MemBlock)*MAX_MEM_BLOCK_CNT,PAGE_SIZE),PERM_WRITEABLE);
	MemBlockNodes = (void*) KERNEL_HEAP_START;

	#endif

	initialize_MemBlocksList(MAX_MEM_BLOCK_CNT);
	struct MemBlock* newblk	=AvailableMemBlocksList.lh_first;
	LIST_REMOVE(&AvailableMemBlocksList,newblk);
	newblk->size=(KERNEL_HEAP_MAX-KERNEL_HEAP_START)-ROUNDUP(sizeof(struct MemBlock)*MAX_MEM_BLOCK_CNT,PAGE_SIZE);
	newblk->sva=(KERNEL_HEAP_START+ROUNDUP(sizeof(struct MemBlock)*MAX_MEM_BLOCK_CNT,PAGE_SIZE));
	LIST_INSERT_HEAD(&(FreeMemBlocksList), newblk);
	//[3] Initialize AvailableMemBlocksList by filling it with the MemBlockNodes
	//[4] Insert a new MemBlock with the remaining heap size into the FreeMemBlocksList



}

void* kmalloc(unsigned int size)
{

	size =ROUNDUP(size,PAGE_SIZE);
	struct MemBlock *ret;
	int chunk;
	if(isKHeapPlacementStrategyFIRSTFIT()){
		ret=alloc_block_FF(size);
	}
	if(isKHeapPlacementStrategyBESTFIT()){
		ret=alloc_block_BF(size);
	}
	if(isKHeapPlacementStrategyNEXTFIT()){
		ret=alloc_block_NF(size);
	}
	if(ret==NULL){
		return NULL;
	}
	chunk=allocate_chunk(ptr_page_directory,ret->sva,size,PERM_WRITEABLE);
	if(chunk==E_NO_MEM){
		return NULL;
		}
	insert_sorted_allocList(ret);
	return (void*)ret->sva;




	//TODO: [PROJECT MS2] [KERNEL HEAP] kmalloc
	// your code is here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy

	//change this "return" according to your answer

}

void kfree(void* virtual_address)
{
	struct MemBlock *ret;
	ret=find_block(&AllocMemBlocksList,(uint32)virtual_address);
	if(ret!=NULL){
			uint32 end=( ret->sva + ret->size);
			end=ROUNDUP(end,PAGE_SIZE);
			uint32 sva=ROUNDDOWN(ret->sva,PAGE_SIZE);
			for(uint32 va= sva;va<end;va+=PAGE_SIZE){
				unmap_frame(ptr_page_directory,va);
			}
			LIST_REMOVE(&AllocMemBlocksList,ret);
			insert_sorted_with_merge_freeList(ret);
	}

	//TODO: [PROJECT MS2] [KERNEL HEAP] kfree
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	struct FrameInfo *ptr_frame_info = to_frame_info(physical_address) ;
	return ptr_frame_info->va;
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details
	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	return virtual_to_physical(ptr_page_directory,virtual_address);
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details
}


void kfreeall()
{
	panic("Not implemented!");

}

void kshrink(uint32 newSize)
{
	panic("Not implemented!");
}

void kexpand(uint32 newSize)
{
	panic("Not implemented!");
}




//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT MS2 - BONUS] [KERNEL HEAP] krealloc
	// Write your code here, remove the panic and write your code
	panic("krealloc() is not implemented yet...!!");

}
