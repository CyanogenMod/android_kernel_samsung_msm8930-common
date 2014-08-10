/* Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#ifndef _MDNIE_LITE_TUNING_DATA_H_
#define _MDNIE_LITE_TUNING_DATA_H_

static char STANDARD_UI_1[] = {
        0xB9,
        0xFF,
        0x83,
        0x89,
};

static char STANDARD_UI_2[] = {
	0xCD,
	0x5A, //password 5A
	0x00, //mask 000
	0x00, //data_width
	0x03, //scr_roi 1 scr algo_roi 1 algo 00 1 0 00 1 0
	0x00, //roi_ctrl
	0x00, //roi1 y end
	0x00, 
	0x00, //roi1 y start
	0x00,
	0x00, //roi1 x end
	0x00,
	0x00, //roi1 x start
	0x00,
	0x00, //roi0 y end
	0x00,
	0x00, //roi0 y start
	0x00,
	0x00, //roi0 x end
	0x00,
	0x00, //roi0 x start
	0x00,
	0x00, //scr Kb
	0xFF, //scr Wb
	0x00, //scr Kg
	0xFF, //scr Wg
	0x00, //scr Kr
	0xFF, //scr Wr
	0xFF, //scr Bb
	0x00, //scr Yb
	0x00, //scr Bg
	0xFF, //scr Yg
	0x00, //scr Br
	0xFF, //scr Yr
	0x00, //scr Gb
	0xFF, //scr Mb
	0xFF, //scr Gg
	0x00, //scr Mg
	0x00, //scr Gr
	0xFF, //scr Mr
	0x00, //scr Rb
	0xFF, //scr Cb
	0x00, //scr Rg
	0xFF, //scr Cg
	0xFF, //scr Rr
	0x00, //scr Cr
	0x02, //sharpen_set cc_en gamma_en 00 0 0
	0xff, //curve24 a
	0x00, //curve24 b
	0x20, //curve23 a
	0x00, //curve23 b
	0x20, //curve22 a
	0x00, //curve22 b
	0x20, //curve21 a
	0x00, //curve21 b
	0xa1, //curve20 a
	0x05, //curve20 b
	0xa2, //curve19 a
	0x09, //curve19 b
	0xa0, //curve18 a
	0x04, //curve18 b
	0xa0, //curve17 a
	0x04, //curve17 b
	0xac, //curve16 a
	0x1c, //curve16 b
	0xac, //curve15 a
	0x1c, //curve15 b
	0xa8, //curve14 a
	0x15, //curve14 b
	0xa8, //curve13 a
	0x15, //curve13 b
	0xa8, //curve12 a
	0x15, //curve12 b
	0xa0, //curve11 a
	0x0a, //curve11 b
	0xa0, //curve10 a
	0x0a, //curve10 b
	0x98, //curve 9 a
	0x01, //curve 9 b
	0x98, //curve 8 a
	0x01, //curve 8 b
	0x98, //curve 7 a
	0x01, //curve 7 b
	0x98, //curve 6 a
	0x01, //curve 6 b
	0x98, //curve 5 a
	0x01, //curve 5 b
	0x98, //curve 4 a
	0x01, //curve 4 b
	0x98, //curve 3 a
	0x01, //curve 3 b
	0x98, //curve 2 a
	0x01, //curve 2 b
	0x08, //curve 1 a
	0x00, //curve 1 b
	0x04, //cc b3 0.1
	0x5b,
	0x1f, //cc b2
	0xc4,
	0x1f, //cc b1
	0xe1,
	0x1f,//cc g3
	0xf4,
	0x04, //cc g2
	0x2b,
	0x1f, //cc g1
	0xe1,
	0x1f, //cc r3
	0xf4,
	0x1f, //cc r2
	0xc4,
	0x04, //cc r1
	0x48,
};

static char NATURAL_UI_1[] = {
        0x00,
};

static char NATURAL_UI_2[] = {
        0x00,
};

static char DYNAMIC_UI_1[] = {
        0x00,
};

static char DYNAMIC_UI_2[] = {
        0x00,
};

static char MOVIE_UI_1[] = {
        0x00,
};

static char MOVIE_UI_2[] = {
        0x00,
};

static char AUTO_UI_1[] = {
        0x00,
};

char AUTO_UI_2[] = {
        0x00,
};

static char STANDARD_GALLERY_1[] = {
        0xB9,
        0xFF,
        0x83,
        0x89,
};

static char STANDARD_GALLERY_2[] = {
	0xCD,
	0x5A,//password 5A
	0x00,//mask 000
	0x00,//data_width
	0x03,//scr_roi 1 scr algo_roi 1 algo 00 1 0 00 1 0
	0x00,//roi_ctrl
	0x00,//roi1 y end
	0x00,
	0x00,//roi1 y start
	0x00,
	0x00,//roi1 x end
	0x00,
	0x00,//roi1 x start
	0x00,
	0x00,//roi0 y end
	0x00,
	0x00,//roi0 y start
	0x00,
	0x00,//roi0 x end
	0x00,
	0x00,//roi0 x start
	0x00,
	0x00,//scr Kb
	0xFF,//scr Wb
	0x00,//scr Kg
	0xFF,//scr Wg
	0x00,//scr Kr
	0xFF,//scr Wr
	0xFF,//scr Bb
	0x00,//scr Yb
	0x00,//scr Bg
	0xFF,//scr Yg
	0x00,//scr Br
	0xFF,//scr Yr
	0x00,//scr Gb
	0xFF,//scr Mb
	0xFF,//scr Gg
	0x00,//scr Mg
	0x00,//scr Gr
	0xFF,//scr Mr
	0x00,//scr Rb
	0xFF,//scr Cb
	0x00,//scr Rg
	0xFF,//scr Cg
	0xFF,//scr Rr
	0x00,//scr Cr
	0x02,//sharpen_set cc_en gamma_en 00 0 0
	0xff,//curve24 a
	0x00,//curve24 b
	0x20,//curve23 a
	0x00,//curve23 b
	0x20,//curve22 a
	0x00,//curve22 b
	0x20,//curve21 a
	0x00,//curve21 b
	0xa1,//curve20 a
	0x05,//curve20 b
	0xa2,//curve19 a
	0x09,//curve19 b
	0xa0,//curve18 a
	0x04,//curve18 b
	0xa0,//curve17 a
	0x04,//curve17 b
	0xac,//curve16 a
	0x1c,//curve16 b
	0xac,//curve15 a
	0x1c,//curve15 b
	0xa8,//curve14 a
	0x15,//curve14 b
	0xa8,//curve13 a
	0x15,//curve13 b
	0xa8,//curve12 a
	0x15,//curve12 b
	0xa0,//curve11 a
	0x0a,//curve11 b
	0xa0,//curve10 a
	0x0a,//curve10 b
	0x98,//curve 9 a
	0x01,//curve 9 b
	0x98,//curve 8 a
	0x01,//curve 8 b
	0x98,//curve 7 a
	0x01,//curve 7 b
	0x98,//curve 6 a
	0x01,//curve 6 b
	0x98,//curve 5 a
	0x01,//curve 5 b
	0x98,//curve 4 a
	0x01,//curve 4 b
	0x98,//curve 3 a
	0x01,//curve 3 b
	0x98,//curve 2 a
	0x01,//curve 2 b
	0x08,//curve 1 a
	0x00,//curve 1 b
	0x04, //cc b3 0.1
	0x5b,
	0x1f,//cc b2
	0xc4,
	0x1f,//cc b1
	0xe1,
	0x1f,//cc g3
	0xf4,
	0x04,//cc g2
	0x2b,
	0x1f,//cc g1
	0xe1,
	0x1f,//cc r3
	0xf4,
	0x1f,//cc r2
	0xc4,
	0x04,//cc r1
	0x48,

};

static char STANDARD_VIDEO_1[] = {
        0xB9,
        0xFF,
        0x83,
        0x89,
};

static char STANDARD_VIDEO_2[] = {
	0xCD,
	0x5A, //password 5A
	0x00, //mask 000
	0x00, //data_width
	0x03, //scr_roi 1 scr algo_roi 1 algo 00 1 0 00 1 0
	0x00, //roi_ctrl
	0x00, //roi1 y end
	0x00, 
	0x00, //roi1 y start
	0x00,
	0x00, //roi1 x end
	0x00,
	0x00, //roi1 x start
	0x00,
	0x00, //roi0 y end
	0x00,
	0x00, //roi0 y start
	0x00,
	0x00, //roi0 x end
	0x00,
	0x00, //roi0 x start
	0x00,
	0x00, //scr Kb
	0xFF, //scr Wb
	0x00, //scr Kg
	0xFF, //scr Wg
	0x00, //scr Kr
	0xFF, //scr Wr
	0xFF, //scr Bb
	0x00, //scr Yb
	0x00, //scr Bg
	0xFF, //scr Yg
	0x00, //scr Br
	0xFF, //scr Yr
	0x00, //scr Gb
	0xFF, //scr Mb
	0xFF, //scr Gg
	0x00, //scr Mg
	0x00, //scr Gr
	0xFF, //scr Mr
	0x00, //scr Rb
	0xFF, //scr Cb
	0x00, //scr Rg
	0xFF, //scr Cg
	0xFF, //scr Rr
	0x00, //scr Cr
	0x06, //sharpen_set cc_en gamma_en 00 0 0
	0xff, //curve24 a
	0x00, //curve24 b
	0x20, //curve23 a
	0x00, //curve23 b
	0x20, //curve22 a
	0x00, //curve22 b
	0x20, //curve21 a
	0x00, //curve21 b
	0xa1, //curve20 a
	0x05, //curve20 b
	0xa2, //curve19 a
	0x09, //curve19 b
	0xa0, //curve18 a
	0x04, //curve18 b
	0xa0, //curve17 a
	0x04, //curve17 b
	0xac, //curve16 a
	0x1c, //curve16 b
	0xac, //curve15 a
	0x1c, //curve15 b
	0xa8, //curve14 a
	0x15, //curve14 b
	0xa8, //curve13 a
	0x15, //curve13 b
	0xa8, //curve12 a
	0x15, //curve12 b
	0xa0, //curve11 a
	0x0a, //curve11 b
	0xa0, //curve10 a
	0x0a, //curve10 b
	0x98, //curve 9 a
	0x01, //curve 9 b
	0x98, //curve 8 a
	0x01, //curve 8 b
	0x98, //curve 7 a
	0x01, //curve 7 b
	0x98, //curve 6 a
	0x01, //curve 6 b
	0x98, //curve 5 a
	0x01, //curve 5 b
	0x98, //curve 4 a
	0x01, //curve 4 b
	0x98, //curve 3 a
	0x01, //curve 3 b
	0x98, //curve 2 a
	0x01, //curve 2 b
	0x08, //curve 1 a
	0x00, //curve 1 b
	0x04, //cc b3 0.1
	0x5b,
	0x1f, //cc b2
	0xc4,
	0x1f, //cc b1
	0xe1,
	0x1f, //cc g3
	0xf4,
	0x04, //cc g2
	0x2b,
	0x1f, //cc g1
	0xe1,
	0x1f, //cc r3
	0xf4,
	0x1f, //cc r2
	0xc4,
	0x04, //cc r1
	0x48,

};

static char NATURAL_VIDEO_1[] = {
        0x00,
};

static char NATURAL_VIDEO_2[] = {
        0x00,
};

static char DYNAMIC_VIDEO_1[] = {
        0x00,
};

static char DYNAMIC_VIDEO_2[] = {
        0x00,
};

static char MOVIE_VIDEO_1[] = {
        0x00,
};

static char MOVIE_VIDEO_2[] = {
        0x00,
};

static char AUTO_VIDEO_1[] = {
        0x00,
};

char AUTO_VIDEO_2[] = {
        0x00,
};

/*temp: same like video tunning */
static char STANDARD_VT_1[] = {
        0xB9,
        0xFF,
        0x83,
        0x89,
};

static char STANDARD_VT_2[] = {
	0xCD,
	0x5A, //password 5A
	0x00, //mask 000
	0x00, //data_width
	0x03, //scr_roi 1 scr algo_roi 1 algo 00 1 0 00 1 0
	0x00, //roi_ctrl
	0x00, //roi1 y end
	0x00, 
	0x00, //roi1 y start
	0x00,
	0x00, //roi1 x end
	0x00,
	0x00, //roi1 x start
	0x00,
	0x00, //roi0 y end
	0x00,
	0x00, //roi0 y start
	0x00,
	0x00, //roi0 x end
	0x00,
	0x00, //roi0 x start
	0x00,
	0x00, //scr Kb
	0xFF, //scr Wb
	0x00, //scr Kg
	0xFF, //scr Wg
	0x00, //scr Kr
	0xFF, //scr Wr
	0xFF, //scr Bb
	0x00, //scr Yb
	0x00, //scr Bg
	0xFF, //scr Yg
	0x00, //scr Br
	0xFF, //scr Yr
	0x00, //scr Gb
	0xFF, //scr Mb
	0xFF, //scr Gg
	0x00, //scr Mg
	0x00, //scr Gr
	0xFF, //scr Mr
	0x00, //scr Rb
	0xFF, //scr Cb
	0x00, //scr Rg
	0xFF, //scr Cg
	0xFF, //scr Rr
	0x00, //scr Cr
	0x06, //sharpen_set cc_en gamma_en 00 0 0
	0x20, //curve24 a
	0x00, //curve24 b
	0x20, //curve23 a
	0x00, //curve23 b
	0x20, //curve22 a
	0x00, //curve22 b
	0x20, //curve21 a
	0x00, //curve21 b
	0x20, //curve20 a
	0x00, //curve20 b
	0x20, //curve19 a
	0x00, //curve19 b
	0x20, //curve18 a
	0x00, //curve18 b
	0x20, //curve17 a
	0x00, //curve17 b
	0x20, //curve16 a
	0x00, //curve16 b
	0x20, //curve15 a
	0x00, //curve15 b
	0x20, //curve14 a
	0x00, //curve14 b
	0x20, //curve13 a
	0x00, //curve13 b
	0x20, //curve12 a
	0x00, //curve12 b
	0x20, //curve11 a
	0x00, //curve11 b
	0x20, //curve10 a
	0x00, //curve10 b
	0x20, //curve 9 a
	0x00, //curve 9 b
	0x20, //curve 8 a
	0x00, //curve 8 b
	0x20, //curve 7 a
	0x00, //curve 7 b
	0x20, //curve 6 a
	0x00, //curve 6 b
	0x20, //curve 5 a
	0x00, //curve 5 b
	0x20, //curve 4 a
	0x00, //curve 4 b
	0x20, //curve 3 a
	0x00, //curve 3 b
	0x20, //curve 2 a
	0x00, //curve 2 b
	0x20, //curve 1 a
	0x00, //curve 1 b
	0x05, //cc b3 0.3
	0x10,
	0x1f, //cc b2
	0x4c,
	0x1f, //cc b1
	0xa4,
	0x1f, //cc g3
	0xdd,
	0x04, //cc g2
	0x7f,
	0x1f, //cc g1
	0xa4,
	0x1f, //cc r3
	0xdd,
	0x1f, //cc r2
	0x4c,
	0x04, //cc r1
	0xd7,

};

static char NATURAL_VT_1[] = {
        0x00,
};

static char NATURAL_VT_2[] = {
        0x00,
};

static char DYNAMIC_VT_1[] = {
        0x00,
};

static char DYNAMIC_VT_2[] = {
        0x00,
};

static char MOVIE_VT_1[] = {
        0x00,
};

static char MOVIE_VT_2[] = {
        0x00,
};

static char AUTO_VT_1[] = {
        0x00,
};

char AUTO_VT_2[] = {
        0x00,
};

static char CAMERA_1[] = {
        0xB9,
        0xFF,
        0x83,
        0x89,
};

static char CAMERA_2[] = {
	0xCD,
	0x5A, //password 5A
	0x00, //mask 000
	0x00, //data_width
	0x03, //scr_roi 1 scr algo_roi 1 algo 00 1 0 00 1 0
	0x00, //roi_ctrl
	0x00, //roi1 y end
	0x00, 
	0x00, //roi1 y start
	0x00,
	0x00, //roi1 x end
	0x00,
	0x00, //roi1 x start
	0x00,
	0x00, //roi0 y end
	0x00,
	0x00, //roi0 y start
	0x00,
	0x00, //roi0 x end
	0x00,
	0x00, //roi0 x start
	0x00,
	0x00, //scr Kb
	0xFF, //scr Wb
	0x00, //scr Kg
	0xFF, //scr Wg
	0x00, //scr Kr
	0xFF, //scr Wr
	0xFF, //scr Bb
	0x00, //scr Yb
	0x00, //scr Bg
	0xFF, //scr Yg
	0x00, //scr Br
	0xFF, //scr Yr
	0x00, //scr Gb
	0xFF, //scr Mb
	0xFF, //scr Gg
	0x00, //scr Mg
	0x00, //scr Gr
	0xFF, //scr Mr
	0x00, //scr Rb
	0xFF, //scr Cb
	0x00, //scr Rg
	0xFF, //scr Cg
	0xFF, //scr Rr
	0x00, //scr Cr
	0x06, //sharpen_set cc_en gamma_en 00 0 0
	0xff, //curve24 a
	0x00, //curve24 b
	0x20, //curve23 a
	0x00, //curve23 b
	0x20, //curve22 a
	0x00, //curve22 b
	0x20, //curve21 a
	0x00, //curve21 b
	0xa1, //curve20 a
	0x05, //curve20 b
	0xa2, //curve19 a
	0x09, //curve19 b
	0xa0, //curve18 a
	0x04, //curve18 b
	0xa0, //curve17 a
	0x04, //curve17 b
	0xac, //curve16 a
	0x1c, //curve16 b
	0xac, //curve15 a
	0x1c, //curve15 b
	0xa8, //curve14 a
	0x15, //curve14 b
	0xa8, //curve13 a
	0x15, //curve13 b
	0xa8, //curve12 a
	0x15, //curve12 b
	0xa0, //curve11 a
	0x0a, //curve11 b
	0xa0, //curve10 a
	0x0a, //curve10 b
	0x98, //curve 9 a
	0x01, //curve 9 b
	0x98, //curve 8 a
	0x01, //curve 8 b
	0x98, //curve 7 a
	0x01, //curve 7 b
	0x98, //curve 6 a
	0x01, //curve 6 b
	0x98, //curve 5 a
	0x01, //curve 5 b
	0x98, //curve 4 a
	0x01, //curve 4 b
	0x98, //curve 3 a
	0x01, //curve 3 b
	0x98, //curve 2 a
	0x01, //curve 2 b
	0x08, //curve 1 a
	0x00, //curve 1 b
	0x04, //cc b3 0.1
	0x5b,
	0x1f, //cc b2
	0xc4,
	0x1f, //cc b1
	0xe1,
	0x1f, //cc g3
	0xf4,
	0x04, //cc g2
	0x2b,
	0x1f, //cc g1
	0xe1,
	0x1f, //cc r3
	0xf4,
	0x1f, //cc r2
	0xc4,
	0x04, //cc r1
	0x48,

};

static char AUTO_CAMERA_1[] = {
        0x00,
};
char AUTO_CAMERA_2[] = {
        0x00,
};

static char CAMERA_OUTDOOR_1[] = {
        0x00,
};

static char CAMERA_OUTDOOR_2[] = {
        0x00,
};

static char COLD_1[] = {
        0x00,
};

static char COLD_2[] = {
        0x00,
};

static char COLD_OUTDOOR_1[] = {
        0x00,
};

static char COLD_OUTDOOR_2[] = {
        0x00,
};

static char WARM_1[] = {
        0x00,
};

static char WARM_2[] = {
        0x00,
};

static char WARM_OUTDOOR_1[] = {
        0x00,
};

static char WARM_OUTDOOR_2[] = {
        0x00,
};

static char NEGATIVE_1[] = {
	0xB9,
	0xFF,
	0x83,
	0x89,
};

static char NEGATIVE_2[] = {
	0xCD,
	0x5A, //password 5A
	0x00, //mask 000
	0x00, //data_width
	0x30, //scr_roi 1 scr algo_roi 1 algo 00 1 0 00 1 0
	0x00, //roi_ctrl
	0x00, //roi1 y end
	0x00, 
	0x00, //roi1 y start
	0x00,
	0x00, //roi1 x end
	0x00,
	0x00, //roi1 x start
	0x00,
	0x00, //roi0 y end
	0x00,
	0x00, //roi0 y start
	0x00,
	0x00, //roi0 x end
	0x00,
	0x00, //roi0 x start
	0x00,
	0xff, //scr Kb
	0x00, //scr Wb
	0xff, //scr Kg
	0x00, //scr Wg
	0xff, //scr Kr
	0x00, //scr Wr
	0x00, //scr Bb
	0xff, //scr Yb
	0xff, //scr Bg
	0x00, //scr Yg
	0xff, //scr Br
	0x00, //scr Yr
	0xff, //scr Gb
	0x00, //scr Mb
	0x00, //scr Gg
	0xff, //scr Mg
	0xff, //scr Gr
	0x00, //scr Mr
	0xff, //scr Rb
	0x00, //scr Cb
	0xff, //scr Rg
	0x00, //scr Cg
	0x00, //scr Rr
	0xff, //scr Cr
	0x00, //sharpen_set cc_en gamma_en 00 0 0
	0x20, //curve24 a
	0x00, //curve24 b
	0x20, //curve23 a
	0x00, //curve23 b
	0x20, //curve22 a
	0x00, //curve22 b
	0x20, //curve21 a
	0x00, //curve21 b
	0x20, //curve20 a
	0x00, //curve20 b
	0x20, //curve19 a
	0x00, //curve19 b
	0x20, //curve18 a
	0x00, //curve18 b
	0x20, //curve17 a
	0x00, //curve17 b
	0x20, //curve16 a
	0x00, //curve16 b
	0x20, //curve15 a
	0x00, //curve15 b
	0x20, //curve14 a
	0x00, //curve14 b
	0x20, //curve13 a
	0x00, //curve13 b
	0x20, //curve12 a
	0x00, //curve12 b
	0x20, //curve11 a
	0x00, //curve11 b
	0x20, //curve10 a
	0x00, //curve10 b
	0x20, //curve 9 a
	0x00, //curve 9 b
	0x20, //curve 8 a
	0x00, //curve 8 b
	0x20, //curve 7 a
	0x00, //curve 7 b
	0x20, //curve 6 a
	0x00, //curve 6 b
	0x20, //curve 5 a
	0x00, //curve 5 b
	0x20, //curve 4 a
	0x00, //curve 4 b
	0x20, //curve 3 a
	0x00, //curve 3 b
	0x20, //curve 2 a
	0x00, //curve 2 b
	0x20, //curve 1 a
	0x00, //curve 1 b
	0x04, //cc b3
	0x00,
	0x00, //cc b2
	0x00,
	0x00, //cc b1
	0x00,
	0x00, //cc g3
	0x00,
	0x04, //cc g2
	0x00,
	0x00, //cc g1
	0x00,
	0x00, //cc r3
	0x00,
	0x00, //cc r2
	0x00,
	0x04, //cc r1
	0x00,

};

static char OUTDOOR_VIDEO_1[] = {
        0x00,
};
static char OUTDOOR_VIDEO_2[] = {
        0x00,
};

/*temp: apply bypass*/
static char COLOR_BLIND_1[] = {
        0x00,
};

char COLOR_BLIND_2[] = {
	0xCD,
	0x5A, //password 5A
	0x00, //mask 000
	0x00, //data_width
	0x30, //scr_roi 1 scr algo_roi 1 algo 00 1 0 00 1 0
	0x00, //roi_ctrl
	0x00, //roi1 y end
	0x00, 
	0x00, //roi1 y start
	0x00,
	0x00, //roi1 x end
	0x00,
	0x00, //roi1 x start
	0x00,
	0x00, //roi0 y end
	0x00,
	0x00, //roi0 y start
	0x00,
	0x00, //roi0 x end
	0x00,
	0x00, //roi0 x start
	0x00,
	0x00, //scr Kb
	0xff, //scr Wb
	0x00, //scr Kg
	0xff, //scr Wg
	0x00, //scr Kr
	0xff, //scr Wr
	0xff, //scr Bb
	0x00, //scr Yb
	0x00, //scr Bg
	0xff, //scr Yg
	0x00, //scr Br
	0xff, //scr Yr
	0x00, //scr Gb
	0xff, //scr Mb
	0xff, //scr Gg
	0x00, //scr Mg
	0x00, //scr Gr
	0xff, //scr Mr
	0x00, //scr Rb
	0xff, //scr Cb
	0x00, //scr Rg
	0xff, //scr Cg
	0xff, //scr Rr
	0x00, //scr Cr
	0x00, //sharpen_set cc_en gamma_en 00 0 0
	0x20, //curve24 a
	0x00, //curve24 b
	0x20, //curve23 a
	0x00, //curve23 b
	0x20, //curve22 a
	0x00, //curve22 b
	0x20, //curve21 a
	0x00, //curve21 b
	0x20, //curve20 a
	0x00, //curve20 b
	0x20, //curve19 a
	0x00, //curve19 b
	0x20, //curve18 a
	0x00, //curve18 b
	0x20, //curve17 a
	0x00, //curve17 b
	0x20, //curve16 a
	0x00, //curve16 b
	0x20, //curve15 a
	0x00, //curve15 b
	0x20, //curve14 a
	0x00, //curve14 b
	0x20, //curve13 a
	0x00, //curve13 b
	0x20, //curve12 a
	0x00, //curve12 b
	0x20, //curve11 a
	0x00, //curve11 b
	0x20, //curve10 a
	0x00, //curve10 b
	0x20, //curve 9 a
	0x00, //curve 9 b
	0x20, //curve 8 a
	0x00, //curve 8 b
	0x20, //curve 7 a
	0x00, //curve 7 b
	0x20, //curve 6 a
	0x00, //curve 6 b
	0x20, //curve 5 a
	0x00, //curve 5 b
	0x20, //curve 4 a
	0x00, //curve 4 b
	0x20, //curve 3 a
	0x00, //curve 3 b
	0x20, //curve 2 a
	0x00, //curve 2 b
	0x20, //curve 1 a
	0x00, //curve 1 b
	0x04, //cc b3
	0x00,
	0x00, //cc b2
	0x00,
	0x00, //cc b1
	0x00,
	0x00, //cc g3
	0x00,
	0x04, //cc g2
	0x00,
	0x00, //cc g1
	0x00,
	0x00, //cc r3
	0x00,
	0x00, //cc r2
	0x00,
	0x04, //cc r1
	0x00,
};

/*temp: apply bypass*/
static char STANDARD_BROWSER_1[] = {
        0xB9,
        0xFF,
        0x83,
        0x89,
};

char STANDARD_BROWSER_2[] = {
	0xCD,
	0x5A, //password 5A
	0x00, //mask 000
	0x00, //data_width
	0x00, //scr_roi 1 scr algo_roi 1 algo 00 1 0 00 1 0
	0x00, //roi_ctrl
	0x00, //roi1 y end
	0x00, 
	0x00, //roi1 y start
	0x00,
	0x00, //roi1 x end
	0x00,
	0x00, //roi1 x start
	0x00,
	0x00, //roi0 y end
	0x00,
	0x00, //roi0 y start
	0x00,
	0x00, //roi0 x end
	0x00,
	0x00, //roi0 x start
	0x00,
	0x00, //scr Kb
	0xFF, //scr Wb
	0x00, //scr Kg
	0xFF, //scr Wg
	0x00, //scr Kr
	0xFF, //scr Wr
	0xFF, //scr Bb
	0x00, //scr Yb
	0x00, //scr Bg
	0xFF, //scr Yg
	0x00, //scr Br
	0xFF, //scr Yr
	0x00, //scr Gb
	0xFF, //scr Mb
	0xFF, //scr Gg
	0x00, //scr Mg
	0x00, //scr Gr
	0xFF, //scr Mr
	0x00, //scr Rb
	0xFF, //scr Cb
	0x00, //scr Rg
	0xFF, //scr Cg
	0xFF, //scr Rr
	0x00, //scr Cr
	0x00, //sharpen_set cc_en gamma_en 00 0 0
	0x20, //curve24 a
	0x00, //curve24 b
	0x20, //curve23 a
	0x00, //curve23 b
	0x20, //curve22 a
	0x00, //curve22 b
	0x20, //curve21 a
	0x00, //curve21 b
	0x20, //curve20 a
	0x00, //curve20 b
	0x20, //curve19 a
	0x00, //curve19 b
	0x20, //curve18 a
	0x00, //curve18 b
	0x20, //curve17 a
	0x00, //curve17 b
	0x20, //curve16 a
	0x00, //curve16 b
	0x20, //curve15 a
	0x00, //curve15 b
	0x20, //curve14 a
	0x00, //curve14 b
	0x20, //curve13 a
	0x00, //curve13 b
	0x20, //curve12 a
	0x00, //curve12 b
	0x20, //curve11 a
	0x00, //curve11 b
	0x20, //curve10 a
	0x00, //curve10 b
	0x20, //curve 9 a
	0x00, //curve 9 b
	0x20, //curve 8 a
	0x00, //curve 8 b
	0x20, //curve 7 a
	0x00, //curve 7 b
	0x20, //curve 6 a
	0x00, //curve 6 b
	0x20, //curve 5 a
	0x00, //curve 5 b
	0x20, //curve 4 a
	0x00, //curve 4 b
	0x20, //curve 3 a
	0x00, //curve 3 b
	0x20, //curve 2 a
	0x00, //curve 2 b
	0x20, //curve 1 a
	0x00, //curve 1 b
	0x04, //cc b3
	0x00,
	0x00, //cc b2
	0x00,
	0x00, //cc b1
	0x00,
	0x00, //cc g3
	0x00,
	0x04, //cc g2
	0x00,
	0x00, //cc g1
	0x00,
	0x00, //cc r3
	0x00,
	0x00, //cc r2
	0x00,
	0x04, //cc r1
	0x00,
};

/*temp: apply bypass*/
static char NATURAL_BROWSER_1[] = {
        0x00,
};

char NATURAL_BROWSER_2[] = {
        0x00,
};

/*temp: apply bypass*/
static char DYNAMIC_BROWSER_1[] = {
        0x00,
};

char DYNAMIC_BROWSER_2[] = {
        0x00,
};

/*temp: apply bypass*/
static char MOVIE_BROWSER_1[] = {
        0x00,
};

char MOVIE_BROWSER_2[] = {
        0x00,
};

/*temp: apply bypass*/
static char AUTO_BROWSER_1[] = {
        0x00,
};

char AUTO_BROWSER_2[] = {
        0x00,
};

/*temp: apply bypass*/
char eBOOK_2[] = {
        0x00,
};

/*temp: apply bypass*/
char AUTO_EBOOK_2[] = {
        0x00,
};
#endif

