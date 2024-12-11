/*
 * chunk_operations.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include <kern/trap/fault_handler.h>
#include <kern/disk/pagefile_manager.h>
#include "kheap.h"
#include "memory_manager.h"


/******************************/
/*[1] RAM CHUNKS MANIPULATION */
/******************************/

//===============================
// 1) CUT-PASTE PAGES IN RAM:
//===============================
//This function should cut-paste the given number of pages from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int cut_paste_pages(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 num_of_pages)
{
	uint32 source_sva=ROUNDDOWN(source_va,PAGE_SIZE);
	uint32 dest_sva=ROUNDDOWN(dest_va,PAGE_SIZE);
	struct FrameInfo * frame_info;
	struct FrameInfo * frame_info1;
	uint32*ptr_page_table=NULL;
	uint32*ptr_page_table1=NULL;
	//set destnation to be ready
	for(int i=0;i<num_of_pages;i++){

		frame_info=get_frame_info(page_directory,dest_sva,&ptr_page_table);
		if(frame_info!=NULL){
			//return -1 break fn
			return -1;
		}

		dest_sva=dest_sva+PAGE_SIZE;
	}
	source_sva=ROUNDDOWN(source_va,PAGE_SIZE);
	dest_sva=ROUNDDOWN(dest_va,PAGE_SIZE);
	//map pages to dest
	for(int i=0;i<num_of_pages;i++){
		ptr_page_table = NULL;
		if(get_page_table(page_directory, dest_sva, &ptr_page_table) == TABLE_NOT_EXIST){
			create_page_table(page_directory, dest_sva);
		}
		ptr_page_table1 = NULL;
		frame_info1=get_frame_info(page_directory,source_sva,&ptr_page_table1);
		map_frame(page_directory,frame_info1,dest_sva,pt_get_page_permissions(page_directory,source_sva));
		source_sva+=PAGE_SIZE;
		dest_sva+=PAGE_SIZE;
	}
	source_sva=ROUNDDOWN(source_va,PAGE_SIZE);
	//unmap
	for(uint32 i=0;i<num_of_pages;i++){
		unmap_frame(page_directory,source_sva);
		source_sva+=PAGE_SIZE;
	}
	return 0;

	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] cut_paste_pages
	// Write your code here, remove the panic and write your code
	//panic("cut_paste_pages() is not implemented yet...!!");
}

//===============================
// 2) COPY-PASTE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int copy_paste_chunk(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 size)
{
	uint32 end=size+dest_va;
	uint32 src_sva=source_va;
	uint32 dst_sva=dest_va;
	uint32*ptr_page_table=NULL;
	struct FrameInfo *frame;

	for(uint32 va=dst_sva;va<end;va+=PAGE_SIZE){
		frame=get_frame_info(page_directory,va,&ptr_page_table);
		uint32 src_perm=pt_get_page_permissions(page_directory,src_sva);
		uint32 dest_per=PERM_USER&src_perm;
		if(frame!=NULL){
			uint32 perms=pt_get_page_permissions(page_directory,va);
			uint32 prem =(perms &= (PERM_WRITEABLE));
			if( prem == 0){
				return -1;
			}
			pt_set_page_permissions(page_directory,va,dest_per, 0);
		}
		if(get_page_table(page_directory, va, &ptr_page_table) == TABLE_NOT_EXIST){
					create_page_table(page_directory, va);
		}

		if(frame==NULL){
			//allocate
			allocate_frame(&frame);
			map_frame(page_directory,frame,va,dest_per|PERM_WRITEABLE);
		}

		src_sva+=PAGE_SIZE;
	}

	//COPY BYTE BY BYTE
	src_sva=source_va;
	for (uint32 i = dest_va ; i <end; i++){
		unsigned char*s = (unsigned char*)src_sva ;
		unsigned char*d = (unsigned char*) i;
		*d = *s ;
		src_sva++;
	}
	return 0;

	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] copy_paste_chunk
	// Write your code here, remove the panic and write your code
	//panic("copy_paste_chunk() is not implemented yet...!!");
}

//===============================
// 3) SHARE RANGE IN RAM:
//===============================
//This function should share the given size from dest_va with the source_va
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int share_chunk(uint32* page_directory, uint32 source_va,uint32 dest_va, uint32 size, uint32 perms)
{
		uint32 src_end=size+source_va;
		uint32 dst_end=size+dest_va;
		uint32 src_sva=source_va;
		uint32 dst_sva=dest_va;
		src_end=ROUNDUP(src_end,PAGE_SIZE);
		dst_end=ROUNDUP(dst_end,PAGE_SIZE);
		src_sva=ROUNDDOWN(src_sva,PAGE_SIZE);
		dst_sva=ROUNDDOWN(dst_sva,PAGE_SIZE);
		uint32*ptr_page_table=NULL;
		uint32*ptr_page_table1=NULL;

		struct FrameInfo * frame_info;
		struct FrameInfo * frame_info1;

		for(uint32 va=dst_sva; va <dst_end;va+=PAGE_SIZE ){
			frame_info=get_frame_info(page_directory,va,&ptr_page_table);
			if(frame_info!=NULL){
				return-1;
			}
			if(get_page_table(page_directory, va, &ptr_page_table) == TABLE_NOT_EXIST){
						create_page_table(page_directory, va);}
		}
		for(uint32 va=src_sva;va<src_end;va+=PAGE_SIZE){
			frame_info1=get_frame_info(page_directory,va,&ptr_page_table1);
			map_frame(page_directory,frame_info1,dst_sva,perms);
			dst_sva+=PAGE_SIZE;
		}
		return 0;

		//TODO: [PROJECT MS2] [CHUNK OPERATIONS] share_chunk
	// Write your code here, remove the panic and write your code
	//panic("share_chunk() is not implemented yet...!!");
}

//===============================
// 4) ALLOCATE CHUNK IN RAM:
//===============================
//This function should allocate in RAM the given range [va, va+size)
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int allocate_chunk(uint32* page_directory, uint32 va, uint32 size, uint32 perms)
{
	uint32 end=size+va;
	uint32 sva=va;
	end=ROUNDUP(end,PAGE_SIZE);
	sva=ROUNDDOWN(sva,PAGE_SIZE);
	uint32*ptr_page_table=NULL;
	struct FrameInfo * frame_info;
	//struct FrameInfo * frame_info1;
	//alloc group of pages
	for(uint32 va = sva ; va<end;va+=PAGE_SIZE){
		//if destination page exist
		frame_info=get_frame_info(page_directory,va,&ptr_page_table);
		if(frame_info!=NULL){
			return -1;
		}
		if(get_page_table(page_directory,va,&ptr_page_table)){
			create_page_table(page_directory, va);
		}
		//allocate
		allocate_frame(&frame_info);
		map_frame(page_directory,frame_info,va,perms);
		//store address in each frame
		frame_info->va=va;
	}

	return 0;
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] allocate_chunk
	// Write your code here, remove the panic and write your code
	//panic("allocate_chunk() is not implemented yet...!!");
}

/*BONUS*/
//=====================================
// 5) CALCULATE ALLOCATED SPACE IN RAM:
//=====================================
void calculate_allocated_space(uint32* page_directory, uint32 sva, uint32 eva, uint32 *num_tables, uint32 *num_pages)
{
	uint32 sva1=ROUNDDOWN(sva,PAGE_SIZE);
	uint32 eva1=ROUNDDOWN(eva,PAGE_SIZE);
	uint32*ptr_page_table=NULL;
	uint32*ptr_page_table2=NULL;
	uint32*ptr_page_table3=NULL;
	struct FrameInfo * frame_info;
	uint32 num_of_pagetables=0;
	uint32 num_of_pages=0;


	if(get_page_table(page_directory,sva1, &ptr_page_table) == TABLE_IN_MEMORY)
	{
		num_of_pagetables++;
		frame_info=get_frame_info(page_directory,sva1,&ptr_page_table);
			if(frame_info!=NULL)
		{

				num_of_pages++;

		}


	}


	for(int i=sva1+PAGE_SIZE;i<=eva1;i+=PAGE_SIZE)
	{
		if(get_page_table(page_directory,i, &ptr_page_table) ==  TABLE_IN_MEMORY)
		{
			get_page_table(page_directory,i-PAGE_SIZE, &ptr_page_table);
			ptr_page_table2=ptr_page_table;
			get_page_table(page_directory,i, &ptr_page_table);
			ptr_page_table3=ptr_page_table;
			if(ptr_page_table3!=ptr_page_table2)
			{
			 num_of_pagetables++;
			}
			frame_info=get_frame_info(page_directory,i,&ptr_page_table);
			if(frame_info!=NULL)
			{
				num_of_pages++;
			}
			//uint32 table_entry = ptr_page_table [PTX(i-PAGE_SIZE)];
			//uint32 table_entry1 = ptr_page_table [PTX(i)];


		}
			//uint32 table_entry1 = ptr_page_table [PTX(i)];


	}


	*num_tables=num_of_pagetables;
	*num_pages=num_of_pages;
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_allocated_space
	// Write your code here, remove the panic and write your code
	//panic("calculate_allocated_space() is not implemented yet...!!");
}

/*BONUS*/
//=====================================
// 6) CALCULATE REQUIRED FRAMES IN RAM:
//=====================================
// calculate_required_frames:
// calculates the new allocation size required for given address+size,
// we are not interested in knowing if pages or tables actually exist in memory or the page file,
// we are interested in knowing whether they are allocated or not.
uint32 calculate_required_frames(uint32* page_directory, uint32 sva, uint32 size)
{

	uint32 sva1=ROUNDDOWN(sva,PAGE_SIZE);
	uint32 eva1=ROUNDUP(sva+size,PAGE_SIZE);

	uint32*ptr_page_table=NULL;
	uint32*ptr_page_table2=NULL;
	uint32*ptr_page_table3=NULL;
	struct FrameInfo * frame_info;
	uint32 num_of_pagetables=0;
		uint32 num_of_pages=0;
	if(get_page_table(page_directory,sva1, &ptr_page_table) == TABLE_IN_MEMORY)
	   {
		//num_of_pagetables++;
		frame_info=get_frame_info(page_directory,sva1,&ptr_page_table);
				if(frame_info==NULL)
					{
					num_of_pages++;
					}}
	else if(get_page_table(page_directory,sva1, &ptr_page_table) == TABLE_NOT_EXIST)
		{
				//create_page_table(page_directory, sva1);
				num_of_pagetables++;
				frame_info=get_frame_info(page_directory,sva1,&ptr_page_table);
				if(frame_info==NULL)
					{

					num_of_pages++;

					}


												}
	for(int i=sva1+PAGE_SIZE;i<eva1;i+=PAGE_SIZE)
		{

		if(get_page_table(page_directory,i, &ptr_page_table) ==  TABLE_IN_MEMORY)
							{
			/*get_page_table(page_directory,i-PAGE_SIZE, &ptr_page_table);
														ptr_page_table2=ptr_page_table;
														get_page_table(page_directory,i, &ptr_page_table);
														ptr_page_table3=ptr_page_table;
														if(ptr_page_table3!=ptr_page_table2)
														{
																  num_of_pagetables++;
														}*/
								frame_info=get_frame_info(page_directory,i,&ptr_page_table);
								if(frame_info==NULL)
								{
									num_of_pages++;
								}
							}


		else if(get_page_table(page_directory,i, &ptr_page_table) ==  TABLE_NOT_EXIST)
									{
									 //create_page_table(page_directory,i);
			 get_page_table(page_directory,i-PAGE_SIZE, &ptr_page_table);
											ptr_page_table2=ptr_page_table;
											get_page_table(page_directory,i, &ptr_page_table);
											ptr_page_table3=ptr_page_table;
											if(ptr_page_table3!=ptr_page_table2)
											{
													  num_of_pagetables++;
											}
									 frame_info=get_frame_info(page_directory,i,&ptr_page_table);
									 if(frame_info==NULL)
									{
										 num_of_pages++;


									}}

							}


	return num_of_pages+num_of_pagetables;







	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_required_frames
	// Write your code here, remove the panic and write your code
	//panic("calculate_required_frames() is not implemented yet...!!");


	//=================================================================================//
	//===========================END RAM CHUNKS MANIPULATION ==========================//
	//=================================================================================//
}

//=================================================================================//
//===========================END RAM CHUNKS MANIPULATION ==========================//
//=================================================================================//

/*******************************/
/*[2] USER CHUNKS MANIPULATION */
/*******************************/

//======================================================
/// functions used for USER HEAP (malloc, free, ...)
//======================================================

//=====================================
// 1) ALLOCATE USER MEMORY:
//=====================================
void allocate_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	// Write your code here, remove the panic and write your code
	panic("allocate_user_mem() is not implemented yet...!!");
}

//=====================================
// 2) FREE USER MEMORY:
//=====================================
void free_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	//address first page only loop for every page
	uint32 end=(virtual_address+size);
	uint32 sva=(virtual_address);
	end=ROUNDUP(end,PAGE_SIZE);
	sva=ROUNDDOWN(sva,PAGE_SIZE);
	//free from page file
	for(uint32 va= sva; va<end;va+=PAGE_SIZE){
		pf_remove_env_page(e,va);
		env_page_ws_invalidate(e,va);
		unmap_frame(e->env_page_directory,va);
	}
	//free from WS

	//free  empty page table
	uint32*ptr_pgtable=	NULL;
	for(uint32 va= sva; va<end;va+=PAGE_SIZE){
		if(get_page_table(e->env_page_directory,va,&ptr_pgtable)==TABLE_IN_MEMORY){
			bool x=0;
			for(int i =0 ; i<1024; i++){
				if(ptr_pgtable[i]!=0){
					x=1;
				}
			}
			if(!x){
				pd_clear_page_dir_entry(e->env_page_directory,va);
				kfree(ptr_pgtable);
			}
		}
	}



	//TODO: [PROJECT MS3] [USER HEAP - KERNEL SIDE] free_user_mem
	// Write your code here, remove the panic and write your code
	//panic("free_user_mem() is not implemented yet...!!");

	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
	//get all pages in given range

}

//=====================================
// 2) FREE USER MEMORY (BUFFERING):
//=====================================
void __free_user_mem_with_buffering(struct Env* e, uint32 virtual_address, uint32 size)
{
	// your code is here, remove the panic and write your code
	panic("__free_user_mem_with_buffering() is not implemented yet...!!");

	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Free any BUFFERED pages in the given range
	//4. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
}

//=====================================
// 3) MOVE USER MEMORY:
//=====================================
void move_user_mem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	//TODO: [PROJECT MS3 - BONUS] [USER HEAP - KERNEL SIDE] move_user_mem
	//your code is here, remove the panic and write your code
	panic("move_user_mem() is not implemented yet...!!");

	// This function should move all pages from "src_virtual_address" to "dst_virtual_address"
	// with the given size
	// After finished, the src_virtual_address must no longer be accessed/exist in either page file
	// or main memory

	/**/
}

//=================================================================================//
//========================== END USER CHUNKS MANIPULATION =========================//
//=================================================================================//

