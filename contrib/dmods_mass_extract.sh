#!/bin/bash
find ../mirror/files.dinknetwork.com/  -iname "*.dmod" | xargs -I{} cp -a {} .
# Fix up missing tar trailing block
# Won't work: need to bunzip first..
#dd if=/dev/zero bs=512 count=2 of=reference.bin 2>/dev/null
#for i in *.dmod; do
#    size=$(stat --printf=%s $i)
#    skip=$((size - 2*512))
#    dd if=$i bs=1 count=1024 skip=$skip of=cur.bin 2>/dev/null
#    if ! diff reference.bin cur.bin; then
#	dd if=/dev/zero bs=512 count=2 >> $i 2>/dev/null
#    fi
#done
#rm cur.bin

find -iname "*.dmod" | xargs -n1 tar xjf

find -type d -print0 | xargs -0 chmod 755
find -type f -print0 | xargs -0 chmod 644

# This one extracts everything in the current directory
#find -iname "dir.ff" | xargs ./ffrextract
./ffrextract

# Actually extracting dir.ff's was optional for this, since they are
# hard-limited to 50 frames
find -iname "*.bmp" | grep -i -o -E '[0-9]+\.bmp' | sed 's/\.bmp$//i' | sort -n | less
# => highest abused sequence is 69
