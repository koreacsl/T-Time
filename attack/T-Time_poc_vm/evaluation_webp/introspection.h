/*
 * This code is based on:
 * 
 * Wilke, L., Sieck, F., & Eisenbarth, T. (2024). 
 * TDXdown: Single-Stepping and Instruction Counting Attacks against Intel TDX.
 * ACM CCS. https://doi.org/10.5281/zenodo.12683611
 * 
 * The function below converts an IDA-based virtual address to its corresponding
 * runtime address, assuming a known load base of the executable segment.
 */

#ifndef TARGET_INTROSPECTION
#define TARGET_INTROSPECTION

#include "pmparser.h"
#include "parse_pagemap.h"
#include "stdbool.h"


enum CodeLoactionName {
    CLN_VP8ParseIntraModeRow,
    CLN_VP8DecodeMB,
    CLN_GetCoeffsFast,
    CodeLocationLength,
};

char* code_location_name_to_string(enum CodeLoactionName c) {
    switch (c)
    {
     case CLN_VP8ParseIntraModeRow:
        return "VP8ParseIntraModeRow";
        break;

    case CLN_VP8DecodeMB:
        return "VP8DecodeMB";
        break;

    case CLN_GetCoeffsFast:
        return "GetCoeffsFast";
        break;

    default:
        printf("Unknown CodeLoactionName: %d", c);
        return "UNKNOWN CodeLoactionName";
        break;
    }
}

/*
 * @brief Converts an IDA-based offset to the corresponding runtime virtual address.
 * 
 * This function is used to translate an address seen in IDA Pro (where the ELF is
 * typically rebased to start at 0x1000) into the actual address where the binary's 
 * executable segment is loaded at runtime.
 * 
 * @param addrExecLoadSegment The runtime virtual address where the ELF's executable segment is loaded.
 * @param ida_offset The virtual address or offset as seen in IDA Pro (e.g., _init_proc is at 0x1000).
 * @return uint64_t The corresponding runtime virtual address.
 */
uint64_t ida_to_pin_addr(uint64_t addrExecLoadSegment, uint64_t ida_offset) {
// For libwebp	
    const uint64_t IDA_BASE = 0x3000;  // Base virtual address used by IDA when analyzing the binary

    return addrExecLoadSegment + ida_offset - IDA_BASE;
}

/*
 * @brief Check if string ends with suffix
 * 
 * @param suffix  suffix
 * @param s value to check for suffix
 * @return true if s ends with given suffix
 * @return false otherwise
 */

bool ends_with_suffix(char* suffix, char* s)
{
    int n1 = strlen(suffix), n2 = strlen(s);
    if (n1 > n2)
        return false;
    for (int i = 0; i < n1; i++)
        if (suffix[n1 - i - 1] != s[n2 - i - 1])
            return false;
    return true;
}

/*
 * @brief Retrieves the runtime base virtual address of the target library's executable segment.
 * 
 * This function parses the current process's memory mappings and searches for a
 * segment that is both executable and corresponds to a specific shared object file 
 * If found, the start address of that segment is returned.
 * 
 * @param out_target_vaddr Output parameter that will contain the base virtual address
 *        of the executable segment of the target library.
 * @return int 0 on success, -1 on mapping parse error, -2 if the target library is not found.
 */

int get_target_base_vaddr(uint64_t* out_target_vaddr){
  int pid = -1;
	procmaps_iterator* maps = pmparser_parse(pid);
	if(maps==NULL){
		printf ("[map]: cannot parse the memory map of %d\n",pid);
		return -1;
	}

	//iterate over areas
	procmaps_struct* maps_tmp=NULL;
	bool found_target = false;
	while( !found_target && ((maps_tmp = pmparser_next(maps)) != NULL) ) {

    bool has_target_name = ends_with_suffix("libwebp.so.7",maps_tmp->pathname);

    if( maps_tmp->is_x && has_target_name ) {
      found_target = true;
      *out_target_vaddr = (uint64_t)maps_tmp->addr_start;
      printf("Matching library:\n");
      pmparser_print(maps_tmp,0); // @@@@@@@ parser_print
      break;
    }
	}

	//mandatory: should free the list
	pmparser_free(maps);

  if(found_target) {
    return 0;
  } else {
    return -2;
  }

}

int print_gpas(const uint64_t* ida, uint64_t ida_len) {
  uint64_t target_base_vaddr = 0;
  if( get_target_base_vaddr(&target_base_vaddr) ) {
      printf("Failed to parse target library base vaddr\n");
      return -1;
  }

  pid_t pid = getpid();
  uint64_t rt_vaddrs[CodeLocationLength] = {0};
  uint64_t rt_paddrs[CodeLocationLength] = {0};
  for( size_t code_location = 0; code_location < CodeLocationLength; code_location++) {
    uint64_t v = ida_to_pin_addr(target_base_vaddr, ida[code_location]);
    rt_vaddrs[code_location] = v;

    uint64_t p;
    if( virt_to_phys_user(&p, pid, v) ) {
      printf("Failed to translate vaddr 0x%jx for code location %s to paddr\n", v, code_location_name_to_string(code_location));
      return -1;
    }
    rt_paddrs[code_location] = p;
  }

  printf("\n\n");
  for( size_t code_location = 0; code_location < CodeLocationLength; code_location++) {
    printf("Code Location %-25s: vaddr=0x%jx, paddr=0x%jx [pfn 0x%jx]\n",code_location_name_to_string(code_location), rt_vaddrs[code_location], rt_paddrs[code_location], rt_paddrs[code_location] >> 12);
  }
  printf("\n\n");

  return 0;
}

#endif 
