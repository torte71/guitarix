/*
 * Copyright (C) 2020 Hermann Meyer, Andreas Degert
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                define PortIndex and plugin uri
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#include "gx_alembic.h"

/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                define controller numbers
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#define CONTROLS 5

/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                include the LV2 interface
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#include "lv2_plugin.cc"

/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                define the plugin settings
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

typedef struct {
    cairo_surface_t *screw;
    cairo_surface_t *logo;
} X11_UI_Private_t;

// setup a color theme
static void set_my_theme(Xputty *main) {
    main->color_scheme->normal = (Colors) {
         /* cairo    / r  / g  / b  / a  /  */
        /*fg */       { 0.85, 0.85, 0.85, 1.00},
        /*bg */       { 0.1, 0.1, 0.1, 1.0},
        /*base */     { 0.1, 0.1, 0.1, 1.0},
        /*text */     { 0.85, 0.85, 0.85, 1.00},
        /*shadow */   { 0.1, 0.1, 0.1, 0.2},
        /*frame */    { 0.0, 0.0, 0.0, 1.0},
        /*light */    { 0.1, 0.1, 0.1, 1.0}
    };

    main->color_scheme->prelight = (Colors) {
        /*fg */       { 1.0, 1.0, 1.0, 1.0},
        /*bg */       { 0.25, 0.25, 0.25, 1.0},
        /*base */     { 0.2, 0.2, 0.2, 1.0},
        /*text */     { 0.7, 0.7, 0.7, 1.0},
        /*shadow */   { 0.1, 0.1, 0.1, 0.4},
        /*frame */    { 0.3, 0.3, 0.3, 1.0},
        /*light */    { 0.3, 0.3, 0.3, 1.0}
    };

    main->color_scheme->selected = (Colors) {
        /*fg */       { 0.9, 0.9, 0.9, 1.0},
        /*bg */       { 0.2, 0.2, 0.2, 1.0},
        /*base */     { 0.1, 0.1, 0.1, 1.0},
        /*text */     { 1.0, 1.0, 1.0, 1.0},
        /*shadow */   { 0.18, 0.18, 0.18, 0.2},
        /*frame */    { 0.18, 0.18, 0.18, 1.0},
        /*light */    { 0.18, 0.18, 0.28, 1.0}
    };
}

// draw the window
static void draw_my_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    X11_UI* ui = (X11_UI*)w->parent_struct;
    X11_UI_Private_t *ps = (X11_UI_Private_t*)ui->private_ptr;
    widget_set_scale(w);
    cairo_set_source_surface (w->crb, w->image,0,0);
    cairo_paint (w->crb);

    cairo_set_source_surface (w->crb, ps->logo,60,40);
    cairo_paint (w->crb);

    cairo_set_source_surface (w->crb, ps->screw,10,10);
    cairo_paint (w->crb);
    cairo_set_source_surface (w->crb, ps->screw,10,w->scale.init_height-34);
    cairo_paint (w->crb);
    cairo_set_source_surface (w->crb, ps->screw,w->scale.init_width-34,w->scale.init_height-34);
    cairo_paint (w->crb);
    cairo_set_source_surface (w->crb, ps->screw,w->scale.init_width-34,10);
    cairo_paint (w->crb);
    widget_reset_scale(w);
}

void plugin_value_changed(X11_UI *ui, Widget_t *w, PortIndex index) {
    // do special stuff when needed
}

void plugin_set_window_size(int *w,int *h,const char * plugin_uri) {
    (*w) = 800; //initial width of main window
    (*h) = 100; //initial height of main window
}

const char* plugin_set_name() {
    return "GxAlembic"; //plugin name to display on UI
}

void plugin_create_controller_widgets(X11_UI *ui, const char * plugin_uri) {
    set_my_theme(&ui->main);
    widget_get_scaled_png(ui->win, LDVAR(studiopre_png));
    X11_UI_Private_t *ps =(X11_UI_Private_t*)malloc(sizeof(X11_UI_Private_t));;
    ps->screw = surface_get_png(ui->win,ps->screw,LDVAR(screwhead_png));
    ps->logo = surface_get_png(ui->win,ps->logo,LDVAR(redeyelogo_png));
    ui->private_ptr = (void*)ps;
    ui->win->func.expose_callback = draw_my_window;

    ui->widget[0] = add_my_image_knob(ui->widget[0], AUDIO_IN,"Gain", ui,240, 15, 64, 74);
    set_adjustment(ui->widget[0]->adj,0.5, 0.5, 0.0, 1.0, 0.01, CL_CONTINUOS);
    widget_get_png(ui->widget[0], LDVAR(studiopreknob_png));

    ui->widget[1] = add_my_image_knob(ui->widget[1], BASS,"Bass", ui,340, 15, 64, 74);
    set_adjustment(ui->widget[1]->adj,0.5, 0.5, 0.0, 1.0, 0.01, CL_CONTINUOS);
    widget_get_surface_ptr(ui->widget[1], ui->widget[0]);

    ui->widget[2] = add_my_image_knob(ui->widget[2], MIDDLE,"Middle", ui,440, 15, 64, 74);
    set_adjustment(ui->widget[2]->adj,0.5, 0.5, 0.0, 1.0, 0.01, CL_CONTINUOS);
    widget_get_surface_ptr(ui->widget[2], ui->widget[0]);

    ui->widget[3] = add_my_image_knob(ui->widget[3], TREBLE,"Treble", ui,540, 15, 64, 74);
    set_adjustment(ui->widget[3]->adj,0.5, 0.5, 0.0, 1.0, 0.01, CL_CONTINUOS);
    widget_get_surface_ptr(ui->widget[3], ui->widget[0]);

    ui->widget[4] = add_my_image_knob(ui->widget[4], VOLUME,"Volume", ui,640, 15, 64, 74);
    set_adjustment(ui->widget[4]->adj,0.5, 0.5, 0.0, 1.0, 0.01, CL_CONTINUOS);
    widget_get_surface_ptr(ui->widget[4], ui->widget[0]);
}

void plugin_cleanup(X11_UI *ui) {
    X11_UI_Private_t *ps = (X11_UI_Private_t*)ui->private_ptr;
    cairo_surface_destroy(ps->screw);
    cairo_surface_destroy(ps->logo);
    free(ps);
    ui->private_ptr = NULL;
}

void plugin_port_event(LV2UI_Handle handle, uint32_t port_index,
                        uint32_t buffer_size, uint32_t format,
                        const void * buffer) {
}
