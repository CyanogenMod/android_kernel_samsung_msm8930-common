/*
 *      Common header file for Tima test cases
 *
 *      Author:    Guruprasad Ganesh   <g.ganesh@sta.samsung.com>
 *
 *      Changes:
 *              GG              :      Uploaded the initial Version
 *
 *
 */

#ifndef __KTEST_COMMON_H__
#define __KTEST_COMMON_H__

#define TTS_MAX_L1_PT   4
#define TTS_PAGE_SIZE   0x1000
#define KPGD_INIT_MM    0xc0004000

extern int scan_kern_pgd(void);
extern int scan_proc_pgd(void);
extern unsigned int call_pkm_modify_code(unsigned long arg);
extern unsigned int call_pkm_modify_revert_code(void);
extern unsigned int copy_pkm_func_ptr(unsigned long arg);
extern void lkmauth_restart(void);


unsigned int tts_call_pkm(void);

#endif/*__KTEST_COMMON_H__*/
