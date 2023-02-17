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


#include "xslider.h"
#include "xslider_private.h"


Widget_t* add_vslider(Widget_t *parent, const char * label,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
wid->widget_type = WT_VSLIDER;
    wid->label = label;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, 1.0,0.01, CL_CONTINUOS);
    wid->adj = wid->adj_y;
    wid->scale.gravity = ASPECT;
    wid->func.expose_callback = _draw_vslider;
    wid->func.enter_callback = transparent_draw;
    wid->func.leave_callback = transparent_draw;
    wid->func.button_release_callback = _slider_released;
    return wid;
}

Widget_t* add_hslider(Widget_t *parent, const char * label,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
wid->widget_type = WT_HSLIDER;
    wid->label = label;
    wid->adj_x = add_adjustment(wid,0.0, 0.0, 0.0, 1.0,0.01, CL_CONTINUOS);
    wid->adj = wid->adj_x;
    wid->scale.gravity = ASPECT;
    wid->func.expose_callback = _draw_hslider;
    wid->func.enter_callback = transparent_draw;
    wid->func.leave_callback = transparent_draw;
    wid->func.button_release_callback = _slider_released;
    return wid;
}
