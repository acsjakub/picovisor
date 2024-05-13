mov al, 'H'
mov byte [0x4000], al
mov al, 'e'
mov byte [0x4000], al
mov al, 'l'
mov byte [0x4000], al
mov byte [0x4000], al
mov al, 'o'
mov byte [0x4000], al
mov al, ' '
mov byte [0x4000], al
mov al, 'W'
mov byte [0x4000], al
mov al, 'o'
mov byte [0x4000], al
mov al, 'r'
mov byte [0x4000], al
mov al, 'l'
mov byte [0x4000], al
mov al, 'd'
mov byte [0x4000], al
mov al, 0x00
mov byte [0x4000], al
    ;;  Here we start interacting with the counter
    ;;  it looks a bit weird but writing into 0x5004 == increasing the counter
    ;;  reading 0x5000 == reading the counter
mov byte [0x5004], al
mov al, byte [0x5000]
add al, 0x30
mov byte [0x4000], al
mov byte [0x5004], al
mov al, byte [0x5000]
add al, 0x30
mov byte [0x4000], al
hlt
