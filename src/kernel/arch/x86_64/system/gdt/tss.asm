global load_tss

; rdi - address
; rsi - gdt
; rdx - selector
load_tss:
    push rbx ; save rbx

    lea rbx, [rsi + rdx] ; length of tss
    mov word [rbx], 108

    mov eax, edi
    lea rbx, [rsi + rdx + 2] ; low bytes of tss
    mov word [rbx], ax

    mov eax, edi
    shr eax, 16
    lea rbx, [rsi + rdx + 4] ; mid
    mov byte [rbx], al

    mov eax, edi
    shr eax, 24
    lea ebx, [rsi + rdx+ 7] ; high
    mov byte [rbx], al

    mov rax, rdi
    shr rax, 32
    lea rbx, [rsi + rdx + 8] ; high 32
    mov dword [rbx], eax

    pop rbx ; restore rbx
    ret 