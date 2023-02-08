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
  // STUB
}

void os_get_window_metrics(Widget_t *w_, Metrics_t *metrics) {
  RECT Rect = {0};
  if (GetWindowRect(w_->widget, &Rect)) {
	  metrics->x = Rect.left;
	  metrics->y = Rect.top;
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
	  x, y, 0, 0, SWP_NOSIZE); //X, Y, width, height, uFlags
////SWP_NOREDRAW
}

void os_resize_window(Display *dpy, Widget_t *w, int x, int y) {
	SetWindowPos(w->widget, NULL, //hWnd, hWndInsertAfter
	  0, 0, x, y, SWP_NOMOVE); //X, Y, width, height, uFlags
}

void os_create_main_window_and_surface(Widget_t *w, Xputty *app, Window win,
                          int x, int y, int width, int height) {
	// prepare window class
//diff:classname
	static TCHAR szClassName[] = TEXT("xputtyMainUIClass");
	WNDCLASS wndclass = {0};
	HINSTANCE hInstance = NULL;
printf("os_create_main_window_and_surface:x=%d:y=%d:w=%d:h=%d\n",x,y,width,height);

//	wndclass.style		   = CS_HREDRAW | CS_VREDRAW; // clear on resize
	wndclass.lpfnWndProc   = WndProc;
	wndclass.hInstance	   = hInstance;
	wndclass.hCursor	   = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground =(HBRUSH)COLOR_WINDOW;
	wndclass.lpszClassName = szClassName;
	wndclass.cbWndExtra    = sizeof(w); // reserve space for SetWindowLongPtr
	RegisterClass(&wndclass);
	// create the window
	w->widget = CreateWindowEx(WS_EX_TOPMOST, // dwExStyle
							szClassName, // lpClassName
							TEXT("Draw Surface"), // lpWindowName
							(WS_CHILD | WS_VISIBLE), // dwStyle
							CW_USEDEFAULT, CW_USEDEFAULT, // X, Y
							width, height, // nWidth, nHeight
//diff:parent=win
							win, // hWndParent (no embeddeding takes place yet)
							NULL, hInstance, NULL); // hMenu, hInstance, lpParam
													//
	// attach a pointer to "w" to this window (so w is available in WndProc)
	SetWindowLongPtr(w->widget, GWLP_USERDATA, (LONG_PTR)w);
//	SetParent(w->widget, win); // embed into parentWindow
//	ShowWindow(w->widget, SW_SHOW);
ShowWindow(w->widget, SW_SHOWNORMAL);
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
printf("os_create_main_window_and_surface:x=%d:y=%d:w=%d:h=%d\n",x,y,width,height);

//	wndclass.style		   = CS_HREDRAW | CS_VREDRAW; // clear on resize
	wndclass.lpfnWndProc   = WndProc;
	wndclass.hInstance	   = hInstance;
	wndclass.hCursor	   = LoadCursor(NULL, IDC_ARROW);
//	wndclass.hbrBackground =(HBRUSH)COLOR_WINDOW;
wndclass.hbrBackground = NULL;
	wndclass.lpszClassName = szClassName;
	wndclass.cbWndExtra    = sizeof(w); // reserve space for SetWindowLongPtr
	RegisterClass(&wndclass);
	// create the window
	w->widget = CreateWindowEx(WS_EX_TOPMOST, // dwExStyle
							szClassName, // lpClassName
							TEXT("Draw Surface"), // lpWindowName
							(WS_CHILD | WS_VISIBLE), // dwStyle
							x, y, // X, Y
							width, height, // nWidth, nHeight
//diff:parent=parent->widget
							parent->widget, // hWndParent (no embeddeding takes place yet)
							NULL, hInstance, NULL); // hMenu, hInstance, lpParam
													//
	// attach a pointer to "w" to this window (so w is available in WndProc)
	SetWindowLongPtr(w->widget, GWLP_USERDATA, (LONG_PTR)w);
	SetParent(w->widget, parent->widget); // embed into parentWindow
//	ShowWindow(w->widget, SW_SHOW);
ShowWindow(w->widget, SW_SHOWNORMAL);
	SetClientSize(w->widget, width, height);
	SetMouseTracking(w->widget, true); // for receiving WM_MOUSELEAVE
//diff:no SizeHints

	// create a permanent surface for drawing (see onPaint() event)
	w->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height); 
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
RedrawWindow(w->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
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

Atom os_register_wm_delete_window(Widget_t * wid) {
  return 0; // STUB
}

// os specific

int key_mapping(Display *dpy, XKeyEvent *xkey) {
  return 0; // STUB
}

/*------------- the event loop ---------------*/

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static double start_value = 0.0;
	static bool blocked = false;
	POINT pt;

	// be aware: "ui" can be NULL during window creation (esp. if there is a debugger attached)
	//gx_AxisFaceUI *ui = (gx_AxisFaceUI *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	Widget_t *ui = (Widget_t *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
printf("HWND:%p msg=%8.8x w=%p l=%p ui=%p\n",hwnd,msg,(void*)wParam,(void*)lParam,ui);

	switch (msg) {
		// MSWin only: React to close requests
		case WM_CLOSE:
			DestroyWindow(hwnd);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		// X11:ConfigureNotify
		case WM_SIZE:
			if (!ui) return DefWindowProc(hwnd, msg, wParam, lParam);
ui->func.configure_callback(ui, NULL);
RedrawWindow(ui->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
//			resize_event(ui); // configure event, we only check for resize events here
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
//			ui->pos_x = GET_X_LPARAM(lParam);
//			ui->pos_y = GET_Y_LPARAM(lParam);
//			blocked = true;
//			button1_event(ui, &start_value); // left mouse button click
			return 0;
		case WM_MOUSEWHEEL:
			if (!ui) return DefWindowProc(hwnd, msg, wParam, lParam);
			// opposed to X11, WM_MOUSEWHEEL doesnt contain mouse coordinates
//			if (GetCursorPos(&pt) && ScreenToClient(hwnd, &pt)) {
//				ui->pos_x = pt.x;
//				ui->pos_y = pt.y;
//			}
//			if (GET_WHEEL_DELTA_WPARAM(wParam) <= 0)
//				scroll_event(ui, -1); // mouse wheel scroll down
//			else
//				scroll_event(ui, 1); // mouse wheel scroll up
			return 0;
		// X11:ButtonRelease
		case WM_LBUTTONUP:
			blocked = false;
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
//			ui->mouse_inside = false;
//			if (!blocked) get_last_active_controller(ui, false);
			return 0;

		// X11:MotionNotify
		case WM_MOUSEMOVE:
			if (!ui) return DefWindowProc(hwnd, msg, wParam, lParam);
//			if (!ui->mouse_inside) {
//				// emulate X11:EnterNotify
//				ui->mouse_inside = true;
//				if (!blocked) get_last_active_controller(ui, true);
//				SetMouseTracking(ui->win, true); // for receiving (next) WM_MOUSELEAVE
//			}
//			// mouse move while button1 is pressed
//			if (wParam & MK_LBUTTON) {
//				motion_event(ui, start_value, GET_Y_LPARAM(lParam));
//			}
			return 0;

		// X11:ClientMessage: not implemented (could be done with WM_USER / RegisterWindowMessage())

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
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
//	_expose(w);

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
