/* This file is part of HINT
 * Copyright 2017-2021 Martin Ruckert, Hochschule Muenchen, Lothstrasse 64, 80336 Muenchen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 * OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the copyright holders shall
 * not be used in advertising or otherwise to promote the sale, use or other
 * dealings in this Software without prior written authorization from the
 * copyright holders.
 */
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
