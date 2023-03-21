/* vim:ts=4:sw=4:noet:
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

#ifndef XWIDGET_PLATFORM_H
#define XWIDGET_PLATFORM_H

#include "xputty.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
					basic X11 widgets
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

typedef struct {
  int width;
  int height;
  int x;
  int y;
  bool visible;
} Metrics_t;


//const char *widget_type_name(Widget_t *w);


bool os_get_keyboard_input(Widget_t *w, XKeyEvent *key, char *buf, size_t bufsize);

void os_free_pixmap(Widget_t *w, Pixmap pixmap);
Display *os_open_display(char *display_name);
void os_close_display(Display *dpy);
Window os_get_root_window(Widget_t *w);
void os_destroy_window(Widget_t *w);
void os_translate_coords(Widget_t *w, Window from_window, Window to_window,
                          int from_x, int from_y, int *to_x, int *to_y);
void os_get_window_metrics(Widget_t *w_, Metrics_t *metrics);
void os_get_surface_size(cairo_surface_t *surface, int *width, int *height);
void os_move_window(Display *dpy, Widget_t *w, int x, int y);
void os_resize_window(Display *dpy, Widget_t *w, int x, int y);
void os_set_widget_surface_size(Widget_t *w, int width, int height);
void os_create_main_window_and_surface(Widget_t *w, Xputty *app, Window win,
                          int x, int y, int width, int height);
void os_create_widget_window_and_surface(Widget_t *w, Xputty *app, Widget_t *parent,
                          int x, int y, int width, int height);
void os_set_title(Widget_t *w, const char *title);
void os_widget_show(Widget_t *w);
void os_widget_hide(Widget_t *w);
void os_show_tooltip(Widget_t *wid, Widget_t *w);
void os_expose_widget(Widget_t *w);
void os_widget_event_loop(void *w_, void* event, Xputty *main, void* user_data);
void os_send_configure_event(Widget_t *w,int x, int y, int width, int height);
void os_send_button_press_event(Widget_t *w);
void os_send_button_release_event(Widget_t *w);
void os_send_systray_message(Widget_t *w);
void os_adjustment_callback(void *w_, void *user_data);
void os_quit(Widget_t *w);
void os_quit_widget(Widget_t *w);

Atom os_register_widget_destroy(Widget_t * wid);
// xputty.cpp xchildlist.cpp
Atom os_register_wm_delete_window(Widget_t * wid);

#ifdef __cplusplus
}
#endif

#endif //XWIDGET_PLATFORM_H
