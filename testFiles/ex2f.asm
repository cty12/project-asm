mov cx 4 
mov dl 4a
mov ah 2
again:
int 21
sub dl 3
loop again
int 20

