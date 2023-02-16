/* vim:ts=4:sw=4:noet:
 *
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

#include "windowsx.h"

#include "xwidget.h"
#include "xwidget_private.h"

// forward declarations
void SetClientSize(HWND hwnd, int clientWidth, int clientHeight);
BOOL SetMouseTracking(HWND hwnd, BOOL enable);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT onPaint( HWND hwnd, WPARAM wParam, LPARAM lParam );

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
			common functions (required)
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

void os_destroy_window(Widget_t *w) {
	debug_print("STUB:os_destroy_window:w=%p:hwnd=%p",w,(w)?w->widget:NULL);

	// mswin automatically sends WM_DESTROY to all child windows
	// floating windows need to be handled manually
	if ((w) && (w->flags & IS_POPUP)) {
		debug_print("STUB:os_destroy_window:DestroyWindow:hwnd=%p",(w)?w->widget:NULL);
		DestroyWindow(w->widget);
	}
	//CloseWindow(w->widget); // crash

	//UnregisterClass(TEXT("xputtyMainUIClass"), NULL);
	//UnregisterClass(TEXT("xputtyWidgetUIClass"), NULL);
	// STUB
}

void os_get_window_metrics(Widget_t *w_, Metrics_t *metrics) {
	RECT Rect = {0};
	POINT Point = {0};
	Widget_t *parent = (Widget_t *)w_->parent;

	if (GetWindowRect(w_->widget, &Rect)) {
		Point.x = Rect.left;
		Point.y = Rect.top;
		ScreenToClient(parent->widget, &Point);
		metrics->x = Point.x;
		metrics->y = Point.y;
		metrics->width = Rect.right - Rect.left;
		metrics->height = Rect.bottom - Rect.top;
	}
	metrics->visible = IsWindowVisible(w_->widget);
}

void os_get_surface_size(cairo_surface_t *surface, int *width, int *height) {
    *width = cairo_image_surface_get_width(surface);
    *height = cairo_image_surface_get_height(surface);
}

void os_set_widget_surface_size(Widget_t *w, int width, int height) {
    // image_surface cant be resized (only xlib_surface can)
    cairo_destroy(w->cr);
    cairo_surface_destroy(w->surface);
    w->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    w->cr = cairo_create(w->surface);
}

void os_move_window(Display *dpy, Widget_t *w, int x, int y) {
	SetWindowPos(w->widget, NULL, //hWnd, hWndInsertAfter
	  x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER); //X, Y, width, height, uFlags
}

void os_resize_window(Display *dpy, Widget_t *w, int x, int y) {
	SetWindowPos(w->widget, NULL, //hWnd, hWndInsertAfter
	  0, 0, x, y, SWP_NOMOVE|SWP_NOZORDER); //X, Y, width, height, uFlags
}

void os_create_main_window_and_surface(Widget_t *w, Xputty *app, Window win,
                          int x, int y, int width, int height) {
	// prepare window class
//diff:classname
	static TCHAR szClassName[] = TEXT("xputtyMainUIClass");
	WNDCLASS wndclass = {0};
	HINSTANCE hInstance = NULL;

debug_print("os_create_main_window_and_surface:x=%d:y=%d:w=%d:h=%d:w=%p:app=%p:win=%p\n",x,y,width,height,w,app,win);

//	wndclass.style		   = CS_HREDRAW | CS_VREDRAW; // clear on resize
	wndclass.lpfnWndProc   = WndProc;
	wndclass.hInstance	   = hInstance;
	wndclass.hCursor	   = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground =(HBRUSH)COLOR_WINDOW;
	wndclass.lpszClassName = szClassName;
	wndclass.cbWndExtra    = sizeof(w); // reserve space for SetWindowLongPtr
	RegisterClass(&wndclass);
	// TODO: pass window style (mainwindow,childwidget,popup,...) to create_window()
	// _WIN32 doesnt allow changing the style afterwards, as it is done in xmenu.cpp
	// (this also removes duplicate code for window/widget creation).
	// For the current situation it is sufficient to set popup style if parent is HWND_DESKTOP.
	DWORD dwStyle;
	if (win == HWND_DESKTOP) {
		dwStyle = WS_POPUP ;
		//dwStyle = WS_OVERLAPPEDWINDOW ;
	} else {
		dwStyle = WS_CHILD ;
	}
	// create the window
	w->widget = CreateWindowEx(WS_EX_TOPMOST, // dwExStyle
							szClassName, // lpClassName
							TEXT("Draw Surface"), // lpWindowName
							dwStyle, // dwStyle
							CW_USEDEFAULT, CW_USEDEFAULT, // X, Y
							width, height, // nWidth, nHeight
//diff:parent=win
							win, // hWndParent (no embeddeding takes place yet)
							NULL, hInstance, NULL); // hMenu, hInstance, lpParam
debug_print("os_create_main_window_and_surface:w=%p:hwnd=%p",w,w->widget);
	// attach a pointer to "w" to this window (so w is available in WndProc)
	SetWindowLongPtr(w->widget, GWLP_USERDATA, (LONG_PTR)w);
	SetParent(w->widget, win); // embed into parentWindow
	SetClientSize(w->widget, width, height);
	SetMouseTracking(w->widget, true); // for receiving WM_MOUSELEAVE
//diff:SizeHints?
//    win_size_hints = XAllocSizeHints();

	// create a permanent surface for drawing (see onPaint() event)
	w->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
}

void os_create_widget_window_and_surface(Widget_t *w, Xputty *app, Widget_t *parent,
                          int x, int y, int width, int height) {
  // STUB
	// prepare window class
//diff:classname
	static TCHAR szClassName[] = TEXT("xputtyWidgetUIClass");
	WNDCLASS wndclass = {0};
	HINSTANCE hInstance = NULL;
printf("os_create_widget_window_and_surface:x=%d:y=%d:w=%d:h=%d:w=%p:app=%p:parent=%p\n",x,y,width,height,w,app,parent);

//	wndclass.style		   = CS_HREDRAW | CS_VREDRAW; // clear on resize
	wndclass.lpfnWndProc   = WndProc;
	wndclass.hInstance	   = hInstance;
	wndclass.hCursor	   = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground =(HBRUSH)COLOR_WINDOW;
//wndclass.hbrBackground = NULL;
	wndclass.lpszClassName = szClassName;
	wndclass.cbWndExtra    = sizeof(w); // reserve space for SetWindowLongPtr
	RegisterClass(&wndclass);
	// create the window
	w->widget = CreateWindowEx(WS_EX_TOPMOST, // dwExStyle
							szClassName, // lpClassName
							TEXT("Draw Surface"), // lpWindowName
							WS_CHILD, // dwStyle
							x, y, // X, Y
							width, height, // nWidth, nHeight
//diff:parent=parent->widget
							parent->widget, // hWndParent (no embeddeding takes place yet)
							NULL, hInstance, NULL); // hMenu, hInstance, lpParam
debug_print("os_create_widget_window_and_surface:w=%p:hwnd=%p",w,w->widget);
													//
	// attach a pointer to "w" to this window (so w is available in WndProc)
	SetWindowLongPtr(w->widget, GWLP_USERDATA, (LONG_PTR)w);
	SetParent(w->widget, parent->widget); // embed into parentWindow
	SetClientSize(w->widget, width, height);
	SetMouseTracking(w->widget, true); // for receiving WM_MOUSELEAVE
//diff:no SizeHints

	// create a permanent surface for drawing (see onPaint() event)
	w->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
}

void os_set_title(Widget_t *w, const char *title) {
	debug_print("STUB:os_set_title:w=%p",w);
	// STUB
}

void os_widget_show(Widget_t *w) {
	debug_print("os_widget_show:w=%p",w);
	ShowWindow(w->widget, SW_SHOW);
}

void os_widget_hide(Widget_t *w) {
	debug_print("os_widget_hide:w=%p",w);
	ShowWindow(w->widget, SW_HIDE);
}

void os_show_tooltip(Widget_t *wid, Widget_t *w) {
	POINT pt;
	debug_print("os_show_tooltip:wid=%p:w=%p",wid,w);
	if (GetCursorPos(&pt)) {
		SetWindowPos(w->widget, NULL, //hWnd, hWndInsertAfter
		  pt.x+10, pt.y-10, 0, 0, SWP_NOSIZE|SWP_NOZORDER); //X, Y, width, height, uFlags
	}
}

void os_expose_widget(Widget_t *w) {
	debug_print("os_expose_widget:w=%p",w);
	RedrawWindow(w->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
}

void os_widget_event_loop(void *w_, void* event, Xputty *main, void* user_data) {
	debug_print("STUB:os_widget_event_loop:w=%p",w_);
	// STUB
}
void os_send_configure_event(Widget_t *w,int x, int y, int width, int height) {
	debug_print("STUB:os_send_configure_event:x=%d:y=%d:width=%d:height=%d:w=%p\n",x,y,width,height,w);
	// STUB
//SetClientSize(w->widget, width, height); // makes no difference
//RedrawWindow(w->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
}
void os_send_button_press_event(Widget_t *w) {
	debug_print("STUB:os_send_button_press_event:w=%p",w);
	// STUB
}
void os_send_button_release_event(Widget_t *w) {
	debug_print("STUB:os_send_button_release_event:w=%p",w);
	// STUB
}
void os_send_systray_message(Widget_t *w) {
	debug_print("STUB:os_send_systray_message:w=%p",w);
	// STUB
}

void os_adjustment_callback(void *w_, void *user_data) {
	Widget_t *w = (Widget_t *)w_;
	transparent_draw(w, user_data);
	RedrawWindow(w->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

void os_quit(Widget_t *w) {
	WPARAM wParam = (WPARAM)get_toplevel_widget(w->app)->widget;
	DWORD msg = os_register_wm_delete_window(w);
	int res = SendMessage(w->widget, msg, wParam, 0); // WM_DELETE_WINDOW
	debug_print("STUB:os_quit:w=%p:hwnd/dest=%p:wPar/toplvl=%16.16llx:msg=%8.8lx:res=%d",w,(w)?w->widget:NULL,wParam,msg,res);
	//CloseWindow(w->widget);
	//UnregisterClass(TEXT("xputtyMainUIClass"), NULL);
	// STUB
}
void os_quit_widget(Widget_t *w) {
	WPARAM wParam = (WPARAM)w->widget;
	DWORD msg = os_register_widget_destroy(w);
	int res = SendMessage(w->widget, msg, wParam, 0); // WIDGET_DESTROY
	debug_print("STUB:os_quit_widget:w=%p:hwnd=%p:msg=%8.8lx:res=%d",w,(w)?w->widget:NULL,msg,res);
//if ((w) && (w->flags & IS_POPUP)) {
//	debug_print("STUB:os_quit_widget:IS_POPUP:DestroyWindow:hwnd=%p",(w)?w->widget:NULL);
//	CloseWindow(w->widget);
//	DestroyWindow(w->widget);
//}
	//CloseWindow(w->widget);
	//UnregisterClass(TEXT("xputtyWidgetUIClass"), NULL);
	// STUB
}

Atom os_register_wm_delete_window(Widget_t * wid) {
	Atom msg = WM_USER + 01;
	//Atom msg = RegisterWindowMessage("XPUTTY_WM_DELETE_WINDOW");
	debug_print("STUB:os_register_wm_delete_window:w=%p:msg=%8.8lx",wid,msg);
	return msg;
	//return 0; // STUB
}

Atom os_register_widget_destroy(Widget_t * wid) {
	Atom msg = WM_USER + 02 ;
	//Atom msg = RegisterWindowMessage("XPUTTY_WIDGET_DESTROY");
	debug_print("STUB:os_register_widget_destroy:w=%p:msg=%8.8lx",wid,msg);
	return msg;
	//return 0; // STUB
}

// os specific

int key_mapping(Display *dpy, XKeyEvent *xkey) {
	debug_print("STUB:key_mapping");
	return 0; // STUB
}

/*------------- the event loop ---------------*/

#define _debugwm
#ifdef _debugwm
#include "winutil.c"
#endif
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	XButtonEvent xbutton;
	XMotionEvent xmotion;
	void *user_data = NULL;

	// be aware: "ui" can be NULL during window creation (esp. if there is a debugger attached)
	//gx_AxisFaceUI *ui = (gx_AxisFaceUI *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	Widget_t *ui = (Widget_t *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
//debug_print("HWND:%p msg=%8.8x w=%p l=%p ui=%p state=%d\n",hwnd,msg,(void*)wParam,(void*)lParam,ui,(ui ? ui->state : 0));
#ifdef _debugwm
debug_wm(hwnd, msg, wParam, lParam, ui);
#endif

	xbutton.window = hwnd;
	xbutton.x = GET_X_LPARAM(lParam);
	xbutton.y = GET_Y_LPARAM(lParam);
	xmotion.window = hwnd;
	xmotion.x = GET_X_LPARAM(lParam);
	xmotion.y = GET_Y_LPARAM(lParam);

	switch (msg) {
		// MSWin only: React to close requests
		case WM_CLOSE:
			debug_print("WM:WM_CLOSE:hwnd=%p:ui=%p",hwnd,ui);
			DestroyWindow(hwnd);
			return 0;
		case WM_DESTROY:
			debug_print("WM:WM_DESTROY:hwnd=%p:ui=%p",hwnd,ui);
			PostQuitMessage(0);
			return 0;

		// X11:ConfigureNotify
		case WM_SIZE:
			if (!ui) return DefWindowProc(hwnd, msg, wParam, lParam);
if (!ui->func.configure_callback) return 0;
			ui->func.configure_callback(ui, user_data);
RedrawWindow(ui->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
			return 0;
		// X11:Expose
		case WM_PAINT:
			if (!ui) return DefWindowProc(hwnd, msg, wParam, lParam);
			return onPaint(hwnd, wParam, lParam); // not possible on mswin: (only fetch the last expose event)

		// MSWin only: Allow keyboard input
		case WM_ACTIVATE:
			SetFocus(hwnd);
			return 0;
		case WM_MOUSEACTIVATE:
			SetFocus(hwnd);
			return MA_ACTIVATE;

		// X11:ButtonPress
		case WM_LBUTTONDOWN:
			if (!ui) return DefWindowProc(hwnd, msg, wParam, lParam);
			SetCapture(hwnd); // also receive WM_MOUSEMOVE from outside this window
            if (ui->state == 4) break;
            if (ui->flags & HAS_TOOLTIP) hide_tooltip(ui);
			xbutton.button = Button1;
            _button_press(ui, &xbutton, user_data);
            debug_print("Widget_t  ButtonPress %i hwnd=%p\n", xbutton.button,hwnd);


			return 0;
		case WM_RBUTTONDOWN:
			if (!ui) return DefWindowProc(hwnd, msg, wParam, lParam);
            if (ui->state == 4) break;
			xbutton.button = Button3;
            _button_press(ui, &xbutton, user_data);
			return 0;
		case WM_MOUSEWHEEL:
			if (!ui) return DefWindowProc(hwnd, msg, wParam, lParam);
			// opposed to X11, WM_MOUSEWHEEL doesnt contain mouse coordinates
			{
				DWORD pos = GetMessagePos();
				pt.x = GET_X_LPARAM(pos);
				pt.y = GET_Y_LPARAM(pos);
				if (ScreenToClient(hwnd, &pt)) {
					ui->pos_x = pt.x;
					ui->pos_y = pt.y;
				}
			}
			if (GET_WHEEL_DELTA_WPARAM(wParam) <= 0) {
				xbutton.button = Button5;
				_button_press(ui, &xbutton, user_data);
			} else {
				xbutton.button = Button4;
				_button_press(ui, &xbutton, user_data);
			}
			// forward WM_MOUSEWHEEL from menuitem to viewport (with slider)
			// (viewport lies below menuitem, so doesnt receive WM_MOUSEWHEEL)
			if(ui->app->hold_grab) {
				Widget_t *view_port = ui->app->hold_grab->childlist->childs[0];
				if (hwnd != view_port->widget)
					SendMessage(view_port->widget, msg, wParam, lParam);
RedrawWindow(view_port->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
			}
			return 0;
		// X11:ButtonRelease
		case WM_LBUTTONUP:
			ReleaseCapture();
			xbutton.button = Button1;
            _check_grab(ui, &xbutton, ui->app);
            if (ui->state == 4) break;
            _has_pointer(ui, &xbutton);
            if(ui->flags & HAS_POINTER) ui->state = 1;
            else ui->state = 0;
            _check_enum(ui, &xbutton);
            ui->func.button_release_callback((void*)ui, &xbutton, user_data);
            debug_print("Widget_t  ButtonRelease %i hwnd=%p\n", xbutton.button,hwnd);
			return 0;
		case WM_RBUTTONUP:
            if (ui->state == 4) break;
			xbutton.button = Button3;
            ui->func.button_release_callback((void*)ui, &xbutton, user_data);
			return 0;

		// X11:KeyPress
		case WM_KEYUP:
			if (!ui) return DefWindowProc(hwnd, msg, wParam, lParam);
//			switch (key_mapping(wParam)) {
//				case 1: set_previous_controller_active(ui); // "-"
//				break;
//				case 2: set_next_controller_active(ui); // "+"
//				break;
//				case 3: key_event(ui, 1); // UP/RIGHT
//				break;
//				case 4: key_event(ui, -1); // DOWN/LEFT
//				break;
//				case 5: set_key_value(ui, 1); // HOME
//				break;
//				case 6: set_key_value(ui, 2); // INSERT
//				break;
//				case 7: set_key_value(ui, 3); // END
//				break;
//				default:
//				break;
//			}
			return DefWindowProc(hwnd, msg, wParam, lParam);

		// X11:LeaveNotify (X11:EnterNotify: see WM_MOUSEMOVE)
		case WM_MOUSELEAVE:
			if (!ui) return DefWindowProc(hwnd, msg, wParam, lParam);
			// close popup menu if cursor moves out of widget
            if(ui->app->hold_grab != NULL) {
				GetCursorPos(&pt);
				Window win_cur = WindowFromPoint(pt);
                bool is_item = false;
				// still inside viewport? (finds menu entries in popup window)
// crashes for message_dialog?
                Widget_t *view_port = ui->app->hold_grab->childlist->childs[0];
                int i = view_port->childlist->elem-1;
                for(;i>-1;i--) {
                    Widget_t *w = view_port->childlist->childs[i];
                    if (win_cur == w->widget) {
                        is_item = true;
                        break;
                    }
                }
				// still inside combobox? (finds combobox-button)
                Widget_t *cbx = NULL;
				if (ui->app->hold_grab->parent_struct) // combobox->parent is Window, not Widget_t!
					cbx = (Widget_t *)ui->app->hold_grab->parent_struct;
				else
					cbx = (Widget_t *)ui->app->hold_grab->parent;
                //Widget_t *cbx = (Widget_t *)ui->app->hold_grab->parent_struct;
                i = cbx->childlist->elem-1;
                for(;i>-1;i--) {
                    Widget_t *w = cbx->childlist->childs[i];
                    if (win_cur == w->widget) {
                        is_item = true;
                        break;
                    }
                }
                if (win_cur == view_port->widget) is_item = true; // inside slider area?
                if (win_cur == cbx->widget) is_item = true; // inside combobox textarea?
                if (!is_item) {
#ifdef _WIN32
					ReleaseCapture();
#else
                    XUngrabPointer(ui->dpy,CurrentTime);
#endif
                    widget_hide(ui->app->hold_grab);
                    ui->app->hold_grab = NULL;
                }
            }

			// for emulating X11:EnterNotify
			ui->mouse_inside = false;

            ui->flags &= ~HAS_FOCUS;
            if (ui->state == 4) break;
            //if(!(xev->xcrossing.state & Button1Mask)) {
			if (!(wParam & MK_LBUTTON)) {
                ui->state = 0;
                ui->func.leave_callback((void*)ui, user_data);
if (!(ui->flags & IS_WINDOW))
	RedrawWindow(hwnd, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
            }
            if (ui->flags & HAS_TOOLTIP) hide_tooltip(ui);
            debug_print("Widget_t LeaveNotify:hwnd=%p",hwnd);

			return 0;

		// X11:MotionNotify
		case WM_MOUSEMOVE:
			if (!ui) return DefWindowProc(hwnd, msg, wParam, lParam);
			if (!ui->mouse_inside) {
				// emulate X11:EnterNotify
				ui->mouse_inside = true;

				ui->flags |= HAS_FOCUS;
				if (ui->state == 4) break;
				//if(!(xev->xcrossing.state & Button1Mask)) {
				if (!(wParam & MK_LBUTTON)) {
					ui->state = 1;
					ui->func.enter_callback((void*)ui, user_data);
if (!(ui->flags & IS_WINDOW))
	RedrawWindow(hwnd, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
					if (ui->flags & HAS_TOOLTIP) show_tooltip(ui);
					else _hide_all_tooltips(ui);
				}
				debug_print("Widget_t EnterNotify:hwnd=%p",hwnd);

				SetMouseTracking(hwnd, true); // for receiving (next) WM_MOUSELEAVE
			}
//			// mouse move while button1 is pressed
			if (wParam & MK_LBUTTON) {
				if (ui->state == 4) return 0;
				adj_set_motion_state(ui, xmotion.x, xmotion.y);
				ui->func.motion_callback((void*)ui, &xmotion, user_data);
				debug_print("Widget_t MotionNotify x = %li Y = %li hwnd=%p\n",pt.x,pt.y,hwnd );
			}
			return 0;

		case WM_USER + 01: // WM_DELETE_WINDOW
			debug_print("WM:WM_DELETE_WINDOW:hwnd=%p:ui=%p",hwnd,ui);
			return 1;
#if 0
			if (ui) {
				//int i = childlist_find_widget(main->childlist, xev.xclient.window);
				int i = childlist_find_widget(ui->app->childlist, (Window)wParam);
				if(i<1) return 0;
				//Widget_t *w = main->childlist->childs[i];
				Widget_t *w = ui->app->childlist->childs[i];
				if(w->flags & HIDE_ON_DELETE) widget_hide(w);
				//else destroy_widget(w, main);
				else destroy_widget(w, ui->app);
				return 0;
			}
#endif
		// X11:ClientMessage:WIDGET_DESTROY
		case WM_USER + 02: // WIDGET_DESTROY
			debug_print("WM:WIDGET_DESTROY:hwnd=%p:ui=%p",hwnd,ui);
			return 2;
#if 0
			if (ui) {
				int ch = childlist_has_child(ui->childlist);
				if (ch) {
					int i = ch;
					for(;i>0;i--) {
						quit_widget(ui->childlist->childs[i-1]);
					}
					quit_widget(ui);
				} else {
					//destroy_widget(ui,ui->app);
				}
				return 0;
			}
#endif

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

LRESULT onPaint( HWND hwnd, WPARAM wParam, LPARAM lParam ) {
	PAINTSTRUCT ps ;
	//gx_AxisFaceUI *ui = (gx_AxisFaceUI *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	Widget_t *w = (Widget_t *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	// The cairo_win32_surface should only exist between BeginPaint()/EndPaint(),
	// otherwise it becomes unusable once the HDC of the owner window changes
	// (what can happen anytime, e.g. on resize).
	// Therefore, w->surface is created as a simple cairo_image_surface,
	// that can exist throughout the plugins lifetime (exception: see resize_event())
	// and is copied to a win32_surface in the onPaint() event (see WM_PAINT).

	// draw onto the image surface first
	transparent_draw(w, NULL);

	// prepare to update window
	HDC hdc = BeginPaint(hwnd, &ps );

	// create the cairo surface and context
	cairo_surface_t *surface = cairo_win32_surface_create (hdc);
	cairo_t *cr = cairo_create (surface);
	// copy contents of the (permanent) image_surface to the win32_surface
	cairo_set_source_surface(cr, w->surface, 0.0, 0.0);
	cairo_paint(cr);

	// cleanup
	cairo_destroy (cr);
	cairo_surface_destroy (surface);

	EndPaint( hwnd, &ps );
	return 0 ;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/

void SetClientSize(HWND hwnd, int clientWidth, int clientHeight) {
	if (IsWindow(hwnd)) {
		DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE) ;
		DWORD dwExStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE) ;
		HMENU menu = GetMenu(hwnd) ;
		RECT rc = {0, 0, clientWidth, clientHeight} ;
		AdjustWindowRectEx(&rc, dwStyle, menu ? TRUE : FALSE, dwExStyle);
		SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
					 SWP_NOZORDER | SWP_NOMOVE) ;
	}
}

// WM_MOUSELEAVE is only reported ONCE after calling TrackMouseEvent(TME_LEAVE)
BOOL SetMouseTracking(HWND hwnd, BOOL enable) {
	TRACKMOUSEEVENT tme;

	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	if (!enable)
		tme.dwFlags |= TME_CANCEL;
	tme.hwndTrack = hwnd;
	tme.dwHoverTime = HOVER_DEFAULT;
	return TrackMouseEvent(&tme);
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */
