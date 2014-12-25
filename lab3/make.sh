VFD=mtximage

echo "Assembling ..."
as86 -o assembly.o assembly.s

echo "Compiling ..."
bcc -c -ansi main.c #queue.c # wait.c kernel.c

echo "Linking ..."
ld86 -d -o mtx assembly.o main.o mtxlib /usr/lib/bcc/libc.a # wait.o kernel.o queue.o

if [ -f mtx ]; then
    echo "Loading ..."
    cp $VFD.bak $VFD
    sudo mount -o loop $VFD /mnt
    sudo cp mtx /mnt/boot
    sudo umount /mnt
    
    echo "Cleaning ..."
    sudo rm *.o mtx
    echo done
    
    echo "Running ..."
    qemu-system-i386 -fda mtximage -no-fd-bootchk
fi
