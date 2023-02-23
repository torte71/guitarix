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


#include "xwidget.h"
#include "xwidget_private.h"

#define XPUTTY_WIDGET_NAME_MAX 256
char *widget_type_name(Widget_t *w) {
    static char name[XPUTTY_WIDGET_NAME_MAX+1] = {0};
    if (!w)
        strncpy(name, "UNKNOWN", XPUTTY_WIDGET_NAME_MAX);
    else
        switch (w->widget_type) {
            case WT_NONE:                       strncpy(name, "WT_NONE",                       XPUTTY_WIDGET_NAME_MAX); break;
            case WT_WINDOW:                     strncpy(name, "WT_WINDOW",                     XPUTTY_WIDGET_NAME_MAX); break;
            case WT_WIDGET:                     strncpy(name, "WT_WIDGET",                     XPUTTY_WIDGET_NAME_MAX); break;
            case WT_BUTTON:                     strncpy(name, "WT_BUTTON",                     XPUTTY_WIDGET_NAME_MAX); break;
            case WT_ON_OFF_BUTTON:              strncpy(name, "WT_ON_OFF_BUTTON",              XPUTTY_WIDGET_NAME_MAX); break;
            case WT_TOGGLE_BUTTON:              strncpy(name, "WT_TOGGLE_BUTTON",              XPUTTY_WIDGET_NAME_MAX); break;
            case WT_IMAGE_TOGGLE_BUTTON:        strncpy(name, "WT_IMAGE_TOGGLE_BUTTON",        XPUTTY_WIDGET_NAME_MAX); break;
            case WT_SWITCH_IMAGE_TOGGLE_BUTTON: strncpy(name, "WT_SWITCH_IMAGE_TOGGLE_BUTTON", XPUTTY_WIDGET_NAME_MAX); break;
            case WT_CHECK_BUTTON:               strncpy(name, "WT_CHECK_BUTTON",               XPUTTY_WIDGET_NAME_MAX); break;
            case WT_CHECK_BOX:                  strncpy(name, "WT_CHECK_BOX",                  XPUTTY_WIDGET_NAME_MAX); break;
            case WT_COMBOBOX:                   strncpy(name, "WT_COMBOBOX",                   XPUTTY_WIDGET_NAME_MAX); break;
            case WT_FILE_DIALOG:                strncpy(name, "WT_FILE_DIALOG",                XPUTTY_WIDGET_NAME_MAX); break;
            case WT_KNOB:                       strncpy(name, "WT_KNOB",                       XPUTTY_WIDGET_NAME_MAX); break;
            case WT_IMAGE_KNOB:                 strncpy(name, "WT_IMAGE_KNOB",                 XPUTTY_WIDGET_NAME_MAX); break;
            case WT_LABEL:                      strncpy(name, "WT_LABEL",                      XPUTTY_WIDGET_NAME_MAX); break;
            case WT_LISTBOX_VIEWPORT:           strncpy(name, "WT_LISTBOX_VIEWPORT",           XPUTTY_WIDGET_NAME_MAX); break;
            case WT_LISTBOX:                    strncpy(name, "WT_LISTBOX",                    XPUTTY_WIDGET_NAME_MAX); break;
            case WT_LISTBOX_ENTRY:              strncpy(name, "WT_LISTBOX_ENTRY",              XPUTTY_WIDGET_NAME_MAX); break;
            case WT_LISTVIEW_VIEWPORT:          strncpy(name, "WT_LISTVIEW_VIEWPORT",          XPUTTY_WIDGET_NAME_MAX); break;
            case WT_LISTVIEW:                   strncpy(name, "WT_LISTVIEW",                   XPUTTY_WIDGET_NAME_MAX); break;
            case WT_MENU:                       strncpy(name, "WT_MENU",                       XPUTTY_WIDGET_NAME_MAX); break;
            case WT_MENU_VIEWPORT:              strncpy(name, "WT_MENU_VIEWPORT",              XPUTTY_WIDGET_NAME_MAX); break;
            case WT_MENU_ITEM:                  strncpy(name, "WT_MENU_ITEM",                  XPUTTY_WIDGET_NAME_MAX); break;
            case WT_MENU_CHECK_ITEM:            strncpy(name, "WT_MENU_CHECK_ITEM",            XPUTTY_WIDGET_NAME_MAX); break;
            case WT_MENU_RADIO_ITEM:            strncpy(name, "WT_MENU_RADIO_ITEM",            XPUTTY_WIDGET_NAME_MAX); break;
            case WT_MESSAGE_DIALOG:             strncpy(name, "WT_MESSAGE_DIALOG",             XPUTTY_WIDGET_NAME_MAX); break;
            case WT_VMETER:                     strncpy(name, "WT_VMETER",                     XPUTTY_WIDGET_NAME_MAX); break;
            case WT_VMETER_SCALE:               strncpy(name, "WT_VMETER_SCALE",               XPUTTY_WIDGET_NAME_MAX); break;
            case WT_HMETER:                     strncpy(name, "WT_HMETER",                     XPUTTY_WIDGET_NAME_MAX); break;
            case WT_HMETER_SCALE:               strncpy(name, "WT_HMETER_SCALE",               XPUTTY_WIDGET_NAME_MAX); break;
            case WT_MIDI_KEYBOARD:              strncpy(name, "WT_MIDI_KEYBOARD",              XPUTTY_WIDGET_NAME_MAX); break;
            case WT_PLAYHEAD:                   strncpy(name, "WT_PLAYHEAD",                   XPUTTY_WIDGET_NAME_MAX); break;
            case WT_VSLIDER:                    strncpy(name, "WT_VSLIDER",                    XPUTTY_WIDGET_NAME_MAX); break;
            case WT_HSLIDER:                    strncpy(name, "WT_HSLIDER",                    XPUTTY_WIDGET_NAME_MAX); break;
            case WT_TOOLTIP:                    strncpy(name, "WT_TOOLTIP",                    XPUTTY_WIDGET_NAME_MAX); break;
            case WT_TUNER:                      strncpy(name, "WT_TUNER",                      XPUTTY_WIDGET_NAME_MAX); break;
            case WT_VALUEDISPLAY:               strncpy(name, "WT_VALUEDISPLAY",               XPUTTY_WIDGET_NAME_MAX); break;
            case WT_TEXT_ENTRY:                 strncpy(name, "WT_TEXT_ENTRY",                 XPUTTY_WIDGET_NAME_MAX); break;
            default:
                strncpy(name, "UNKNOWN", XPUTTY_WIDGET_NAME_MAX);
        }
    return name;
}
void destroy_widget(Widget_t * w, Xputty *main) {
#ifdef _WIN32
debug_print("destroy_widget:main=%p:w=%p:hwnd=%p",main,w,w?w->widget:0);
#endif
    int count = childlist_find_child(main->childlist, w);
    if (count == 0 && main->run == true) {
        quit(w);
    } else if(childlist_find_child(main->childlist, w)>=0) {
        if(w->flags & REUSE_IMAGE) {
            w->image = NULL;
        }
        if(w->flags & HAS_MEM) {
            w->func.mem_free_callback(w, NULL);
        }
        childlist_remove_child(main->childlist, w);
        int ch = childlist_has_child(w->childlist);
        if (ch) {
            int i = ch;
            for(;i>0;i--) {
                destroy_widget(w->childlist->childs[i-1],main);
            }
            destroy_widget(w,main);
        }
        if(w->flags & IS_WIDGET) {
            Widget_t *p = (Widget_t *) w->parent;
            childlist_remove_child(p->childlist, w);
        }
        delete_adjustment(w->adj_x);
        delete_adjustment(w->adj_y);
        childlist_destroy(w->childlist);
        cairo_surface_destroy(w->image);
        cairo_destroy(w->crb);
        cairo_surface_destroy(w->buffer);
        cairo_destroy(w->cr);
        cairo_surface_destroy(w->surface);

	os_destroy_window(w);
        free(w->childlist);
        free(w);
        w = NULL;
    }
}

void configure_event(void *w_, void* user_data) {
    Widget_t *wid = (Widget_t*)w_;
    Metrics_t m;

    os_get_window_metrics(wid, &m);
    if (wid->width != m.width || wid->height != m.height) {
        wid->scale.scale_x    = (float)wid->scale.init_width - m.width;
        wid->scale.scale_y    = (float)wid->scale.init_height - m.height;
        wid->scale.cscale_x   = (float)((float)wid->scale.init_width/(float)m.width);
        wid->scale.cscale_y   = (float)((float)wid->scale.init_height/(float)m.height);
        wid->scale.rcscale_x   = (float)((float)m.width/(float)wid->scale.init_width);
        wid->scale.rcscale_y   = (float)((float)m.height/(float)wid->scale.init_height);
        wid->scale.ascale     = wid->scale.cscale_x < wid->scale.cscale_y ? 
                                wid->scale.cscale_y : wid->scale.cscale_x;

        _resize_surface(wid, m.width, m.height); 

        debug_print("Widget_t configure callback width %i height %i\n", m.width, m.height);

        _resize_childs(wid);
    }
}

void widget_reset_scale(Widget_t *w) {
    cairo_scale(w->crb, w->scale.cscale_x,w->scale.cscale_y);
}

void widget_set_scale(Widget_t *w) {
    cairo_scale(w->crb, w->scale.rcscale_x,w->scale.rcscale_y);
}

Widget_t *create_window(Xputty *app, Window win,
                          int x, int y, int width, int height) {

    Widget_t *w = (Widget_t*)malloc(sizeof(Widget_t));
    assert(w != NULL);
    w->image = NULL;

    w->flags = IS_WINDOW;
    w->flags &= ~NO_AUTOREPEAT;
    w->flags &= ~FAST_REDRAW;
    w->flags &= ~HIDE_ON_DELETE;
    w->flags &= ~REUSE_IMAGE;
    w->app = app;
    w->parent = &win;
    w->parent_struct = NULL;
w->widget_type = WT_WINDOW;
    w->label = NULL;
    memset(w->input_label, 0, 32 * (sizeof w->input_label[0]));
    w->state = 0;
    w->data = 0;
    w->x = x;
    w->y = y;
    w->width = width;
    w->height = height;
    w->scale.init_x = x;
    w->scale.init_y = y;
    w->scale.init_width = width;
    w->scale.init_height = height;
    w->scale.scale_x  = 0.0;
    w->scale.scale_y  = 0.0;
    w->scale.cscale_x = 1.0;
    w->scale.cscale_y = 1.0;
    w->scale.rcscale_x = 1.0;
    w->scale.rcscale_y = 1.0;
    w->scale.ascale   = 1.0;
    w->scale.gravity  = CENTER;
    w->adj_x = NULL;
    w->adj_y = NULL;
    w->adj   = NULL;
    w->childlist = (Childlist_t*)malloc(sizeof(Childlist_t));
    assert(w->childlist != NULL);
    childlist_init(w->childlist);
    w->event_callback = widget_event_loop;
    w->func.expose_callback = _dummy_callback;
    w->func.configure_callback = configure_event;
    w->func.button_press_callback = _dummy1_callback;
    w->func.button_release_callback = _dummy1_callback;
    w->func.motion_callback = _dummy1_callback;
    w->func.adj_callback = os_adjustment_callback;
    w->func.value_changed_callback = _dummy_callback;
    w->func.key_press_callback = _dummy1_callback;
    w->func.key_release_callback = _dummy1_callback;
    w->func.enter_callback = _dummy_callback;
    w->func.leave_callback = _dummy_callback;
    w->func.user_callback = _dummy_callback;
    w->func.mem_free_callback = _dummy_callback;
    w->func.configure_notify_callback = _dummy_callback;
    w->func.map_notify_callback = _dummy_callback;
    w->func.unmap_notify_callback = _dummy_callback;
    w->func.dialog_callback = _dummy_callback;

    childlist_add_child(app->childlist,w);
    //XMapWindow(app->dpy, w->widget);
    debug_print("size of Func_t = %lu\n", sizeof(w->func)/sizeof(void*));

    debug_print("assert(w)\n");
    os_create_main_window_and_surface(w, app, win, x, y, width, height);
    os_create_cairo_context_and_buffer(w);

    return w;
}

void create_cairo_context_and_buffer(Widget_t *w) {
    int width = w->scale.init_width;
    int height = w->scale.init_height;
    assert(cairo_surface_status(w->surface) == CAIRO_STATUS_SUCCESS);
    w->cr = cairo_create(w->surface);
    cairo_select_font_face (w->cr, "Roboto", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_NORMAL);

    w->buffer = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width, height);
    assert(cairo_surface_status(w->buffer) == CAIRO_STATUS_SUCCESS);
    w->crb = cairo_create (w->buffer);
    cairo_select_font_face (w->crb, "Roboto", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_NORMAL);
}

Widget_t *create_widget(Xputty *app, Widget_t *parent,
                          int x, int y, int width, int height) {

    Widget_t *w = (Widget_t*)malloc(sizeof(Widget_t));
    assert(w != NULL);
    w->image = NULL;
    
    w->flags = IS_WIDGET | USE_TRANSPARENCY;
    w->flags &= ~NO_AUTOREPEAT;
    w->flags &= ~FAST_REDRAW;
    w->flags &= ~HIDE_ON_DELETE;
    w->flags &= ~REUSE_IMAGE;
    w->app = app;
    w->parent = parent;
    w->parent_struct = NULL;
w->widget_type = WT_WIDGET;
    w->label = NULL;
    memset(w->input_label, 0, 32 * (sizeof w->input_label[0]));
    w->state = 0;
    w->data = 0;
    w->x = x;
    w->y = y;
    w->width = width;
    w->height = height;
    w->scale.gravity = CENTER;
    w->scale.init_width = width;
    w->scale.init_height = height;
    w->scale.init_x   = x;
    w->scale.init_y   = y;
    w->scale.scale_x  = 0.0;
    w->scale.scale_y  = 0.0;
    w->scale.cscale_x = 1.0;
    w->scale.cscale_y = 1.0;
    w->scale.rcscale_x = 1.0;
    w->scale.rcscale_y = 1.0;
    w->scale.ascale   = 1.0;
    w->adj_x = NULL;
    w->adj_y = NULL;
    w->adj   = NULL;
    w->childlist = (Childlist_t*)malloc(sizeof(Childlist_t));
    assert(w->childlist != NULL);
    childlist_init(w->childlist);
    childlist_add_child(parent->childlist, w);
    w->event_callback = widget_event_loop;
    w->func.expose_callback = _dummy_callback;
    w->func.configure_callback = configure_event;
    w->func.button_press_callback = _dummy1_callback;
    w->func.button_release_callback = _dummy1_callback;
    w->func.motion_callback = _dummy1_callback;
    w->func.adj_callback = os_adjustment_callback;
    w->func.value_changed_callback = _dummy_callback;
    w->func.key_press_callback = _dummy1_callback;
    w->func.key_release_callback = _dummy1_callback;
    w->func.enter_callback = _dummy_callback;
    w->func.leave_callback = _dummy_callback;
    w->func.user_callback = _dummy_callback;
    w->func.mem_free_callback = _dummy_callback;
    w->func.configure_notify_callback = _dummy_callback;
    w->func.map_notify_callback = _dummy_callback;
    w->func.unmap_notify_callback = _dummy_callback;
    w->func.dialog_callback = _dummy_callback;

    childlist_add_child(app->childlist,w);
    //XMapWindow(app->dpy, w->widget);
    debug_print("size of Widget_t = %ld\n", sizeof(struct Widget_t));

    debug_print("assert(w)\n");
    os_create_widget_window_and_surface(w, app, parent, x, y, width, height);
    os_create_cairo_context_and_buffer(w);

    return w;
}

void connect_func(void (**event)(), void (*handler)()) {
    debug_print("address of a is: %p\n", (void*)event);
    debug_print("address of b is: %p\n", (void*)handler);
    *event = handler;
    debug_print("address of a is: %p\n", (void*)(*event));
}

void widget_set_title(Widget_t *w, const char *title) {
    os_set_title(w, title);
}

void widget_show(Widget_t *w) {
    os_widget_show(w);
}

void widget_hide(Widget_t *w) {
    int i=0;
    for(;i<w->childlist->elem;i++) {
        widget_hide(w->childlist->childs[i]);
    }
    os_widget_hide(w);
}

void widget_show_all(Widget_t *w) {
    if (w->flags & IS_POPUP || w->flags & IS_TOOLTIP) {
        return;
    } else {
        os_widget_show(w);
        int i=0;
        for(;i<w->childlist->elem;i++) {
            widget_show_all(w->childlist->childs[i]);
        }
    }
}

void pop_widget_show_all(Widget_t *w) {
    os_widget_show(w);
    int i=0;
    for(;i<w->childlist->elem;i++) {
        pop_widget_show_all(w->childlist->childs[i]);
    }
}

void show_tooltip(Widget_t *wid) {
    int i = 0;
    for(;i<wid->childlist->elem;i++) {
        Widget_t *w = wid->childlist->childs[i];
        if (w->flags & IS_TOOLTIP) {
	    os_show_tooltip(wid, w);
            widget_show(w);
            break;
        }
    }
}

void hide_tooltip(Widget_t *wid) {
    int i = 0;
    for(;i<wid->childlist->elem;i++) {
        Widget_t *w = wid->childlist->childs[i];
        if (w->flags & IS_TOOLTIP) {
            widget_hide(w);
            break;
        }
    }
}

Widget_t *get_toplevel_widget(Xputty *main) {
    return  main->childlist->childs[0];
}

void expose_widget(Widget_t *w) {
    os_expose_widget(w);
}

void transparent_draw(void * w_, void* user_data) {
    Widget_t *wid = (Widget_t*)w_;

    cairo_push_group (wid->cr);

    if (wid->flags & USE_TRANSPARENCY) {
        Widget_t *parent = (Widget_t*)wid->parent;
	Metrics_t m;
	os_get_window_metrics(wid, &m);
        debug_print("Widget_t _transparency \n");
        cairo_set_source_surface (wid->crb, parent->buffer, -m.x, -m.y);
        cairo_paint (wid->crb);
    }

    cairo_push_group (wid->crb);
    wid->func.expose_callback(wid, user_data);
    cairo_pop_group_to_source (wid->crb);
    cairo_paint (wid->crb);

    cairo_set_source_surface (wid->cr, wid->buffer,0,0);
    cairo_paint (wid->cr);

    cairo_pop_group_to_source (wid->cr);
    cairo_paint (wid->cr);
    _propagate_child_expose(wid);
}

void widget_event_loop(void *w_, void* event, Xputty *main, void* user_data) {
    os_widget_event_loop(w_, event, main, user_data);
}

void send_configure_event(Widget_t *w,int x, int y, int width, int height) {
    os_send_configure_event(w, x, y, width, height);
}

void send_button_press_event(Widget_t *w) {
    os_send_button_press_event(w);
}

void send_button_release_event(Widget_t *w) {
    os_send_button_release_event(w);
}

void send_systray_message(Widget_t *w) {
    os_send_systray_message(w);
}

void quit(Widget_t *w) {
    os_quit(w);
}

void quit_widget(Widget_t *w) {
    os_quit_widget(w);
}

