/* vim:ts=4:sw=4:et:
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

#ifdef _WIN32 //PlatformWrappers

#ifdef __cplusplus
extern "C" {
#endif

#include "windowsx.h"

#include "xwidget.h"
#include "xwidget_private.h"

TCHAR szMainUIClassName[]   = TEXT("xputtyMainUI____0123456789ABCDEF");
TCHAR szWidgetUIClassName[] = TEXT("xputtyWidgetUI__0123456789ABCDEF");

// forward declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT onPaint( HWND hwnd, WPARAM wParam, LPARAM lParam );

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
			internal helper functions
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

void debug_lasterror(const char *prefix, const char *prefix2) {
	LPSTR msg = nullptr;
	DWORD err = GetLastError();
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER
                |FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&msg, 0, NULL);
	if (size) {
		debug_print("%s%s:ERR=%8.8lx (%ld): %s",prefix?prefix:"",prefix2?prefix2:"",err,err,msg);
		LocalFree(msg);
	} else {
		DWORD fmt_err = GetLastError();
		debug_print("%s%s:ERROR:FormatMessage for ERR=%8.8lx (%ld) returned %8.8lx (%ld)",
				(prefix ? prefix : ""), (prefix2 ? prefix2 : ""), err, err, fmt_err, fmt_err);
	}
}

/*---------------------------------------------------------------------
            codepage conversion
----------------------------------------------------------------------*/

// general codepage conversion
char *convert_cp(DWORD cp_from, DWORD cp_to, char *s_from) {
    char *s_to = NULL;
    int flags = MB_PRECOMPOSED; // | MB_ERR_INVALID_CHARS;
    // prepare conversion to WideChar (using cp_from) - get required space
    size_t size = MultiByteToWideChar(cp_from, flags, s_from, -1, NULL, 0);
    if (size) {
        // convert Ansi to WideChar (pwc)
        wchar_t *pwc= (wchar_t*)malloc(size*2);
        size_t size_wc = MultiByteToWideChar(cp_from, flags, s_from, -1, pwc, size);
        if (size_wc) {
            // prepare conversion to cp_to - get required space
            flags = 0;
            size = WideCharToMultiByte(cp_to, flags, pwc, size_wc, NULL, 0, NULL, NULL);
            if (size) {
                // convert WideChar (pwc) to Ansi using cp_to
                s_to = (char*)malloc(size+1);
                memset(s_to, 0 , size+1);
                size = WideCharToMultiByte(cp_to, flags, pwc, size_wc, s_to, size, NULL, NULL);
            }
        }
        free(pwc);
    }
    // needs to be free()d by caller
	return s_to;
}

// convert active codepage to utf8
char *utf8_from_locale(char *localestr) {
	return (convert_cp(GetACP(), CP_UTF8, localestr));
}

// convert utf8 to active codepage
char *locale_from_utf8(char *utf8str) {
	return (convert_cp(CP_UTF8, GetACP(), utf8str));
}

/*---------------------------------------------------------------------
            window settings
----------------------------------------------------------------------*/

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
-----------------------------------------------------------------------
			common functions (required)
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

bool os_get_keyboard_input(Widget_t *w, XKeyEvent *key, char *buf, size_t bufsize) {
	char ansibuf[2];
	ansibuf[0] = (char)key->vk;
	ansibuf[1] = 0;
	char *utf8 = utf8_from_locale(ansibuf);
	int l=min(bufsize, strlen(utf8));
	strncpy(buf, utf8, l);
	buf[l] = 0;
	free(utf8);
	return key->vk_is_final_char; // for only feeding readily processed chars into input widgets
}

void os_free_pixmap(Widget_t *w, Pixmap pixmap) {
    // Pixmap stuff not implemented at all
}

Display *os_open_display(char *display_name) {
	// nothing to do on MSWin
	return (Display *)1;
}

void os_close_display(Display *dpy) {
	// nothing to do on MSWin
}

void os_destroy_window(Widget_t *w) {
	debug_print("STUB:os_destroy_window:w=%p:hwnd=%p:%s",w,(w)?w->widget:NULL,widget_type_name(w));

	// mswin automatically sends WM_DESTROY to all child windows
	// floating windows need to be handled manually
	if (w && (IsWindow(w->widget))) {
		debug_print("STUB:os_destroy_window:DestroyWindow:hwnd=%p",(w)?w->widget:NULL);
		DestroyWindow(w->widget);
	} else {
		debug_print("STUB:os_destroy_window:DestroyWindow:NOTFOUND:hwnd=%p",(w)?w->widget:NULL);
	}
}

Window os_get_root_window(Widget_t *w) {
	return HWND_DESKTOP;
}

void os_translate_coords(Widget_t *w, Window from_window, Window to_window,
                          int from_x, int from_y, int *to_x, int *to_y) {
    POINT Point = {from_x, from_y};
	MapWindowPoints(from_window, to_window, &Point, 1);
    *to_x = Point.x;
    *to_y = Point.y;
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
	WNDCLASS wndclass = {0};
	HINSTANCE hInstance = NULL;

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
	SetParent(w->widget, win); // embed into parentWindow
	SetMouseTracking(w->widget, true); // for receiving WM_MOUSELEAVE
//diff:SizeHints?
//    win_size_hints = XAllocSizeHints();

}

void os_create_widget_window_and_surface(Widget_t *w, Xputty *app, Widget_t *parent,
                          int x, int y, int width, int height) {
  // STUB
	// prepare window class
	WNDCLASS wndclass = {0};
	HINSTANCE hInstance = NULL;

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
	// UnregisterClass silently fails, if there are still more windows of this class
	if (UnregisterClass(szMainUIClassName, NULL)) {
		debug_print("UnregisterMainClass:%s:OK", szMainUIClassName);
	} else
		debug_lasterror("UnregisterMainClass:", szMainUIClassName);

	if (UnregisterClass(szWidgetUIClassName, NULL)) {
		debug_print("UnregisterWidgetClass:%s:OK", szWidgetUIClassName);
	} else
		debug_lasterror("UnregisterWidgetClass" ,szWidgetUIClassName);

}
void os_quit_widget(Widget_t *w) {
	// who invokes this?
	WPARAM wParam = (WPARAM)w->widget;
	DWORD msg = os_register_widget_destroy(w);
	int res = SendMessage(w->widget, msg, wParam, 0); // WIDGET_DESTROY
	debug_print("STUB:os_quit_widget:w=%p:hwnd=%p:msg=%8.8lx:res=%d",w,(w)?w->widget:NULL,msg,res);
	// STUB
}

Atom os_register_wm_delete_window(Widget_t * wid) {
	Atom msg = WM_USER + 01;
	//Atom msg = RegisterWindowMessage("XPUTTY_WM_DELETE_WINDOW");
	debug_print("STUB:os_register_wm_delete_window:w=%p:msg=%8.8lx",wid,msg);
	return msg;
}

Atom os_register_widget_destroy(Widget_t * wid) {
	Atom msg = WM_USER + 02 ;
	//Atom msg = RegisterWindowMessage("XPUTTY_WIDGET_DESTROY");
	debug_print("STUB:os_register_widget_destroy:w=%p:msg=%8.8lx",wid,msg);
	return msg;
}

// os specific


/*------------- the event loop ---------------*/

void dumpkey(WORD xkey) {
	switch (xkey) {
		case XK_space:                    debug_print("XK_space:\n"); break;
		case XK_0:                        debug_print("XK_0:\n"); break;
		case XK_2:                        debug_print("XK_2:\n"); break;
		case XK_3:                        debug_print("XK_3:\n"); break;
		case XK_5:                        debug_print("XK_5:\n"); break;
		case XK_6:                        debug_print("XK_6:\n"); break;
		case XK_7:                        debug_print("XK_7:\n"); break;
		case XK_9:                        debug_print("XK_9:\n"); break;
		case XK_m:                        debug_print("XK_m:\n"); break;
		case XK_q:                        debug_print("XK_q:\n"); break;
		case XK_a:                        debug_print("XK_a:\n"); break;
		case XK_b:                        debug_print("XK_b:\n"); break;
		case XK_c:                        debug_print("XK_c:\n"); break;
		case XK_d:                        debug_print("XK_d:\n"); break;
		case XK_e:                        debug_print("XK_e:\n"); break;
		case XK_g:                        debug_print("XK_g:\n"); break;
		case XK_h:                        debug_print("XK_h:\n"); break;
		case XK_i:                        debug_print("XK_i:\n"); break;
		case XK_j:                        debug_print("XK_j:\n"); break;
		case XK_n:                        debug_print("XK_n:\n"); break;
		case XK_o:                        debug_print("XK_o:\n"); break;
		case XK_p:                        debug_print("XK_p:\n"); break;
		case XK_r:                        debug_print("XK_r:\n"); break;
		case XK_s:                        debug_print("XK_s:\n"); break;
		case XK_t:                        debug_print("XK_t:\n"); break;
		case XK_u:                        debug_print("XK_u:\n"); break;
		case XK_v:                        debug_print("XK_v:\n"); break;
		case XK_w:                        debug_print("XK_w:\n"); break;
		case XK_x:                        debug_print("XK_x:\n"); break;
		case XK_y:                        debug_print("XK_y:\n"); break;
		case XK_z:                        debug_print("XK_z:\n"); break;
		case XK_comma:                    debug_print("XK_comma:\n"); break;
		case XK_quotedbl:                 debug_print("XK_quotedbl:\n"); break;
		case XK_parenleft:                debug_print("XK_parenleft:\n"); break;
		case XK_minus:                    debug_print("XK_minus:\n"); break;
		case XK_plus:                     debug_print("XK_plus:\n"); break;
		case XK_agrave:                   debug_print("XK_agrave:\n"); break;
		case XK_ccedilla:                 debug_print("XK_ccedilla:\n"); break;
		case XK_eacute:                   debug_print("XK_eacute:\n"); break;
		case XK_egrave:                   debug_print("XK_egrave:\n"); break;
		case XK_udiaeresis:               debug_print("XK_udiaeresis:\n"); break;
		case XK_dead_circumflex:          debug_print("XK_dead_circumflex:\n"); break;
		case XK_dead_diaeresis:           debug_print("XK_dead_diaeresis:\n"); break;
		case XK_Tab:                      debug_print("XK_Tab:\n"); break;
		case XK_Up:                       debug_print("XK_Up:\n"); break;
		case XK_Right:                    debug_print("XK_Right:\n"); break;
		case XK_Down:                     debug_print("XK_Down:\n"); break;
		case XK_Left:                     debug_print("XK_Left:\n"); break;
		case XK_Home:                     debug_print("XK_Home:\n"); break;
		case XK_Insert:                   debug_print("XK_Insert:\n"); break;
		case XK_End:                      debug_print("XK_End:\n"); break;
		case XK_BackSpace:                debug_print("XK_BackSpace:\n"); break;
		case XK_KP_Subtract:              debug_print("XK_KP_Subtract:\n"); break;
		case XK_KP_Add:                   debug_print("XK_KP_Add:\n"); break;
		case XK_KP_Up:                    debug_print("XK_KP_Up:\n"); break;
		case XK_KP_Right:                 debug_print("XK_KP_Right:\n"); break;
		case XK_KP_Down:                  debug_print("XK_KP_Down:\n"); break;
		case XK_KP_Left:                  debug_print("XK_KP_Left:\n"); break;
		case XK_KP_Home:                  debug_print("XK_KP_Home:\n"); break;
		case XK_KP_Insert:                debug_print("XK_KP_Insert:\n"); break;
		case XK_KP_End:                   debug_print("XK_KP_End:\n"); break;
		case XK_KP_Enter:                 debug_print("XK_KP_Enter:\n"); break;
		case XK_Return:                   debug_print("XK_Return:\n"); break;
		default:
				debug_print("XK_UNKNOWN!!!:\n");
	}
}

void build_xkey_event(XKeyEvent *ev, UINT msg, WPARAM wParam, LPARAM lParam) {
	UINT uVirtKey = (UINT)wParam;
	UINT uScanCode = (UINT)(HIWORD(lParam) & 0x1FF);
	BYTE lpKeyState[256];
	if (GetKeyboardState(lpKeyState)) {
		//https://stackoverflow.com/questions/42667205/maximum-number-of-characters-output-from-win32-tounicode-toascii
		// required size for the return buffer isn't exactly clear, maybe 255, so 1K should be a safe guess
		WCHAR lpChar[1024];
		UINT uFlags = 0x04; // 1=menu is active, 4=dont change anything
		if (msg == WM_CHAR) {
			ev->vk = uVirtKey;
			ev->vk_is_final_char = 1;
		} else {
			ToUnicode(uVirtKey, uScanCode, lpKeyState, lpChar, 2, uFlags);
			ev->vk = lpChar[0];
			ev->vk_is_final_char = 0;
		}
	}
	// handle special characters (only in KEYUP/DOWN?)
	switch (uScanCode) {
		case 0x0029: ev->keycode = XK_dead_circumflex;	break;
		case 0x000e: ev->keycode = XK_BackSpace;		break;
		case 0x000f: ev->keycode = XK_Tab;				break;
		case 0x001c: ev->keycode = XK_Return;			break;
		case 0x0147: ev->keycode = XK_Home;				break;
		case 0x014b: ev->keycode = XK_Left;				break;
		case 0x0148: ev->keycode = XK_Up;				break;
		case 0x014d: ev->keycode = XK_Right;			break;
		case 0x0150: ev->keycode = XK_Down;				break;
		case 0x014f: ev->keycode = XK_End;				break;
		case 0x0152: ev->keycode = XK_Insert;			break;
		case 0x011c: ev->keycode = XK_KP_Enter;			break;
		case 0x0047: ev->keycode = XK_KP_Home;			break;
		case 0x004b: ev->keycode = XK_KP_Left;			break;
		case 0x0048: ev->keycode = XK_KP_Up;			break;
		case 0x004d: ev->keycode = XK_KP_Right;			break;
		case 0x0050: ev->keycode = XK_KP_Down;			break;
		case 0x004f: ev->keycode = XK_KP_End;			break;
		case 0x0052: ev->keycode = XK_KP_Insert;		break;
		case 0x004e: ev->keycode = XK_KP_Add;			break;
		case 0x004a: ev->keycode = XK_KP_Subtract;		break;
		default:
			if (ev->vk == 0xfc) //'ü'
				ev->keycode = XK_udiaeresis;
			else if (ev->vk == 0xdc) //'Ü'
				ev->keycode = XK_dead_diaeresis;
			else
				ev->keycode = ev->vk;
	}
	dumpkey(ev->keycode);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	XButtonEvent xbutton;
	XMotionEvent xmotion;
	XKeyEvent xkey;
	void *user_data = NULL;

	// be aware: "wid" can be NULL during window creation (esp. if there is a debugger attached)
	Widget_t *wid = (Widget_t *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	Xputty *main = wid ? wid-> app : NULL;

	xbutton.window = hwnd;
	xbutton.x = GET_X_LPARAM(lParam);
	xbutton.y = GET_Y_LPARAM(lParam);
	xmotion.window = hwnd;
	xmotion.x = GET_X_LPARAM(lParam);
	xmotion.y = GET_Y_LPARAM(lParam);

	switch (msg) {
		case WM_CREATE:
			debug_print("WM:WM_CREATE:hwnd=%p:wid=%p",hwnd,wid);
			{
				CREATESTRUCT *pCreate = (CREATESTRUCT *)lParam;
				wid = (Widget_t *)pCreate->lpCreateParams;
				// CreateWindowEx() hasnt returned yet, so wid->widget is not set
				wid->widget = hwnd;
				// make "wid" available in messageloop events via GetWindowLongPtr()
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)wid);
			}
			return 0;

		// MSWin only: React to close requests
		case WM_CLOSE:
			// standalone
			debug_print("WM:WM_CLOSE:hwnd=%p:wid=%p",hwnd,wid);
			if (hwnd == main->childlist->childs[0]->widget) {
				// is main window: end application
				PostQuitMessage(0); // end messageloop (continuing to main_quit())
			} else // is sub window (menu, dialog, ...): close
				DestroyWindow(hwnd);
			return 0;
		case WM_DESTROY:
			debug_print("WM:WM_DESTROY:hwnd=%p:wid=%p",hwnd,wid);
			if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
			// terminate app/messageloop if main window is destroyed
			//if (wid->app->childlist->elem && wid->app->childlist->childs[0] == wid)
			//	PostQuitMessage(0);
			return 0;

		// X11:ConfigureNotify
		case WM_SIZE:
			if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
            if (!wid->func.configure_callback) return 0;
			wid->func.configure_callback(wid, user_data);
            RedrawWindow(wid->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
			return 0;
		// X11:Expose
		case WM_PAINT:
			if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
			if (!(wid->crb)) {
				debug_print("WM_PAINT:BAILOUT:wid->crb==NULL");
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
		case WM_RBUTTONDOWN:
			if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
			SetCapture(hwnd); // also receive WM_MOUSEMOVE from outside this window
			if (msg == WM_LBUTTONDOWN)
				xbutton.button = Button1;
			else
				xbutton.button = Button3;
            if (wid->state == 4) break;
            if (wid->flags & HAS_TOOLTIP) hide_tooltip(wid);
            _button_press(wid, &xbutton, user_data);
            debug_print("Widget_t  ButtonPress %i\n", xbutton.button);
			return 0;
		case WM_MOUSEWHEEL:
			if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
			// opposed to X11, WM_MOUSEWHEEL doesnt contain mouse coordinates
			{
				DWORD pos = GetMessagePos();
				pt.x = GET_X_LPARAM(pos);
				pt.y = GET_Y_LPARAM(pos);
				if (ScreenToClient(hwnd, &pt)) {
					wid->pos_x = pt.x;
					wid->pos_y = pt.y;
				}
			}
			if (GET_WHEEL_DELTA_WPARAM(wParam) <= 0) {
				xbutton.button = Button5;
				_button_press(wid, &xbutton, user_data);
			} else {
				xbutton.button = Button4;
				_button_press(wid, &xbutton, user_data);
			}
			// forward WM_MOUSEWHEEL from menuitem to viewport (with slider)
			// (viewport lies below menuitem, so doesnt receive WM_MOUSEWHEEL)
			if(wid->app->hold_grab) {
				Widget_t *view_port = wid->app->hold_grab->childlist->childs[0];
				if (hwnd != view_port->widget)
					SendMessage(view_port->widget, msg, wParam, lParam);
                RedrawWindow(view_port->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
			}
			return 0;
		// X11:ButtonRelease
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			ReleaseCapture();
			if (msg == WM_LBUTTONUP)
				xbutton.button = Button1;
			else
				xbutton.button = Button3;
            _check_grab(wid, &xbutton, wid->app);
            if (wid->state == 4) break;
            _has_pointer(wid, &xbutton);
            if(wid->flags & HAS_POINTER) wid->state = 1;
            else wid->state = 0;
            _check_enum(wid, &xbutton);
            wid->func.button_release_callback((void*)wid, &xbutton, user_data);
            debug_print("Widget_t  ButtonRelease %i\n", xbutton.button);
			return 0;

		// X11:KeyPress and X11:KeyRelease
		// The resulting character (e.g. from dead-key combinations) cannot be
		// determined from WM_KEYUP or WM_KEYDOWN: WM_CHAR has to be used instead.
		// To workaround that, WM_CHAR fires key_press- and key_release_event()
		// after another, with the flag "->vk_is_final_char" set, so the client
		// code can differentiate between real KEYUP/DOWN and fake CHAR events.
		case WM_CHAR:
		case WM_KEYDOWN:
		case WM_KEYUP:
			build_xkey_event(&xkey, msg, wParam, lParam);
			// X11:KeyPress
			if (msg != WM_KEYUP) { // WM_KEYDOWN and WM_CHAR: key_press_callback()
				if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
				if (wid->state == 4) return 0;
				// on Linux, retrigger check happens in KeyRelease (WM_KEYUP)
				unsigned short is_retriggered = 0;
				if(wid->flags & NO_AUTOREPEAT) {
					if ((HIWORD(lParam) & KF_REPEAT) == KF_REPEAT)
						is_retriggered = 1;
				}
				if (!is_retriggered) {
					_check_keymap(wid, xkey);
					wid->func.key_press_callback((void *)wid, &xkey, user_data);
					debug_print("Widget_t KeyPress %x\n", xkey.keycode);
				}
			}
			//X11:KeyRelease
			if (msg != WM_KEYDOWN) { // WM_KEYUP and WM_CHAR: key_release_callback()
				if (wid->state == 4) return 0;
				// On MSWin, the REPEAT flag is always set for WM_KEYUP,
				// so the retrigger check has to take place in WM_KEYDOWN instead
				wid->func.key_release_callback((void *)wid, &xkey, user_data);
				debug_print("Widget_t KeyRelease %x\n", xkey.keycode);
			}
			return 0;

		// X11:LeaveNotify (X11:EnterNotify: see WM_MOUSEMOVE)
		case WM_MOUSELEAVE:
			if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
			// xputty -> xwidget: handled by "ButtonPress" event on Linux
			// close popup menu if cursor moves out of widget
            if(wid->app->hold_grab != NULL) {
				GetCursorPos(&pt);
				Window win_cur = WindowFromPoint(pt);
                bool is_item = false;
				// still inside viewport? (finds menu entries in popup window)
                Widget_t *view_port = wid->app->hold_grab->childlist->childs[0];
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
				menu = (Widget_t *)wid->app->hold_grab->parent_struct;
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
					ReleaseCapture();
                    widget_hide(wid->app->hold_grab);
                    wid->app->hold_grab = NULL;
                }
            }

			// for emulating X11:EnterNotify
			wid->mouse_inside = false;

            wid->flags &= ~HAS_FOCUS;
            if (wid->state == 4) break;
            //if(!(xev->xcrossing.state & Button1Mask)) {
			if (!(wParam & MK_LBUTTON)) {
                wid->state = 0;
                wid->func.leave_callback((void*)wid, user_data);
                if (!(wid->flags & IS_WINDOW))
                    RedrawWindow(hwnd, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
            }
            if (wid->flags & HAS_TOOLTIP) hide_tooltip(wid);
            debug_print("Widget_t LeaveNotify:hwnd=%p",hwnd);

			return 0;

		// X11:MotionNotify
		case WM_MOUSEMOVE:
			if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
			if (!wid->mouse_inside) {
				// emulate X11:EnterNotify
				wid->mouse_inside = true;

				wid->flags |= HAS_FOCUS;
				if (wid->state == 4) break;
				//if(!(xev->xcrossing.state & Button1Mask)) {
				if (!(wParam & MK_LBUTTON)) {
					wid->state = 1;
					wid->func.enter_callback((void*)wid, user_data);
                    if (!(wid->flags & IS_WINDOW))
                        RedrawWindow(hwnd, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
					if (wid->flags & HAS_TOOLTIP) show_tooltip(wid);
					else _hide_all_tooltips(wid);
				}
				debug_print("Widget_t EnterNotify:hwnd=%p",hwnd);

				SetMouseTracking(hwnd, true); // for receiving (next) WM_MOUSELEAVE
			}
			// hovering, etc.
			if (wid->state == 4) return 0;
			if (wParam & MK_LBUTTON) // TODO: why is this if() required here, but not on linux?
				adj_set_motion_state(wid, xmotion.x, xmotion.y);
			wid->func.motion_callback((void*)wid, &xmotion, user_data);
			debug_print("Widget_t MotionNotify x = %li Y = %li hwnd=%p\n",pt.x,pt.y,hwnd );
			return 0;

		case WM_USER + 01: // WM_DELETE_WINDOW
			{
			debug_print("WM:WM_DELETE_WINDOW:hwnd=%p:wid=%p",hwnd,wid);
#if 0
			Xputty * main = wid->app;
			// xwidget -> xputty (run_embedded())
			if (wid) {
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
			if (wid) {
				if (hwnd == main->childlist->childs[0]->widget) { // main window (this is not invoked for any other window?)
					main->run = false;
					destroy_widget(wid, main);
				} else {
					int i = childlist_find_widget(main->childlist, (Window)wParam);
					if(i<1) return 0;
					Widget_t *w = main->childlist->childs[i];
					if(w->flags & HIDE_ON_DELETE) widget_hide(w);
					else { destroy_widget(main->childlist->childs[i],main);
					}
				}
			}
#endif
			}
			return 1;
		// X11:ClientMessage:WIDGET_DESTROY
		case WM_USER + 02: // WIDGET_DESTROY
			debug_print("WM:WIDGET_DESTROY:hwnd=%p:wid=%p",hwnd,wid);
#if 1
			//os_widget_event_loop()
			if (wid) {
				int ch = childlist_has_child(wid->childlist);
				if (ch) {
					int i = ch;
					for(;i>0;i--) {
						quit_widget(wid->childlist->childs[i-1]);
					}
					quit_widget(wid);
				} else {
					destroy_widget(wid,wid->app);
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

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */
