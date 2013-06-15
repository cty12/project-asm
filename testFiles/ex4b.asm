mov bh 8
mov cx 24 
mov ah 2
mov dl 22
again:
int 21
inc dl
dec bh
jnz next 
mov dh dl
mov dl a 
int 21
mov dl dh
mov bh 8 
next:
loop again
int 20

