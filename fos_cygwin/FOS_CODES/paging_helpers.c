/*
 * paging_helpers.c
 *
 *  Created on: Sep 30, 2022
 *      Author: HP
 */
#include "memory_manager.h"

/*[2.1] PAGE TABLE ENTRIES MANIPULATION */
inline void pt_set_page_permissions(uint32* page_directory, uint32 virtual_address, uint32 permissions_to_set, uint32 permissions_to_clear)
{
		uint32 *ptr_page_table=NULL;
		int ret=get_page_table(page_directory,virtual_address,&ptr_page_table);
		if(ret==TABLE_IN_MEMORY){
			ptr_page_table[PTX(virtual_address)]=ptr_page_table[PTX(virtual_address)]|permissions_to_set;
			ptr_page_table[PTX(virtual_address)]=ptr_page_table[PTX(virtual_address)]&(~permissions_to_clear);
		}
		else{
			panic("Invalid va");
		}
		tlb_invalidate((void *)NULL, (void *)virtual_address);




	//TODO: [PROJECT MS2] [PAGING HELPERS] pt_set_page_permissions
	// Write your code here, remove the panic and write your code
	//panic("pt_set_page_permissions() is not implemented yet...!!");
}

inline int pt_get_page_permissions(uint32* page_directory, uint32 virtual_address )
{

	uint32 *ptr_page_table = NULL;
			int ret = get_page_table(page_directory, virtual_address, &ptr_page_table);
			if (ret == TABLE_IN_MEMORY)
			{
				uint32 check=0x0000fff;
				return ptr_page_table[PTX(virtual_address)]&check;
			}else{
				return -1;
			}
	//TODO: [PROJECT MS2] [PAGING HELPERS] pt_get_page_permissions
	// Write your code here, remove the panic and write your code
	//panic("pt_get_page_permissions() is not implemented yet...!!");
}

inline void pt_clear_page_table_entry(uint32* page_directory, uint32 virtual_address)
{
	uint32 *ptr_page_table = NULL;
		int ret = get_page_table(page_directory, virtual_address, &ptr_page_table);
		if (ret == TABLE_IN_MEMORY)
		{
			ptr_page_table[PTX(virtual_address)]=0;
		}else{
			panic("Invalid va");
		}
		tlb_invalidate((void *)NULL, (void *)virtual_address);
	//TODO: [PROJECT MS2] [PAGING HELPERS] pt_clear_page_table_entry
	// Write your code here, remove the panic and write your code
	//panic("pt_clear_page_table_entry() is not implemented yet...!!");
}

/***********************************************************************************************/

/*[2.2] ADDRESS CONVERTION*/
inline int virtual_to_physical(uint32* page_directory, uint32 virtual_address)
{
	uint32 *ptr=NULL;
	int ret=get_page_table(page_directory,virtual_address,&ptr);
	if(ret==TABLE_IN_MEMORY){
		int page=ptr[PTX(virtual_address)]>>12;
		int physical=page*PAGE_SIZE;
		return physical;
	}
	else{
		return -1;
	}
	//TODO: [PROJECT MS2] [PAGING HELPERS] virtual_to_physical
	// Write your code here, remove the panic and write your code
	//panic("virtual_to_physical() is not implemented yet...!!");
}

/***********************************************************************************************/

/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/

///============================================================================================
/// Dealing with page directory entry flags

inline uint32 pd_is_table_used(uint32* page_directory, uint32 virtual_address)
{
	return ( (page_directory[PDX(virtual_address)] & PERM_USED) == PERM_USED ? 1 : 0);
}

inline void pd_set_table_unused(uint32* page_directory, uint32 virtual_address)
{
	page_directory[PDX(virtual_address)] &= (~PERM_USED);
	tlb_invalidate((void *)NULL, (void *)virtual_address);
}

inline void pd_clear_page_dir_entry(uint32* page_directory, uint32 virtual_address)
{
	page_directory[PDX(virtual_address)] = 0 ;
	tlbflush();
}
