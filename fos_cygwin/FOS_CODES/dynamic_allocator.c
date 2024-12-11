/*
 * dyn_block_management.c
 *
 *  Created on: Sep 21, 2022
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//===========================
// PRINT MEM BLOCK LISTS:
//===========================

void print_mem_block_lists()
{
	cprintf("\n=========================================\n");
	struct MemBlock* blk ;
	struct MemBlock* lastBlk = NULL ;
	cprintf("\nFreeMemBlocksList:\n");
	uint8 sorted = 1 ;
	LIST_FOREACH(blk, &FreeMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nFreeMemBlocksList is NOT SORTED!!\n") ;

	lastBlk = NULL ;
	cprintf("\nAllocMemBlocksList:\n");
	sorted = 1 ;
	LIST_FOREACH(blk, &AllocMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nAllocMemBlocksList is NOT SORTED!!\n") ;
	cprintf("\n=========================================\n");

}

//********************************************************************************//
//********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//===============================
// [1] INITIALIZE AVAILABLE LIST:
//===============================
void initialize_MemBlocksList(uint32 numOfBlocks)
{
	LIST_INIT(&AvailableMemBlocksList);
		for(int i=0;i<numOfBlocks;i++){
			LIST_INSERT_HEAD(&AvailableMemBlocksList, &(MemBlockNodes[i]));
		}
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] initialize_MemBlocksList
	// Write your code here, remove the panic and write your code
	//panic("initialize_MemBlocksList() is not implemented yet...!!");

}

//===============================
// [2] FIND BLOCK:
//===============================
struct MemBlock *find_block(struct MemBlock_List *blockList, uint32 va)
{

	struct MemBlock* blk ;
	blk=blockList->lh_first;
	while(blk!=NULL){
		if(blk->sva==va){
			return blk;
		}
		else {
			blk=blk->prev_next_info.le_next;
		}
	}
	return blk	;


	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] find_block
	// Write your code here, remove the panic and write your code
	//panic("find_block() is not implemented yet...!!");

}

//=========================================
// [3] INSERT BLOCK IN ALLOC LIST [SORTED]:
//=========================================
void insert_sorted_allocList(struct MemBlock *blockToInsert)
{
	struct MemBlock *blk;
		blk=AllocMemBlocksList.lh_first;
		if(AllocMemBlocksList.size==0){
			LIST_INSERT_HEAD(&(AllocMemBlocksList), blockToInsert);
		}
		else
		{

			if((AllocMemBlocksList.lh_last->sva)<blockToInsert->sva){
								LIST_INSERT_TAIL(&(AllocMemBlocksList), blockToInsert);
							}
			else if ((AllocMemBlocksList.lh_first->sva)>blockToInsert->sva) {
				LIST_INSERT_HEAD(&(AllocMemBlocksList), blockToInsert);
			}

			else{
			      LIST_FOREACH(blk, &(AllocMemBlocksList))
				{
				if(((blk->sva)<=(blockToInsert->sva))&&((blk->prev_next_info.le_next->sva)>(blockToInsert->sva))){
					LIST_INSERT_AFTER(&(AllocMemBlocksList),blk,blockToInsert);
					break;

				}
				}
			}

		}

	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] insert_sorted_allocList
	// Write your code here, remove the panic and write your code
	//panic("insert_sorted_allocList() is not implemented yet...!!");
}

//=========================================
// [4] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
struct MemBlock *alloc_block_FF(uint32 size)
{

	struct MemBlock* blk = LIST_FIRST(&FreeMemBlocksList);
	while(blk!= NULL){
		if(size == blk->size){
			LIST_REMOVE(&FreeMemBlocksList,blk);
			return blk;

		}
		if(size < blk->size){
			struct MemBlock* newblk	=AvailableMemBlocksList.lh_first;
			LIST_REMOVE(&AvailableMemBlocksList,newblk);
			newblk->sva=blk->sva;
			newblk->size= size;
			blk->size=(blk->size - size);
			blk->sva=(blk->sva+size);
			return newblk;
		}
		blk=blk->prev_next_info.le_next;
	}
	return 	blk;

}

//=========================================
// [5] ALLOCATE BLOCK BY BEST FIT:
//=========================================
struct MemBlock *alloc_block_BF(uint32 size)
{


	struct MemBlock* blk = LIST_FIRST(&FreeMemBlocksList);
	uint32 min = 1024*1024*1024;
	while(blk!=NULL){
		if(size == blk->size){
			LIST_REMOVE(&FreeMemBlocksList,blk);
			return blk;
		}
		if(size < blk->size){
			struct MemBlock* srch;
			LIST_FOREACH(srch, &FreeMemBlocksList){
				if(srch -> size == size){
					LIST_REMOVE(&FreeMemBlocksList, srch);
					return srch;
				}
				if(min> (srch->size -size)){
					min = srch->size -size;
					blk=srch;
				}
			}
			struct MemBlock* newblk	=AvailableMemBlocksList.lh_first;
			LIST_REMOVE(&AvailableMemBlocksList,newblk);
			newblk->sva=blk->sva;
			newblk->size=size;
			blk->size=(blk->size-size);
			blk->sva=(blk->sva+size);
			return newblk;


		}
		blk=blk->prev_next_info.le_next;

	}
	return NULL;
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] alloc_block_BF
	// Write your code here, remove the panic and write your code
	//panic("alloc_block_BF() is not implemented yet...!!");
}


//=========================================
// [7] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
struct MemBlock * glo= 	NULL;
struct MemBlock *alloc_block_NF(uint32 size)
{
	if(glo==NULL){
		struct MemBlock *blk=LIST_FIRST(&FreeMemBlocksList);
		while(blk!= NULL){
				if(size == blk->size){
					glo=blk->prev_next_info.le_next;
					LIST_REMOVE(&FreeMemBlocksList,blk);
					return blk;

				}
				if(size < blk->size){
					struct MemBlock* newblk	=AvailableMemBlocksList.lh_first;
					LIST_REMOVE(&AvailableMemBlocksList,newblk);
					glo=blk;
					newblk->sva=blk->sva;
					newblk->size= size;
					blk->size=(blk->size - size);
					blk->sva=(blk->sva+size);

					return newblk;
				}
				blk=blk->prev_next_info.le_next;
			}
	}
	else{
		struct MemBlock *gblk=glo;
		while(gblk!=NULL){
			if(gblk->size==size){
				glo=gblk->prev_next_info.le_next;

				LIST_REMOVE(&FreeMemBlocksList,gblk);
				return gblk;
			}
			if(gblk->size > size ){
				struct MemBlock* newblk	=AvailableMemBlocksList.lh_first;
				LIST_REMOVE(&AvailableMemBlocksList,newblk);
				glo=gblk;
				newblk->sva=gblk->sva;
				newblk->size= size;
				gblk->size=(gblk->size - size);
				gblk->sva=(gblk->sva+size);
				return newblk;
			}
			gblk=gblk->prev_next_info.le_next;
		}
		struct MemBlock *tblk=LIST_FIRST(&FreeMemBlocksList);
		while(tblk!= NULL){
				if(size == tblk->size){
					glo=tblk->prev_next_info.le_next;
					LIST_REMOVE(&FreeMemBlocksList,tblk);
					return tblk;

				}
				if(size < tblk->size){
					struct MemBlock* newblk	=AvailableMemBlocksList.lh_first;
					LIST_REMOVE(&AvailableMemBlocksList,newblk);
					glo=tblk;
					newblk->sva=tblk->sva;
					newblk->size= size;
					tblk->size=(tblk->size - size);
					tblk->sva=(tblk->sva+size);

					return newblk;
				}
				tblk=tblk->prev_next_info.le_next;
			}
	}
	return NULL;


	//TODO: [PROJECT MS1 - BONUS] [DYNAMIC ALLOCATOR] alloc_block_NF
	// Write your code here, remove the panic and write your code
	//panic("alloc_block_NF() is not implemented yet...!!");

}

//===================================================
// [8] INSERT BLOCK (SORTED WITH MERGE) IN FREE LIST:
//===================================================
void insert_sorted_with_merge_freeList(struct MemBlock *blockToInsert)
{

	struct MemBlock *blk;
	blk=FreeMemBlocksList.lh_first;
	if(FreeMemBlocksList.size==0){
		LIST_INSERT_HEAD(&(FreeMemBlocksList), blockToInsert);

	}
	else{
		if(FreeMemBlocksList.lh_last->sva<blockToInsert->sva){
			//merge prev.
			if((FreeMemBlocksList.lh_last->size + FreeMemBlocksList.lh_last->sva) == blockToInsert->sva ){
				FreeMemBlocksList.lh_last->size=FreeMemBlocksList.lh_last->size+blockToInsert->size;
				blockToInsert->sva=0;
				blockToInsert->size=0;
				LIST_INSERT_HEAD(&AvailableMemBlocksList,blockToInsert);
			}
			else{
				LIST_INSERT_TAIL(&(FreeMemBlocksList), blockToInsert);

			}
		}
		//merge next
		else if ((FreeMemBlocksList.lh_first->sva)>blockToInsert->sva) {
			if(blockToInsert->size + blockToInsert->sva == FreeMemBlocksList.lh_first->sva){
					FreeMemBlocksList.lh_first->sva=blockToInsert->sva;
					FreeMemBlocksList.lh_first->size=FreeMemBlocksList.lh_first->size+blockToInsert->size;
					blockToInsert->sva=0;
					blockToInsert->size=0;
					LIST_INSERT_HEAD(&AvailableMemBlocksList,blockToInsert);

				}
				else{
					LIST_INSERT_HEAD(&(FreeMemBlocksList), blockToInsert);

				}
		}

		else{
			LIST_FOREACH(blk, &(FreeMemBlocksList)){
				if(((blk->sva)<=(blockToInsert->sva))&&((blk->prev_next_info.le_next->sva)>(blockToInsert->sva))){
					//merge  in between
					if((blk->size+blk->sva == blockToInsert->sva) && ((blk->prev_next_info.le_next->sva)==(blockToInsert->size+blockToInsert->sva)) ){
						struct MemBlock* next = LIST_NEXT(blk);
						blockToInsert->size= blk->size + blockToInsert->size + next->size;
						blockToInsert->sva = blk->sva;
						LIST_INSERT_AFTER(&FreeMemBlocksList, blk, blockToInsert);
						blk->size=0;
						blk->sva=0;
						LIST_REMOVE(&FreeMemBlocksList,blk);
						LIST_INSERT_HEAD(&AvailableMemBlocksList,blk);
						next->size=0;
						next->sva=0;
						LIST_REMOVE(&FreeMemBlocksList,	next);
						LIST_INSERT_HEAD(&AvailableMemBlocksList,next);
					}
					//merge prev
					else if(blk->size+blk->sva == blockToInsert->sva  ){

						blk->size=blk->size+blockToInsert->size;
						blockToInsert->size=0;
						blockToInsert->sva=0;
						LIST_INSERT_HEAD(&AvailableMemBlocksList,blockToInsert);
					}
					//merge next
					else if((blk->prev_next_info.le_next->sva)==(blockToInsert->size+blockToInsert->sva)){
						blk->prev_next_info.le_next->sva=blockToInsert->sva;
						blk->prev_next_info.le_next->size=blk->prev_next_info.le_next->size+blockToInsert->size;
						blockToInsert->size=0;
						blockToInsert->sva=0;
						LIST_INSERT_HEAD(&AvailableMemBlocksList,blockToInsert);
					}

					else{LIST_INSERT_AFTER(&(FreeMemBlocksList),blk,blockToInsert);}
		    		break;
			}

		}
	}
}
}
