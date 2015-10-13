CC65_HOME=/usr/local/share/cc65/
CC65_INC=/usr/local/share/cc65/include/
CC65_ASMINC=/usr/local/share/cc65/asminc/
CC65_LIB=/usr/local/share/cc65/lib/
CC65_CFG=/usr/local/share/cc65/cfg/
FRANNY=/usr/local/franny/bin/franny
XXD=/usr/bin/xxd

VERSION=2015_10_13_alpha

.PHONY: all clean release release-clean run run-xex

all:	gemdrop.xex

clean:
	-rm gemdrop.atr
	-rm gemdrop.xex
	-rm gemdrop.o
	-rm gemdrop.s
	-rm lib/sound.o
	-rm lib/sound.s
	-rm gemdrop.atr.in1
	-rm gemdrop.atr.in
	-rm gemdrop.map
	-rm gemdrop-font.h
	-rm title-font.h
	-rm data/generated/title1.fnt
	-rm data/generated/title2.fnt
	-rm tools/title-to-font
	-rm tools/title-to-font.o
	-rm tools/fonts-to-h
	-rm tools/fonts-to-h.o

release:	gemdrop.xex
	-rm -rf release
	mkdir release
	mkdir release/gemdrop_deluxe_${VERSION}
	cp gemdrop.xex release/gemdrop_deluxe_${VERSION}/
	cp README.txt release/gemdrop_deluxe_${VERSION}/
	cp CHANGES.txt release/gemdrop_deluxe_${VERSION}/
	cp LICENSE.txt release/gemdrop_deluxe_${VERSION}/
	cd release && zip -r gemdrop_deluxe_${VERSION}.zip gemdrop_deluxe_${VERSION}

release-clean:
	-rm -rf release

gemdrop.xex:	gemdrop.o lib/sound.o atari.cfg
	ld65 \
		--cfg-path "." \
		--lib-path "${CC65_LIB}" \
		-o gemdrop.xex \
		-t atari \
		-m gemdrop.map \
		gemdrop.o \
		lib/sound.o \
		atari.lib

gemdrop.o:	gemdrop.s
	ca65 -I "${CC65_ASMINC}" -t atari gemdrop.s

lib/sound.o:	lib/sound.s
	ca65 -I "${CC65_ASMINC}" -t atari lib/sound.s

gemdrop.s:	gemdrop.c \
		gemdrop-font.h \
		title-font.h \
		lib/sound.h
	cc65 -I "${CC65_INC}" -D VERSION="\"${VERSION}\"" -t atari gemdrop.c

gemdrop-font.h:	data/gemdrop1.fnt data/gemdrop2.fnt tools/fonts-to-h
	tools/fonts-to-h --merge data/gemdrop1.fnt data/gemdrop2.fnt gemdrop-font.h

title-font.h:	data/generated/title1.fnt data/generated/title2.fnt tools/fonts-to-h
	tools/fonts-to-h data/generated/title1.fnt data/generated/title2.fnt title-font.h

data/generated/title1.fnt:	tools/title-to-font data/source/gemdrop_deluxe.png
	tools/title-to-font 1

data/generated/title2.fnt:	tools/title-to-font data/source/gemdrop_deluxe.png
	tools/title-to-font 2

lib/sound.s:	lib/sound.c \
		lib/sound.h
	cc65 -I "${CC65_INC}" -t atari lib/sound.c

gemdrop.atr:	gemdrop.atr.in gemdrop.xex
	cp gemdrop.atr.in gemdrop.atr
	${FRANNY} -A gemdrop.atr -i gemdrop.xex -o GEMDROP.AR0

run:	gemdrop.atr
	atari800 -audio16 -nobasic gemdrop.atr

run-xex:	gemdrop.xex
	atari800 -audio16 -nobasic -run gemdrop.xex

gemdrop.atr.in:	mydos/dos.sys mydos/dup.sys mydos/sd_mydos.xxd
	${FRANNY} -C gemdrop.atr.in1 -d s -f a
	${FRANNY} -F gemdrop.atr.in1
	${FRANNY} -A gemdrop.atr.in1 -i mydos/dos.sys -o DOS.SYS
	${FRANNY} -A gemdrop.atr.in1 -i mydos/dup.sys -o DUP.SYS
	( cat mydos/sd_mydos.xxd ; \
	  ${XXD} -s 0x180 gemdrop.atr.in1 ) | \
		${XXD} -r > gemdrop.atr.in

tools/fonts-to-h:     tools/fonts-to-h.o
	$(CC) $(CFLAGS) $(LDFLAGS) tools/fonts-to-h.o -o tools/fonts-to-h

tools/fonts-to-h.o:   tools/fonts-to-h.c
	$(CC) $(CFLAGS) tools/fonts-to-h.c -c -o tools/fonts-to-h.o

tools/title-to-font:     tools/title-to-font.o
	$(CC) $(CFLAGS) $(LDFLAGS) tools/title-to-font.o -o tools/title-to-font

tools/title-to-font.o:   tools/title-to-font.c
	$(CC) $(CFLAGS) tools/title-to-font.c -c -o tools/title-to-font.o

