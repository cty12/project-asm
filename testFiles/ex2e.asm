mov cx 4 
mov dl 43
mov ah 2
again:
int 21
add dl 3
loop again
int 20

