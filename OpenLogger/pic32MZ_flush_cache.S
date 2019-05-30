#include <xc.h>
#include <cp0defs.h>

    /* Symbols defined in linker script */
    .weak __pic32_init_cache_program_base_addr
    .weak __pic32_init_cache_data_base_addr

    /* ==================================== */
    #define Index_Store_Tag_I 0x08      /* intialize the instruction cache */
    #define Index_Store_Tag_D 0x09      /* intialize the data cache */  
    #define Index_Store_Tag_F 0x15      /* flush the data cache line */

    #define tmp t0
    #define cfg t1
    #define icachesize t2
    #define ilinesize t3
    #define iways t4
    #define dcachesize t5
    #define dlinesize t6
    #define dways t7
    #define save_ra v1

    .sdata; .globl __pic32_icache_size; .type __pic32_icache_size,@object; .size __pic32_icache_size,4; __pic32_icache_size:; .word -1
    .sdata; .globl __pic32_icache_linesize; .type __pic32_icache_linesize,@object; .size __pic32_icache_linesize,4; __pic32_icache_linesize:; .word -1
    .sdata; .globl __pic32_icache_ways; .type __pic32_icache_ways,@object; .size __pic32_icache_ways,4; __pic32_icache_ways:; .word 1

    .sdata; .globl __pic32_dcache_size; .type __pic32_dcache_size,@object; .size __pic32_dcache_size,4; __pic32_dcache_size:; .word -1
    .sdata; .globl __pic32_dcache_linesize; .type __pic32_dcache_linesize,@object; .size __pic32_dcache_linesize,4; __pic32_dcache_linesize:; .word -1
    .sdata; .globl __pic32_dcache_ways; .type __pic32_dcache_ways,@object; .size __pic32_dcache_ways,4; __pic32_dcache_ways:; .word 1

    .sdata; .globl __pic32_scache_size; .type __pic32_scache_size,@object; .size __pic32_scache_size,4; __pic32_scache_size:; .word -1
    .sdata; .globl __pic32_scache_linesize; .type __pic32_scache_linesize,@object; .size __pic32_scache_linesize,4; __pic32_scache_linesize:; .word -1
    .sdata; .globl __pic32_scache_ways; .type __pic32_scache_ways,@object; .size __pic32_scache_ways,4; __pic32_scache_ways:; .word 1

    .section .text.__pic32MZ_flush_cache, code, keep
    .set nomips16
    .set noreorder

    .global __pic32MZ_flush_cache
    .ent __pic32MZ_flush_cache
        
__pic32MZ_flush_cache:

        .set noreorder

        /* Walk the cache and flush */
8:      la a0,__pic32_init_cache_data_base_addr
        bne a0,zero,0f
        /* Use a default if the symbol is not defined */
        li a0,0x80000000  /* KSEG_DATA_BASE */

0:      beqz dcachesize,8f
        addu a1,a0,dcachesize # limit = base + dcachesize
1:      addu a0,dlinesize
        bne a0,a1,1b
        cache Index_Store_Tag_F,-4(a0) # BDSLOT: clear tag
        nop

       .set reorder
8:
        jr ra
        nop

  .end __pic32MZ_flush_cache