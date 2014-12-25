VFD=../mtximage

#echo "*** Clean-up drive **********************************"
#sudo mount -o loop $VFD /mnt
#sudo rm /mnt/bin/*
#sudo umount /mnt

echo "--------------------- make $1 -----------------------"
bcc -c -ansi $1.c
as86 -o assembly.o assembly.s
ld86 -o $1 assembly.o $1.o ../mtxlib /usr/lib/bcc/libc.a

if [ -f $1 ]; then
    echo "*** Copy to disk ************************************"
    sudo mount -o loop $VFD /mnt
    sudo cp $1 /mnt/bin/$1
    sudo umount /mnt
fi

echo "*** Clean-up build **********************************"
rm *.o $1
