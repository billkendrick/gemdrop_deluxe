/*
  Gem Drop Deluxe
  by Bill Kendrick
  bill@newbreedsoftware.com
  http://www.newbreedsoftware.com/gemdrop/

  August 17, 1997 - Sept. 24, 1997
  Ported to C (cc65): July 3, 2015 - March 2, 2021
*/

#include <atari.h>
#include <peekpoke.h>
#include <string.h> /* for memset() & bzero() */
#include "lib/sound.h"
/* FIXME #include "lib/player2.h" */

#define CHBASE_DEFAULT 0xE0 /* Location of OS ROM default character set */
#define DLI_START asm("pha"); asm("txa"); asm("pha"); asm("tya"); asm("pha");
#define DLI_END asm("pla"); asm("tay"); asm("pla"); asm("tax"); asm("pla"); asm("rti");

#pragma data-name (push,"GEMDROP_FONT")
#include "gemdrop-font.h"
#pragma data-name (pop)

#pragma data-name (push,"TITLE_FONT")
#include "title-font.h"
#pragma data-name (pop)

#pragma bss-name (push,"PMG")
unsigned char pmg[1024];
#pragma bss-name (pop)


/* GEMINC.ACT began here */

#define ATARI 0
#define SEGA 1

#define PIECE_1A 1
#define PIECE_1B 2
#define PIECE_1C 3
#define PIECE_1D 4
#define NUM_PIECE_1 (PIECE_1D - PIECE_1A + 1)

#define PIECE_BOMB     5
#define PIECE_WILDCARD 6
#define PIECE_CLOCK    7

#define PIECE_SPECIALS  PIECE_BOMB
#define NUM_SPECIALS    (PIECE_CLOCK - PIECE_BOMB + 1)

#define PIECE_2A 8
#define PIECE_2B 9
#define PIECE_2C 10
#define PIECE_2D 11
#define NUM_PIECE_2 (PIECE_2D - PIECE_2A + 1)

#define SCORE 0
#define EXPLOSION 1

#define MAX_KILLS 110

unsigned char
  Blocks[110], /* The game board */
  KillsX[MAX_KILLS], KillsY[MAX_KILLS], /* Blocks being removed during a match */
  ExY[4]; /* Y position of score/explosion PMGs (X handled via HPOSx) */
unsigned int ScrVals[15] = { /* Score per block, for matches */
 0, 0, 0, 1, 2, 5, 10, 15, 20, 25, 30, 50, 100, 100, 100
};
unsigned char
  Level, /* Current level */
  Controller, /* What kind of controller is being used */
  Gameover, /* Flag denoting game over */
  Frozen, /* Countdown timer signifying new block additions are frozen ("clock" block used) */
  Carrying, /* Which block (if any) player is carrying */
  HowMany, /* How many blocks player is carrying */
  LevelDone, /* Flag for when level is complete (reached required # of lines) */
  FLIP, /* Flag for animating Super IRG font */
  CHAddr, /* Font addr (page #) */
  TCHAddr, /* Title font addr (page #) */
  NumKills, /* Index into KillsX/Y when adding to list of blocks to remove */
  WhichPM, /* Rolling counter for deciding which PMG to assign to a score/explosion */
  ExAnim0,ExAnim1,ExAnim2,ExAnim3, /* Countdown timers for score/explosion PMGs */
  TOGL, /* Sub-counter for counting down score/explosion countdown timers */
  KillScore, /* How many points to receive for matches (see SrcVals[]) */
  Lines, /* Lines achieved this level */
  Max_Level, /* Max level reached (so you can pick from main menu in the future) */
  FirstRound, /* Flag for whether first pass of 'more blocks' occurred (reprieve at beginning of each level) */
  LinesNeeded,
  Happy, /* Whether or not player should appear 'happy' */
  ScrH,HiScrH, /* Most significant (10,000-990,000) of score & high score */
  flicker; /* Whether or not to use SuperIRG mode */
unsigned int SC; /* Used as a pointer to screen memory (via PEEKW(88)) */
void * OLDVEC; /* Pointer to old VVBLKD vector */
unsigned int
  DL, /* Used as a pointer to Display List (via PEEKW(560)) */
  Fnt, /* Font base address */
  TFnt; /* Title font base address */
unsigned int
  Scr,HiScr; /* Least significiant (0,000-9,999) of score & high Score */


/* http://www.atarimagazines.com/v3n2/assemblylanguage.html */
void my_graphics(unsigned char mode) {
  char sdev[] = {"S:"};

  /* FIXME: Why does this crash? :( */
  /*
  IOCB6.command = IOCB_OPEN;
  IOCB6.buffer  = &sdev;
  IOCB6.buflen  = (unsigned char) 2;
  IOCB6.aux1    = 12;
  IOCB6.aux2    = mode;
  CALL_CIOV6;
  */
}

/* Set things up for the program */
void Setup(void) {
 unsigned char i;

 my_graphics(2);
 SC = PEEKW(88);
 DL = PEEKW(560);
 OS.sdmctl = 0;

 Fnt=((unsigned int) &gemdrop_font);
 CHAddr=Fnt/256;

 TFnt=((unsigned int) &title_font);
 TCHAddr=TFnt/256;

 Level=1;

 /* Decide if it's likely a Sega controller is connected */
 Controller = ATARI;
 if (OS.paddl0==1) {
   Controller=SEGA;
 }

 POKEY_WRITE.skctl = SKCTL_KEYBOARD_DEBOUNCE | SKCTL_KEYBOARD_SCANNING;
 SOUND_INIT();

 bzero(pmg, 1024);
 GTIA_WRITE.gractl = GRACTL_MISSLES | GRACTL_PLAYERS;
 GTIA_WRITE.hposp0 = 0;
 GTIA_WRITE.hposp1 = 0;
 GTIA_WRITE.hposp2 = 0;
 GTIA_WRITE.hposp3 = 0;
 GTIA_WRITE.hposm0 = 0;
 GTIA_WRITE.hposm1 = 0;
 GTIA_WRITE.hposm2 = 0;
 GTIA_WRITE.hposm3 = 0;
 GTIA_WRITE.sizep0 = 0;
 GTIA_WRITE.sizep1 = 0;
 GTIA_WRITE.sizep2 = 0;
 GTIA_WRITE.sizep3 = 0;
 GTIA_WRITE.sizem = 0;
 ANTIC.pmbase = ((unsigned int) &pmg)/256;
 OS.pcolr0 = 0;
 OS.pcolr2 = 0;

 for (i = 0; i < 4; i++) {
   ExY[i] = 0;
 }

 TOGL=0;

/*
 FIXME
 Open(1,"D:GEMDROP.DAT",4,0)
 Max_Level=InputBD(1)
 HiScr=InputCD(1)
 HiScrH=InputBD(1)
 Close(1)
*/
 flicker = 1;
 Max_Level = 5;
}

#if VBI >= 1
/* VBI that animates Super IRG Font, and handles countdown timers &
   fade-out effect of score/explosion PMGs */
void VBLANKD(void) {
 if (flicker) {
   FLIP=4-FLIP;
   OS.chbas = CHAddr+FLIP;
 } else {
   OS.chbas = CHAddr+8;
 }

 TOGL=TOGL+1;
 if (TOGL==4) {
  TOGL=0;

  if (ExAnim0>0) {
   OS.pcolr0=ExAnim0;
   ExAnim0=ExAnim0-1;
  } else {
   GTIA_WRITE.hposp0=0;
  }

  if (ExAnim1>0) {
   OS.pcolr1=ExAnim1;
   ExAnim1=ExAnim1-1;
  } else {
   GTIA_WRITE.hposp1=0;
  }

  if (ExAnim2>0) {
   OS.pcolr2=ExAnim2;
   ExAnim2=ExAnim2-1;
  } else {
   GTIA_WRITE.hposp2=0;
  }

  if (ExAnim3>0) {
   OS.pcolr3=ExAnim3;
   ExAnim3=ExAnim3-1;
  } else {
   GTIA_WRITE.hposp3=0;
  }
 }

  asm("jmp (%v)", OLDVEC);
}

/* Set up and enable the VBI */
void mySETVBV(void * Addr) {
 FLIP=0;

 OS.critic = 1;
 OS.vvblkd = (void *) Addr;
 OS.critic = 0;

 ANTIC.nmien = NMIEN_VBI;
}
#endif

/* Draw a score at a given address in memory */

/* Addr = memory address to begin at (leftmost numeral) */
/* Scr = pair of bytes representing 0-9,999 points */
/* ScrH = one byte representing 0-99 */
/* (Therefore, max score could be 999,999) */
void DrawScr(unsigned int Addr, unsigned int Scr, unsigned char ScrH) {
 unsigned char V,I,J;
 unsigned int Z,SS;

 SS=ScrH;
 Z=10;

 for (I = 0; I <= 1; I++) {
  V=SS/Z;
  POKE(Addr+I,96+V);

  for (J=1; J <= V; J++) {
   SS=SS-Z;
  }

  Z=Z/10;
 }

 SS=Scr;
 Z=1000;

 for (I = 0; I <= 3; I++) {
  V=SS/Z;
  POKE(Addr+I+2,96+V);

  for (J=1; J <= V; J++) {
   SS=SS-Z;
  }

  Z=Z/10;
 }
}


/* Draw a pair of digits at an address */
/* Addr = memory address to begin at (leftmost numeral) */
/* Num = number (00-99) to display */
void Draw2Digits(unsigned int Addr, unsigned char Num) {
 unsigned char Z,N;
 unsigned int I;

 Z=10;
 N=Num;

 for (I=0; I<=1; I++) {
  POKE(Addr+I,(N/Z)+96);
  N=N-((N/Z)*Z);
  Z=Z/10;
 }
}


#define Rand(X) (POKEY_READ.random % (X))

/* Choose a random block, based on the current level */
unsigned char RandBlock(void) {
 unsigned char I;

 if (Rand(40)<1) {
  /* All levels have a slight chance of a non-grab-able block
     (clock, bomb, wildcard) */
  I=PIECE_SPECIALS+Rand(NUM_SPECIALS);
 } else {
  /* All levels include the first 4 grab-able bocks */
  I=Rand(NUM_PIECE_1)+PIECE_1A;

  if (Level>=15 && Rand(20)<10) {
   /* Levels 15-20 have 4 additional grab-able blocks */
   I=I-PIECE_1A+PIECE_2A;
  }
 }

 return (I);
}


/* Draw the game screen */
void DrawGameScreen(void) {
 unsigned char A;

 /* Set up IRG text mode */
 OS.sdmctl = 0;
 bzero((unsigned char *) SC, 960);
 POKE(DL+0,112);
 POKE(DL+1,112);
 POKE(DL+2,112);
 POKE(DL+3,68);
 POKEW(DL+4,SC);
 for (A=6; A<=28; A++) {
  POKE(DL+A,4);
 }
 POKE(DL+29,65);
 POKEW(DL+30,DL);

 /* Set font & colors */
 OS.chbas=CHAddr;

 OS.color0 = 74;
 OS.color1 = 206;
 OS.color2 = 138;
 OS.color3 = 30;

#if VBI >= 1
 /* Enable VBI for Super IRG Mode */
 OLDVEC=OS.vvblkd;
 mySETVBV((void*)VBLANKD);
#endif

 memcpy((unsigned char *) (SC+2),"\xd1\xd2\xd3\xd4\xd5",5);
 memset((unsigned char *) (SC+41),'`',7);

 memcpy((unsigned char *) (SC+31),"\xD6\xD7\xD8\xD6\xD1\xD2\xD3\xD4\xD5",9);
 memset((unsigned char *) (SC+72),'`',7);
 DrawScr(SC+72,HiScr,HiScrH);

 memcpy((unsigned char *)(SC+122),"\xD9\xD7\xDB\xD5\xD1",5);
 memcpy((unsigned char *) (SC+153),"\xD9\xD5\xDA\xD5\xD9",5);
 OS.sdmctl = DMACTL_PLAYFIELD_NORMAL | DMACTL_DMA_MISSILES | DMACTL_DMA_PLAYERS | DMACTL_DMA_FETCH;
}

/* Draw a block at the given X/Y position of the game grid */
void DrawBlock(unsigned char X, unsigned char Y) {
 unsigned int Loc,V;
 unsigned char C;

 Loc=SC+(unsigned int)Y*80+10+(X << 1);
 C=Blocks[Y*10+X];

 if (C==0) {
  POKEW(Loc,0);
  POKEW(Loc+40,0);
 } else {
  V=(C << 2)-2;
  V=V+((C << 2)-1)*256;
  POKEW(Loc,V);

  V=C << 2;
  V=V+((C << 2)+1)*256;
  POKEW(Loc+40,V);
 }
}

#pragma optimize (push, off)
void dli(void) {
  DLI_START

  if (ANTIC.vcount < 40) {
    /* Div. between "GEM DROP" & "DELUXE" */
    while (ANTIC.vcount < 38) {
    }
    ANTIC.wsync = 0;
    ANTIC.chbase = TCHAddr + 4;
  } else {
    ANTIC.chbase = CHBASE_DEFAULT;
  }

  DLI_END
}
#pragma optimize (pop)

void dli_init(void) {
  ANTIC.nmien = NMIEN_VBI;
  while (ANTIC.vcount < 124) ;
  OS.vdslst = (void *) dli;
  ANTIC.nmien = NMIEN_VBI | NMIEN_DLI;
}

void dli_clear(void) {
  ANTIC.nmien = NMIEN_VBI;
}

/* Draw title screen / menu */
unsigned char Title() {
 unsigned char A,Quit,Ok,HIGH,LOW;

 /* Set up large text mode */
 OS.sdmctl = 0;
 bzero((unsigned char *) SC, 960);
 POKE(DL+0,112);
 POKE(DL+1,112);
 POKE(DL+2,112);
 POKE(DL+3,64+7);
 POKEW(DL+4,SC);
 POKE(DL+6,7+128);
 for (A=7; A<=9; A++) {
  POKE(DL+A,7);
 }
 POKE(DL+10,7+128);
 for (A=11; A<=22; A++) {
  POKE(DL+A,6);
 }
 POKE(DL+23,65);
 POKEW(DL+24,DL);
 dli_init();
 /* FIXME: Set up display list interrupt */
 OS.sdmctl = DMACTL_PLAYFIELD_NORMAL | DMACTL_DMA_FETCH;

 /* Set font & colors */
 OS.chbas = TCHAddr;

 /* FIXME: */
 OS.color0 = 74;
 OS.color1 = 206;
 OS.color2 = 138;
 OS.color3 = 30;
 OS.color4 = 4 - flicker * 4;

 for (A = 0; A < 60; A++) {
   POKE(SC+A,A);
   POKE(SC+A+60,A);
   POKE(SC+A+120,A);
 }

/* FIXME: New style for menu */
#if 0
 /* "LEVEL: xx" */
 HIGH=Level/10;
 LOW=Level-(HIGH*10);
 memcpy((unsigned char *)Fnt+2032,(unsigned char *)Fnt+1408+(HIGH << 3),8);
 memcpy((unsigned char *)Fnt+2040,(unsigned char *)Fnt+1408+(LOW << 3),8);

 memcpy((unsigned char *) (SC+87),"HIJ""\x40""~""\x7F",6);

 /* "INPUT: x" */
 memcpy((unsigned char *) (SC+127),"KLM",3);
 POKE(SC+132,'O'-Controller);
#endif

 Quit=0;
 Ok=0;

 /* FIXME playCMC(0); */

 /* In case they were aborting game via START key, wait until they release */
 OS.ch=KEY_NONE;
 do {
 }
 while (GTIA_WRITE.consol != 7 || OS.strig0 == 0);

 /* Title screen main loop */
 do {
  if (OS.ch==KEY_ESC) {
   /* Quit game */
   Ok=1;
   Quit=1;
  }
  if (GTIA_WRITE.consol==5) {
   /* FIXME: Allow joystick input */
   /* Change level */
   Level=Level+1;
   if (Level>Max_Level) { Level=1; }

   /* FIXME: New style for menu */
   HIGH=Level/10;
   LOW=Level-(HIGH*10);
   memcpy((unsigned char *)Fnt+2032,(unsigned char *)Fnt+1408+(HIGH << 3),8);
   memcpy((unsigned char *)Fnt+2040,(unsigned char *)Fnt+1408+(LOW << 3),8);

   /* FIXME: Allow joystick input */
   OS.rtclok[2]=0;
   do {
   } while (GTIA_WRITE.consol != 7 && OS.rtclok[2] != 20);
  }
  else if (GTIA_WRITE.consol == 3) {
   /* FIXME: Allow joystick input */
   /* Change controller */
   Controller=1-Controller;
   /* FIXME: New style for menu */
   POKE(SC+132,'O'-Controller);

   /* FIXME: Allow joystick input */
   OS.rtclok[2]=0;
   do {
   } while (GTIA_WRITE.consol != 7 && OS.rtclok[2] != 20);
  } else if (OS.ch == KEY_F) {
   /* Toggle flickering */
   flicker = !flicker;
   OS.color4 = 4 - flicker * 4;
   OS.ch = KEY_NONE;
   /* FIXME: New style for menu */
  } else if (GTIA_WRITE.consol == 6 || OS.strig0 == 0) {
   Ok=1;
  }
 }
 while (!Ok);

 /* Stay here until they release START or firebutton */
 OS.ch=KEY_NONE;
 do {
 } while (GTIA_WRITE.consol != 7 || OS.strig0 == 0);

 return (Quit);
}

/* Set up a new level */
void InitLevel(void) {
 unsigned char X,Y,YY;

 /* Erase all blocks */
 for (Y=0; Y<= 10; Y++) {
  for (X=0; X<= 9; X++) {
   Blocks[Y*10+X]=0;
  }
 }

 /* Higher levels have more blocks to start */
 YY=Level;
 if (Level>14) {
  /* Starting level 15, they start higher again */
  YY=YY-9;
 }
 if (YY>9) { YY=9; }

 /* Fill the screen with random blocks */
 for (Y=0; Y <= YY; Y++) {
  for (X=0; X <= 9; X++) {
   Blocks[Y*10+X]=RandBlock();
  }
 }

 /* Draw all blocks */
 for (Y=0; Y <= YY; Y++) {
  for (X=0; X <= 9; X++) {
   DrawBlock(X,Y);
  }
 }

 LevelDone=0;

 Carrying=0;
 HowMany=0;
 Frozen=0;
 FirstRound=0;
 if (YY<6) {
  Happy=1;
 } else {
  Happy=0;
 }

 WhichPM=0;
 GTIA_WRITE.hposp0 = 0;
 GTIA_WRITE.hposp1 = 0;
 GTIA_WRITE.hposp2 = 0;
 GTIA_WRITE.hposp3 = 0;

 Lines=0;
 LinesNeeded=(Level*3)+2;

 Draw2Digits(SC+164,Lines);
 Draw2Digits(SC+204,LinesNeeded);
 Draw2Digits(SC+195,Level);
}

/* Erase the player and any surrounding blocks they're carrying */
void EraseYou(unsigned char X) {
 bzero((unsigned char *) (SC+888+(X << 1)),6);
 bzero((unsigned char *) (SC+928+(X << 1)),6);
}

/* Draw the player and any surrounding blocks they're carrying */
void DrawYou(unsigned char X) {
 unsigned int Loc,V1,V2;

 Loc=SC+890+(X << 1);

 if (HowMany<3) {
  /* Fewer than 3 blocks held, show the player */
  if (Happy) {
   POKEW(Loc,27498); /* 106,107 */
   POKEW(Loc+40,28012); /* 108,109 */
  } else {
   POKEW(Loc,30582); /* 118,119 */
   POKEW(Loc+40,31096); /* 120,121 */
  }
 }

 if (Carrying != 0) {
  /* Draw first block on the right */
  V1=(Carrying << 2)-2;
  V1=V1+((Carrying << 2)-1)*256;

  V2=Carrying*4;
  V2=V2+((Carrying << 2)+1)*256;

  POKEW(Loc+2,V1);
  POKEW(Loc+42,V2);

  if (HowMany>=2) {
   /* Draw second block on the left */
   POKEW(Loc-2,V1);
   POKEW(Loc+38,V2);

   if (HowMany>=3) {
    /* 3 or more blocks? Draw one in the middle */
    POKEW(Loc,V1);
    POKEW(Loc+40,V2);
   }
  }
 }
}

/* Track a 'killed' block during a cascade */
void AddKill(unsigned char X, unsigned char Y) {
 KillsX[NumKills]=X;
 KillsY[NumKills]=Y;

 NumKills=NumKills+1;
 if (NumKills>=MAX_KILLS) {
  NumKills=MAX_KILLS - 1;
 }

 /* FIXME: The PMG scores imply you're getting X pts per explosion,
  * but in reality, you've always only ever got just the highest;
  * there should be a KillsScr[] array, which accumulates each score
  * value (or better yet, the KillScore index into ScrVals[]),
  * and at the end when score is applied to a match, they're all
  * added to your score.  e.g., X X 10 20 20 50 would add 100 points,
  * rather than just 50 (as it did in the original version) -bjk 2015.07.10
  */
 KillScore=KillScore+1;
 if (KillScore>13) {
  KillScore=13;
 }
}

/* Round-robin assign players to the scoring/explosion effects */
void ExplodeBlock(unsigned char X,unsigned char Y,unsigned char Typ) {
 WhichPM=WhichPM+1;
 if (WhichPM==4) { WhichPM=0; }
 bzero((unsigned char *) (pmg+512+WhichPM*128+ExY[WhichPM]),8);
 POKE((unsigned char *) (53248U + WhichPM), 48 + ((X + 5) << 3));
 if (WhichPM==0) {
  ExAnim0=15;
 } else if (WhichPM==1) {
  ExAnim1=15;
 } else if (WhichPM==2) {
  ExAnim2=15;
 } else if (WhichPM==3) {
  ExAnim3=15;
 }
 ExY[WhichPM]=16+(Y << 3);

 if (Typ!=EXPLOSION) {
  memcpy((unsigned char *)pmg+512+WhichPM*128+ExY[WhichPM],(unsigned char *)Fnt+504+(KillScore << 3),8);
 } else {
  memcpy((unsigned char *)pmg+512+WhichPM*128+ExY[WhichPM],(unsigned char *)Fnt+616,8);
 }
}

/* Determine if two pieces are the same */
/* FIXME: Can this be #define'd as a macro? */
unsigned char Same(unsigned char A,unsigned char B) {
 unsigned char Match;

 Match=0;
 if (A==B /* same piece */
     || (A>=PIECE_SPECIALS && A<(PIECE_SPECIALS+NUM_SPECIALS)) /* A was any special */
     || B==PIECE_WILDCARD /* B was a wildcard special */) {
  Match=1;
 }
 return(Match);
}

/* Remove an individual block, apply specials' effects,
   and cascade on matches */
void KillBlock(unsigned char X,unsigned char Y) {
 unsigned char B,C;

 C=Blocks[Y*10+X];

 if (C!=0) {
  Blocks[Y*10+X]=0;
  DrawBlock(X,Y);
  ExplodeBlock(X,Y,SCORE);

  for (B=0; B<=100; B++) {
   SOUND(0,KillScore*20,B,10);
  }
  POKEY_WRITE.audf1=0;
  POKEY_WRITE.audc1=0;

  if (C==PIECE_BOMB) {
   /* Bomb: Blow up adjacent blocks (no cascading) */
   if (Y>0) {
    /* Blow up above */
    Blocks[(Y-1)*10+X]=0;
    DrawBlock(X,Y-1);
    ExplodeBlock(X,Y-1,EXPLOSION);
   }

   if (Y<10) {
    /* Blow up below */
    Blocks[(Y+1)*10+X]=0;
    DrawBlock(X,Y+1);
    ExplodeBlock(X,Y+1,EXPLOSION);
   }

   if (X>0) {
    /* Blow up to left */
    Blocks[Y*10+X-1]=0;
    DrawBlock(X-1,Y);
    ExplodeBlock(X-1,Y,EXPLOSION);
   }

   if (X<9) {
    /* Blow up to right */
    Blocks[Y*10+X+1]=0;
    DrawBlock(X+1,Y);
    ExplodeBlock(X+1,Y,EXPLOSION);
   }
  } else if (C==PIECE_CLOCK) {
   /* Clock: Freeze block advancement counter */
   Frozen=255;
  }

  /* Check for matches */
  if (Y>0) {
   /* Match above? */
   if (Same(Blocks[(Y-1)*10+X],C)) {
    AddKill(X,Y-1);
   }
  }

  if (Y<10) {
   /* Match below? */
   if (Same(Blocks[(Y+1)*10+X],C)) {
    AddKill(X,Y+1);
   }
  }

  if (X>0) {
   /* Match to left? */
   if (Same(Blocks[Y*10+X-1],C)) {
    AddKill(X-1,Y);
   }
  }

  if (X<9) {
   /* Match to right? */
   if (Same(Blocks[Y*10+X+1],C)) {
    AddKill(X+1,Y);
   }
  }
 }
}

/* Annoyed 'honk' effect */
void Honk(void) {
 unsigned char pause;

 SOUND(0,50,12,10);
 OS.color4=72;

 for (pause = 0; pause < 5; pause++) {
   while (ANTIC.vcount > 0) {}
 }

 OS.color4=0;
 POKEY_WRITE.audf1=0;
 POKEY_WRITE.audc1=0;
}


void Throw(unsigned char X) {
 unsigned int LastY;
 unsigned char Y,C,Last,NextLast,B,YTop,YBot,
    KX,KY,Ok,DoIt;

 if (Carrying != 0) {
  Last=0;
  NextLast=0;
  LastY=-1;
  for (Y=0; Y<=10; Y++) {
   C=Blocks[Y*10+X];
   if (C!=0) {
    NextLast=Last;
    Last=C;
    LastY=Y;
   }
  }

  Ok=0;
  if (Same(Last,Carrying)) {
   if (Same(NextLast,Carrying) || HowMany>1) {
    Ok=1;
   }
  }

  DoIt=1;

  YBot=LastY+1+HowMany-1;
  if (YBot>10) {
   DoIt=0;
   YBot=10;
   if (Ok || HowMany>2) {
    DoIt=1;
   }
  }

  if (DoIt) {
   for (Y=LastY+1; Y<=YBot; Y++) {
    Blocks[Y*10+X]=Carrying;
    DrawBlock(X,Y);
   }

   for (B=0; B<=200; B++) {
    SOUND(0,200-B,10,10);
   }
   POKEY_WRITE.audf1=0;
   POKEY_WRITE.audc1=0;

   Ok=0;

   if (Same(Last,Carrying)) {
    if (Same(NextLast,Carrying) || HowMany>1) {
     Ok=1;
    }
   }

   if (HowMany>2) {
    Ok=1;
   }

   if (Ok) {
    YTop=0;
    for (Y=0; Y<=YBot; Y++) {
     if (Blocks[Y*10+X]!=Carrying) {
      YTop=Y+1;
     }
    }

    Carrying=0;
    HowMany=0;
    EraseYou(X);
    DrawYou(X);

    NumKills=0;
    KillScore=0;
    AddKill(X,YBot);

    Lines=Lines+1;

    Draw2Digits(SC+164,Lines);

    B=0;
    do {
     KX=KillsX[B];
     KY=KillsY[B];
     KillBlock(KX,KY);

     B=B+1;
    }
    while (B<NumKills);

    Scr=Scr+ScrVals[KillScore];
    if (Scr>=10000) {
     Scr=Scr-10000;
     ScrH=ScrH+1;
    }

    DrawScr(SC+41,Scr,ScrH);

    if (Scr>=HiScr && ScrH>=HiScrH) {
     HiScr=Scr;
     HiScrH=ScrH;
     DrawScr(SC+72,Scr,ScrH);
    }
   } else {
    Carrying=0;
    HowMany=0;
    EraseYou(X);
    DrawYou(X);
   }
  } else {
   Honk();
  }
 }

 if (Lines>=LinesNeeded) {
  LevelDone=1;
 }
}

void Grab(unsigned char X) {
 unsigned char Y,C,Last,B,Ok;
 signed char LastY;

 Last=0;
 LastY=0;

 for (Y=0; Y<=10; Y++) {
  C=Blocks[Y*10+X];
  if (C!=0) {
   Last=C;
   LastY=Y;
  }
 }

 if (Last==0 || (Last>=PIECE_SPECIALS && Last<(PIECE_SPECIALS+NUM_SPECIALS))) {
  Honk();
 } else {
  if (Last!=Carrying && Carrying!=0) {
   Honk();
  } else {
   Carrying=Last;
   Blocks[LastY*10+X]=0;
   DrawBlock(X,LastY);
   HowMany=HowMany+1;

   do {
    Ok=0;
    LastY=LastY-1;
    if (LastY>=0) {
     if (Blocks[LastY*10+X]==Last) {
      Blocks[LastY*10+X]=0;
      DrawBlock(X,LastY);
      Ok=1;
      HowMany=HowMany+1;
     }
    }
   }
   while (Ok);

   for (B=0; B<=200; B++) {
    SOUND(0,B,10,10);
   }
  }
  POKEY_WRITE.audf1=0;
  POKEY_WRITE.audc1=0;
 }
}

void AddMore(void) {
 unsigned char X,Y,Y1;

 for (X=0; X<=9; X++) {
  if (Blocks[10*10+X]!=0) {
   Gameover=1;
  }
 }

 if (!Gameover) {
  for (Y=0; Y<= 9; Y++) {
   Y1=10-Y;

   for (X=0; X<= 9; X++) {
    Blocks[Y1*10+X]=Blocks[(Y1-1)*10+X];
    DrawBlock(X,Y1);
   }
  }

  for (X=0; X<= 9;X++) {
   Blocks[0*10+X]=RandBlock();
   DrawBlock(X,0);
  }
 }
}

/* GEMDROP.ACT began here */

void CheckHappy(void) {
 unsigned char X,Y;

 Happy=1;
 for (Y=8; Y<=10; Y++) {
  for (X=0; X<=9; X++) {
   if (Blocks[Y*10+X]!=0) {
    Happy=0;
   }
  }
 }
}

void LevelEndFX(unsigned char YourX) {
 unsigned char X,Y,B,Togl;
 unsigned int Loc;

 Loc=SC+890+(YourX << 1);
 Togl=0;

 for (Y=0; Y<=10; Y++) {
  for (X=0; X<=9; X++) {
   Blocks[Y*10+X]=0;
   DrawBlock(X,Y);
  }
  for (X=0; X<=9; X++) {
   if (Rand(10)<2) {
    ExplodeBlock(X,Y,EXPLOSION);
   }
  }
  for (B=0; B<=150; B++) {
   SOUND(0,B,0,15-B/10);
  }

  Togl=1-Togl;
  if (Togl==0) {
   POKEW(Loc,28526); /* 110,111 */
   POKEW(Loc+40,29040); /* 112,113 */
  } else {
   POKEW(Loc,29554); /* 114,115 */
   POKEW(Loc+40,30068); /* 116,117 */
  }
 }
}

#define CHBASE_BYTE (CHBASE_DEFAULT * 256)

void Level15FX(void) {
 unsigned char X,T;

 GTIA_WRITE.hposp0 = 0;
 GTIA_WRITE.hposp1 = 0;
 GTIA_WRITE.hposp2 = 0;
 GTIA_WRITE.hposp3 = 0;
 bzero((unsigned char *) pmg,1024);

 memcpy((unsigned char *) (pmg+512+  0+16+40),(unsigned char *) (CHBASE_BYTE+('U'-32)*8),8); /* FIXME: Warning: Constant is long */
 memcpy((unsigned char *) (pmg+512+128+16+40),(unsigned char *) (CHBASE_BYTE+('H'-32)*8),8); /* FIXME: Warning: Constant is long */
 memcpy((unsigned char *) (pmg+512+256+16+40),(unsigned char *) (CHBASE_BYTE+('O'-32)*8),8); /* FIXME: Warning: Constant is long */
 memcpy((unsigned char *) (pmg+512+384+16+40),(unsigned char *) (CHBASE_BYTE+('H'-32)*8),8); /* FIXME: Warning: Constant is long */

 T=0;
 OS.pcolr0 = 15;
 OS.pcolr1 = 15;
 OS.pcolr2 = 15;
 OS.pcolr3 = 15;
 for (X=0; X<=230; X++) {
  GTIA_WRITE.hposp0 = 250-X;
  GTIA_WRITE.hposp1 = 250-X+8;
  GTIA_WRITE.hposp2 = 250-X+24;
  GTIA_WRITE.hposp3 = 250-X+32;
  while (ANTIC.vcount > 0) {}
  SOUND(0,X,4,T);
  if ((X % 10)==0) {
   OS.color4=T;
   T=15-T;
  }
 }
 OS.color4=0;
 SOUND(0,0,0,0);
 bzero((unsigned char *) pmg,1024);
}

void Play(void) {
 unsigned char X,S,K,OX,A,B,FrozCount,
  HappyTest,Fire,SegaFire,OAct;
 unsigned int Clicks;
 unsigned int Q,Loc;

 DrawGameScreen();
 InitLevel();
 Clicks=0;
 X=5;
 Gameover=0;
 OAct=0;
 FrozCount=0;
 Scr=0;
 ScrH=0;

 DrawYou(X);

 do {
  /* Read joystick input */
  S=OS.stick0;
  Fire=OS.strig0;
  if (OS.paddl0==228 && Controller==SEGA) {
    SegaFire=0;
  } else {
    SegaFire=1;
  }

  /* Simulate key-repeat effect with joystick input */
  if (S!=15 || Fire==0 || SegaFire==0) {
   if (OAct==1 && OS.rtclok[2]<8) {
    S=15;
    Fire=1;
    SegaFire=1;
   } else {
    OS.rtclok[2]=0;
    OAct=1; /* FIXME: Note: Wow, this used to be above, so JS input was laggy in the original!?! -bjk 2015.07.04 */
   }
  } else {
   OAct=0;
   OS.rtclok[2]=10;
  }

  /* Get keyboard input */
  K=OS.ch;
  OS.ch=KEY_NONE;

  /* Map joystick input to key inputs */
  if (S==7) {
   K=KEY_ASTERISK;
  } else if (S==11) {
   K=KEY_PLUS;
  } else if (S==14 || SegaFire==0) {
   K=KEY_DASH;
  } else if (S==13 || Fire==0) {
   K=KEY_EQUALS;
  }

  OX=X;

  if (K==KEY_ESC || GTIA_WRITE.consol<7) {
   /* Abort game */
   Gameover=1;
  } else if (K==KEY_ASTERISK) {
   /* Move right */
   X=X+1;
   if (X>9) { X=0; }
  } else if (K==KEY_PLUS) {
   /* Move left */
   if (X>0) {
    X=X-1;
   } else {
    X=9;
   }
  } else if (K==KEY_DASH) {
   /* Try to throw a block */
   Throw(X);
  } else if (K==KEY_EQUALS) {
   /* Try to grab a block */
   Grab(X);
   EraseYou(X);
   DrawYou(X);
  } else if (K==KEY_SPACE) {
   /* Pause game */
   OS.color0=2;
   OS.color1=4;
   OS.color2=6;
   OS.color3=8;
   OS.ch=KEY_NONE;
   do {
     if (OS.ch == KEY_F) {
       /* Toggle flickering */
       flicker = !flicker;
       OS.ch  = KEY_NONE;
     }
   } while (OS.ch!=KEY_SPACE && OS.ch!=KEY_ESC && GTIA_WRITE.consol==7);
   if (OS.ch !=KEY_SPACE) {
    Gameover=1;
   }
   OS.ch =KEY_NONE;
   OS.color0=74;
   OS.color1=206;
   OS.color2=138;
   OS.color3=30;
  } else if (K == KEY_F) {
    /* Toggle flickering */
    flicker = !flicker;
  }

  /* Draw your player, if you moved */
  if (OX!=X) {
   EraseYou(OX);
   DrawYou(X);
  }


  if (!Frozen) {
   /* Check happiness (occasionally) */
   HappyTest=HappyTest+1;
   if (HappyTest>=100) {
    HappyTest=0;
    CheckHappy();
    DrawYou(X);
   }

   Clicks=Clicks+1;

   /* FIXME: #define these -bjk 2015.07.10 */
   if (Level<15) {
    Q=Level*125;
   } else {
    Q=(Level-10)*100;
   }

   /* Shake screen for a moment before adding new blocks */
   /* FIXME: #define the #s Clicks is compared to -bjk 2015.07.10 */
   if (Clicks>4500-Q && FirstRound==1) {
    POKE(DL+2,96+(16*(Clicks % 2)));
    SOUND(0,100,0,8*(Clicks % 2));
   }

   /* Add new blocks */
   /* FIXME: #define the #s Clicks is compared to -bjk 2015.07.10 */
   if (Clicks>4800-Q) {
    if (FirstRound) {
     SOUND(0,0,0,0);
     POKE(DL+2,112);
     Clicks=0;
     AddMore();
    } else {
     /* (But first do two counter cycles, at beginning of level) */
     FirstRound=1;
     Clicks=0;
    }
   }
  } else {
   /* Frozen! */
   Happy=1;

   FrozCount=FrozCount+1;

   if (FrozCount==60) {
    FrozCount=0;
    Frozen=Frozen-1;
    
    /* "Tick-tock" sound effect */
    if ((Frozen % 10)==0) {
     for (B=0; B<=15; B++) {
      SOUND(0,(Frozen % 20)*10+50,10,15-B);
     }
    }
   }
  }

  /* End of level? */
  if (LevelDone) {
   LevelEndFX(X);

   Level=Level+1;
   if (Level>20) {
    Level=20;
   }
   if (Level>Max_Level) {
    Max_Level=Level;
   }

   if (Level==15) {
     Level15FX();
   }

   InitLevel();
   Clicks=0;
  }
 }
 while (!Gameover);

 GTIA_WRITE.hposp0 = 0;
 GTIA_WRITE.hposp1 = 0;
 GTIA_WRITE.hposp2 = 0;
 GTIA_WRITE.hposp3 = 0;
 GTIA_WRITE.hposm0 = 0;
 GTIA_WRITE.hposm1 = 0;
 GTIA_WRITE.hposm2 = 0;
 GTIA_WRITE.hposm3 = 0;
 GTIA_WRITE.sizep0 = 0;
 GTIA_WRITE.sizep1 = 0;
 GTIA_WRITE.sizep2 = 0;
 GTIA_WRITE.sizep3 = 0;
 GTIA_WRITE.sizem = 0;

 SOUND_INIT();

 Loc=SC+880+10+(X << 1);

 POKEW(Loc,30582); /* 118,119 */
 POKEW(Loc+40,31096); /* 120,121 */

 for (A=0; A<=250; A=A+4) {
  SOUND(0,A,10,15);
  memset((unsigned char *)SC+(A/12)*40+10,0xDC,20);
  while (ANTIC.vcount > 0) {}
 }
 memset((unsigned char *)SC+(A/12)*40+10,0xDC,20);

 memcpy((unsigned char *) SC+131,"]]]\xDC]]]\xDC]]]]]\xDC]]]\xDC",18);
 memcpy((unsigned char *) SC+171,"]\x40\x40\x40]\x40]\x40]\x40]\x40]\x40]\x40\x40\x40",18);
 memcpy((unsigned char *) SC+211,"]\x40]\xDC]]]\x40]\x40]\x40]\x40]]\xDC\xDC",18);
 memcpy((unsigned char *) SC+251,"]\x40]\x40]\x40]\x40]\x40\xDC\x40]\x40]\x40\x40\xDC",18);
 memcpy((unsigned char *) SC+291,"]]]\x40]\x40]\x40]\x40\xDC\xDC]\x40]]]\xDC",18);
 memcpy((unsigned char *) SC+331,"\xDC\x40\x40\x40\xDC\x40\xDC\x40\xDC\x40\xDC\xDC\xDC\x40\xDC\x40\x40\x40",18);

 memcpy((unsigned char *) SC+413,"]]]\xDC]\xDC]\xDC]]]\xDC]]]\xDC",16);
 memcpy((unsigned char *) SC+453,"]\x40]\x40]\x40]\x40]\x40\x40\x40]\x40]\x40",16);
 memcpy((unsigned char *) SC+493,"]\x40]\x40]\x40]\x40]]\xDC\xDC]]\xDC\x40",16);
 memcpy((unsigned char *) SC+533,"]\x40]\x40\xDC]\xDC\x40]\x40\x40\xDC]\x40]\xDC",16);
 memcpy((unsigned char *) SC+573,"]]]\x40\xDC]\x40\xDC]]]\xDC]\x40]\x40",16);
 memcpy((unsigned char *) SC+613,"\xDC\x40\x40\x40\xDC\xDC\x40\xDC\xDC\x40\x40\x40\xDC\x40\xDC\x40",16);

 SOUND_INIT();

 OS.ch=KEY_NONE;
 do { }
 while (GTIA_WRITE.consol!=7 || OS.strig0==0);

 do { }
 while (OS.ch==KEY_NONE && GTIA_WRITE.consol==7 && OS.strig0);

 OS.ch=KEY_NONE;
 do { }
 while (GTIA_WRITE.consol!=7 || OS.strig0==0);

#if VBI >= 1
 mySETVBV(OLDVEC);
#endif
}


/* Main */
void main(void) {
 unsigned char Quit;

 Setup();
 Quit = 0;
 while (!Quit) {
  Quit=Title();
  if (!Quit) {
   Play();
   SOUND_INIT();
/*
   FIXME
   Open(1,"D:GEMDROP.DAT",8,0)
   PrintBDE(1,Max_Level)
   PrintCDE(1,HiScr)
   PrintBDE(1,HiScrH)
   Close(1)
*/
  }
 }
}
