  .text
  .global longest_inc_subseq
longest_inc_subseq:
        sub     sp, sp, #80
        str     x0, [sp, 24]
        str     x1, [sp, 16]
        str     x2, [sp, 8]
        str     xzr, [sp, 72]
        str     xzr, [sp, 64]
        b       .init_checker
.init_loop:
        ldr     x4, [sp, 64]
        lsl     x4, x4, 3
        ldr     x1, [sp, 16]
        add     x4, x1, x4
        mov     x1, 1
        str     x1, [x4]
        ldr     x3, [sp, 64]
        add     x3, x3, 1
        str     x3, [sp, 64]
.init_checker:
        ldr     x3, [sp, 64]
        ldr     x1, [sp, 8]
        cmp     x1, x3
        bhi     .init_loop
        str     xzr, [sp, 56]
        b       .checker_big_loop
.start_big_loop:
        str     xzr, [sp, 48]
        b       .inner_loop_checker
.start_inner_loop:
        ldr     x0, [sp, 56]
        lsl     x0, x0, 3
        ldr     x1, [sp, 24]
        add     x0, x1, x0
        ldr     x1, [x0]
        
        ldr     x0, [sp, 48]
        lsl     x0, x0, 3
        ldr     x2, [sp, 24]
        add     x0, x2, x0
        ldr     x0, [x0]
        cmp     x1, x0
        
        ble     .inc_j
        
        ldr     x0, [sp, 48]
        lsl     x0, x0, 3
        ldr     x1, [sp, 16]
        add     x0, x1, x0
        ldr     x0, [x0]
        add     x1, x0, 1
        
        ldr     x0, [sp, 56]
        lsl     x0, x0, 3
        ldr     x2, [sp, 16]
        add     x0, x2, x0
        ldr     x0, [x0]
        
        cmp     x1, x0
        
        bls     .inc_j
        
        ldr     x0, [sp, 48]
        lsl     x0, x0, 3
        ldr     x1, [sp, 16]
        add     x0, x1, x0
        ldr     x1, [x0]
        
        ldr     x0, [sp, 56]
        lsl     x0, x0, 3
        ldr     x2, [sp, 16]
        add     x0, x2, x0
        add     x1, x1, 1
        
        str     x1, [x0]

.inc_j:
        ldr     x0, [sp, 48]
        add     x0, x0, 1
        str     x0, [sp, 48]
.inner_loop_checker:
        ldr     x1, [sp, 48]
        ldr     x0, [sp, 56]
        cmp     x1, x0
        blt     .start_inner_loop
        
        ldr     x0, [sp, 56]
        add     x0, x0, 1
        str     x0, [sp, 56]
.checker_big_loop:
        ldr     x4, [sp, 56]
        ldr     x1, [sp, 8]
        cmp     x1, x4
        bhi     .start_big_loop
        
        str     xzr, [sp, 40]
        b       .final_loop
        
.loop_checker:
        ldr     x0, [sp, 40]
        lsl     x0, x0, 3
        ldr     x1, [sp, 16]
        add     x0, x1, x0
        ldr     x1, [x0]
        ldr     x0, [sp, 72]
        cmp     x1, x0
        bls     .inc_i
        ldr     x0, [sp, 40]
        lsl     x0, x0, 3
        ldr     x1, [sp, 16]
        add     x0, x1, x0
        ldr     x0, [x0]
        str     x0, [sp, 72]
.inc_i:
        ldr     x0, [sp, 40]
        add     x0, x0, 1
        str     x0, [sp, 40]
        
.final_loop:
        ldr     x0, [sp, 40]
        ldr     x1, [sp, 8]
        cmp     x1, x0
        bhi     .loop_checker
        ldr     x0, [sp, 72]
        add     sp, sp, 80
        ret
