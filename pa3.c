/**********************************************************************
 * Copyright (c) 2019
 *  Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "types.h"
#include "list_head.h"
#include "vm.h"

/**
 * Ready queue of the system
 */
extern struct list_head processes;

/**
 * The current process
 */
extern struct process *current;

/**
 * alloc_page()
 *
 * DESCRIPTION
 *   Allocate a page from the system. This function is implemented in vm.c
 *   and use to get a page frame from the system.
 *
 * RETURN
 *   PFN of the newly allocated page frame.
 */
extern unsigned int alloc_page(void);



/**
 * TODO translate()
 *
 * DESCRIPTION
 *   Translate @vpn of the @current to @pfn. To this end, walk through the
 *   page table of @current and find the corresponding PTE of @vpn.
 *   If such an entry exists and OK to access the pfn in the PTE, fill @pfn
 *   with the pfn of the PTE and return true.
 *   Otherwise, return false.
 *   Note that you should not modify any part of the page table in this function.
 *
 * RETURN
 *   @true on successful translation
 *   @false on unable to translate. This includes the case when @rw is for write
 *   and the @writable of the pte is false.
 */
bool translate(enum memory_access_type rw, unsigned int vpn, unsigned int *pfn)
{
	struct pte_directory *pte_d = current->pagetable.outer_ptes[vpn / NR_PTES_PER_PAGE];
	if (!pte_d || pte_d->ptes[vpn%NR_PTES_PER_PAGE].valid == false) {
		return false;
	}
	else {
		*pfn = pte_d->ptes[vpn % NR_PTES_PER_PAGE].pfn;
		return true;
	}

	/*** DO NOT MODIFY THE PAGE TABLE IN THIS FUNCTION ***/
}


/**
 * TODO handle_page_fault()
 *
 * DESCRIPTION
 *   Handle the page fault for accessing @vpn for @rw. This function is called
 *   by the framework when the translate() for @vpn fails. This implies;
 *   1. Corresponding pte_directory is not exist
 *   2. pte is not valid
 *   3. pte is not writable but @rw is for write
 *   You can assume that all pages are writable; this means, when a page fault
 *   happens with valid PTE without writable permission, it was set for the
 *   copy-on-write.
 *
 * RETURN
 *   @true on successful fault handling
 *   @false otherwise
 */
bool handle_page_fault(enum memory_access_type rw, unsigned int vpn)
{
	struct pte_directory *p = current->pagetable.outer_ptes[vpn/NR_PTES_PER_PAGE];

	if (!p) {
		//printf("is !p\n");
		current->pagetable.outer_ptes[vpn / NR_PTES_PER_PAGE] = (struct pte_directory*)malloc(sizeof(struct pte_directory));

		current->pagetable.outer_ptes[vpn / NR_PTES_PER_PAGE]->ptes[vpn % NR_PTES_PER_PAGE].pfn = alloc_page();
		current->pagetable.outer_ptes[vpn / NR_PTES_PER_PAGE]->ptes[vpn % NR_PTES_PER_PAGE].valid = true;
		current->pagetable.outer_ptes[vpn / NR_PTES_PER_PAGE]->ptes[vpn % NR_PTES_PER_PAGE].writable = true;
		return true;
	}
	else if (p->ptes[vpn%NR_PTES_PER_PAGE].valid == false) {		// p는 있지만, %16에 해당하는 pte가 없을때.
		p->ptes[vpn % NR_PTES_PER_PAGE].pfn = alloc_page();
		current->pagetable.outer_ptes[vpn / NR_PTES_PER_PAGE]->ptes[vpn % NR_PTES_PER_PAGE].valid = true;
		current->pagetable.outer_ptes[vpn / NR_PTES_PER_PAGE]->ptes[vpn % NR_PTES_PER_PAGE].writable = true;
		return true;
	}

	/*
	if (p->ptes[vpn % 16].valid == false) {
		printf("a\n");
		p->ptes[vpn / 16].valid = true;
		p->ptes[vpn/16].writable = true;
	}

	if (p->ptes[vpn / 16].writable == false && rw == true) {	// writable == false / 읽으려고 함
		
	}

	printf("\ncalled hpf, vpn : %d, rw : %d\n",vpn,rw);
	*/
	return true;
}


/**
 * TODO switch_process()
 *
 * DESCRIPTION
 *   If there is a process with @pid in @processes, switch to the process.
 *   The @current process at the moment should be put to the **TAIL** of the
 *   @processes list, and @current should be replaced to the requested process.
 *   Make sure that the next process is unlinked from the @processes.
 *   If there is no process with @pid in the @processes list, fork a process
 *   from the @current. This implies the forked child process should have
 *   the identical page table entry 'values' to its parent's (i.e., @current)
 *   page table. Also, should update the writable bit properly to implement
 *   the copy-on-write feature.
 */
void switch_process(unsigned int pid)
{
}

