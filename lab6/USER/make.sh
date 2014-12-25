VFD=../mtximage     # change VFD to YOUR virtual fd

echo "--------------------- make $1 -----------------------"
as86 -o user.o user.s
bcc -c -ansi $1.c ucode.c
ld86 -o $1 user.o $1.o ucode.o ../mtxlib /usr/lib/bcc/libc.a

if [ -f $1 ]; then
    echo "*** Copy to disk ************************************"
    sudo mount -o loop $VFD /mnt
    sudo cp $1 /mnt/bin/$1
    sudo umount /mnt

    echo "*** Clean-up build **********************************"
    rm *.o $1
    echo done $1
fi
