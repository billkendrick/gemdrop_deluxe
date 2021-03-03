/* http://www.atariarchives.org/dere/chapt07.php */

#include "sound.h"

#include <atari.h>
//#define POKEY_READ  (*(struct __x_pokey_read*)0xD200)
//#define POKEY_WRITE (*(struct __x_pokey_write*)0xD200)

unsigned char AUDCTL;

void SOUND_INIT(void) {
  AUDCTL = 0;
  POKEY_WRITE.audctl = 0;

  POKEY_WRITE.audf1 = 0;
  POKEY_WRITE.audc1 = 0;
  POKEY_WRITE.audf2 = 0;
  POKEY_WRITE.audc2 = 0;
  POKEY_WRITE.audf3 = 0;
  POKEY_WRITE.audc3 = 0;
  POKEY_WRITE.audf4 = 0;
  POKEY_WRITE.audc4 = 0;

  POKEY_WRITE.skctl = SKCTL_KEYBOARD_DEBOUNCE | SKCTL_KEYBOARD_SCANNING;
}

void SOUND(unsigned char chan, unsigned char pitch,
           unsigned char distortion, unsigned char volume) {
  unsigned char audc;

  audc = ((distortion & 14) << 4) | (volume & 15);

  switch (chan) {
    case 0:
      AUDCTL = AUDCTL & 0xAF; /* disable bits 4 & 6 */
      POKEY_WRITE.audc1 = audc;
      POKEY_WRITE.audf1 = pitch;
      break;
    case 1:
      AUDCTL = AUDCTL & 0xAF; /* disable bits 4 & 6 */
      POKEY_WRITE.audc2 = audc;
      POKEY_WRITE.audf2 = pitch;
      break;
    case 2:
      AUDCTL = AUDCTL & 0xD7; /* disable bits 3 & 5 */
      POKEY_WRITE.audc3 = audc;
      POKEY_WRITE.audf3 = pitch;
      break;
    case 3:
      AUDCTL = AUDCTL & 0xD7; /* disable bist 3 & 5 */
      POKEY_WRITE.audc4 = audc;
      POKEY_WRITE.audf4 = pitch;
      break;
    default:
      break;
  }
  POKEY_WRITE.audctl = AUDCTL;
}

void DSOUND(unsigned char chan, unsigned int pitch,
           unsigned char distortion, unsigned char volume) {
  unsigned char audca, audcb;

  audca = ((distortion & 14) << 4);
  audcb = audca | (volume & 15);

  switch (chan) {
    case 0:
      AUDCTL = AUDCTL | 0x50; /* enable bits 4 & 6 */
      POKEY_WRITE.audc1 = audca;
      POKEY_WRITE.audc2 = audcb;
      POKEY_WRITE.audf1 = pitch & 0xFF;
      POKEY_WRITE.audf2 = pitch >> 8;
      break;
    case 1:
      AUDCTL = AUDCTL | 0x28; /* enable bit 3 & 5 */
      POKEY_WRITE.audc3 = audca;
      POKEY_WRITE.audc4 = audcb;
      POKEY_WRITE.audf3 = pitch & 0xFF;
      POKEY_WRITE.audf4 = pitch >> 8;
      break;
    default:
      break;
  }
  POKEY_WRITE.audctl = AUDCTL;
}
