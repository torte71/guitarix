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

#ifdef __linux__

#ifdef __cplusplus
extern "C" {
#endif

#include "xwidget.h"
#include "xwidget_private.h"

// compat wrappers

void os_destroy_window(Widget_t *w) {
    if (w->xic) XDestroyIC(w->xic);
    if (w->xim) XCloseIM(w->xim);
    XUnmapWindow(w->app->dpy, w->widget);
    XDestroyWindow(w->app->dpy, w->widget);
}

void os_get_window_size(Widget_t *w_, int *x, int *y, int *width, int *height) {
    Widget_t *wid = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(wid->app->dpy, (Window)wid->widget, &attrs);
    *width = attrs.width;
    *height = attrs.height;
}

void os_create_main_window_and_surface(Widget_t *w, Xputty *app, Window win,
                          int x, int y, int width, int height) {
    XSetWindowAttributes attributes;
    attributes.save_under = True;
    attributes.override_redirect = 0;

    long event_mask = StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask
                    |ButtonPressMask|Button1MotionMask;



    w->widget = XCreateWindow(app->dpy, win , x, y, width, height, 0,
                            CopyFromParent, InputOutput, CopyFromParent,
                            CopyFromParent, &attributes);
    debug_print("XCreateWindow\n");

    XSetLocaleModifiers("");
    w->xim = XOpenIM(app->dpy, 0, 0, 0);
    if(!w->xim){
        XSetLocaleModifiers("@im=none");
        w->xim = XOpenIM(app->dpy, 0, 0, 0);
    }

    w->xic = XCreateIC(w->xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                    XNClientWindow, w->widget, XNFocusWindow,  w->widget, NULL);

    XSetICFocus(w->xic);

    XSelectInput(app->dpy, w->widget, event_mask);

    XSizeHints* win_size_hints;
    win_size_hints = XAllocSizeHints();
    win_size_hints->flags =  PMinSize|PBaseSize|PWinGravity;
    win_size_hints->min_width = width/2;
    win_size_hints->min_height = height/2;
    win_size_hints->base_width = width;
    win_size_hints->base_height = height;
    win_size_hints->win_gravity = CenterGravity;
    XSetWMNormalHints(app->dpy, w->widget, win_size_hints);
    XFree(win_size_hints);

    w->surface =  cairo_xlib_surface_create (app->dpy, w->widget,  
                  DefaultVisual(app->dpy, DefaultScreen(app->dpy)), width, height);

}

void os_create_widget_window_and_surface(Widget_t *w, Xputty *app, Widget_t *parent,
                          int x, int y, int width, int height) {
    XSetWindowAttributes attributes;
    attributes.save_under = True;
    attributes.override_redirect = True;

    long event_mask = StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask
                    |ButtonPressMask|Button1MotionMask;



    w->widget = XCreateWindow(app->dpy, parent->widget , x, y, width, height, 0,
                            CopyFromParent, InputOutput, CopyFromParent,
                            CopyFromParent|CWOverrideRedirect, &attributes);
    debug_print("XCreateWindow\n");

    XSetLocaleModifiers("");
    w->xim = XOpenIM(app->dpy, 0, 0, 0);
    if(!w->xim){
        XSetLocaleModifiers("@im=none");
        w->xim = XOpenIM(app->dpy, 0, 0, 0);
    }

    w->xic = XCreateIC(w->xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                    XNClientWindow, w->widget, XNFocusWindow,  w->widget, NULL);

    XSetICFocus(w->xic);

    XSelectInput(app->dpy, w->widget, event_mask);

    w->surface =  cairo_xlib_surface_create (app->dpy, w->widget,  
                  DefaultVisual(app->dpy, DefaultScreen(app->dpy)), width, height);
}

void os_set_title(Widget_t *w, const char *title) {
    XStoreName(w->app->dpy, w->widget, title);
}

void os_widget_show(Widget_t *w) {
    w->func.map_notify_callback(w, NULL);
    XMapWindow(w->app->dpy, w->widget);
}

void os_widget_hide(Widget_t *w) {
    w->func.unmap_notify_callback(w, NULL);
    XUnmapWindow(w->app->dpy, w->widget);
}

void os_show_tooltip(Widget_t *wid, Widget_t *w) {
    unsigned int mask;
    int x, y, rx, ry;
    Window child, root;
    XQueryPointer(wid->app->dpy, wid->widget, &root, &child, &rx, &ry, &x, &y, &mask);
    int x1, y1;
    XTranslateCoordinates( wid->app->dpy, wid->widget, DefaultRootWindow(wid->app->dpy),
							       x, y, &x1, &y1, &child );
    XMoveWindow(w->app->dpy,w->widget,x1+10, y1-10);
}

void os_expose_widget(Widget_t *w) {
    XEvent exp;
    memset(&exp, 0, sizeof(exp));
    exp.type = Expose;
    exp.xexpose.window = w->widget;
    XSendEvent(w->app->dpy, w->widget, False, ExposureMask, (XEvent *)&exp);
}

void os_widget_event_loop(void *w_, void* event, Xputty *main, void* user_data) {
    Widget_t *wid = (Widget_t*)w_;
    XEvent *xev = (XEvent*)event;
    if (XFilterEvent(xev, wid->widget))
        return;
    
    switch(xev->type) {
        case ConfigureNotify:
            wid->func.configure_callback(w_, user_data);
            debug_print("Widget_t ConfigureNotify \n");
        break;

        case Expose:
            if (xev->xexpose.count == 0) {
                transparent_draw(w_, user_data);
                debug_print("Widget_t Expose \n");
            }
        break;

        case ButtonPress:
            if (wid->state == 4) break;
            if (wid->flags & HAS_TOOLTIP) hide_tooltip(wid);
            _button_press(wid, &xev->xbutton, user_data);
            debug_print("Widget_t  ButtonPress %i\n", xev->xbutton.button);
        break;

        case ButtonRelease:
            _check_grab(wid, &xev->xbutton, main);
            if (wid->state == 4) break;
            _has_pointer(wid, &xev->xbutton);
            if(wid->flags & HAS_POINTER) wid->state = 1;
            else wid->state = 0;
            _check_enum(wid, &xev->xbutton);
            wid->func.button_release_callback(w_, &xev->xbutton, user_data);
            debug_print("Widget_t  ButtonRelease %i\n", xev->xbutton.button);
        break;

        case KeyPress:
            if (wid->state == 4) break;
            _check_keymap(wid, xev->xkey);
            wid->func.key_press_callback(w_, &xev->xkey, user_data);
            debug_print("Widget_t KeyPress %u\n", xev->xkey.keycode);
        break;

        case KeyRelease: 
            if (wid->state == 4) break;
            {
            unsigned short is_retriggered = 0;
            if(wid->flags & NO_AUTOREPEAT) {
                if (XEventsQueued(main->dpy, QueuedAlready)) {
                    XEvent nev;
                    XPeekEvent(main->dpy, &nev);
                    if (nev.type == KeyPress && nev.xkey.time == xev->xkey.time &&
                        nev.xkey.keycode == xev->xkey.keycode && 
                        (nev.xkey.keycode > 119 || nev.xkey.keycode < 110)) {
                        XNextEvent (main->dpy, xev);
                        is_retriggered = 1;
                    }
                }
            }
            if (!is_retriggered) {
                wid->func.key_release_callback(w_, &xev->xkey, user_data);
                debug_print("Widget_t KeyRelease %u\n", xev->xkey.keycode);
            }
        }
        break;

        case LeaveNotify:
            wid->flags &= ~HAS_FOCUS;
            if (wid->state == 4) break;
            if(!(xev->xcrossing.state & Button1Mask)) {
                wid->state = 0;
                wid->func.leave_callback(w_, user_data);
            }
            if (wid->flags & HAS_TOOLTIP) hide_tooltip(wid);
            debug_print("Widget_t LeaveNotify \n");
        break;

        case EnterNotify:
            wid->flags |= HAS_FOCUS;
            if (wid->state == 4) break;
            if(!(xev->xcrossing.state & Button1Mask)) {
                wid->state = 1;
                wid->func.enter_callback(w_, user_data);
                if (wid->flags & HAS_TOOLTIP) show_tooltip(wid);
                else _hide_all_tooltips(wid);
            }
            debug_print("Widget_t EnterNotify \n");
        break;

        case MotionNotify:
            if (wid->state == 4) break;
            adj_set_motion_state(wid, xev->xmotion.x, xev->xmotion.y);
            wid->func.motion_callback(w_,&xev->xmotion, user_data);
            debug_print("Widget_t MotionNotify x = %i Y = %i \n",xev->xmotion.x,xev->xmotion.y );
        break;

        case ClientMessage:
            if (xev->xclient.message_type == XInternAtom(wid->app->dpy, "WIDGET_DESTROY", 1)) {
                int ch = childlist_has_child(wid->childlist);
                if (ch) {
                    int i = ch;
                    for(;i>0;i--) {
                        quit_widget(wid->childlist->childs[i-1]);
                    }
                    quit_widget(wid);
                } else {
                    destroy_widget(wid,main);
                }
            }

        default:
        break;
    }
    if (main->queue_event) {
        main->queue_event = false;
        transparent_draw(w_, user_data);
    }
}

void os_send_configure_event(Widget_t *w,int x, int y, int width, int height) {
    XConfigureEvent notify;
    memset(&notify, 0, sizeof(notify));
    notify.type = ConfigureNotify;
    notify.display = w->app->dpy;
    notify.send_event = True;
    notify.event = w->widget;
    notify.window = w->widget;
    notify.x = x;
    notify.y = y;
    notify.width = width;
    notify.height = height;
    notify.border_width = 0;
    notify.above = None;
    notify.override_redirect = 1;
    XSendEvent( w->app->dpy, w->widget, true, StructureNotifyMask, (XEvent*)&notify );    
}

void os_send_button_press_event(Widget_t *w) {
    XEvent event;
    memset(&event, 0, sizeof(XEvent));
    XWindowAttributes attr;
    XGetWindowAttributes(w->app->dpy, w->widget, &attr);
    event.type = ButtonPress;
    event.xbutton.same_screen = true;
    event.xbutton.root = None;
    event.xbutton.window = w->widget;
    event.xbutton.subwindow = None;
    event.xbutton.x = 1;
    event.xbutton.y = 1;
    event.xbutton.x_root = attr.x;
    event.xbutton.y_root = attr.y;
    event.xbutton.state = 0;
    event.xbutton.button = Button1;
    XSendEvent(w->app->dpy, PointerWindow, True, ButtonPressMask, &event);
}

void os_send_button_release_event(Widget_t *w) {
    XEvent event;
    memset(&event, 0, sizeof(XEvent));
    XWindowAttributes attr;
    XGetWindowAttributes(w->app->dpy, w->widget, &attr);
    event.type = ButtonRelease;
    event.xbutton.same_screen = true;
    event.xbutton.root = None;
    event.xbutton.window = w->widget;
    event.xbutton.subwindow = None;
    event.xbutton.x = 1;
    event.xbutton.y = 1;
    event.xbutton.x_root = attr.x;
    event.xbutton.y_root = attr.y;
    event.xbutton.state = 0;
    event.xbutton.button = Button1;
    XSendEvent(w->app->dpy, PointerWindow, True, ButtonReleaseMask, &event);
}

void os_send_systray_message(Widget_t *w) {
    XEvent event;
    Screen *xscreen;
    char buf[256];
    buf[0]=0;
    
    xscreen=DefaultScreenOfDisplay(w->app->dpy);
    sprintf(buf,"_NET_SYSTEM_TRAY_S%d",XScreenNumberOfScreen (xscreen));
    Atom selection_atom = XInternAtom (w->app->dpy,buf,0);

    Window tray = XGetSelectionOwner (w->app->dpy,selection_atom);
    Atom visualatom = XInternAtom(w->app->dpy, "_NET_SYSTEM_TRAY_VISUAL", False);
    VisualID value = XVisualIDFromVisual(DefaultVisual(w->app->dpy, DefaultScreen(w->app->dpy)));
    XChangeProperty(w->app->dpy, w->widget, visualatom, XA_VISUALID, 32,
            PropModeReplace, (unsigned char*)&value, 1);

    if ( tray != None)
        XSelectInput (w->app->dpy,tray,StructureNotifyMask);

    memset(&event, 0, sizeof(event));
    event.xclient.type = ClientMessage;
    event.xclient.window = tray;
    event.xclient.message_type = XInternAtom (w->app->dpy, "_NET_SYSTEM_TRAY_OPCODE", False );
    event.xclient.format = 32;
    event.xclient.data.l[0] = CurrentTime;
    event.xclient.data.l[1] = SYSTEM_TRAY_REQUEST_DOCK;
    event.xclient.data.l[2] = w->widget;
    event.xclient.data.l[3] = 0;
    event.xclient.data.l[4] = 0;

    XSendEvent(w->app->dpy, tray, False, NoEventMask, &event);
}

void os_quit(Widget_t *w) {
    Atom WM_DELETE_WINDOW = XInternAtom(w->app->dpy, "WM_DELETE_WINDOW", True);
    XClientMessageEvent xevent;
    xevent.type = ClientMessage;
    xevent.message_type = WM_DELETE_WINDOW;
    xevent.display = w->app->dpy;
    xevent.window = get_toplevel_widget(w->app)->widget;
    xevent.format = 16;
    xevent.data.l[0] = WM_DELETE_WINDOW;
    XSendEvent(w->app->dpy, w->widget, 0, 0, (XEvent *)&xevent);
}

void os_quit_widget(Widget_t *w) {
    Atom QUIT_WIDGET = XInternAtom(w->app->dpy, "WIDGET_DESTROY", False);
    XClientMessageEvent xevent;
    xevent.type = ClientMessage;
    xevent.message_type = QUIT_WIDGET;
    xevent.display = w->app->dpy;
    xevent.window = w->widget;
    xevent.format = 16;
    xevent.data.l[0] = 1;
    XSendEvent(w->app->dpy, w->widget, 0, 0, (XEvent *)&xevent);
}

// TODO

int key_mapping(Display *dpy, XKeyEvent *xkey) {
    if (xkey->keycode == XKeysymToKeycode(dpy,XK_Tab))
        return (xkey->state == ShiftMask) ? 1 : 2;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Up))
        return 3;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Right))
        return 4;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Down))
        return 5;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Left))
        return 6;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Home))
        return 7;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Insert))
        return 8;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_End))
        return 9;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Return))
        return 10;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_BackSpace))
        return 11;
    // keypad
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Subtract))
        return 1;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Add))
        return 2;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Up))
        return 3;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Right))
        return 4;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Down))
        return 5;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Left))
        return 6;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Home))
        return 7;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Insert))
        return 8;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_End))
        return 9;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Enter))
        return 10;
    else return 0;
}



#ifdef __cplusplus
}
#endif

#endif /* __linux__ */
