#!/bin/sh

SRCNAME=jquery-3.2.1.min.js
GZNAME=$SRCNAME.gz
OUTNAME=$GZNAME.h

wget -O $SRCNAME https://code.jquery.com/$SRCNAME
cat $SRCNAME | gzip -9 > $GZNAME

echo "const PROGMEM char asset_jquery[] = {" > $OUTNAME
cat $GZNAME | xxd -i >> $OUTNAME
echo "};" >> $OUTNAME

echo "#define ASSET_JQUERY_LEN $(ls -l jquery-3.2.1.min.js.gz | awk '{print $5}')" >> $OUTNAME
