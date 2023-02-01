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

#ifdef _WIN32

#ifdef __cplusplus
extern "C" {
#endif

#include "xwidget.h"
#include "xwidget_private.h"

void os_destroy_window(Widget_t *w) {
  // STUB
}

void os_get_window_size(Widget_t *w_, int *x, int *y, int *width, int *height) {
  // STUB
}

void os_create_main_window_and_surface(Widget_t *w, Xputty *app, Window win,
                          int x, int y, int width, int height) {
  // STUB
}

void os_create_widget_window_and_surface(Widget_t *w, Xputty *app, Widget_t *parent,
                          int x, int y, int width, int height) {
  // STUB
}

void os_set_title(Widget_t *w, const char *title) {
  // STUP
}

void os_widget_show(Widget_t *w) {
  // STUP
}

void os_widget_hide(Widget_t *w) {
  // STUP
}

void os_show_tooltip(Widget_t *wid, Widget_t *w) {
  // STUP
}

void os_expose_widget(Widget_t *w) {
  // STUP
}

void os_widget_event_loop(void *w_, void* event, Xputty *main, void* user_data) {
  // STUB
}
void os_send_configure_event(Widget_t *w,int x, int y, int width, int height) {
  // STUB
}
void os_send_button_press_event(Widget_t *w) {
  // STUB
}
void os_send_button_release_event(Widget_t *w) {
  // STUB
}
void os_send_systray_message(Widget_t *w) {
  // STUB
}
void os_quit(Widget_t *w) {
  // STUB
}
void os_quit_widget(Widget_t *w) {
  // STUB
}

// os specific

int key_mapping(Display *dpy, XKeyEvent *xkey) {
  return 0; // STUB
}

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */
