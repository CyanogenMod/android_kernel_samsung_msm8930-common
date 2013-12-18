/*
 *      Contains implementations of test cases for L1PGT
 *
 *      Author:    Guruprasad Ganesh   <g.ganesh@sta.samsung.com>
 *
 *      Changes:
 *              GG              :      Uploaded the initial Version
 *              AB              :      Adding Pagewalk functionalities
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/tima/tts_common.h>
#include "ktest_common.h"
#include <asm/proc-fns.h>
#include <asm/highmem.h>
#include <linux/mm_types.h>

#define TAG "TIMA_RKP "

#define LOWER_12 0xFFF

#define L1_NENTRIES (1 << 12)
#define L1_ENTRY_PAGE_TABLE_PHYSICAL_ADDRESS_MASK ~0x3FF
#define L1_ENTRY_PAGE_TABLE_FLAGS_MASK 0x3FF

#define L2_NENTRIES (1 << 8)
#define L2_ENTRY_SMALL_PAGE_PHYSICAL_ADDRESS_MASK ~0xFFF
#define L2_ENTRY_SMALL_PAGE_FLAGS_MASK 0xFFF

#define PGD_SHIFT   20
#define PTE_SHIFT   12
#define GET_PGD_INDEX(pgd) ((pgd >> PGD_SHIFT) << 2)
#define GET_PTE_ADDR(pgd_val,index) ( (pgd_val & (~0x3ff)) | (index << 2) )

#define PADDR_ARRAY_SIZE 0x100000
#define GET_PTE_PADDR_INDEX(paddr) (paddr >> PTE_SHIFT)
#define GET_PTE_FLAGS(paddr) (paddr&0xfff)

static unsigned int paddr_array[PADDR_ARRAY_SIZE];
static int l2_page_walk(unsigned long L2_virtaddr);
int l1_page_walk(unsigned long pgd);

extern char tts_err_log[TTS_MAX_RES_LEN];

static int __is_l1_page_table_entry(unsigned long x)
{
	return (((x & 0x3) == 0x1)? 1 : 0);
}

static int __is_l2_small_page_entry(unsigned long x)
{
	return (((x & 0x2) == 0x2)? 1 : 0);
}

static int __is_pgd_invalid(unsigned long x)
{
        return(x & 0x3fff);
}

static int __is_page_readonly(unsigned long x)
{
        return (((x & 0x230) == 0x210)?1:0);
}

static inline void tts_push_l2_flags(unsigned long paddr) 
{
    paddr_array[GET_PTE_PADDR_INDEX(paddr)] = GET_PTE_FLAGS(paddr);
}
static inline void __init_paddr_array(void)
{
        memset(paddr_array,0,PADDR_ARRAY_SIZE);
}



/*
 * check_pte_readonly- Check whether particular page is readonly
 */

static int check_pte_readonly(unsigned long pgd)
{
        unsigned long init_pgd,pgd_val,init_pte,pte_val;

        init_pgd = (unsigned long)KPGD_INIT_MM;
        
        /* Geting address of First Level Descriptor */
        init_pgd += GET_PGD_INDEX(pgd);
    
        /*Get value of First Level Descriptor*/
        pgd_val = (unsigned long)*(unsigned long *)init_pgd;

        /* Check whether the descriptor pointing towards a page table*/
        if(!__is_l1_page_table_entry(pgd_val))
        {
                TTS_ERR_LOG("Not a page");
                return TTS_TC_RET_FAILURE;
        }
        /*Get address of first page table entry */
        init_pte = (unsigned long)__va(GET_PTE_ADDR(pgd_val,0));
       /* init_pte += ((pgd >> 12) & 0xff) << 2;*/

        /*Get Value of initial Page Table Entry */
        pte_val = *(unsigned long *)init_pte;
        TTS_DBG_LOG("\npassed pgd = %lx init_pgd = %lx init_pte =%lx pteval =%lx ",pgd,init_pgd,init_pte,pte_val);

        if (__is_page_readonly(pte_val))
        {
                TTS_DBG_LOG("###READ-ONLY###");
                return TTS_TC_RET_SUCCESS;
        }

        TTS_ERR_LOG("###PAGE IS READ-WRITE###");
        return TTS_TC_RET_FAILURE;
}


/*
 * l1_page_walk- walk through each entries of L1 table pointed by pgd`
 */
int l1_page_walk(unsigned long pgd)
{

        unsigned int i = 0;
        unsigned int L2_tables_count = 0;
        unsigned int L2_tables_entries_count = 0;

        unsigned long L1_entry;
        unsigned long L1_flags;
        unsigned long L2_tbl_physaddr;
        unsigned long L2_tbl_virtaddr;

        TTS_DBG_LOG (TAG "\nPassed pgd = %lx\n", pgd);

        for (i = 0; i < L1_NENTRIES; i++) {
                L1_entry = *(unsigned long *)(pgd + i*4);
                L1_flags = L1_entry & L1_ENTRY_PAGE_TABLE_FLAGS_MASK;
                if (__is_l1_page_table_entry(L1_flags)) {
                        L2_tables_count++;
                        L2_tbl_physaddr = L1_entry & L1_ENTRY_PAGE_TABLE_PHYSICAL_ADDRESS_MASK;
                        L2_tbl_virtaddr = (unsigned long)__va(L2_tbl_physaddr);

                        TTS_DBG_LOG(TAG  "%4u: L2_TABLE_PHYSADDR = 0x%08lx, FLAGS = 0x%03lx, L2_TABLE_VIRTADDR = 0x%08lx\n",
                                        i, L2_tbl_physaddr, L1_flags, L2_tbl_virtaddr);

                        L2_tables_entries_count += l2_page_walk(L2_tbl_virtaddr);
                }
        }

        TTS_DBG_LOG (TAG "\n");
        TTS_DBG_LOG (TAG "L2_tables_count =         %u\n", L2_tables_count);
        TTS_DBG_LOG (TAG "L2_tables_entries_count = %u\n", L2_tables_entries_count);

        return TTS_TC_RET_SUCCESS;
}

/*
 * l2_page_walk- walk through each entries of L2 table pointed by pgd
 */
int l2_page_walk(unsigned long L2_virtaddr) 
{
	int i;

	unsigned int L2_entries_count = 0;

	unsigned long L2_entry;
	unsigned long L2_flags;

	unsigned long physaddr;
	unsigned long virtaddr;
			
	for (i = 0; i < L2_NENTRIES; i++) {
		L2_entry = *(unsigned long *)(L2_virtaddr + i*4);
		L2_flags = L2_entry & L2_ENTRY_SMALL_PAGE_FLAGS_MASK;
		if (__is_l2_small_page_entry(L2_flags)) {
			L2_entries_count++;
            physaddr = L2_entry & L2_ENTRY_SMALL_PAGE_PHYSICAL_ADDRESS_MASK;
            paddr_array[(physaddr >> PTE_SHIFT)] = L2_flags;

            virtaddr = (unsigned long)__va(physaddr);

			TTS_DBG_LOG(TAG  "      %3u: PHYSADDR = 0x%08lx, FLAGS = 0x%03lx, VIRTADDR = 0x%08lx\n",
				   i, physaddr, L2_flags, virtaddr);
		}
	}

	TTS_DBG_LOG (TAG "      L2_entries_count = %u\n", L2_entries_count);

	return L2_entries_count;
				
}

int l1_analyze_addr(unsigned long pgd)
{

        unsigned int i = 0;
        unsigned int L2_tables_count = 0;
        unsigned int L2_tables_entries_count = 0;
        unsigned int hitcount=0,nonhitcount=0,hitdouble = 0;

        unsigned long L1_entry;
        unsigned long L1_flags;
        unsigned long L2_tbl_physaddr;

        TTS_DBG_LOG (TAG "\nPassed pgd = %lx\n", pgd);

        for (i = 0; i < L1_NENTRIES; i++) {
                L1_entry = *(unsigned long *)(pgd + i*4);
                L1_flags = L1_entry & L1_ENTRY_PAGE_TABLE_FLAGS_MASK;
                if (__is_l1_page_table_entry(L1_flags)) {
                        L2_tables_count++;
 
                        L2_tbl_physaddr = L1_entry & L1_ENTRY_PAGE_TABLE_PHYSICAL_ADDRESS_MASK;
                        if(paddr_array[GET_PGD_INDEX(L2_tbl_physaddr)]) {
                                hitcount++;
                        } else if(paddr_array[GET_PGD_INDEX(L2_tbl_physaddr)+1]) {
                                hitdouble++;
                        } else {
                                nonhitcount++;
                        }
                        if((paddr_array[GET_PGD_INDEX(L2_tbl_physaddr)])) {
                                printk("\nG2PG2P L2_tble_physaddr = %lx L2_tble_vaddr = %lx\n",L2_tbl_physaddr,(unsigned long)__va(L2_tbl_physaddr));
                        }
                }
        }

        TTS_DBG_LOG (TAG "\n");
        printk(TAG"\n VGGP hitcount = %d doublehit = %d nonhitcount =%d \n",hitcount,hitdouble,nonhitcount);
        TTS_DBG_LOG (TAG "L2_tables_count =         %u\n", L2_tables_count);
        TTS_DBG_LOG (TAG "L2_tables_entries_count = %u\n", L2_tables_entries_count);

        return TTS_TC_RET_SUCCESS;
}
int tima_check_kern_l2_attributes(void)
{
    __init_paddr_array();
    l1_page_walk((unsigned long)KPGD_INIT_MM);
    return(l1_analyze_addr(((unsigned long)KPGD_INIT_MM)));
}
/*
 * scan_kern_pgd- Scan the kernel process descriptors to find out whether pages are readonly`
 */

int scan_kern_pgd(void)
{
    unsigned long kpgd;

/*    kpgd = (unsigned long)(init_mm.pgd); */
    kpgd = (unsigned long)KPGD_INIT_MM;

    return l1_page_walk(kpgd);
}


/*
 * scan_proc_pgd - Scan the  process descriptors to find out whether pages are readonly
 */
int scan_proc_pgd(void)
{
    unsigned long kpgd;
    struct task_struct *task = NULL;

    for_each_process(task)
    {
            struct mm_struct *mm;
            int i = 0;

            mm = task->active_mm;//or mm?

            if(!mm || __is_pgd_invalid((unsigned long)(mm->pgd)))
                    continue;
            for (i=0 ; i< TTS_MAX_L1_PT; i++)
            {
                kpgd =(unsigned long)(mm->pgd) + i* TTS_PAGE_SIZE;
                if( check_pte_readonly(kpgd)  == TTS_TC_RET_FAILURE)
                {
                        return TTS_TC_RET_FAILURE;
                }
            }
    }

    return TTS_TC_RET_SUCCESS;
}
int tts_walk_proc_pgd(void)
{
    struct task_struct *task = NULL;

    for_each_process(task)
    {
            l1_page_walk((unsigned long)(task->active_mm->pgd));

    }
    return TTS_TC_RET_SUCCESS;
}
