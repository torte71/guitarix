#include "xputty.h"
#include "xwidgets.h"
//#include "dialogs/xfile-dialog.h" // missing xdgmime.h
#include "dialogs/xmessage-dialog.h"
#include "dialogs/xmidi_keyboard.h"

Widget_t *mainwin = NULL;

/** your own expose function */
static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    cairo_set_source_rgb (w->crb, 1, 1, 1);
    cairo_paint (w->crb);
}

static void quit_button_pressed(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *main = w->app->childlist->childs[0];
    DestroyWindow(main->widget);
}

Widget_t *message_dialog;
static void msg_button_pressed(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    //dialog_callback
    message_dialog = open_message_dialog(w, INFO_BOX, "message_dialog_title", "message_dialog_message|line2|line3", NULL);
    //message_dialog = open_message_dialog(w, WARNING_BOX, "message_dialog_title", "message_dialog_message|line2|line3", NULL);
    //message_dialog = open_message_dialog(w, ERROR_BOX, "message_dialog_title", "message_dialog_message|line2|line3", NULL);
    //message_dialog = open_message_dialog(w, QUESTION_BOX, "message_dialog_title", "message_dialog_message|line2|line3", NULL); // no user defined choices
    //message_dialog = open_message_dialog(w, SELECTION_BOX, "message_dialog_title", "message_dialog_message|line2|line3", "choice1|choice2|choice3");
    //message_dialog = open_message_dialog(w, ENTRY_BOX, "message_dialog_title", "message_dialog_message|line2|line3", NULL); // no user defined choices
}
Widget_t *midi_keyboard;
static void midi_button_pressed(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    midi_keyboard = open_midi_keyboard(w);
    widget_show(midi_keyboard);
}
Widget_t *menu;
static void menu_button_pressed(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    //pop_menu_show(mainwin, menu, 3, false); //void pop_menu_show(Widget_t *parent, Widget_t *menu, int elem, bool above);
    pop_menu_show(w, menu, 3, true); //void pop_menu_show(Widget_t *parent, Widget_t *menu, int elem, bool above);
}

int main (int argc, char ** argv)
{
    /** acces the main struct */
    Xputty app;
    /** init the main struct */
    main_init(&app);
    /** create a Window on default root window */
#ifdef _WIN32
    //mainwin = create_window(&app, HWND_DESKTOP, 0, 0, 300, 900);
    mainwin = create_window(&app, (HWND)-1, 0, 0, 300, 900);
    set_light_theme(&app);
#else
    Widget_t *mainwin = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 300, 900);
    /** acces Xlib function */
    XStoreName(app.dpy, mainwin->widget, "Hello world");
#endif
    /** overwrite event handler with your own */
    mainwin->func.expose_callback = draw_window;

int x=0,y=0,width=0,height=0;
height = 64;
y += height + 4; height =     64; width = height ; Widget_t* button              = add_button(mainwin, "buttonlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* on_off_button       = add_on_off_button(mainwin, "on_off_buttonlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* toggle_button       = add_toggle_button(mainwin, "toggle_buttonlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* image_toggle_button = add_image_toggle_button(mainwin, "image_toggle_buttonlabel", x, y, width, height);

y += height + 4; height =     64; width = height ; Widget_t* check_button        = add_check_button(mainwin, "check_buttonlabel", x, y, width, height);
//wrong width
y += height + 4; height =     64; width = height ; Widget_t* check_box           = add_check_box(mainwin, "check_boxlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* knob                = add_knob(mainwin, "knoblabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* label               = add_label(mainwin, "label", x, y, width, height);
// widget->data: requires portindex binding
//y += height + 4; height =     64; width = height ; Widget_t* switch_image_button = add_switch_image_button(mainwin, "switch_image_buttonlabel", x, y, width, height);
// widget->data: requires portindex binding
//y += height + 4; height =     64; width = height ; Widget_t* image_knob          = add_image_knob(mainwin, "label", x, y, width, height);
#if 1 // CRASH
y += height + 4; height =  1* 64; width = height ; Widget_t* combobox            = add_combobox(mainwin, "comboboxlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* combobox_entry      = combobox_add_entry(combobox, "combobox_entrylabel");
y += height + 4; height =     64; width = height ; Widget_t* combobox_entry2     = combobox_add_entry(combobox, "combobox_entry2label");
#endif

y = 0; x = 128;
// 2 nebeneinander
y += height + 4; height =     64; width = height ; Widget_t* vmeter              = add_vmeter(mainwin, "vmeterlabel", true, x, y, width, height);
// 2 untereinander
y += height + 4; height =     64; width = height ; Widget_t* hmeter              = add_hmeter(mainwin, "hmeterlabel", true, x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* vslider             = add_vslider(mainwin, "vsliderlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* hslider             = add_hslider(mainwin, "hsliderlabel", x, y, width, height);
#if 0
// crash if WS_OVERLAPPEDWINDOW
y += height + 4; height =     64; width = height ; Widget_t* tuner               = add_tuner(mainwin, "tunerlabel", x, y, width, height);
#endif
y += height + 4; height =     64; width = height ; Widget_t* valuedisplay        = add_valuedisplay(mainwin, "valuedisplaylabel", x, y, width, height);
#if 0
// crash after close
y += height + 4; height =  1* 64; width = height ; Widget_t* listbox             = add_listbox(mainwin, "listboxlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* listbox_entry       = listbox_add_entry(listbox, "listbox_entrylabel");
y += height + 4; height =  1* 64; width = height ; Widget_t* listview            = add_listview(mainwin, "listviewlabel", x, y, width, height);
#endif

Adjustment_t clip = {0};
Adjustment_t cut = {0};
y += height + 4; height =     64; width = height ; Widget_t* playhead            = add_playhead(mainwin, "playheadlabel", &clip, &cut, x, y, width, height);

//menu = create_menu(mainwin, 3*25);
menu = create_menu(image_toggle_button, 3*25);
Widget_t* menu_item           = menu_add_item(menu, "menu_itemlabel");
Widget_t* menu_check_item     = menu_add_check_item(menu, "menu_check_itemlabel");
Widget_t* menu_radio_item     = menu_add_radio_item(menu, "menu_radio_itemlabel");
image_toggle_button->func.button_press_callback = menu_button_pressed;
//pop_menu_show(mainwin, menu, 3, true); //void pop_menu_show(Widget_t *parent, Widget_t *menu, int elem, bool above);


//add_tooltip(mainwin, "tooltiplabel");

//unused; missing xdgmime.h
//y += height + 4; height =     64; width = height ; Widget_t* file_button         = add_file_button(mainwin, "file_buttonlabel", x, y, width, height, "C:/", "");


// close window button
button->func.button_press_callback = quit_button_pressed;
on_off_button->func.button_press_callback = msg_button_pressed;
toggle_button->func.button_press_callback = midi_button_pressed;

    /** map the Window to display */
    widget_show_all(mainwin);
    /** run the event loop */
    main_run(&app);
MSG msg;
while(GetMessage(&msg, NULL, 0, 0))
{
    TranslateMessage(&msg);
    DispatchMessage(&msg);
}
printf("loop exit\n");

    /** clean up after the event loop is finished */
    main_quit(&app);
    return 0;
}
