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

#include "xchildlist.h"
#include "xchildlist_private.h"


void childlist_init(Childlist_t *childlist) {
    childlist->childs = (Widget_t**)malloc(sizeof(Widget_t*) * 4);
    assert(childlist->childs != NULL);
    memset(childlist->childs, 0, 4 * sizeof(Widget_t*));
    childlist->cap =4;
    childlist->size = sizeof(childlist);
    childlist->elem = 0;
    int i = 0;
    for(;i<childlist->cap;i++) {
        childlist->childs[i] = NULL;
    }
}

void childlist_destroy(Childlist_t *childlist) {
    if(childlist) free(childlist->childs);
}

void childlist_add_child(Childlist_t *childlist, Widget_t *child) {
#ifdef _WIN32
debug_print("childlist_add_child:list=%p:child=%p--hwnd=%p %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
    childlist,
    child,
    child->widget,
widget_type_name(child),
    0 != (child->flags & IS_WIDGET        )?"WID":"",
    0 != (child->flags & IS_WINDOW        )?"WIN":"",
    0 != (child->flags & IS_POPUP         )?"POP":"",
    0 != (child->flags & IS_RADIO         )?"RAD":"",
    0 != (child->flags & IS_TOOLTIP       )?"TIP":"",
    0 != (child->flags & USE_TRANSPARENCY )?"TRN":"",
    0 != (child->flags & HAS_FOCUS        )?"FOC":"",
    0 != (child->flags & HAS_POINTER      )?"PTR":"",
    0 != (child->flags & HAS_TOOLTIP      )?"HTT":"",
    0 != (child->flags & HAS_MEM          )?"MEM":"",
    0 != (child->flags & NO_AUTOREPEAT    )?"REP":"",
    0 != (child->flags & FAST_REDRAW      )?"FST":"",
    0 != (child->flags & HIDE_ON_DELETE   )?"HID":"",
    0 != (child->flags & REUSE_IMAGE      )?"IMG":"");
#endif
    if(!childlist) childlist_init(childlist);
    if (childlist->cap < childlist->elem+2) {
         _childlist_add_elem(childlist);
     }
    childlist->childs[childlist->elem] = child;
    debug_print("Childlist_t  add_child\n");
    if (child->flags & IS_WINDOW)
	os_register_wm_delete_window(child);
    childlist->elem +=1;
}

void childlist_remove_child(Childlist_t *childlist, Widget_t *child) {
    if (!childlist) return;
    int it = childlist_find_child(childlist, child);
    if(it >= 0){
        childlist->childs[it] = NULL;
        childlist->elem--;
        int i = it;
        for(;i<childlist->elem;i++) {
            childlist->childs[i] = childlist->childs[i+1];
        }
        childlist->childs[childlist->elem+1] = NULL;
    }
}

int childlist_find_child(Childlist_t *childlist, Widget_t *child) {
    int i = 0;
    for(;i<childlist->elem;i++) {
        if(childlist->childs[i] == child) {
            return i;
        }
    }
    return -1;
}

int childlist_find_widget(Childlist_t *childlist, Window child_window) {
    int i = childlist->elem-1;
    for(;i>-1;i--) {
        if(childlist->childs[i]->widget == child_window) {
            return i;
        }
    }
    return -1;
}

int childlist_has_child(Childlist_t *childlist) {
    return childlist->elem;
}
