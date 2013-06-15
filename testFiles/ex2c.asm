mov cx 4 
mov dl 34
mov ah 2
again:
int 21
inc dl
loop again
int 20

