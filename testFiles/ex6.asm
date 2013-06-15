; fib
mov cx 7
mov ah 3

mov bh 0 
mov bl 1 

   next:
    mov dl bh
    int 21
    mov bh bl
    add bl dl    

loop next
int 20