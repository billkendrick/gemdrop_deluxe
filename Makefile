CC65_HOME=/usr/local/share/cc65/
CC65_INC=/usr/local/share/cc65/include/
CC65_ASMINC=/usr/local/share/cc65/asminc/
CC65_LIB=/usr/local/share/cc65/lib/
CC65_CFG=/usr/local/share/cc65/cfg/
FRANNY=/usr/local/franny/bin/franny
XXD=/usr/bin/xxd

.PHONY: all clean run run-xex

all:	gemdrop.atr

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
		lib/sound.h \
		extended_headers/antic.h extended_headers/gtia.h \
		extended_headers/pbi.h extended_headers/pia.h \
		extended_headers/pokey.h \
		extended_headers/page0.h extended_headers/page2.h \
		extended_headers/atascii.h
	cc65 -I "${CC65_INC}" -t atari gemdrop.c

lib/sound.s:	lib/sound.c \
		lib/sound.h \
		extended_headers/pokey.h
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
