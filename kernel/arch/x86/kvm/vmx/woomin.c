//#include "woomin.h"
#include <linux/types.h>
#include <linux/module.h>
#include <linux/cpu.h>
#include <linux/mmu_context.h>
#include <linux/module.h>
#include <linux/woomin_step.h>

#include <asm/fpu/xcr.h>
#include <asm/tdx.h>

#include "capabilities.h"
#include "x86_ops.h"
#include "common.h"
#include "mmu.h"
#include "tdx_arch.h"
#include "tdx.h"
#include "vmx.h"
#include "x86.h"
#include <linux/module.h>
#include <trace/events/kvm.h>
#include "trace.h"

#include "tdx_arch.h"
#include "vmx.h"
#include "x86.h"
#include "x86_ops.h"
#include "common.h"
#include "mmu.h"


uint64_t a, d;
uint64_t start, end;
uint64_t start_n, end_n;
uint64_t temp_n;


// woomin_step page related
uint64_t fault_count = 0;
bool woomin_block = false;
uint64_t fault_gfn;

uint64_t current_tsc=0;
uint64_t past_tsc=0;
uint64_t past_tsc_t=0;
uint64_t first_tsc=0;

int woomin_fault_index = 0;
int woomin_list[500];
bool woomin_blocked[500];
char woomin_str[500][30];
bool tracker = false;

void *woomin_flush_va[10]; // flush virtual address
struct page *woomin_flush_page[10];
uint64_t woomin_flush_gfn[10];
uint64_t woomin_flush_gpa[10];
volatile uint64_t flush_data;
bool flush_tracker = false;
int woomin_flush_index = 0;


void *woomin_flush_direct_va; //
struct page *woomin_flush_direct_page;
EXPORT_SYMBOL(woomin_flush_direct_va);
EXPORT_SYMBOL(woomin_flush_direct_page);

extern void tdx_track(struct kvm *kvm);

zap_unzap_fnptr_t woomin_tdx_sept_zap_private_spte_fnptr = NULL;
EXPORT_SYMBOL(woomin_tdx_sept_zap_private_spte_fnptr);
zap_unzap_fnptr_t woomin_tdx_sept_unzap_private_spte_fnptr = NULL;
EXPORT_SYMBOL(woomin_tdx_sept_unzap_private_spte_fnptr);
zap_unzap_fnptr_t woomin_tdx_sept_rezap_private_spte_fnptr = NULL;
EXPORT_SYMBOL(woomin_tdx_sept_rezap_private_spte_fnptr);

/// flush pointer
void (*woomin_tdx_flush_load_fnptr)(int num) = NULL;
EXPORT_SYMBOL(woomin_tdx_flush_load_fnptr);
/// direct flush pointer
int (*woomin_tdx_reload_direct_fnptr)(void) = NULL;
EXPORT_SYMBOL(woomin_tdx_reload_direct_fnptr);


//woomin_sept_gpa_to_hpa_fnptr_t woomin_sept_gpa_to_hpa_fnptr = NULL;
//EXPORT_SYMBOL(woomin_sept_gpa_to_hpa_fnptr);


EXPORT_SYMBOL(a);
EXPORT_SYMBOL(d);
EXPORT_SYMBOL(start);
EXPORT_SYMBOL(end);
EXPORT_SYMBOL(start_n);
EXPORT_SYMBOL(end_n);
EXPORT_SYMBOL(temp_n);
EXPORT_SYMBOL(woomin_block);
EXPORT_SYMBOL(fault_count);
EXPORT_SYMBOL(fault_gfn);

EXPORT_SYMBOL(woomin_fault_index); 
EXPORT_SYMBOL(woomin_list);        
EXPORT_SYMBOL(woomin_blocked);
EXPORT_SYMBOL(woomin_str);

EXPORT_SYMBOL(current_tsc);
EXPORT_SYMBOL(past_tsc);
EXPORT_SYMBOL(past_tsc_t);
EXPORT_SYMBOL(tracker);
EXPORT_SYMBOL(first_tsc);

// flush address
EXPORT_SYMBOL(woomin_flush_va);
EXPORT_SYMBOL(woomin_flush_page);
EXPORT_SYMBOL(woomin_flush_gfn);
EXPORT_SYMBOL(flush_data);
EXPORT_SYMBOL(woomin_flush_gpa);
EXPORT_SYMBOL(flush_tracker);
EXPORT_SYMBOL(woomin_flush_index);
