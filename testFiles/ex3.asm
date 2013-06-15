mov cx 4f 
mov dl 22
again:
mov ah 2
int 21
inc dl
loop again
int 20

