#ifndef MAIN_H
#define MAIN_H

#define VERSION "Version 1.1"

/* colors foreground and background for day and night mode */
#define RGB(R,G,B) (((((R)<<8)| G)<<8) | B)
#define GET_R(C) (((C)>>16)&0xFF)
#define GET_G(C) (((C)>>8)&0xFF)
#define GET_B(C) ((C)&0xFF)
#define FG_DAY RGB(0x00,0x00,0x00) /* black */
#define BG_DAY RGB(0xFF,0xFF,0xFF) /* white */
#define FG_NIGHT RGB(0xFF,0xFF,0xE8) /* warm white */
#define BG_NIGHT RGB(0x00,0x00,0x08) /* dark blue */

#define BF_ON_DAY RGB(0xD0,0xD0,0xD0) /* lighter grey */
#define BF_OFF_DAY RGB(0xA0,0xA0,0xA0) /* light grey */
#define BF_ON_NIGHT RGB(0x90,0x90, 0x90) /* medium grey */
#define BF_OFF_NIGHT RGB(0x60,0x60, 0x60) /* dark grey */


#endif /*MAIN_H*/ 
