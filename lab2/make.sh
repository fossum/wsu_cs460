rm *.o

echo "****************************************************************************"

echo compiling
   as86 -o  assembly.o  assembly.s
   bcc  -c -ansi main.c sandbox/printlib.c sandbox/types.c
echo linking
   ld86 -d assembly.o main.o sandbox/printlib.o sandbox/types.o mylib /usr/lib/bcc/libc.a

echo "****************************************************************************"

echo cp a.out to mtximage/boot/mtx
   cp mtximage.bak mtximage
   sudo mount -o loop mtximage /mnt
   sudo cp a.out /mnt/boot/mtx
   sudo umount /mnt
echo done

#  Use YOUR MTX booter of LAB#1 to boot up MTX
qemu-system-i386 -fda mtximage -no-fd-bootchk

