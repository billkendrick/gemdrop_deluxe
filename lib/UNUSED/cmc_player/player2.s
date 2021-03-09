; cmc player v 2.1
; by Marcin Lewandowski
; based on cmc player v 2.0
; by Janusz Pelc

; player CMC skrocony
; init X-lsb, Y-msb muzyki
; Optimized (PLAYER2.ASM) by Jaskier

; Converted from Quick Assembler to cc65 assembler by Bill Kendrick, 10/2015
; playCMC(

;	opt 6

;	org $2000

        .autoimport     on
	.export		_playCMC

; strona 0
.SEGMENT "ZP"

	.export volume := 1

	.export numins := 3

	.export frq    := 6
	.export znieks := 10
	.export audc   := 14

	.export czest1 := 15
	.export czest2 := 18
	.export czest3 := 21
	.export zniek  := 24
	.export count1 := 27
	.export count2 := 30
	.export lopad  := 33
	.export numptr := 36
	.export poswpt := 39
	.export ilewol := 42
	.export czygrx := 45
	.export czygrc := 48

	.export dana0  := 49
	.export dana1  := 52
	.export dana2  := 55
	.export dana3  := 58
	.export dana4  := 61
	.export dana5  := 64
	.export ladr   := 67
	.export hadr   := 70

	.export posptr := 73
	.export possng := 74
	.export pocrep := 75
	.export konrep := 76
	.export ilrep  := 77
	.export tmpo   := 78
	.export ltemp  := 79
	.export b1     := 80
	.export b2     := 81
	.export b3     := 82
	.export czygr  := 83

	.export adrmus := 84
	.export adradr := 86
	.export adrsng := 88
	.export addr := 90
	.export word := 92

.SEGMENT "CODE"

; init

.proc   _playCMC: near

.SEGMENT "CODE"

init:	lda #3
	sta $d20f
	stx adrmus
	stx word
	sty adrmus+1
	sty word+1
	clc
	txa
	adc #$14
	sta adradr
	tya
	adc #0
	sta adradr+1
	stx adrsng
	iny
	iny
	sty adrsng+1

	ldx #8
l9:	lda #0
	sta $d200,x
	cpx #3
	bcs l10
	sta volume,x
	lda #$ff
	sta count1,x
	lda #$80
	sta czygrx,x
	sta czygrc
l10:	dex
	bpl l9

	lda #0
	ldx #9
l5:	sta poswpt,x
	dex
	bpl l5
	sta posptr
	sta possng
	lda #1
	sta czygr
	lda #$ff
	sta konrep

	ldy #$13
	lda (word),y
	tax
	ldy #0
	inc word+1
	inc word+1
	lda (word),y
	cmp #$cf
	bne l8
	tya
	clc
	adc #$55
	tay
	lda (word),y
	bmi l8
	tax
l8:	stx tmpo
	stx ltemp
	rts

.endproc

inst:	lda #0
	sta count1,x
	sta count2,x
	sta lopad,x

	lda b3
	asl a
	asl a
	asl a
	sta word
	clc
	lda adrmus
	adc #$30
	pha
	lda adrmus+1
	adc #1
	tay
	pla
	clc
	adc word
	sta ladr,x
	tya
	adc #0
	sta hadr,x

	clc
	lda adrmus
	adc #$94
	sta word
	lda adrmus+1
	adc #0
	sta word+1
	lda b3
	asl a
	adc b3
	asl a
	tay
	lda (word),y
	sta dana0,x
	iny
	lda (word),y
	sta dana1,x
	and #7
	sta b1
	iny
	lda (word),y
	sta dana2,x
	iny
	lda (word),y
	sta dana3,x
	iny
	lda (word),y
	sta dana4,x
	iny
	lda (word),y
	sta dana5,x

	ldy #0
	lda b1
	cmp #3
	bne l14
	ldy #2
l14:	cmp #7
	bne l15
	ldy #4
l15:	lda tab3,y
	sta word
	lda tab3+1,y
	sta word+1
	lda dana2,x
	lsr a
	lsr a
	lsr a
	lsr a
	clc
	adc b2
	sta b2
	sta zm2+1
	tay
	lda b1
	cmp #7
	bne l16
	tya
	asl a
	tay
	lda (word),y
	sta czest1,x
	iny
	sty b2
	jmp l17
l16:	lda (word),y
	sta czest1,x
	lda dana2,x
	and #$f
	clc
	adc b2
	sta b2
l17:	ldy b2
	lda b1
	cmp #5
	php
	lda (word),y
	plp
	beq l18
	cmp czest1,x
	bne l18
	sec
	sbc #1
l18:	sta czest2,x
	lda dana0,x
	pha
	and #3
	tay
	lda tab4,y
	sta zniek,x
	pla
	lsr a
	lsr a
	lsr a
	lsr a
	ldy #$3e
	cmp #$f
	beq l19
	ldy #$37
	cmp #$e
	beq l19
	ldy #$30
	cmp #$d
	beq l19
	clc
zm2:	adc #0
	tay
l19:	lda tab5,y
	sta czest3,x
	rts

;--- play

play:	lda czygr
	beq play-1
	lda czygrc
	beq g2
	jmp dal3
g2:	lda tmpo
	cmp ltemp
	beq g3
	jmp dal2
g3:	lda posptr
	beq g4
	jmp dal1
g4:	ldx #2
g5:	ldy czygrx,x
	bmi g6
	sta czygrx,x
g6:	sta poswpt,x
	dex
	bpl g5

	lda adrsng
	sta addr
	lda adrsng+1
	sta addr+1
	ldy possng
	sty word
g7:	cpy konrep
	bne g8
	lda ilrep
	beq g8
	lda possng
	ldy pocrep
	sty possng
	dec ilrep
	bne g7
	sta possng
	tay
	bpl g7
g8:	ldx #0
g9:	lda (addr),y
	cmp #$fe
	bne g10
	ldy possng
	iny
	cpy word
	beq g11
	sty possng
	jmp g7
g10:	sta numptr,x
	clc
	tya
	adc #$55
	tay
	inx
	cpx #3
	bcc g9
	ldy possng
	lda (addr),y
	bpl dal1
	cmp #$ff
	beq dal1
	lsr a
	lsr a
	lsr a
	and #$e
	tax
	lda tab2,x
	sta zm3+1
	lda tab2+1,x
	sta zm3+2
	lda numptr+1
	sta word+1
zm3:	jsr g12
	sty possng
	cpy #$55
	bcs g11
	cpy word
	bne g7
g11:	ldy word
	sty possng
	jmp up-1

g12:	ldy #$ff
	rts
jump:	bmi g12
	tay
	rts
up:	bmi g12
	sec
	tya
	sbc word+1
	tay
	rts
down:	bmi g12
	clc
	tya
	adc word+1
	tay
	rts
temp:	bmi g12
	sta tmpo
	sta ltemp
	iny
	rts
rep:	bmi g12
	lda numptr+2
	bmi g12
	sta ilrep
	iny
	sty pocrep
	clc
	tya
	adc word+1
	sta konrep
	rts
break:	dey
	bmi g13
	lda (addr),y
	cmp #$8f
	beq g13
	cmp #$ef
	bne break
g13:	iny
	rts

dal1:	ldx #2
v1:	lda ilewol,x
	beq v2
	dec ilewol,x
	bpl v7
v2:	lda czygrx,x
	bne v7
	ldy numptr,x
	cpy #$40
	bcs v7
	lda adradr
	sta addr
	lda adradr+1
	sta addr+1
	lda (addr),y
	sta word
	clc
	tya
	adc #$40
	tay
	lda (addr),y
	sta word+1
	and word
	cmp #$ff
	beq v7
v3:	ldy poswpt,x
	lda (word),y
	and #$c0
	bne v4
	lda (word),y
	and #$3f
	sta numins,x
	inc poswpt,x
	bpl v3
v4:	cmp #$40
	bne v5
	lda (word),y
	and #$3f
	sta b2
	lda numins,x
	sta b3
	jsr inst
	jmp v6
v5:	cmp #$80
	bne v7
	lda (word),y
	and #$3f
	sta ilewol,x
v6:	inc poswpt,x
v7:	dex
	bpl v1

	ldx posptr
	inx
	txa
	and #$3f
	sta posptr

dal2:	dec ltemp
	bne dal3
	lda tmpo
	sta ltemp
	lda posptr
	bne dal3
	inc possng

dal3:	ldy czest2
	lda dana1
	and #7
	cmp #5
	beq a1
	cmp #6
	bne a2
a1:	dey
a2:	sty frq+3
	ldy #0
	cmp #5
	beq a3
	cmp #6
	bne a4
a3:	ldy #2
a4:	cmp #7
	bne a5
	ldy #$28
a5:	sty audc

	ldx #2
loop:	lda dana1,x
	and #$e0
	sta znieks,x
	lda ladr,x
	sta addr
	lda hadr,x
	sta addr+1
	lda count1,x
	cmp #$ff
	beq y4
	cmp #$f
	bne y2
	lda lopad,x
	beq y4
	dec lopad,x
	lda lopad,x
	bne y4
	ldy volume,x
	beq y1
	dey
y1:	tya
	sta volume,x
	lda dana3,x
	sta lopad,x
	jmp y4
y2:	lda count1,x
	lsr a
	tay
	lda (addr),y
	bcc y3
	lsr a
	lsr a
	lsr a
	lsr a
y3:	and #$f
	sta volume,x
y4:	ldy czest1,x
	lda dana1,x
	and #7
	cmp #1
	bne y6
	dey
	tya
	iny
	cmp czest2,x
	php
	lda #1
	plp
	bne y5
	asl a
	asl a
y5:	and count2,x
	beq y6
	ldy czest2,x
	cpy #$ff
	bne y6
	lda #0
	sta volume,x
y6:	tya
	sta frq,x
	lda #1
	sta b1
	lda count1,x
	cmp #$f
	beq y9
	and #7
	tay
	lda tab9,y
	sta word
	lda count1,x
	and #8
	php
	txa
	plp
	clc
	beq y7
	adc #3
y7:	tay
	lda dana4,y
	and word
	beq y9
	lda czest3,x
	sta frq,x
	stx b1
	dex
	bpl y8
	sta frq+3
	lda #0
	sta audc
y8:	inx
	lda zniek,x
	sta znieks,x
y9:	lda count1,x
	and #$f
	cmp #$f
	beq y10
	inc count1,x
	lda count1,x
	cmp #$f
	bne y10
	lda dana3,x
	sta lopad,x
y10:	lda czygrx,x
	bpl y11
	lda volume,x
	bne y11
	lda #$40
	sta czygrx,x
y11:	inc count2,x
	ldy #0
	lda dana1,x
	lsr a
	lsr a
	lsr a
	lsr a
	bcc y12
	dey
y12:	lsr a
	bcc y13
	iny
y13:	clc
	tya
	adc czest1,x
	sta czest1,x
	lda czest2,x
	cmp #$ff
	bne y14
	ldy #0
y14:	clc
	tya
	adc czest2,x
	sta czest2,x
	dex
	bmi x1
	jmp loop

x1:	lda znieks
	sta znieks+3
	lda dana1
	and #7
	tax
	ldy #3
	lda b1
	beq x2
	ldy tab10,x
x2:	tya
	pha
	lda tab8,y
	php
	and #$7f
	tax
	tya
	and #3
	asl a
	tay
	lda frq,x
d0:	sta $d200,y
	iny
	lda volume,x
	cpx #3
	bne x3
	lda volume
x3:	ora znieks,x
	plp
	bpl d1
	lda #0
d1:	sta $d200,y
	pla
	tay
	dey
	and #3
	bne x2
	ldy #8
	lda audc
d2:	sta $d200,y
	rts

;--- tablice

tab2:	.word	g12
	.word	jump
	.word	up
	.word	down
	.word	temp
	.word	rep
	.word	break

tab3:	.word	tab5
	.word	tab6
	.word	tab7

tab4:	.byte	$80,$a0
	.byte	$20,$40

tab5:	.byte	$ff,$f1,$e4,$d7
	.byte	$cb,$c0,$b5,$aa
	.byte	$a1,$98,$8f,$87
	.byte	$7f,$78,$72,$6b
	.byte	$65,$5f,$5a,$55
	.byte	$50,$4b,$47,$43
	.byte	$3f,$3c,$38,$35
	.byte	$32,$2f,$2c,$2a
	.byte	$27,$25,$23,$21
	.byte	$1f,$1d,$1c,$1a
	.byte	$18,$17,$16,$14
	.byte	$13,$12,$11,$10
	.byte	$0f,$0e,$0d,$0c
	.byte	$0b,$0a,$09,$08
	.byte	$07,$06,$05,$04
	.byte	$03,$02,$01,$00
	.byte	$00

tab6:	.byte	$00,$00,$00,$00
	.byte	$f2,$e9,$da,$ce
	.byte	$bf,$b6,$aa,$a1
	.byte	$98,$8f,$89,$80
	.byte	$7a,$71,$6b,$65
	.byte	$5f,$00,$56,$50
	.byte	$67,$60,$5a,$55
	.byte	$51,$4c,$48,$43
	.byte	$3f,$3d,$39,$34
	.byte	$33,$39,$2d,$2a
	.byte	$28,$25,$24,$21
	.byte	$1f,$1e,$00,$00
	.byte	$0f,$0e,$0d,$0c
	.byte	$0b,$0a,$09,$08
	.byte	$07,$06,$05,$04
	.byte	$03,$02,$01,$00
	.byte	$00

tab7:	.word	$b38,$a8c,$a00,$96a
	.word	$8e8,$86a,$7ef,$780
	.word	$708,$6ae,$646,$5e6
	.word	$595,$541,$4f6,$4b0
	.word	$46e,$430,$3f6,$3bb
	.word	$384,$352,$322,$2f4
	.word	$2c8,$2a0,$27a,$255
	.word	$234,$214,$1f5,$1d8
	.word	$1bd,$1a4,$18d,$177
	.word	$160,$14e,$138,$127
	.word	$115,$106,$0f7,$0e8
	.word	$0db,$0cf,$0c3,$0b8
	.word	$0ac,$0a2,$09a,$090
	.word	$088,$07f,$078,$070
	.word	$06a,$064,$05e,$057
	.word	$052,$032,$00a

tab8:	.byte	$00,$01,$02,$83
	.byte	$00,$01,$02,$03
	.byte	$01,$00,$02,$83
	.byte	$01,$00,$02,$03
	.byte	$01,$02,$80,$03

tab9:	.byte	$80,$40,$20,$10
	.byte	$08,$04,$02,$01

tab10:	.byte	3,3,3,3
	.byte	7,$b,$f,$13

;	end
