#!/usr/bin/perl

## -*- Makefile -*-
##
## User: ericfoss
## Time: Dec 2, 2014 2:28:42 PM
##

use 5.012;
use warnings;

# Restore backup VDISK
system("cp vdisk.bak vdisk");

# Compile/load code
foreach (qw/ init login fsh more cat grep l2u cp/) {
    system("./mku", "$_");
    if (! stat('program.out')) {
        die "Failed to compile code";
    }
}

# Copy edited files to disk
my ($sect) = (`./ptable ./vdisk 1` =~ /(\d+)/);
my $offset = $sect * 512;
if(system("sudo mount -o loop,offset=$offset ./vdisk /mnt")) {
    die "Could not mount filesystem";
}
system("sudo mkdir /mnt/user/a");
# From disk
#system("sudo cp /mnt/f1 ./");
# To /
foreach (qw/ f f1 /) {
    say "Copying $_ into /$_";
    if(system("sudo cp ./$_ /mnt/$_")) {
        die "Could not copy $_ into /mnt/$_";
    }
}
# To /etc/
foreach (qw/ passwd /) {
    say "Copying $_ into /etc/$_";
    if(system("sudo cp ./$_ /mnt/etc/$_")) {
        die "Could not copy $_ into /mnt/etc/$_";
    }
}
# To /bin/
foreach (qw/ test /) {
    say "Copying $_ into /etc/$_";
    if(system("sudo cp ./$_ /mnt/bin/$_")) {
        die "Could not copy $_ into /mnt/bin/$_";
    }
}
if(system("sudo umount /mnt")) {
    die "Could not unmount filesystem";
}

# Start Qemu
open(my $term, "./q 1 2>&1 |") || die "Failed: $!\n";
while (<$term>) {
    if ($_ =~ /\/dev\/pts\/(\d+)\s+\(label (\w+)\)/) {
        print $_;
        #say "putty -serial /dev/pts/$1 &";
        system("putty -serial -title $2 /dev/pts/$1 &");
    }
}
