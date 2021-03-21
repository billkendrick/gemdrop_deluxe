CC65=/usr/bin/cc65
CA65=/usr/bin/ca65
CC65_HOME=/usr/share/cc65/
CC65_INC=/usr/share/cc65/include/
CC65_ASMINC=/usr/share/cc65/asminc/
CC65_LIB=/usr/share/cc65/lib/
CC65_CFG=/usr/share/cc65/cfg/
FRANNY=/usr/local/franny/bin/franny
XXD=/usr/bin/xxd

VERSION=2021_03_20_beta
VERSION_UCASE=$(shell echo "$(VERSION)" | tr '[:lower:]' '[:upper:]' | tr '_' '-')

.PHONY: all clean release release-clean run run-xex run-atr

all:	gemdrop.xex gemdropd.xex

clean:
	-rm gemdrop.atr
	-rm gemdrop.xex
	-rm gemdropd.xex
	-rm gemdropu.xex
	-rm gemdrop.dat.in
	-rm gemdrop.o
	-rm gemdropd.o
	-rm gemdropu.o
	-rm gemdrop.s
	-rm gemdropd.s
	-rm gemdropu.s
	-rm lib/sound.o
	-rm lib/sound.s
	-rm lib/rmtplayr.obx
	-rm gemdrop.map
	-rm gemdrop-font.h
	-rm title-font.h
	-rm text-font.h
	-rm song1.h
	-rm data/generated/title1.fnt
	-rm data/generated/title2.fnt
	-rm data/generated/text.fnt
	-rm tools/title-to-font
	-rm tools/title-to-font.o
	-rm tools/fonts-to-h
	-rm tools/fonts-to-h.o
	-rm blank.atr

release:	gemdrop.xex gemdropd.xex gemdrop.atr
	-rm -rf release
	mkdir release
	mkdir release/gemdrop_deluxe_${VERSION}
	cp gemdrop.xex gemdrop.atr gemdropd.xex \
		release/gemdrop_deluxe_${VERSION}/
	cat README.txt | sed -e "s/\$$VERSION/${VERSION}/" \
		> release/gemdrop_deluxe_${VERSION}/README.txt
	cp CHANGES.txt release/gemdrop_deluxe_${VERSION}/
	cp LICENSE.txt release/gemdrop_deluxe_${VERSION}/
	cd release && zip -r gemdrop_deluxe_${VERSION}.zip gemdrop_deluxe_${VERSION}

release-clean:
	-rm -rf release

# FIXME: Use clever Makefile tricks for "gemdrop.xex", "gemdropd.xex", and "gemdropu.xex"
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

gemdropd.xex:	gemdropd.o lib/sound.o atari.cfg
	ld65 \
		--cfg-path "." \
		--lib-path "${CC65_LIB}" \
		-o gemdropd.xex \
		-t atari \
		-m gemdrop.map \
		gemdropd.o \
		lib/sound.o \
		atari.lib

gemdropu.xex:	gemdropu.o lib/sound.o atari.cfg
	ld65 \
		--cfg-path "." \
		--lib-path "${CC65_LIB}" \
		-o gemdropu.xex \
		-t atari \
		-m gemdrop.map \
		gemdropu.o \
		lib/sound.o \
		atari.lib

# FIXME: Use clever Makefile tricks for "gemdrop.o" and "gemdropd.o"
gemdrop.o:	gemdrop.s
	${CA65} -I "${CC65_ASMINC}" -t atari gemdrop.s

gemdropd.o:	gemdropd.s
	${CA65} -I "${CC65_ASMINC}" -t atari gemdropd.s

gemdropu.o:	gemdropu.s
	${CA65} -I "${CC65_ASMINC}" -t atari gemdropu.s

lib/sound.o:	lib/sound.s
	${CA65} -I "${CC65_ASMINC}" -t atari lib/sound.s

# FIXME: Use clever Makefile tricks for "gemdrop.s" and "gemdropd.s"
gemdrop.s:	gemdrop.c \
		gemdrop-font.h \
		title-font.h \
		text-font.h \
		song1.h \
		lib/sound.h \
		lib/rmtplayr.h
	${CC65} -I "${CC65_INC}" \
		-D VERSION="\"${VERSION_UCASE} XEX\"" \
		-t atari gemdrop.c \
		-o gemdrop.s

gemdropd.s:	gemdrop.c \
		gemdrop-font.h \
		title-font.h \
		text-font.h \
		song1.h \
		lib/sound.h \
		lib/rmtplayr.h
	${CC65} -I "${CC65_INC}" \
		-D VERSION="\"${VERSION_UCASE} DISK\"" \
		-D DISK=\"1\" \
		-t atari gemdrop.c \
		-o gemdropd.s

gemdropu.s:	gemdrop.c \
		gemdrop-font.h \
		title-font.h \
		text-font.h \
		song1.h \
		lib/sound.h \
		lib/rmtplayr.h
	${CC65} -I "${CC65_INC}" \
		-D VERSION="\"${VERSION_UCASE} UDOS\"" \
		-D DISK=\"1\" \
		-D UDOS=\"1\" \
		-t atari gemdrop.c \
		-o gemdropu.s

gemdrop-font.h:	data/gemdrop1.fnt data/gemdrop2.fnt tools/fonts-to-h
	# tools/fonts-to-h --merge data/gemdrop1.fnt data/gemdrop2.fnt gemdrop-font.h
	tools/fonts-to-h data/gemdrop1.fnt data/gemdrop2.fnt gemdrop-font.h

title-font.h:	data/generated/title1.fnt data/generated/title2.fnt tools/fonts-to-h
	tools/fonts-to-h data/generated/title1.fnt data/generated/title2.fnt title-font.h

text-font.h:	data/generated/text.fnt tools/fonts-to-h
	tools/fonts-to-h data/generated/text.fnt text-font.h

song1.h:	data/song.rmt
	${XXD} -i data/song.rmt > song1.h

data/generated:
	-mkdir data/generated

data/generated/title1.fnt:	data/generated tools/title-to-font data/source/gemdrop_deluxe.png
	tools/title-to-font data/source/gemdrop_deluxe.png 1 data/generated/title1.fnt

data/generated/title2.fnt:	data/generated tools/title-to-font data/source/gemdrop_deluxe.png
	tools/title-to-font data/source/gemdrop_deluxe.png 2 data/generated/title2.fnt

data/generated/text.fnt:	data/generated tools/title-to-font data/source/font.png
	tools/title-to-font data/source/font.png 1 data/generated/text.fnt

lib/sound.s:	lib/sound.c \
		lib/sound.h
	${CC65} -I "${CC65_INC}" -t atari lib/sound.c

lib/rmtplayr.h:	lib/rmtplayr.obx
	${XXD} -i lib/rmtplayr.obx > lib/rmtplayr.h

lib/rmtplayr.obx:	lib/rmtplayr.a65 lib/rmt_feat.a65
	cd lib/ ; xasm rmtplayr.a65

gemdrop.atr:	gemdrop.atr.in gemdropu.xex gemdrop.dat.in
	cp gemdrop.atr.in gemdrop.atr
	${FRANNY} -A gemdrop.atr -i gemdropu.xex -o AUTORUN
	${FRANNY} -A gemdrop.atr -i gemdrop.dat.in -o GEMDROP.DAT

# uDOS requires a file exist to be able to overwrite it(!)
gemdrop.dat.in:
	dd if=/dev/zero of=gemdrop.dat.in bs=128 count=1

run-atr:	gemdrop.atr
	atari800 -audio16 -nobasic gemdrop.atr

run-xex:	gemdrop.xex
	atari800 -audio16 -nobasic -run gemdrop.xex

tools/fonts-to-h:     tools/fonts-to-h.o
	$(CC) $(CFLAGS) $(LDFLAGS) tools/fonts-to-h.o -o tools/fonts-to-h

tools/fonts-to-h.o:   tools/fonts-to-h.c
	$(CC) $(CFLAGS) tools/fonts-to-h.c -c -o tools/fonts-to-h.o

tools/title-to-font:     tools/title-to-font.o
	$(CC) $(CFLAGS) $(LDFLAGS) tools/title-to-font.o -o tools/title-to-font

tools/title-to-font.o:   tools/title-to-font.c
	$(CC) $(CFLAGS) tools/title-to-font.c -c -o tools/title-to-font.o

# Creates a blank ATR, for generating a fresh "gemdrop.atr.in" by hand.
# Steps:
# 1. Download "udos.atr" from this ABBUC thread
#    http://www.abbuc.de/community/forum/viewtopic.php?f=3&t=10347)
#    (FIXME: is there an official home for this tool besides this forum
#    thread?)
# 2. Boot an Atari (or emulator) with "udos.atr" in D1:, and this disk
#    image ("blank.atr") in D2:
# 3. From the XDOS prompt, run UDOSINIT.COM
# 4. Press [2] to select D2: as the target drive
# 5. Press [Shift]+[W] to write uDOS to D2:
# 6. Rename "blank.atr" to "gemdrop.atr.in"
#    (e.g., "mv blank.atr gemdrop.atr.in")
#    to copy over the existing file; be sure to commit!
blank.atr:
	${FRANNY} -C blank.atr -d s -f a
	${FRANNY} -F blank.atr

