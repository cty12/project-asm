mov cx 4f 
mov dl 41
again:
mov ah 2
int 21
jmp next
loop again
next:
int 20

