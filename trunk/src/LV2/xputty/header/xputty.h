/*
 *                           0BSD 
 * 
 *                    BSD Zero Clause License
 * 
 *  Copyright (c) 2019 Hermann Meyer
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.

 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

#pragma once

#ifndef XPUTTY1_H_
#define XPUTTY1_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <limits.h>

#include <math.h>
#include <cairo.h>
#ifdef _WIN32
#include <windows.h>
#include <cairo-win32.h>
#elif __linux__
#include <cairo-xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#elif _APPLE_
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
					define debug print
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#ifndef DEBUG
#define DEBUG 0
#ifndef NDEBUG
#define NDEBUG // switch of assertion checks
#endif
#endif

/**
 * @brief debug_print         - print out state messages when compiled with
 * the -DDEBUG flag
 */

#ifdef _WIN32
#define debug_print(...) \
            { char xxdeb[1024]; snprintf(xxdeb, 1024, __VA_ARGS__); OutputDebugString(xxdeb); }
#else
#define debug_print(...) \
            ((void)((DEBUG) ? fprintf(stderr, __VA_ARGS__) : 0))
#endif


/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
				define min/max if not defined already
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

/**
 * @brief min         - set a maximal value (x) as return value
 */

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

/**
 * @brief max         - set a minimal value (x) as return value
 */

#ifndef max
#define max(x, y) ((x) < (y) ? (y) : (x))
#endif

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
					define check if char holds UTF 8 string
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

/**
 * @brief IS_UTF8         - check if a char contain UTF 8 formatted signs
 */

#define IS_UTF8(c) (((c)&0xc0)==0xc0)

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
				forward declareted structs
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#ifdef _WIN32

typedef unsigned long int XID;
typedef unsigned long int Atom;
typedef XID *Display;
//typedef XID Window;
#define Window HWND
typedef XID Pixmap;
typedef XID KeySym;
typedef void *XIC;
typedef void *XIM;

typedef struct {
  int x;
  int y;
  int keycode;
  unsigned int state;	/* key or button mask */
} XKeyEvent;
typedef struct {
  int x;
  int y;
  unsigned int state;	/* key or button mask */
  Window window;
} XMotionEvent;
typedef struct {
  int button;
  int x;
  int y;
  Window window;
} XButtonEvent;

#define Button1 1
#define Button2 2
#define Button3 3
#define Button4 4
#define Button5 5

#define ShiftMask	(1<<0)
#define Button1Mask (1<<8)

// xmidi_keyboard.cpp
#define XK_space	VK_SPACE
#define XK_0	'0'
#define XK_2	'2'
#define XK_3	'3'
#define XK_5	'5'
#define XK_6	'6'
#define XK_7	'7'
#define XK_9	'9'
#define XK_m	'm'
#define XK_q	'q'
#define XK_a	'a'
#define XK_b	'b'
#define XK_c	'c'
#define XK_d	'd'
#define XK_e	'e'
#define XK_g	'g'
#define XK_h	'h'
#define XK_i	'i'
#define XK_j	'j'
#define XK_n	'n'
#define XK_o	'o'
#define XK_p	'p'
#define XK_r	'r'
#define XK_s	's'
#define XK_t	't'
#define XK_u	'u'
#define XK_v	'v'
#define XK_w	'w'
#define XK_x	'x'
#define XK_y	'y'
#define XK_z	'z'
#define XK_comma	','
#define XK_quotedbl	'"'
#define XK_parenleft	'('
#define XK_minus	'-'
#define XK_plus		'+'
#define XK_agrave	'à'
#define XK_ccedilla	'ç'
#define XK_eacute	'é'
#define XK_egrave	'è'
#define XK_udiaeresis	'ü'
#define XK_dead_circumflex	'^'
// ??
#define XK_dead_diaeresis	'ü'
/*
#define XK_Tab		VK_TAB
#define XK_Up		VK_UP
#define XK_Right	VK_RIGHT
#define XK_Down		VK_DOWN
#define XK_Left		VK_LEFT
#define XK_Home		VK_HOME
#define XK_Insert	VK_INSERT
#define XK_End		VK_END
#define XK_BackSpace	VK_BACK
#define XK_KP_Subtract	VK_SUBTRACT
#define XK_KP_Add	VK_ADD
#define XK_KP_Up	VK_UP
#define XK_KP_Right	VK_RIGHT
#define XK_KP_Down	VK_DOWN
#define XK_KP_Left	VK_LEFT
#define XK_KP_Home	VK_HOME
#define XK_KP_Insert	VK_INSERT
#define XK_KP_End	VK_END
#define XK_KP_Enter	VK_RETURN
#define XK_Return	VK_RETURN
*/
#endif


/**
 * @brief Childlist_t       - maintain a Widget_t list of childs for a Widget_t
 */

typedef struct Childlist_t Childlist_t;

/**
 * @brief Adjustment_t       - Adjustment_t for a Widget_t
 */

typedef struct Adjustment_t Adjustment_t ;

/**
 * @brief Widget_t           - the Widget_t base struct
 */

typedef struct Widget_t Widget_t;

/**
 * @brief XColor_t           - the Widget_t Color struct
 */

typedef struct XColor_t XColor_t;

/**
 * @brief  Xputty          - the main struct.It should be declared
 * before any other call to a Xputty function.
 */

typedef struct Xputty Xputty;

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
				xputty library headers
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

// library header
#include "xwidget.h"
#include "xadjustment.h"
#include "xchildlist.h"
#include "xcolor.h"
#include "xpngloader.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Xputty             - the main struct. 
 * \n It should be declared before any other call to a Xputty function.
 * \n Xputty store a pointer in the childlist,
 *  to any Widget_t related to this instance of libxputty.
 * \n The first created Widget_t is the toplevel window.
 * \n When the toplevel window call destroy_widget(), Xputty call
 * destroy_widget() for all remaining Widget_t's in the main childlist.
 * \n So any allocated memory should be released before the
 * toplevel window finally close.
 * @param *childlist         - pointer to the main childlist
 * @param *dpy               - pointer to the display in use
 * @param run                - bool to quit the main loop
 * @param *color_scheme      - theming scheme for all Widget_t
 * @param *hold_grab         - pointer to a modal Widget_t
 */

struct Xputty{
/** pointer to the main childlist */
    Childlist_t *childlist;
/** pointer to the display in use */
    Display *dpy;
/** theming scheme for all Widget_t */
    XColor_t *color_scheme;
/** pointer to a modal Widget_t */
    Widget_t *hold_grab;
/** bool to quit the main loop */
    bool run;
/** small fontsize for all Widget_t*/
    int small_font;
/** normal fontsize  for all Widget_t*/
    int normal_font;
/** big fontsize  for all Widget_t*/
    int big_font;
    
    bool queue_event;
};

/**
 * @brief main_init         - open the Display and init the
 * main->childlist.
 * \n Set the bool run to true.
 * \n The bool run is used to terminate the main event loop.
 * \n main_init() should be called directly after the declaration of Xputty
 * before the first Widget_t get created.
 * \n Any Widget_t created afterwards will be added to the main childlist.
 * \n The main childlist is used to check if a Widget_t is valid to receive a Event.
 * \n Xputty check if a Widget_t is registered in the main childlist, and only forward
 * events when it found the Widget_t in the list.
 * \n When a Widget_t call destroy_widget() any childs of this Widget_t receive
 * a call to destroy_widget() to release there memory, they get removed from the main childlist
 * and finally the Widget_t itself will be removed from the main childlist as well.
 * On main_quit() any remaining Widget_t from the main childlist will be destroyed,
 * to ensure that we leave the memory clean.
 * @param *main             - pointer to the main Xputty struct
 * @return void
 */

void main_init(Xputty *main);

/**
 * @brief main_run          - start the main event loop.
 * \n It should be start after your Widget_t's been created.
 * \n You could create and destroy additional Widget_t's
 * at any time later during run.
 * @param *main             - pointer to the main Xputty struct
 * @return void
 */

void main_run(Xputty *main);

/**
 * @brief run_embedded      - the main event loop to run embedded UI's.
 * \n It should be start after your Widget_t's been created.
 * \n You could create and destroy additional Widget_t's
 * at any time later during run.
 * @param *main             - pointer to the main Xputty struct
 * @return void
 */

void run_embedded(Xputty *main);

/**
 * @brief main_quit         - destroy all remaining Widget_t's from the
 * main->childlist.
 * \n Free all resources which may be allocated between init
 * and quit.
 * \n It should be called after main_run()/run_embedded();
 * @param *main             - pointer to the main Xputty struct
 * @return void
 */

void main_quit(Xputty *main);

#ifdef __cplusplus
}
#endif

#endif //XPUTTY_H_

