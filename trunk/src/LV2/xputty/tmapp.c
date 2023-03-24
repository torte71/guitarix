#include "xputty.h"
#include "xwidgets.h"
#include "dialogs/xfile-dialog.h"
#include "dialogs/xmessage-dialog.h"
#include "dialogs/xmidi_keyboard.h"

Widget_t *mainwin = NULL;
Widget_t *menu = NULL;

/** your own expose function */
static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    cairo_set_source_rgb (w->crb, 1, 1, 1);
    cairo_paint (w->crb);
}

static void quit_button_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *main = w->app->childlist->childs[0];

#ifdef _WIN32
    SendMessage(main->widget, WM_CLOSE, 0, 0);
#endif
}
static void destroy_button_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *main = w->app->childlist->childs[0];
printf("destroy_button_released:w=%p:%s:u=%p:btn=%p:%s:main=%p:%s\n",w, widget_type_name((Widget_t*)w), user_data,
    button_, widget_type_name((Widget_t*)button_),
    main, widget_type_name(main) );
  //endless loop
  destroy_widget(main, w->app);
  quit_widget(main);
}

void msg_button_dialog_callback(void * widget, void* user_data) {
  printf("msg_button_dialog_callback:w=%p:u=%p:%s\n",widget,user_data,widget_type_name((Widget_t*)widget));
  int i = *(int*)user_data;
  char *str = *(char**)user_data;
  printf("*user_data=%d\n",i);
  printf("str=%s\n",str);
  char *cv = convert_cp(CP_UTF8, 850, str); // IBM850 = DOS Latin1 for console
  printf("cv=%s\n",cv);
}
Widget_t *message_dialog;
static void msg_button_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    //dialog_callback
    //message_dialog = open_message_dialog(w, INFO_BOX, "message_dialog_title", "message_dialog_message|line2|line3", NULL);
    //message_dialog = open_message_dialog(w, WARNING_BOX, "message_dialog_title", "message_dialog_message|line2|line3", NULL);
    //message_dialog = open_message_dialog(w, ERROR_BOX, "message_dialog_title", "message_dialog_message|line2|line3", NULL);
    //message_dialog = open_message_dialog(w, QUESTION_BOX, "message_dialog_title", "message_dialog_message|line2|line3", NULL); // no user defined choices
    //message_dialog = open_message_dialog(w, SELECTION_BOX, "message_dialog_title", "message_dialog_message|line2|line3", "choice1|choice2|choice3");
    message_dialog = open_message_dialog(w, ENTRY_BOX, "message_dialog_title", "message_dialog_message|line2|line3", NULL); // no user defined choices
}
Widget_t *midi_keyboard;
static void midi_button_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    midi_keyboard = open_midi_keyboard(w);
    widget_show(midi_keyboard);
}
static void menu_button_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;

    Widget_t *menuwin = create_window(w->app, (HWND)-1, 0, 0, 200, 100);
    menu = create_menu(menuwin, 3*25);
    menu->parent_struct = w; // for check_grab()
    Widget_t* menu_item           = menu_add_item(menu, "menu_itemlabel");
    Widget_t* menu_check_item     = menu_add_check_item(menu, "menu_check_itemlabel");
    Widget_t* menu_radio_item     = menu_add_radio_item(menu, "menu_radio_itemlabel");
    //pop_menu_show(mainwin, menu, 3, false); //void pop_menu_show(Widget_t *parent, Widget_t *menu, int elem, bool above);
    pop_menu_show(w, menu, 3, true); //void pop_menu_show(Widget_t *parent, Widget_t *menu, int elem, bool above);
}

Widget_t *file_dialog;
static void file_button_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    file_dialog = open_file_dialog(w, "c:\\", "*");
}


int main (int argc, char ** argv)
{
    /** acces the main struct */
    Xputty app;
    /** init the main struct */
    main_init(&app);
    /** create a Window on default root window */
#ifdef _WIN32 //WindowBorders
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
#if 1 // msg
#if 1 //small
y += height + 4; height =     64; width = height ; Widget_t* on_off_button       = add_on_off_button(mainwin, "on_off_buttonlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* toggle_button       = add_toggle_button(mainwin, "toggle_buttonlabel", x, y, width, height);
#endif
y += height + 4; height =     64; width = height ; Widget_t* image_toggle_button = add_image_toggle_button(mainwin, "image_toggle_buttonlabel", x, y, width, height);

y += height + 4; height =     64; width = height ; Widget_t* check_button        = add_check_button(mainwin, "check_buttonlabel", x, y, width, height);
#if 1 //small
//wrong width
y += height + 4; height =     64; width = height ; Widget_t* check_box           = add_check_box(mainwin, "check_boxlabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* knob                = add_knob(mainwin, "knoblabel", x, y, width, height);
y += height + 4; height =     64; width = height ; Widget_t* label               = add_label(mainwin, "label", x, y, width, height);
// widget->data: requires portindex binding
//y += height + 4; height =     64; width = height ; Widget_t* switch_image_button = add_switch_image_button(mainwin, "switch_image_buttonlabel", x, y, width, height);
// widget->data: requires portindex binding
//y += height + 4; height =     64; width = height ; Widget_t* image_knob          = add_image_knob(mainwin, "label", x, y, width, height);
#if 1
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
#if 1
// crash after close? seems gone
y += height + 4; height =  2* 64; width = height ; Widget_t* listbox             = add_listbox(mainwin, "listboxlabel", x, y, width, height);
y += height + 4; height =  0* 64; width = height ; Widget_t* listbox_entry       = listbox_add_entry(listbox, "listbox_entrylabel");
y += height + 4; height =  0* 64; width = height ; Widget_t* listbox_entry2      = listbox_add_entry(listbox, "listbox_entry2label");
y += height + 4; height =  0* 64; width = height ; Widget_t* listbox_entry3      = listbox_add_entry(listbox, "listbox_entry3label");
y += height + 4; height =  1* 64; width = height ; Widget_t* listview            = add_listview(mainwin, "listviewlabel", x, y, width, height);
#endif

Adjustment_t clip = {0};
Adjustment_t cut = {0};
y += height + 4; height =     64; width = height ; Widget_t* playhead            = add_playhead(mainwin, "playheadlabel", &clip, &cut, x, y, width, height);

//add_tooltip(mainwin, "tooltiplabel");

//unused; missing xdgmime.h
//y += height + 4; height =     64; width = height ; Widget_t* file_button         = add_file_button(mainwin, "file_buttonlabel", x, y, width, height, "C:/", "");

#endif

#if 1 //small
on_off_button->func.button_release_callback = msg_button_released;
toggle_button->func.button_release_callback = midi_button_released;
//check_button->func.button_release_callback = destroy_button_released;
check_button->func.button_release_callback = file_button_released;
//on_off_button->func.dialog_callback = msg_button_dialog_callback;
on_off_button->func.dialog_callback = msg_button_dialog_callback;
#endif
image_toggle_button->func.button_release_callback = menu_button_released;
//image_toggle_button->func.button_release_callback = file_button_released;

// close window button
//button->func.button_release_callback = quit_button_released;
button->func.button_release_callback = file_button_released;
#endif //msg

    /** map the Window to display */
    widget_show_all(mainwin);
    /** run the event loop */
    main_run(&app);

    /** clean up after the event loop is finished */
    main_quit(&app);
    return 0;
}
