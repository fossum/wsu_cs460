echo compiling
   as86 -o  s.o  s.s
   bcc  -c -ansi main.c printlib.c types.c

echo linking
   ld86 -d s.o main.o printlib.o types.o /usr/lib/bcc/libc.a

rm *.o

echo cp a.out to mtximage/boot/mtx
   cp mtximage.bak mtximage
   sudo mount -o loop mtximage /mnt
   sudo cp a.out /mnt/boot/mtx
   sudo umount /mnt
echo done

#  Use YOUR MTX booter of LAB#1 to boot up MTX
qemu-system-i386 -fda mtximage -no-fd-bootchk