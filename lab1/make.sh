#!/bin/sh

echo "*************************************************************************"
rm bs.o t.o a.out
echo comiling ......
as86 -o bs.o  bs.s
bcc  -c -ansi t.c
if [ $? -ne 0 ]; then
    exit 1
fi

echo linking .......
ld86 -d bs.o  t.o  /usr/lib/bcc/libc.a
echo check a.out size
ls -l a.out

echo dump a.out to a VIRTUAL FD
cp mtximage.bak mtximage
dd if=a.out of=mtximage bs=1024 count=1 conv=notrunc
echo "*************************************************************************"

actualsize=$(wc -c "a.out" | cut -f 1 -d ' ')
if [ $actualsize -gt 1024 ]; then
    echo "a.out is too big"
else
    qemu-system-i386 -fda mtximage -no-fd-bootchk -localtime -serial mon:stdio
fi
