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

#include "xputty.h"

#ifdef _WIN32
#define True true
#define False false
#endif


void main_init(Xputty *main) {
    main->dpy = os_open_display(0);
    assert(main->dpy);
    main->childlist = (Childlist_t*)malloc(sizeof(Childlist_t));
debug_print("%s:main=%p:main->childlist=%p",__FUNCTION__,main,main->childlist);
    assert(main->childlist);
    childlist_init(main->childlist);
    main->color_scheme = (XColor_t*)malloc(sizeof(XColor_t));
    assert(main->color_scheme);
    set_dark_theme(main);
    main->hold_grab = NULL;
    main->run = true;
    main->small_font = 10;
    main->normal_font = 12;
    main->big_font = 16;
    main->queue_event = false;
}

void main_run(Xputty *main) {
#ifndef _WIN32
    Widget_t * wid = main->childlist->childs[0]; 
    Atom WM_DELETE_WINDOW = os_register_wm_delete_window(wid);

    XEvent xev;
    int ew;

    while (main->run && (XNextEvent(main->dpy, &xev)>=0)) {
        ew = childlist_find_widget(main->childlist, xev.xany.window);
        if(ew  >= 0) {
            Widget_t * w = main->childlist->childs[ew];
            w->event_callback(w, &xev, main, NULL);
        }

        switch (xev.type) {
        case ButtonPress:
            if(main->hold_grab != NULL) {
                Widget_t *view_port = main->hold_grab->childlist->childs[0];
                bool is_item = False;
                int i = view_port->childlist->elem-1;
                for(;i>-1;i--) {
                    Widget_t *w = view_port->childlist->childs[i];
                    if (xev.xbutton.window == w->widget) {
                        is_item = True;
                        break;
                    }
                }
                if (xev.xbutton.window == view_port->widget) is_item = True;
                if (!is_item) {
                    XUngrabPointer(main->dpy,CurrentTime);
                    widget_hide(main->hold_grab);
                    main->hold_grab = NULL;
                }
            }
            break;
            case ClientMessage:
                /* delete window event */
                if (xev.xclient.data.l[0] == (long int)WM_DELETE_WINDOW &&
                        xev.xclient.window == wid->widget) {
                    main->run = false;
                } else {
                    int i = childlist_find_widget(main->childlist, xev.xclient.window);
                    if(i<1) return;
                    Widget_t *w = main->childlist->childs[i];
                    if(w->flags & HIDE_ON_DELETE) widget_hide(w);
                    else destroy_widget(main->childlist->childs[i],main);
                }
            break;
        }
    }
#endif
}

void run_embedded(Xputty *main) {
#ifndef _WIN32
    XEvent xev;
    int ew = -1;

    while (XPending(main->dpy) > 0) {
        XNextEvent(main->dpy, &xev);
        ew = childlist_find_widget(main->childlist, xev.xany.window);
        if(ew  >= 0) {
            Widget_t * w = main->childlist->childs[ew];
            unsigned short retrigger = 0;
            if (xev.type == Expose && XEventsQueued(main->dpy, QueuedAlready)) {
                XEvent nev;
                XPeekEvent(main->dpy, &nev);
                if (nev.type == ConfigureNotify) {
                    retrigger = 1;
                    main->queue_event = true;
                }
            }
            if (!retrigger) {
                w->event_callback(w, &xev, main, NULL);
            }
        }
        switch (xev.type) {
        case ButtonPress:
            if(main->hold_grab != NULL) {
                Widget_t *view_port = main->hold_grab->childlist->childs[0];
                bool is_item = False;
                int i = view_port->childlist->elem-1;
                for(;i>-1;i--) {
                    Widget_t *w = view_port->childlist->childs[i];
                    if (xev.xbutton.window == w->widget) {
                        is_item = True;
                        break;
                    }
                }
                if (xev.xbutton.window == view_port->widget) is_item = True;
                if (!is_item) {
                    XUngrabPointer(main->dpy,CurrentTime);
                    widget_hide(main->hold_grab);
                    main->hold_grab = NULL;
                }
            }
        break;
        case ClientMessage:
            /* delete window event */
            if (xev.xclient.data.l[0] == (long int)XInternAtom(main->dpy, "WM_DELETE_WINDOW", True) ) {
                int i = childlist_find_widget(main->childlist, xev.xclient.window);
                if(i<1) return;
                Widget_t *w = main->childlist->childs[i];
                if(w->flags & HIDE_ON_DELETE) widget_hide(w);
                else destroy_widget(w, main);
            }
        break;
        }
    }
#endif
}

void main_quit(Xputty *main) {
    int i = main->childlist->elem-1;
    for(;i>-1;i--) {
        Widget_t *w = main->childlist->childs[i];
debug_print("%s:%d:destroy_widget:w=%p",__FUNCTION__,i,w);
        destroy_widget(w, main);
    }
debug_print("%s:childlist_destroy:list=%p",__FUNCTION__,main->childlist);
    childlist_destroy(main->childlist);
    free(main->childlist);
    free(main->color_scheme);
    os_close_display(main->dpy);
    debug_print("quit\n");
}
