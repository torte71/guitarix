#include "xputty.h"
#include "xwidgets.h"

/** your own expose function */
static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    cairo_set_source_rgb (w->crb, 1, 1, 1);
    cairo_paint (w->crb);
}

int main (int argc, char ** argv)
{
    /** acces the main struct */
    Xputty app;
    /** init the main struct */
    main_init(&app);
    /** create a Window on default root window */
#ifdef _WIN32
Widget_t *w = create_window(&app, 0, //DefaultRootWindow(app.dpy)
	0, 0, 300, 900);
#else
    Widget_t *w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 300, 900);
    /** acces Xlib function */
    XStoreName(app.dpy, w->widget, "Hello world");
#endif
    /** overwrite event handler with your own */
    w->func.expose_callback = draw_window;

int x=0,y=0,width=0,height=0;
height = 64; 

y += height + 4; height =     64; width = height ; Widget_t* button              = add_button(w, "buttonlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* on_off_button       = add_on_off_button(w, "on_off_buttonlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* toggle_button       = add_toggle_button(w, "toggle_buttonlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* image_toggle_button = add_image_toggle_button(w, "image_toggle_buttonlabel", x, y, width, height);
// widget->data: requires portindex binding
//y += height + 4; height =     64; width = height ; Widget_t* switch_image_button = add_switch_image_button(w, "switch_image_buttonlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* check_button        = add_check_button(w, "check_buttonlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* check_box           = add_check_box(w, "check_boxlabel", x, y, width, height);
y += height + 4; height =  1* 64; width = height ; Widget_t* combobox            = add_combobox(w, "comboboxlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* combobox_entry      = combobox_add_entry(combobox, "combobox_entrylabel");
y += height + 4; height =     64; width = height ; Widget_t* combobox_entry2     = combobox_add_entry(combobox, "combobox_entry2label");
y += height + 4; height =     64; width = height ; Widget_t* knob                = add_knob(w, "knoblabel", x, y, width, height);
// widget->data: requires portindex binding
//y += height + 4; height =     64; width = height ; Widget_t* image_knob          = add_image_knob(w, "label", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* label               = add_label(w, "label", x, y, width, height);

y = 0; x = 128;
y += height + 4; height =     64; width = height ; Widget_t* vmeter              = add_vmeter(w, "vmeterlabel", true, x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* hmeter              = add_hmeter(w, "hmeterlabel", true, x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* vslider             = add_vslider(w, "vsliderlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* hslider             = add_hslider(w, "hsliderlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* tuner               = add_tuner(w, "tunerlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* valuedisplay        = add_valuedisplay(w, "valuedisplaylabel", x, y, width, height);
y += height + 4; height =  1* 64; width = height ; Widget_t* listbox             = add_listbox(w, "listboxlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* listbox_entry       = listbox_add_entry(listbox, "listbox_entrylabel");
y += height + 4; height =  1* 64; width = height ; Widget_t* listview            = add_listview(w, "listviewlabel", x, y, width, height);

//y += height + 4; height =     64; width = height ; Widget_t* playhead            = add_playhead(w, "playheadlabel", Adjustment_t *clip, Adjustment_t *cut, x, y, width, height);

// not seen working on linux
//Widget_t* menu = create_menu(w, 25);
//Widget_t* menu_item           = menu_add_item(menu, "menu_itemlabel");
//Widget_t* menu_check_item     = menu_add_check_item(menu, "menu_check_itemlabel");
//Widget_t* menu_radio_item     = menu_add_radio_item(menu, "menu_radio_itemlabel");

add_tooltip(w, "tooltiplabel");

    /** map the Window to display */
    widget_show_all(w);
    /** run the event loop */
    main_run(&app);
    /** clean up after the event loop is finished */
    main_quit(&app);
    return 0;
}
