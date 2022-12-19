
#include <string.h>
#include <peekpoke.h>
#include <atari.h>

typedef unsigned char byte;
typedef unsigned short word;
typedef signed char sbyte;

//

void delay(byte i) {
  while (i--) {
    waitvsync();
  }
}

void pokey16_set_ctrl(char ch, byte val) {
  switch (ch) {
    case 0: POKEY_WRITE.audc2 = val; break;
    case 1: POKEY_WRITE.audc4 = val; break;
  }
}

void pokey16_set_period(char ch, word period) {
  switch (ch) {
    case 0:
      POKEY_WRITE.audf1 = period & 0xff;
      POKEY_WRITE.audf2 = period >> 8;
      break;
    case 1:
      POKEY_WRITE.audf3 = period & 0xff;
      POKEY_WRITE.audf4 = period >> 8;
      break;
  }
}

//
// MUSIC ROUTINES
//

#define NV 2

// python mknotes.py -f 894895 -b 7
const word note_table[64] = {
0, 0, 31423, 29659, 27994, 26422, 24939, 23539, 22217, 20970, 19792, 18681, 17632, 16642, 15708, 14826, 13993, 13207, 12466, 11766, 11105, 10481, 9893, 9337, 8813, 8318, 7850, 7409, 6993, 6600, 6229, 5879, 5549, 5237, 4943, 4665, 4403, 4155, 3922, 3701, 3493, 3297, 3111, 2936, 2771, 2615, 2468, 2329, 2198, 2074, 1957, 1847, 1743, 1645, 1552, 1465, 1382, 1304, 1230, 1161, 1095, 1034, 975, 920 };

struct {
  byte volume;
} voice[NV];

byte cur_duration = 0;

const byte* music_ptr = NULL;

byte next_music_byte() {
  return *music_ptr++;
}

void play_music() {
  byte ch;
  byte freech = 0;
  for (ch=0; ch<NV; ch++) {
    if (voice[ch].volume) {
      pokey16_set_ctrl(ch, 0xa0 | --voice[ch].volume);
    } else {
      freech = ch;
    }
  }
  if (music_ptr) {
    ch = freech;
    while (cur_duration == 0) {
      byte note = next_music_byte();
      if ((note & 0x80) == 0) {
        word period = note_table[note & 63];
        pokey16_set_period(ch, period);
        voice[ch].volume = 15;
        ch = ch ? ch-1 : NV-1;
      } else {
        if (note == 0xff)
          music_ptr = NULL;
        cur_duration = note & 63;
      }
    }
    cur_duration--;
  }
}

void start_music(const byte* music) {
  music_ptr = music;
  cur_duration == 0;
  POKEY_WRITE.audctl = 0b01111000;
}

#ifdef __MAIN__

const byte music1[] = {
0x35,0x8a,0x37,0x8a,0x33,0x3f,0x8a,0x30,0x3c,0x94,0x3e,0x32,0x8a,0x3a,0x2e,0x94,0x35,0x29,0x8a,0x37,0x2b,0x8a,0x33,0x27,0x8a,0x30,0x24,0x94,0x32,0x26,0x8a,0x2e,0x22,0x94,0x29,0x1d,0x8a,0x2b,0x1f,0x8a,0x27,0x1b,0x8a,0x24,0x18,0x94,0x1a,0x26,0x8a,0x18,0x24,0x8a,0x17,0x23,0x8a,0x16,0x22,0xa8,0x3a,0x35,0x32,0x94,0x29,0x26,0x22,0x8a,0x2a,0x8a,0x2b,0x1b,0x8a,0x33,0x8a,0x22,0x1f,0x27,0x8a,0x2b,0x8a,0x33,0x22,0x16,0x94,0x2b,0x27,0x25,0x8a,0x33,0x8a,0x20,0x14,0x94,0x27,0x24,0x94,0x1f,0x13,0x8a,0x3f,0x37,0x33,0x8a,0x38,0x35,0x27,0x8a,0x39,0x36,0x8a,0x3a,0x37,0x16,0x8a,0x3f,0x37,0x33,0x8a,0x35,0x38,0x22,0x8a,0x3a,0x37,0x8a,0x16,0x8a,0x3e,0x35,0x32,0x8a,0x38,0x35,0x26,0x94,0x3f,0x33,0x37,0x94,0x1f,0x22,0x27,0x94,0x27,0x22,0x1f,0x94,0x29,0x26,0x22,0x8a,0x2a,0x8a,0x2b,0x1b,0x8a,0x33,0x8a,0x1f,0x22,0x27,0x8a,0x2b,0x8a,0x33,0x22,0x16,0x94,0x2b,0x27,0x25,0x8a,0x33,0x8a,0x20,0x14,0x94,0x24,0x27,0x94,0x1f,0x13,0x94,0x3c,0x33,0x30,0x8a,0x3a,0x33,0x2e,0x8a,0x39,0x33,0x2d,0x8a,0x3c,0x30,0x8a,0x3f,0x33,0x1d,0x8a,0x37,0x8a,0x1d,0x8a,0x35,0x8a,0x3f,0x33,0x27,0x8a,0x3c,0x30,0x8a,0x35,0x38,0x26,0x94,0x22,0x16,0x94,0x24,0x18,0x94,0x29,0x26,0x1a,0x8a,0x2a,0x8a,0x2b,0x1b,0x8a,0x33,0x8a,0x27,0x22,0x1f,0x8a,0x2b,0x8a,0x33,0x16,0x22,0x94,0x2b,0x27,0x25,0x8a,0x33,0x8a,0x20,0x14,0x94,0x27,0x24,0x94,0x13,0x1f,0x8a,0x3f,0x33,0x37,0x8a,0x38,0x35,0x27,0x8a,0x39,0x36,0x8a,0x37,0x3a,0x16,0x8a,0x3f,0x37,0x33,0x8a,0x38,0x35,0x27,0x8a,0x3a,0x37,0x8a,0x16,0x8a,0x3e,0x35,0x32,0x8a,0x38,0x35,0x20,0x94,0x3f,0x37,0x33,0x94,0x27,0x22,0x1f,0x94,0x2b,0x27,0x22,0x94,0x3f,0x33,0x8a,0x35,0x8a,0x37,0x27,0x1b,0x8a,0x3f,0x33,0x8a,0x35,0x2b,0x22,0x8a,0x37,0x8a,0x25,0x19,0x8a,0x3f,0x33,0x8a,0x35,0x2b,0x27,0x8a,0x3f,0x33,0x8a,0x37,0x24,0x18,0x8a,0x3f,0x33,0x8a,0x35,0x2c,0x27,0x8a,0x37,0x8a,0x23,0x17,0x8a,0x3f,0x33,0x8a,0x35,0x2c,0x27,0x8a,0x3f,0x33,0x8a,0x3a,0x37,0x22,0x8a,0x3f,0x37,0x33,0x8a,0x38,0x35,0x2b,0x8a,0x3a,0x37,0x8a,0x16,0x8a,0x3e,0x32,0x35,0x8a,0x35,0x38,0x26,0x94,0x3f,0x33,0x37,0x94,0x22,0x16,0x94,0x24,0x18,0x94,0x29,0x26,0x1a,0x8a,0x2a,0x8a,0x2b,0x1b,0x8a,0x33,0x8a,0x27,0x22,0x1f,0x8a,0x2b,0x8a,0x33,0x22,0x16,0x94,0x2b,0x25,0x22,0x8a,0x33,0x8a,0x20,0x14,0x94,0x27,0x24,0x94,0x1f,0x13,0x8a,0x3f,0x37,0x33,0x8a,0x35,0x38,0x27,0x8a,0x39,0x36,0x8a,0x3a,0x37,0x16,0x8a,0x3f,0x37,0x33,0x8a,0x38,0x35,0x27,0x8a,0x3a,0x37,0x8a,0x16,0x8a,0x3e,0x35,0x32,0x8a,0x35,0x38,0x20,0x94,0x37,0x33,0x3f,0x94,0x27,0x22,0x1f,0x94,0x27,0x22,0x1f,0x94,0x29,0x26,0x22,0x8a,0x2a,0x8a,0x2b,0x1b,0x8a,0x33,0x8a,0x22,0x1f,0x27,0x8a,0x2b,0x8a,0x33,0x22,0x16,0x94,0x2b,0x27,0x25,0x8a,0x33,0x8a,0x20,0x14,0x94,0x27,0x24,0x94,0x1f,0x13,0x94,0x3c,0x33,0x30,0x8a,0x3a,0x33,0x2e,0x8a,0x2d,0x33,0x39,0x8a,0x3c,0x30,0x8a,0x3f,0x33,0x27,0x8a,0x37,0x8a,0x1d,0x8a,0x35,0x8a,0x33,0x3f,0x27,0x8a,0x3c,0x30,0x8a,0x38,0x35,0x26,0x94,0x22,0x16,0x94,0x24,0x18,0x94,0x29,0x26,0x1a,0x8a,0x2a,0x8a,0x2b,0x1b,0x8a,0x33,0x8a,0x22,0x27,0x1f,0x8a,0x2b,0x8a,0x33,0x22,0x16,0x94,0x2b,0x27,0x25,0x8a,0x33,0x8a,0x20,0x14,0x94,0x24,0x27,0x94,0x1f,0x13,0x8a,0x3f,0x37,0x33,0x8a,0x38,0x35,0x27,0x8a,0x36,0x39,0x8a,0x3a,0x37,0x16,0x8a,0x3f,0x37,0x33,0x8a,0x38,0x35,0x27,0x8a,0x37,0x3a,0x8a,0x16,0x8a,0x3e,0x35,0x32,0x8a,0x38,0x35,0x22,0x94,0x33,0x37,0x3f,0x94,0x27,0x22,0x1f,0x94,0x22,0x27,0x2b,0x94,0x3f,0x33,0x8a,0x35,0x8a,0x37,0x27,0x1b,0x8a,0x3f,0x33,0x8a,0x35,0x2b,0x27,0x8a,0x37,0x8a,0x25,0x19,0x8a,0x3f,0x33,0x8a,0x35,0x2b,0x22,0x8a,0x3f,0x33,0x8a,0x37,0x24,0x18,0x8a,0x33,0x3f,0x8a,0x35,0x2c,0x27,0x8a,0x37,0x8a,0x23,0x17,0x8a,0x3f,0x33,0x8a,0x35,0x2c,0x27,0x8a,0x3f,0x33,0x8a,0x3a,0x37,0x22,0x8a,0x3f,0x37,0x33,0x8a,0x38,0x35,0x2b,0x8a,0x37,0x3a,0x8a,0x16,0x8a,0x3e,0x35,0x32,0x8a,0x38,0x35,0x22,0x94,0x33,0x37,0x3f,0x94,0x22,0x16,0x94,0x1b,0x0f,0x8a,0x37,0x2b,0x33,0x8a,0x38,0x2c,0x35,0x8a,0x39,0x36,0x2d,0x8a,0x3a,0x37,0x2e,0x94,0x3c,0x37,0x30,0x8a,0x3a,0x2e,0x37,0x8a,0x16,0x8a,0x37,0x33,0x2b,0x8a,0x2c,0x38,0x35,0x8a,0x39,0x36,0x2d,0x8a,0x3a,0x37,0x2e,0x94,0x3c,0x37,0x30,0x8a,0x2e,0x37,0x3a,0x8a,0x16,0x8a,0x37,0x8a,0x33,0x27,0x2b,0x8a,0x2e,0x8a,0x30,0x14,0x8a,0x32,0x8a,0x33,0x2c,0x27,0x8a,0x35,0x8a,0x37,0x20,0x8a,0x35,0x8a,0x33,0x2c,0x27,0x8a,0x35,0x8a,0x2e,0x1f,0x8a,0x37,0x8a,0x38,0x2b,0x27,0x8a,0x3a,0x8a,0x3c,0x16,0x8a,0x3a,0x8a,0x37,0x22,0x2b,0x8a,0x38,0x8a,0x3a,0x37,0x2e,0x94,0x3c,0x30,0x37,0x8a,0x3a,0x37,0x2e,0x8a,0x16,0x8a,0x37,0x33,0x2b,0x8a,0x38,0x2c,0x35,0x8a,0x39,0x36,0x2d,0x8a,0x3a,0x37,0x2e,0x94,0x3c,0x37,0x30,0x8a,0x2e,0x37,0x3a,0x8a,0x1f,0x8a,0x3a,0x8a,0x3c,0x1e,0x8a,0x3d,0x8a,0x3e,0x3a,0x35,0x8a,0x3e,0x35,0x3a,0x8a,0x22,0x29,0x26,0x8a,0x3e,0x33,0x39,0x8a,0x1d,0x8a,0x3c,0x8a,0x39,0x33,0x29,0x8a,0x35,0x8a,0x3a,0x32,0x29,0x94,0x20,0x14,0x94,0x1f,0x13,0x8a,0x37,0x2b,0x33,0x8a,0x38,0x35,0x2c,0x8a,0x39,0x36,0x2d,0x8a,0x2e,0x3a,0x37,0x94,0x3c,0x37,0x30,0x8a,0x3a,0x37,0x2e,0x8a,0x16,0x8a,0x33,0x2b,0x37,0x8a,0x38,0x35,0x2c,0x8a,0x39,0x36,0x2d,0x8a,0x3a,0x37,0x2e,0x94,0x30,0x37,0x3c,0x8a,0x3a,0x2e,0x37,0x8a,0x16,0x8a,0x37,0x8a,0x33,0x2b,0x27,0x8a,0x2e,0x8a,0x30,0x14,0x8a,0x32,0x8a,0x33,0x2c,0x27,0x8a,0x35,0x8a,0x37,0x20,0x8a,0x35,0x8a,0x33,0x23,0x2c,0x8a,0x35,0x8a,0x33,0x1f,0x94,0x2b,0x22,0x27,0x94,0x1b,0x8a,0x2e,0x8a,0x2d,0x2b,0x27,0x8a,0x2e,0x8a,0x33,0x2c,0x27,0x94,0x30,0x2c,0x27,0x8a,0x33,0x8a,0x21,0x24,0x2a,0x8a,0x30,0x8a,0x33,0x2a,0x27,0x8a,0x30,0x8a,0x2e,0x2b,0x27,0x8a,0x33,0x8a,0x37,0x2b,0x22,0x8a,0x3a,0x8a,0x2b,0x27,0x22,0x8a,0x37,0x8a,0x33,0x2b,0x27,0x8a,0x2e,0x8a,0x30,0x2d,0x1d,0x94,0x33,0x2d,0x24,0x94,0x37,0x2c,0x26,0x8a,0x35,0x2c,0x8a,0x26,0x22,0x8a,0x33,0x2b,0x8a,0x27,0x1b,0x94,0x22,0x16,0x94,0x1f,0x13,0x8a,0x3f,0x37,0x8a,0x38,0x1d,0x11,0x8a,0x39,0x8a,0x3a,0x37,0x2e,0x94,0x3c,0x30,0x37,0x8a,0x3a,0x37,0x2e,0x8a,0x16,0x8a,0x37,0x33,0x2b,0x8a,0x38,0x2c,0x35,0x8a,0x39,0x36,0x2d,0x8a,0x3a,0x37,0x2e,0x94,0x3c,0x37,0x30,0x8a,0x2e,0x37,0x3a,0x8a,0x16,0x8a,0x37,0x8a,0x33,0x2b,0x27,0x8a,0x2e,0x8a,0x30,0x14,0x8a,0x32,0x8a,0x33,0x2c,0x27,0x8a,0x35,0x8a,0x37,0x20,0x8a,0x35,0x8a,0x33,0x23,0x27,0x8a,0x35,0x8a,0x2e,0x1f,0x8a,0x37,0x8a,0x38,0x27,0x2b,0x8a,0x3a,0x8a,0x3c,0x16,0x8a,0x3a,0x8a,0x37,0x2b,0x27,0x8a,0x38,0x8a,0x3a,0x37,0x2e,0x94,0x3c,0x30,0x37,0x8a,0x3a,0x37,0x2e,0x8a,0x16,0x8a,0x37,0x33,0x2b,0x8a,0x38,0x2c,0x35,0x8a,0x39,0x36,0x2d,0x8a,0x3a,0x37,0x2e,0x94,0x3c,0x37,0x30,0x8a,0x2e,0x37,0x3a,0x8a,0x1f,0x8a,0x3a,0x8a,0x3c,0x1e,0x8a,0x3d,0x8a,0x3e,0x3a,0x35,0x8a,0x3e,0x35,0x3a,0x8a,0x29,0x26,0x22,0x8a,0x3e,0x33,0x39,0x8a,0x1d,0x8a,0x3c,0x8a,0x39,0x33,0x27,0x8a,0x35,0x8a,0x3a,0x32,0x29,0x94,0x20,0x14,0x94,0x1f,0x13,0x8a,0x37,0x2b,0x33,0x8a,0x38,0x35,0x2c,0x8a,0x39,0x36,0x2d,0x8a,0x2e,0x3a,0x37,0x94,0x3c,0x37,0x30,0x8a,0x3a,0x37,0x2e,0x8a,0x16,0x8a,0x33,0x2b,0x37,0x8a,0x38,0x35,0x2c,0x8a,0x39,0x36,0x2d,0x8a,0x3a,0x37,0x2e,0x94,0x30,0x3c,0x37,0x8a,0x37,0x2e,0x3a,0x8a,0x16,0x8a,0x37,0x8a,0x33,0x2b,0x27,0x8a,0x2e,0x8a,0x30,0x14,0x8a,0x32,0x8a,0x33,0x27,0x24,0x8a,0x35,0x8a,0x37,0x20,0x8a,0x35,0x8a,0x33,0x2c,0x27,0x8a,0x35,0x8a,0x33,0x1f,0x94,0x2b,0x27,0x22,0x94,0x1b,0x8a,0x2e,0x8a,0x2d,0x2b,0x27,0x8a,0x2e,0x8a,0x33,0x2c,0x27,0x94,0x30,0x2c,0x20,0x8a,0x33,0x8a,0x2a,0x27,0x24,0x8a,0x30,0x8a,0x33,0x2a,0x27,0x8a,0x30,0x8a,0x2e,0x2b,0x22,0x8a,0x33,0x8a,0x37,0x2b,0x27,0x8a,0x3a,0x8a,0x2b,0x27,0x22,0x8a,0x37,0x8a,0x33,0x2b,0x22,0x8a,0x2e,0x8a,0x30,0x2d,0x27,0x94,0x33,0x2d,0x24,0x94,0x37,0x2c,0x26,0x8a,0x35,0x2c,0x8a,0x26,0x22,0x8a,0x33,0x2b,0x8a,0x27,0x1b,0x94,0x22,0x16,0x94,0x1b,0x0f,0x94,0x29,0x8a,0x2a,0x8a,0x2b,0x1b,0x8a,0x33,0x8a,0x27,0x1f,0x22,0x8a,0x2b,0x8a,0x33,0x22,0x16,0x94,0x2b,0x27,0x25,0x8a,0x33,0x8a,0x20,0x14,0x94,0x27,0x24,0x94,0x1f,0x13,0x8a,0x3f,0x37,0x33,0x8a,0x38,0x35,0x27,0x8a,0x39,0x36,0x8a,0x3a,0x37,0x16,0x8a,0x3f,0x37,0x33,0x8a,0x38,0x35,0x27,0x8a,0x3a,0x37,0x8a,0x16,0x8a,0x3e,0x35,0x32,0x8a,0x38,0x35,0x22,0x94,0x3f,0x37,0x33,0x94,0x27,0x22,0x1f,0x94,0x22,0x1f,0x27,0x94,0x29,0x26,0x22,0x8a,0x2a,0x8a,0x2b,0x1b,0x8a,0x33,0x8a,0x27,0x22,0x1f,0x8a,0x2b,0x8a,0x33,0x22,0x16,0x94,0x2b,0x27,0x22,0x8a,0x33,0x8a,0x20,0x14,0x94,0x27,0x24,0x94,0x1f,0x13,0x94,0x33,0x3c,0x30,0x8a,0x3a,0x33,0x2e,0x8a,0x39,0x33,0x2d,0x8a,0x3c,0x30,0x8a,0x3f,0x33,0x27,0x8a,0x37,0x8a,0x1d,0x8a,0x35,0x8a,0x3f,0x33,0x27,0x8a,0x3c,0x30,0x8a,0x35,0x38,0x26,0x94,0x22,0x16,0x94,0x24,0x18,0x94,0x29,0x26,0x1a,0x8a,0x2a,0x8a,0x2b,0x1b,0x8a,0x33,0x8a,0x1f,0x22,0x27,0x8a,0x2b,0x8a,0x33,0x22,0x16,0x94,0x2b,0x27,0x25,0x8a,0x33,0x8a,0x20,0x14,0x94,0x24,0x27,0x94,0x1f,0x13,0x8a,0x3f,0x37,0x33,0x8a,0x38,0x35,0x27,0x8a,0x39,0x36,0x8a,0x37,0x3a,0x16,0x8a,0x3f,0x33,0x37,0x8a,0x35,0x38,0x27,0x8a,0x3a,0x37,0x8a,0x16,0x8a,0x3e,0x35,0x32,0x8a,0x38,0x35,0x26,0x94,0x3f,0x37,0x33,0x94,0x27,0x22,0x1f,0x94,0x22,0x2b,0x27,0x94,0x3f,0x33,0x8a,0x35,0x8a,0x37,0x27,0x1b,0x8a,0x3f,0x33,0x8a,0x35,0x2b,0x27,0x8a,0x37,0x8a,0x25,0x19,0x8a,0x33,0x3f,0x8a,0x35,0x27,0x22,0x8a,0x3f,0x33,0x8a,0x37,0x24,0x18,0x8a,0x3f,0x33,0x8a,0x35,0x2c,0x27,0x8a,0x37,0x8a,0x23,0x17,0x8a,0x3f,0x33,0x8a,0x35,0x23,0x27,0x8a,0x3f,0x33,0x8a,0x3a,0x37,0x16,0x8a,0x33,0x37,0x3f,0x8a,0x38,0x35,0x2b,0x8a,0x3a,0x37,0x8a,0x16,0x8a,0x32,0x35,0x3e,0x8a,0x38,0x35,0x26,0x94,0x3f,0x37,0x33,0x94,0x22,0x16,0x94,0x3f,0x37,0x33,0xa8,0x38,0x3c,0x14,0x8a,0x3b,0x8a,0x3c,0x38,0x24,0x94,0x1b,0x94,0x3f,0x3c,0x38,0x94,0x38,0x3d,0x19,0x94,0x31,0x2c,0x29,0x8a,0x30,0x8a,0x31,0x20,0x8a,0x33,0x8a,0x35,0x2c,0x25,0x94,0x38,0x35,0x11,0x8a,0x37,0x8a,0x38,0x35,0x2c,0x94,0x18,0x94,0x3c,0x38,0x35,0x94,0x35,0x3a,0x3d,0x94,0x2e,0x25,0x29,0x8a,0x2d,0x8a,0x2e,0x1d,0x8a,0x30,0x8a,0x31,0x29,0x25,0x8a,0x3a,0x8a,0x35,0x25,0x19,0x94,0x3a,0x25,0x29,0x8a,0x35,0x8a,0x22,0x16,0x8a,0x3a,0x8a,0x35,0x23,0x17,0x94,0x33,0x24,0x18,0x94,0x24,0x27,0x2c,0x94,0x38,0x1d,0x94,0x2c,0x29,0x24,0x94,0x37,0x1f,0x8a,0x3b,0x8a,0x3e,0x26,0x29,0x8a,0x8a,0x23,0x8a,0x8a,0x3e,0x2b,0x29,0x8a,0x3f,0x8a,0x3c,0x2b,0x27,0xa8,0x3d,0x22,0x27,0x94,0x1b,0x94,0x3c,0x38,0x14,0x8a,0x3b,0x8a,0x38,0x3c,0x2c,0x94,0x1b,0x94,0x3f,0x3c,0x38,0x94,0x3d,0x38,0x19,0x94,0x31,0x2c,0x25,0x8a,0x30,0x8a,0x31,0x20,0x8a,0x33,0x8a,0x35,0x2c,0x29,0x94,0x38,0x35,0x11,0x8a,0x37,0x8a,0x38,0x35,0x2c,0x94,0x18,0x94,0x35,0x3c,0x38,0x94,0x3d,0x3a,0x35,0x94,0x2e,0x29,0x25,0x8a,0x2d,0x8a,0x2e,0x1d,0x8a,0x30,0x8a,0x31,0x29,0x25,0x8a,0x3a,0x8a,0x35,0x25,0x19,0x94,0x3a,0x29,0x25,0x8a,0x35,0x8a,0x16,0x22,0x8a,0x3a,0x8a,0x35,0x23,0x17,0x94,0x33,0x24,0x18,0x8a,0x20,0x14,0x8a,0x1f,0x13,0x8a,0x11,0x1d,0x8a,0x38,0x32,0x2f,0x9e,0x38,0x8a,0x3c,0x33,0x30,0x8a,0x3f,0x33,0x8a,0x2c,0x27,0x24,0x8a,0x3a,0x31,0x8a,0x27,0x1b,0x8a,0x33,0x8a,0x35,0x31,0x1b,0x8a,0x37,0x8a,0x38,0x30,0x20,0x94,0x32,0x8a,0x33,0x8a,0x35,0x8a,0x37,0x8a,0x38,0x8a,0x3a,0x8a,0x3c,0x38,0x14,0x8a,0x3b,0x8a,0x38,0x3c,0x2c,0x94,0x1b,0x94,0x3f,0x3c,0x38,0x94,0x3d,0x38,0x19,0x94,0x31,0x2c,0x29,0x8a,0x30,0x8a,0x31,0x20,0x8a,0x33,0x8a,0x35,0x29,0x2c,0x94,0x38,0x35,0x11,0x8a,0x37,0x8a,0x38,0x35,0x2c,0x94,0x18,0x94,0x3c,0x35,0x38,0x94,0x35,0x3d,0x3a,0x94,0x2e,0x29,0x25,0x8a,0x2d,0x8a,0x2e,0x1d,0x8a,0x30,0x8a,0x31,0x29,0x25,0x8a,0x3a,0x8a,0x35,0x25,0x19,0x94,0x3a,0x29,0x25,0x8a,0x35,0x8a,0x22,0x16,0x8a,0x3a,0x8a,0x35,0x23,0x17,0x94,0x33,0x24,0x18,0x94,0x2c,0x27,0x24,0x94,0x38,0x1d,0x94,0x2c,0x29,0x24,0x94,0x37,0x1f,0x8a,0x3b,0x8a,0x3e,0x26,0x2b,0x8a,0x8a,0x23,0x8a,0x8a,0x3e,0x2b,0x29,0x8a,0x3f,0x8a,0x3c,0x2b,0x27,0xa8,0x3d,0x2b,0x27,0x94,0x1b,0x94,0x3c,0x38,0x14,0x8a,0x3b,0x8a,0x38,0x3c,0x24,0x94,0x1b,0x94,0x3f,0x3c,0x38,0x94,0x3d,0x38,0x19,0x94,0x31,0x2c,0x29,0x8a,0x30,0x8a,0x31,0x20,0x8a,0x33,0x8a,0x35,0x2c,0x29,0x94,0x35,0x38,0x11,0x8a,0x37,0x8a,0x38,0x35,0x2c,0x94,0x18,0x94,0x3c,0x38,0x35,0x94,0x3d,0x3a,0x35,0x94,0x2e,0x25,0x29,0x8a,0x2d,0x8a,0x2e,0x1d,0x8a,0x30,0x8a,0x31,0x29,0x25,0x8a,0x3a,0x8a,0x35,0x25,0x19,0x94,0x3a,0x29,0x25,0x8a,0x35,0x8a,0x22,0x16,0x8a,0x3a,0x8a,0x35,0x23,0x17,0x94,0x33,0x24,0x18,0x8a,0x20,0x14,0x8a,0x1f,0x13,0x8a,0x1d,0x11,0x8a,0x38,0x32,0x2f,0x9e,0x38,0x8a,0x30,0x33,0x3c,0x8a,0x3f,0x33,0x8a,0x2c,0x27,0x24,0x8a,0x3a,0x31,0x8a,0x27,0x1b,0x8a,0x33,0x8a,0x35,0x31,0x1b,0x8a,0x37,0x8a,0x30,0x38,0x20,0xa8,0x3f,0x3c,0x38,0xa8,0x33,0x20,0x27,0x94,0x30,0x2c,0x27,0x8a,0x33,0x8a,0x2a,0x27,0x24,0x8a,0x30,0x8a,0x33,0x2a,0x21,0x8a,0x30,0x8a,0x2e,0x2b,0x27,0x8a,0x33,0x8a,0x37,0x22,0x27,0x8a,0x3a,0x8a,0x2b,0x27,0x22,0x8a,0x37,0x8a,0x33,0x2b,0x27,0x8a,0x2e,0x8a,0x30,0x2d,0x27,0x94,0x33,0x2d,0x24,0x94,0x37,0x2c,0x26,0x8a,0x35,0x2c,0x8a,0x26,0x22,0x8a,0x33,0x2b,0x8a,0x27,0x1b,0xa8,0x33,0x37,0x3a,0xa8,0x2c,0x29,0x14,0x94,0x2b,0x28,0x20,0x8a,0x2c,0x29,0x8a,0x18,0x8a,0x2b,0x28,0x8a,0x29,0x2c,0x24,0x94,0x14,0x8a,0x30,0x8a,0x2c,0x35,0x24,0x8a,0x30,0x8a,0x33,0x18,0x8a,0x35,0x8a,0x33,0x24,0x20,0x8a,0x30,0x8a,0x2e,0x2b,0x1b,0x94,0x2d,0x2a,0x27,0x8a,0x2e,0x2b,0x8a,0x16,0x8a,0x2d,0x2a,0x8a,0x2e,0x2b,0x1f,0x94,0x1b,0x8a,0x33,0x8a,0x37,0x2e,0x27,0x8a,0x33,0x8a,0x35,0x16,0x8a,0x37,0x8a,0x35,0x1f,0x22,0x8a,0x33,0x8a,0x35,0x32,0x16,0x94,0x34,0x31,0x26,0x8a,0x35,0x32,0x8a,0x1a,0x8a,0x34,0x31,0x8a,0x32,0x35,0x26,0x94,0x16,0x8a,0x38,0x8a,0x3c,0x32,0x22,0x8a,0x38,0x8a,0x3a,0x1d,0x8a,0x3c,0x8a,0x3a,0x26,0x22,0x8a,0x38,0x8a,0x3f,0x33,0x27,0x8a,0x3f,0x33,0x8a,0x3f,0x33,0x27,0xa8,0x3c,0x33,0x27,0x94,0x3a,0x33,0x1f,0x94,0x2e,0x2b,0x8a,0x2e,0x2b,0x8a,0x2e,0x2b,0x94,0x2b,0x2e,0x94,0x2c,0x29,0x14,0x94,0x2b,0x28,0x24,0x8a,0x2c,0x29,0x8a,0x18,0x8a,0x2b,0x28,0x8a,0x2c,0x29,0x24,0x94,0x14,0x8a,0x30,0x8a,0x35,0x2c,0x20,0x8a,0x30,0x8a,0x33,0x18,0x8a,0x35,0x8a,0x33,0x24,0x20,0x8a,0x30,0x8a,0x2e,0x2b,0x1b,0x94,0x2a,0x2d,0x27,0x8a,0x2e,0x2b,0x8a,0x16,0x8a,0x2d,0x2a,0x8a,0x2e,0x2b,0x1f,0x94,0x1b,0x8a,0x33,0x8a,0x37,0x2e,0x27,0x8a,0x33,0x8a,0x35,0x16,0x8a,0x37,0x8a,0x35,0x27,0x22,0x8a,0x33,0x8a,0x30,0x20,0x14,0x8a,0x2f,0x8a,0x30,0x1d,0x11,0x8a,0x3a,0x30,0x8a,0x1f,0x13,0x8a,0x38,0x30,0x8a,0x20,0x14,0x8a,0x33,0x30,0x8a,0x37,0x2e,0x22,0x8a,0x36,0x8a,0x37,0x22,0x2b,0x8a,0x3c,0x8a,0x2a,0x27,0x21,0x8a,0x3f,0x8a,0x3a,0x2b,0x27,0x8a,0x37,0x8a,0x33,0x2d,0x18,0x94,0x33,0x2d,0x1d,0x94,0x37,0x2c,0x32,0x8a,0x35,0x32,0x2c,0x8a,0x26,0x1a,0x8a,0x33,0x2e,0x2b,0x8a,0x27,0x1b,0x94,0x2b,0x2e,0x8a,0x2e,0x2b,0x8a,0x2e,0x2b,0x94,0x2e,0x2b,0x94,0x2c,0x29,0x14,0x94,0x2b,0x28,0x20,0x8a,0x29,0x2c,0x8a,0x18,0x8a,0x2b,0x28,0x8a,0x2c,0x29,0x24,0x94,0x14,0x8a,0x30,0x8a,0x35,0x2c,0x24,0x8a,0x30,0x8a,0x33,0x18,0x8a,0x35,0x8a,0x33,0x24,0x20,0x8a,0x30,0x8a,0x2e,0x2b,0x1b,0x94,0x2d,0x2a,0x27,0x8a,0x2e,0x2b,0x8a,0x16,0x8a,0x2d,0x2a,0x8a,0x2e,0x2b,0x1f,0x94,0x1b,0x8a,0x33,0x8a,0x37,0x2e,0x27,0x8a,0x33,0x8a,0x35,0x16,0x8a,0x37,0x8a,0x35,0x27,0x22,0x8a,0x33,0x8a,0x35,0x32,0x16,0x94,0x31,0x34,0x26,0x8a,0x35,0x32,0x8a,0x1a,0x8a,0x34,0x31,0x8a,0x35,0x32,0x26,0x94,0x16,0x8a,0x38,0x8a,0x3c,0x32,0x20,0x8a,0x38,0x8a,0x3a,0x1d,0x8a,0x3c,0x8a,0x3a,0x26,0x22,0x8a,0x38,0x8a,0x33,0x3f,0x27,0x8a,0x3f,0x33,0x8a,0x3f,0x33,0x1e,0xa8,0x3c,0x33,0x27,0x94,0x3a,0x33,0x27,0x94,0x2b,0x2e,0x8a,0x2e,0x2b,0x8a,0x2e,0x2b,0x94,0x2e,0x2b,0x94,0x2c,0x29,0x14,0x94,0x2b,0x28,0x20,0x8a,0x2c,0x29,0x8a,0x18,0x8a,0x2b,0x28,0x8a,0x2c,0x29,0x24,0x94,0x14,0x8a,0x30,0x8a,0x35,0x2c,0x24,0x8a,0x30,0x8a,0x33,0x18,0x8a,0x35,0x8a,0x33,0x24,0x20,0x8a,0x30,0x8a,0x2e,0x2b,0x1b,0x94,0x2d,0x2a,0x27,0x8a,0x2e,0x2b,0x8a,0x16,0x8a,0x2a,0x2d,0x8a,0x2e,0x2b,0x27,0x94,0x1b,0x8a,0x33,0x8a,0x37,0x2e,0x27,0x8a,0x33,0x8a,0x35,0x16,0x8a,0x37,0x8a,0x35,0x22,0x1f,0x8a,0x33,0x8a,0x30,0x20,0x14,0x8a,0x2f,0x8a,0x30,0x1d,0x11,0x8a,0x3a,0x30,0x8a,0x1f,0x13,0x8a,0x38,0x30,0x8a,0x20,0x14,0x8a,0x33,0x30,0x8a,0x37,0x2e,0x22,0x8a,0x36,0x8a,0x37,0x2b,0x27,0x8a,0x3c,0x8a,0x21,0x27,0x2a,0x8a,0x3f,0x8a,0x3a,0x2b,0x27,0x8a,0x37,0x8a,0x33,0x2d,0x24,0x94,0x33,0x2d,0x1d,0x94,0x37,0x32,0x2c,0x8a,0x35,0x2c,0x32,0x8a,0x26,0x1a,0x8a,0x33,0x2e,0x2b,0x8a,0x27,0x1b,0x94,0x22,0x16,0x94,0x3f,0x3a,0x37,0xff
};

void main() {
  music_ptr = 0;
  cur_duration = 0;
  while (1) {
    if (!music_ptr) start_music(music1);
    play_music();
    delay(1);
  }
}

#endif