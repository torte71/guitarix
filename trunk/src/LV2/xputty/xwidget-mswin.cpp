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

TCHAR szMainUIClassName[]   = TEXT("xputtyMainUI____0123456789ABCDEF");
TCHAR szWidgetUIClassName[] = TEXT("xputtyWidgetUI__0123456789ABCDEF");

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
void debug_lasterror(const char *prefix, const char *prefix2) {
	LPSTR msg = nullptr;
	DWORD err = GetLastError();
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								 NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, NULL);
	if (size) {
		debug_print("%s%s:ERR=%8.8lx (%ld): %s",prefix?prefix:"",prefix2?prefix2:"",err,err,msg);
		LocalFree(msg);
	} else {
		DWORD fmt_err = GetLastError();
		debug_print("%s%s:ERROR:FormatMessage for ERR=%8.8lx (%ld) returned %8.8lx (%ld)",
				(prefix ? prefix : ""), (prefix2 ? prefix2 : ""), err, err, fmt_err, fmt_err);
	}
}

void os_destroy_window(Widget_t *w) {
	debug_print("STUB:os_destroy_window:w=%p:hwnd=%p:%s",w,(w)?w->widget:NULL,widget_type_name(w));

	// mswin automatically sends WM_DESTROY to all child windows
	// floating windows need to be handled manually
	//if ((w) && ( (w->flags & IS_WINDOW)
	//		  || (w->flags & WT_MENU)
	//		  || (w->flags & WT_TOOLTIP)
	//		  || (w->flags & WT_FILE_DIALOG)
	//		  || (w->flags & WT_MESSAGE_DIALOG)
	//		  || (w->flags & WT_MIDI_KEYBOARD) )) {
	if (w && (IsWindow(w->widget))) {
		debug_print("STUB:os_destroy_window:DestroyWindow:hwnd=%p",(w)?w->widget:NULL);
		DestroyWindow(w->widget);
		//SendMessage(w->widget, WM_CLOSE, 0, 0);
	} else {
		debug_print("STUB:os_destroy_window:DestroyWindow:NOTFOUND:hwnd=%p",(w)?w->widget:NULL);
	}
	//UnregisterClass(TEXT("xputtyMainUIClass"), NULL);
	//UnregisterClass(TEXT("xputtyWidgetUIClass"), NULL);
	// STUB
}

void os_get_window_metrics(Widget_t *w_, Metrics_t *metrics) {
	RECT WindowRect;
	RECT ClientRect;
	POINT Point;
	Widget_t *parent = (Widget_t *)w_->parent;

	if (GetWindowRect(w_->widget, &WindowRect) \
	 && GetClientRect(w_->widget, &ClientRect)) {
		Point.x = WindowRect.left; // WindowRect has correct coords, but wrong width/height
		Point.y = WindowRect.top;  // ClientRect has correct width/height, but top/left == 0
		ScreenToClient(parent->widget, &Point); // "parent" is intentional (coords are relative to owner widget)
		metrics->x = Point.x;
		metrics->y = Point.y;
		metrics->width = ClientRect.right - ClientRect.left;
		metrics->height = ClientRect.bottom - ClientRect.top;
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
	WNDCLASS wndclass = {0};
	HINSTANCE hInstance = NULL;

debug_print("os_create_main_window_and_surface:x=%d:y=%d:w=%d:h=%d:w=%p:app=%p:win=%p\n",x,y,width,height,w,app,win);
	snprintf(szMainUIClassName+16, 16, "%p", WndProc);
	snprintf(szWidgetUIClassName+16, 16, "%p", WndProc);

	// create a permanent surface for drawing (see onPaint() event)
	w->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

//	wndclass.style		   = CS_HREDRAW | CS_VREDRAW; // clear on resize
	wndclass.lpfnWndProc   = WndProc;
	wndclass.hInstance	   = hInstance;
	wndclass.hCursor	   = LoadCursor(NULL, IDC_ARROW);
	//wndclass.hbrBackground =(HBRUSH)COLOR_WINDOW;
	wndclass.hbrBackground =NULL;
	wndclass.lpszClassName = szMainUIClassName;
	wndclass.cbWndExtra    = sizeof(w); // reserve space for SetWindowLongPtr
	RegisterClass(&wndclass);
	// TODO: pass window style (mainwindow,childwidget,popup,...) to create_window()
	// _WIN32 doesnt allow changing the style afterwards, as it is done in xmenu.cpp
	// (this also removes duplicate code for window/widget creation).
	// For the current situation it is sufficient to set popup style if parent is HWND_DESKTOP.
	DWORD dwStyle = 0;
	DWORD dwExStyle = 0;
	// dwExStyle:
	//   WS_EX_APPWINDOW - force(!) taskbar icon
	//   WS_EX_CONTROLPARENT - contains children, uses tab+cursor nav.
	//   WS_EX_NOPARENTNOTIFY - no PARENTNOTIFY on create/destroy
	//   WS_EX_TOOLWINDOW - no taskbar icon
	//   WS_EX_TOPMOST - above all others
	if (win == (HWND)-1) {
		// Dialogs with border
		dwStyle = WS_OVERLAPPEDWINDOW;
		dwExStyle = WS_EX_CONTROLPARENT;
		win = HWND_DESKTOP;
		// include border widths
		RECT Rect = {0};
		BOOL bMenu = false;
		Rect.right = width;
		Rect.bottom = height;
		if (AdjustWindowRectEx(&Rect, dwStyle, bMenu, dwExStyle)) {
			width = Rect.right - Rect.left;
			height = Rect.bottom - Rect.top;
		}
	} else
	if (win == HWND_DESKTOP) {
		// Floating without border (popup, tooltip)
		dwStyle = WS_POPUP;
		dwExStyle = WS_EX_CONTROLPARENT | WS_EX_TOOLWINDOW;
	} else {
		// Embedded widget
		dwStyle = WS_CHILD;
		dwExStyle = WS_EX_CONTROLPARENT | WS_EX_TOOLWINDOW;
	}
	// create the window
	w->widget = CreateWindowEx(dwExStyle, szMainUIClassName,
							TEXT("Draw Surface"), // lpWindowName
							dwStyle, // dwStyle
							CW_USEDEFAULT, CW_USEDEFAULT, // X, Y
							width, height, // nWidth, nHeight
							win, // hWndParent (no embeddeding takes place yet)
							NULL, hInstance, (LPVOID)w); // hMenu, hInstance, lpParam
debug_print("os_create_main_window_and_surface:w=%p:hwnd=%p:width=%d:height=%d",w,w->widget,width,height);
	SetParent(w->widget, win); // embed into parentWindow
	SetMouseTracking(w->widget, true); // for receiving WM_MOUSELEAVE
//diff:SizeHints?
//    win_size_hints = XAllocSizeHints();

}

void os_create_widget_window_and_surface(Widget_t *w, Xputty *app, Widget_t *parent,
                          int x, int y, int width, int height) {
  // STUB
	// prepare window class
//diff:classname
	WNDCLASS wndclass = {0};
	HINSTANCE hInstance = NULL;
printf("os_create_widget_window_and_surface:x=%d:y=%d:w=%d:h=%d:w=%p:app=%p:parent=%p\n",x,y,width,height,w,app,parent);

	// create a permanent surface for drawing (see onPaint() event)
	w->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

//	wndclass.style		   = CS_HREDRAW | CS_VREDRAW; // clear on resize
	wndclass.lpfnWndProc   = WndProc;
	wndclass.hInstance	   = hInstance;
	wndclass.hCursor	   = LoadCursor(NULL, IDC_ARROW);
	//wndclass.hbrBackground =(HBRUSH)COLOR_WINDOW;
	wndclass.hbrBackground = NULL;
	wndclass.lpszClassName = szWidgetUIClassName;
	wndclass.cbWndExtra    = sizeof(w); // reserve space for SetWindowLongPtr
	RegisterClass(&wndclass);
	// create the window
	DWORD dwExStyle = WS_EX_CONTROLPARENT;
	w->widget = CreateWindowEx(dwExStyle, szWidgetUIClassName,
							TEXT("Draw Surface"), // lpWindowName
							WS_CHILD, // dwStyle
							x, y, // X, Y
							width, height, // nWidth, nHeight
							parent->widget, // hWndParent (no embeddeding takes place yet)
							NULL, hInstance, (LPVOID)w); // hMenu, hInstance, lpParam
debug_print("os_create_widget_window_and_surface:w=%p:hwnd=%p",w,w->widget);
													//
	SetParent(w->widget, parent->widget); // embed into parentWindow
	SetMouseTracking(w->widget, true); // for receiving WM_MOUSELEAVE
//diff:no SizeHints

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
	debug_print("STUB:os_quit:w=%p",w);
	if (w) {
		WPARAM wParam = (WPARAM)get_toplevel_widget(w->app)->widget;
		DWORD msg = os_register_wm_delete_window(w);
		int res = SendMessage(w->widget, msg, wParam, 0); // WM_DELETE_WINDOW
		debug_print("STUB:os_quit:w=%p:hwnd/dest=%p:wPar/toplvl=%16.16llx:msg=%8.8lx:res=%d",w,(w)?w->widget:NULL,wParam,msg,res);
	}
	// STUB

	// UnregisterClass silently fails, if there are still more windows of this class
	if (UnregisterClass(szMainUIClassName, NULL)) {
		debug_print("UnregisterMainClass:%s:OK", szMainUIClassName);
	} else
		debug_lasterror("UnregisterMainClass:", szMainUIClassName);

	if (UnregisterClass(szWidgetUIClassName, NULL)) {
		debug_print("UnregisterWidgetClass:%s:OK",szWidgetUIClassName);
	} else
		debug_lasterror("UnregisterWidgetClass:%s",szWidgetUIClassName);

}
void os_quit_widget(Widget_t *w) {
	// who invokes this?
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
	Xputty *main = ui ? ui-> app : NULL;
//debug_print("HWND:%p msg=%8.8x w=%p l=%p ui=%p state=%d\n",hwnd,msg,(void*)wParam,(void*)lParam,ui,(ui ? ui->state : 0));
#ifdef _debugwm
debug_wm(hwnd, msg, wParam, lParam, ui, widget_type_name(ui));
#endif

	xbutton.window = hwnd;
	xbutton.x = GET_X_LPARAM(lParam);
	xbutton.y = GET_Y_LPARAM(lParam);
	xmotion.window = hwnd;
	xmotion.x = GET_X_LPARAM(lParam);
	xmotion.y = GET_Y_LPARAM(lParam);

	switch (msg) {
		case WM_CREATE:
			debug_print("WM:WM_CREATE:hwnd=%p:ui=%p",hwnd,ui);
			{
				CREATESTRUCT *pCreate = (CREATESTRUCT *)lParam;
				ui = (Widget_t *)pCreate->lpCreateParams;
				// CreateWindowEx() hasnt returned yet, so ui->widget is not set
				ui->widget = hwnd;
				// make "ui" available in messageloop events via GetWindowLongPtr()
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ui);
			}
			return 0;

		// MSWin only: React to close requests
		case WM_CLOSE:
			// standalone
			debug_print("WM:WM_CLOSE:hwnd=%p:ui=%p",hwnd,ui);
			if (hwnd == main->childlist->childs[0]->widget) {
				// is main window: end application
				PostQuitMessage(0); // end messageloop (continuing to main_quit())
			} else // is sub window (menu, dialog, ...): close
				DestroyWindow(hwnd);
			return 0;
		case WM_DESTROY:
			debug_print("WM:WM_DESTROY:hwnd=%p:ui=%p",hwnd,ui);
			if (!ui) return DefWindowProc(hwnd, msg, wParam, lParam);
			// terminate app/messageloop if main window is destroyed
			//if (ui->app->childlist->elem && ui->app->childlist->childs[0] == ui)
			//	PostQuitMessage(0);
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
			if (!(ui->crb)) {
				debug_print("WM_PAINT:BAILOUT:ui->crb==NULL");
				return 0;
			}
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
			// xputty -> xwidget: handled by "ButtonPress" event on Linux
			// close popup menu if cursor moves out of widget
            if(ui->app->hold_grab != NULL) {
				GetCursorPos(&pt);
				Window win_cur = WindowFromPoint(pt);
                bool is_item = false;
				// still inside viewport? (finds menu entries in popup window)
                Widget_t *view_port = ui->app->hold_grab->childlist->childs[0];
				if (view_port) { // should never be NULL, but who knows :)
					int i = view_port->childlist->elem-1;
					for(;i>-1;i--) {
						Widget_t *w = view_port->childlist->childs[i];
						if (win_cur == w->widget) {
							is_item = true;
							break;
						}
					}
					if (view_port && win_cur == view_port->widget) is_item = true; // inside slider area?
				}
				// still inside combobox? (finds combobox-button)
                Widget_t *menu = NULL;
				menu = (Widget_t *)ui->app->hold_grab->parent_struct;
				if (menu) { // can be NULL if not contained in combobox
					int i = menu->childlist->elem-1;
					for(;i>-1;i--) {
						Widget_t *w = menu->childlist->childs[i];
						if (win_cur == w->widget) {
							is_item = true;
							break;
						}
					}
					if (menu && win_cur == menu->widget) is_item = true; // inside combobox textarea?
				}
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
			// hovering, etc.
			if (ui->state == 4) return 0;
			if (wParam & MK_LBUTTON) // TODO: why is this if() required here, but not on linux?
				adj_set_motion_state(ui, xmotion.x, xmotion.y);
			ui->func.motion_callback((void*)ui, &xmotion, user_data);
			debug_print("Widget_t MotionNotify x = %li Y = %li hwnd=%p\n",pt.x,pt.y,hwnd );
			return 0;

		case WM_USER + 01: // WM_DELETE_WINDOW
			{
			debug_print("WM:WM_DELETE_WINDOW:hwnd=%p:ui=%p",hwnd,ui);
#if 0
			Xputty * main = ui->app;
			// xwidget -> xputty (run_embedded())
			if (ui) {
				int i = childlist_find_widget(main->childlist, (Window)wParam);
				if(i<1) return 0;
				Widget_t *w = main->childlist->childs[i];
				if(w->flags & HIDE_ON_DELETE) widget_hide(w);
				else destroy_widget(w, main);
				return 0;
			}
#endif
#if 1
			// xwidget -> xputty (main_run())
			if (ui) {
				if (hwnd == main->childlist->childs[0]->widget) { // main window (this is not invoked for any other window?)
					main->run = false;
					destroy_widget(ui, main);
				} else {
					int i = childlist_find_widget(main->childlist, (Window)wParam);
					if(i<1) return 0;
					Widget_t *w = main->childlist->childs[i];
					if(w->flags & HIDE_ON_DELETE) widget_hide(w);
					else destroy_widget(main->childlist->childs[i],main);
				}
			}
#endif
			}
			return 1;
		// X11:ClientMessage:WIDGET_DESTROY
		case WM_USER + 02: // WIDGET_DESTROY
			debug_print("WM:WIDGET_DESTROY:hwnd=%p:ui=%p",hwnd,ui);
#if 1
			//os_widget_event_loop()
			if (ui) {
				int ch = childlist_has_child(ui->childlist);
				if (ch) {
					int i = ch;
					for(;i>0;i--) {
						quit_widget(ui->childlist->childs[i-1]);
					}
					quit_widget(ui);
				} else {
					destroy_widget(ui,ui->app);
				}
				return 0;
			}
#endif
			return 2;

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
