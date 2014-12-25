VFD=mtximage

echo "*** Compiling *******************************************"
as86 -o assembly.o assembly.s
bcc  -c -ansi main.c
ld86 -d -o mtx assembly.o main.o mtxlib /usr/lib/bcc/libc.a

if [ -f mtx ]; then
    cp $VFD.bak $VFD
    sudo mount -o loop $VFD /mnt
    sudo cp mtx /mnt/boot
    sudo umount /mnt
    rm *.o mtx
    
    cd USER
    ./make.sh u1
    ./make.sh u2
    cd ..
    
    qemu-system-i386 -fda $VFD -no-fd-bootchk
fi
