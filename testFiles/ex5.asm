; 2^n sequence

mov cx 7
mov ah 3

mov al 1
mov dl 1
next:
    int 21
    add dl al
    mov al dl 
loop next

int 20
