VFD=mtximage   # change VFD to YOUR virtual fd

echo "*** Compiling *******************************************"
cd src
as86 -o assembly.o assembly.s
bcc  -c -ansi *.c
ld86 -d -o ../mtx assembly.o main.o forkexec.o int.o kernel.o \
    my_assert.o pipe.o printlib.o wait.o ../mtxlib /usr/lib/bcc/libc.a
rm *.o
cd ..

if [ -f mtx ]; then
    cp $VFD.bak $VFD
    sudo mount -o loop $VFD /mnt
    sudo cp mtx /mnt/boot
    sudo umount /mnt
    rm mtx
    
    cd USER
    ./make.sh u1
    #./make.sh u2
    cd ..
    
    qemu-system-i386 -fda $VFD -no-fd-bootchk
fi
