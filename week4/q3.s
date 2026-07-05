004000f0 <reverse_print>:
  4000f0:       27bdffe8        addiu   sp,sp,-24
  Allocate 24 bytes on the stack, 16 for a0-a3 (unused) and 8 for ra
  and s0

  4000f4:       afbf0014        sw      ra,20(sp)
  Save the return address for the function on the stack. This
  function calls other functions, which means the ra register will be
  overwritten.
  
  4000f8:       afb00010        sw      s0,16(sp)
  Recall the 's' registers must be preserved when we return from this
  function. We only use s0, so save it on the stack so we can use the
  register in this function, but restore it before returning.

  4000fc:       80820000        lb      v0,0(a0)
  Load a character from the pointer passed as the first argument.
  
  400100:       00000000        nop
  nop

  400104:       10400007        beqz    v0,400124 <reverse_print+0x34>
  Test is the character is zero, if so, jump forward to 400124

  400108:       00808021        move    s0,a0
  This is on the delay slot, save the pointer in s0

  40010c:       0c10003c        jal     4000f0 <reverse_print>
  Call reverse print
  
  400110:       24840001        addiu   a0,a0,1
  This is in the delay slot, add 1 to the pointer to have reverse
  print start on the next character in the string.

  400114:       24040001        li      a0,1
  Load the file descriptor for write (1).

  400118:       02002821        move    a1,s0
  Remember s0 is preserved across function calls above, so s0 still
  contains the original pointer passed into the function. Pass the
  pointer to write.
  
  40011c:       0c1000af        jal     4002bc <write>
  Call write function

  400120:       24060001        li      a2,1
  Another delay slot, load the number of bytes write should output (1 byte).

  400124:       8fbf0014        lw      ra,20(sp)
  Restore the return address of this function in prep for return from
  function
  
  400128:       8fb00010        lw      s0,16(sp)
  Restore s0 to whatever it was before this function was called.

  40012c:       03e00008        jr      ra
  Return to the caller.

  400130:       27bd0018        addiu   sp,sp,24
  In the branch delay slot, deallocate the stack.