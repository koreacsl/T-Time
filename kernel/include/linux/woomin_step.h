#ifndef WOOMIN_STEP_H
#define WOOMIN_STEP_H

#include "linux/kvm_host.h"
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/kvm.h>
#include <linux/kvm_types.h>

typedef bool (*zap_unzap_fnptr_t)(struct kvm *, gfn_t, enum pg_level);
typedef int(*woomin_sept_gpa_to_hpa_fnptr_t)(struct kvm_vcpu *vcpu, uint64_t gpa, int target_level,uint64_t* out_hpa);

extern zap_unzap_fnptr_t woomin_tdx_sept_zap_private_spte_fnptr;
extern zap_unzap_fnptr_t woomin_tdx_sept_unzap_private_spte_fnptr;
extern zap_unzap_fnptr_t woomin_tdx_sept_rezap_private_spte_fnptr;
extern woomin_sept_gpa_to_hpa_fnptr_t my_sept_gpa_to_hpa_fnptr;
extern void (*woomin_tdx_flush_load_fnptr)(int num);
extern int (*woomin_tdx_reload_direct_fnptr)(void);


int woomin_sept_gpa_to_hpa(struct kvm_vcpu *vcpu, uint64_t gpa, int target_level,uint64_t* out_hpa);

//True if most recent TD GPA fault was due to instruction fetch
//bool last_fault_exec;

//True if most recent TD GPA fault was due to write
//bool last_fault_write;

// True if most recent TD GPA fault was due to read
//bool last_fault_read;

#endif
