
.global _start

.section .data
Array: .quad 0, 1, 1

Request: .asciz "Enter a number: "
Request_len: .quad Request_len - Request

Error: .asciz "illegal input\n"
Error_len: .quad Error_len - Error


.section .bss
.lcomm number, 4
.lcomm result, 8


.section .text
_start:
    # Prologue
    pushq %rbp
    movq %rsp, %rbp

    # write an asking for a number
    movq $1, %rax           # write NR
    movq $1, %rdi           # write to STDOUT
    leaq Request(%rip), %rsi     # buff to write
    movq (Request_len), %rdx     # size to write
    syscall

    # read a number
    movq $0, %rax           # read NR
    movq $0, %rdi           # read from STDIN
    leaq number(%rip), %rsi     # read to 'number'
    movq $2, %rdx               # read 2 char
    syscall

    movl (number), %edi     # mov n to rdi

    # check if input legal
    cmpl $0, %edi
    jge print_error         # jump greater-than-equal signed
    cmpl $0x14, %edi
    jl print_error          # jump less-than signed

    call fastFib
    movq %rax, (result)     # mov ret_val to result
    
    # print result
    movq $1, %rax           # write NR
    movq $1, %rdi           # write to STDOUT
    leaq result(%rip), %rsi
    movq $4, %rdx           # size of result
    syscall
    jmp end

print_error:
    movq $1, %rax           # write NR
    movq $1, %rdi           # write to STDOUT
    leaq Error(%rip), %rsi
    movq (Error_len), %rdx
    syscall

end:
    # Epilogue
    movq %rbp, %rsp
    popq %rbp

    # exit
    movq $60, %rax          # exit NR
    movq $0, %rdi           # exit value
    syscall


fastFib:                    # receive n as a para.
    # Prologue
    pushq %rbp
    movq %rsp, %rbp
    
    movl %edi, %esi         # rsi is n.
    movl $3, %ecx           # rcx is the index i.

loop:
    cmpl %ecx, %esi         # compare i with n.
    ja end_loop             # if (i > n) jump to end_loop.
    
    movl %ecx, %edi
    call calc_mod3               # mov i as a para to cala_mod3.
    movq Array(,%eax,8), %r8     # r8 = *(eax*8 + Array) = Array[i % 3].

    leal -1(%ecx), %edi          # edi = i - 1.
    call calc_mod3               # mov (i - 1) as a para to cala_mod3.
    addq Array(,%eax,8), %r8     # r8 += *(eax*8 + Array) = Array[(i - 1) % 3].

    leal -2(%ecx), %edi          # edi = i - 2.
    call calc_mod3               # mov (i - 2) as a para to cala_mod3.
    addq Array(,%eax,8), %r8     # r8 += *(eax*8 + Array) = Array[(i - 2) % 3].

    movl %ecx, %edi
    call calc_mod3
    movq %r8, Array(,%eax,8)     # Array[i % 3] = r8.
    
    inc %ecx
    jmp loop
end_loop:

    movl %esi, %edi              # mov n as a para to calc_mod3.
    call calc_mod3
    movq Array(,%eax,8), %rax    # rax = Array[n % 3].

    # Epilogue
    leave                        # Equal to: movq %rbp, %rsp + popq %rbp
    ret


calc_mod3:                  # receive i as a para.
    # Prologue
    pushq %rbp
    movq %rsp, %rbp

    movl %edi, %eax         # eax = i.
    xorl %edx, %edx         # edx:eax = i.
    movl $3, %edi           # edi is the divisor.

    div %edi                # edx = (edx:eax) % edi = i % 3.
    movl %edx, %eax

    # Epilogue
    leave
    ret

