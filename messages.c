#include <string.h>
#include <stdlib.h>
#include "helpers.h"
#include "luautils.h"
#include "settings.h"
#include "messages.h"
#include "common.h"
#include "types.h"
#include "bspwm.h"
#include "utils.h"
#include "tree.h"

void process_message(char *msg, char *rsp)
{
    char *cmd = strtok(msg, TOKEN_SEP);

    if (cmd == NULL)
        return;

    if (strcmp(cmd, "get") == 0) {
        char *name = strtok(NULL, TOKEN_SEP);
        get_setting(name, rsp);
    } else if (strcmp(cmd, "set") == 0) {
        char *name = strtok(NULL, TOKEN_SEP);
        char *value = strtok(NULL, TOKEN_SEP);
        set_setting(name, value);
    } else if (strcmp(cmd, "dump") == 0) {
        dump_tree(desk->root, rsp, 0);
    } else if (strcmp(cmd, "layout") == 0) {
        char *lyt = strtok(NULL, TOKEN_SEP);
        if (lyt != NULL) {
            layout_t l;
            if (parse_layout(lyt, &l)) {
                desk->layout = l;
                apply_layout(desk, desk->root);
            }
        }
    } else if (strcmp(cmd, "split_ratio") == 0) {
        char *value = strtok(NULL, TOKEN_SEP);
        split_ratio_cmd(value);
    } else if (strcmp(cmd, "presel") == 0) {
        char *dir = strtok(NULL, TOKEN_SEP);
        if (dir != NULL) {
            direction_t d;
            if (parse_direction(dir, &d)) {
                split_mode = MODE_MANUAL;
                split_dir = d;
                draw_triple_border(desk->focus, active_border_color_pxl);
            }
        }
    } else if (strcmp(cmd, "push") == 0 || strcmp(cmd, "pull") == 0) {
        char *dir = strtok(NULL, TOKEN_SEP);
        if (dir != NULL) {
            fence_move_t m;
            direction_t d;
            if (parse_fence_move(cmd, &m) && parse_direction(dir, &d))
                move_fence(desk->focus, d, m);
        }
    } else if (strcmp(cmd, "send_to") == 0) {
        char *name = strtok(NULL, TOKEN_SEP);
        if (name != NULL) {
            desktop_t *d = find_desktop(name);
            transfer_node(desk, d, desk->focus);
        }
    } else if (strcmp(cmd, "use") == 0) {
        char *name = strtok(NULL, TOKEN_SEP);
        if (name != NULL) {
            desktop_t *d = find_desktop(name);
            select_desktop(d);
        }
    } else if (strcmp(cmd, "cycle") == 0) {
        char *dir = strtok(NULL, TOKEN_SEP);
        if (dir != NULL) {
            cycle_dir_t d;
            if (parse_cycle_direction(dir, &d)) {
                skip_client_t k;
                char *skip = strtok(NULL, TOKEN_SEP);
                if (parse_skip_client(skip, &k))
                    cycle_leaf(desk, desk->focus, d, k);
            }
        }
    } else if (strcmp(cmd, "alternate") == 0) {
        alternate_desktop();
    } else if (strcmp(cmd, "add") == 0) {
        char *name = strtok(NULL, TOKEN_SEP);
        if (name != NULL) {
            add_desktop(name);
        }
    } else if (strcmp(cmd, "move") == 0) {
        char *dir = strtok(NULL, TOKEN_SEP);
        if (dir != NULL) {
            direction_t d;
            if (parse_direction(dir, &d)) {
                node_t *n = find_neighbor(desk->focus, d);
                focus_node(desk, n);
            }
        }
    } else if (strcmp(cmd, "quit") == 0) {
        quit();
    }
}

void get_setting(char *name, char* rsp)
{
    if (name == NULL)
        return;

    if (strcmp(name, "inner_border_width") == 0)
        sprintf(rsp, "%u\n", inner_border_width);
    else if (strcmp(name, "main_border_width") == 0)
        sprintf(rsp, "%u\n", main_border_width);
    else if (strcmp(name, "outer_border_width") == 0)
        sprintf(rsp, "%u\n", outer_border_width);
    else if (strcmp(name, "border_width") == 0)
        sprintf(rsp, "%u\n", border_width);
    else if (strcmp(name, "window_gap") == 0)
        sprintf(rsp, "%i\n", window_gap);
    else if (strcmp(name, "left_padding") == 0)
        sprintf(rsp, "%i\n", left_padding);
    else if (strcmp(name, "right_padding") == 0)
        sprintf(rsp, "%i\n", right_padding);
    else if (strcmp(name, "top_padding") == 0)
        sprintf(rsp, "%i\n", top_padding);
    else if (strcmp(name, "bottom_padding") == 0)
        sprintf(rsp, "%i\n", bottom_padding);
    else if (strcmp(name, "normal_border_color") == 0)
        sprintf(rsp, "%s\n", normal_border_color);
    else if (strcmp(name, "active_border_color") == 0)
        sprintf(rsp, "%s\n", active_border_color);
    else if (strcmp(name, "inner_border_color") == 0)
        sprintf(rsp, "%s\n", inner_border_color);
    else if (strcmp(name, "outer_border_color") == 0)
        sprintf(rsp, "%s\n", outer_border_color);
    else if (strcmp(name, "presel_border_color") == 0)
        sprintf(rsp, "%s\n", presel_border_color);
    else if (strcmp(name, "locked_border_color") == 0)
        sprintf(rsp, "%s\n", locked_border_color);
    else if (strcmp(name, "wm_name") == 0)
        sprintf(rsp, "%s\n", wm_name);
    else if (strcmp(name, "adaptive_window_border") == 0)
        sprintf(rsp, "%s\n", BOOLSTR(adaptive_window_border));
    else if (strcmp(name, "adaptive_window_gap") == 0)
        sprintf(rsp, "%s\n", BOOLSTR(adaptive_window_gap));
}

void set_setting(char *name, char *value)
{
    if (name == NULL || value == NULL)
        return;

    if (strcmp(name, "inner_border_width") == 0) {
        sscanf(value, "%u", &inner_border_width);
        border_width = inner_border_width + main_border_width + outer_border_width;
    } else if (strcmp(name, "main_border_width") == 0) {
        sscanf(value, "%u", &main_border_width);
        border_width = inner_border_width + main_border_width + outer_border_width;
    } else if (strcmp(name, "outer_border_width") == 0) {
        sscanf(value, "%u", &outer_border_width);
        border_width = inner_border_width + main_border_width + outer_border_width;
    } else if (strcmp(name, "window_gap") == 0) {
        sscanf(value, "%i", &window_gap);
    } else if (strcmp(name, "left_padding") == 0) {
        sscanf(value, "%i", &left_padding);
        update_root_dimensions();
    } else if (strcmp(name, "right_padding") == 0) {
        sscanf(value, "%i", &right_padding);
        update_root_dimensions();
    } else if (strcmp(name, "top_padding") == 0) {
        sscanf(value, "%i", &top_padding);
        update_root_dimensions();
    } else if (strcmp(name, "bottom_padding") == 0) {
        sscanf(value, "%i", &bottom_padding);
        update_root_dimensions();
    } else if (strcmp(name, "normal_border_color") == 0) {
        strncpy(normal_border_color, value, sizeof(normal_border_color));
        normal_border_color_pxl = get_color(normal_border_color);
    } else if (strcmp(name, "active_border_color") == 0) {
        strncpy(active_border_color, value, sizeof(active_border_color));
        active_border_color_pxl = get_color(active_border_color);
    } else if (strcmp(name, "inner_border_color") == 0) {
        strncpy(inner_border_color, value, sizeof(inner_border_color));
        inner_border_color_pxl = get_color(inner_border_color);
    } else if (strcmp(name, "outer_border_color") == 0) {
        strncpy(outer_border_color, value, sizeof(outer_border_color));
        outer_border_color_pxl = get_color(outer_border_color);
    } else if (strcmp(name, "presel_border_color") == 0) {
        strncpy(presel_border_color, value, sizeof(presel_border_color));
        presel_border_color_pxl = get_color(presel_border_color);
    } else if (strcmp(name, "locked_border_color") == 0) {
        strncpy(locked_border_color, value, sizeof(locked_border_color));
        locked_border_color_pxl = get_color(locked_border_color);
    } else if (strcmp(name, "adaptive_window_border") == 0) {
        bool b;
        if (parse_bool(value, &b))
            adaptive_window_border = b;
    } else if (strcmp(name, "adaptive_window_gap") == 0) {
        bool b;
        if (parse_bool(value, &b))
            adaptive_window_gap = b;
    } else if (strcmp(name, "wm_name") == 0) {
        strncpy(wm_name, value, sizeof(wm_name));
        return;
    }
    apply_layout(desk, desk->root);
}

void split_ratio_cmd(char *value)
{
    if (value == NULL || desk == NULL || desk->focus == NULL)
        return;

    sscanf(value, "%lf", &desk->focus->split_ratio);
}

bool parse_bool(char *value, bool *b)
{
    if (strcmp(value, "true") == 0) {
        *b = true;
        return true;
    } else if (strcmp(value, "false") == 0) {
        *b = false;
        return true;
    }
    return false;
}

bool parse_layout(char *s, layout_t *l)
{
    if (strcmp(s, "monocle") == 0) {
        *l = LAYOUT_MONOCLE;
        return true;
    } else if (strcmp(s, "tiled") == 0) {
        *l = LAYOUT_TILED;
        return true;
    }
    return false;
}

bool parse_direction(char *s, direction_t *d)
{
    if (strcmp(s, "up") == 0) {
        *d = DIR_UP;
        return true;
    } else if (strcmp(s, "down") == 0) {
        *d = DIR_DOWN;
        return true;
    } else if (strcmp(s, "left") == 0) {
        *d = DIR_LEFT;
        return true;
    } else if (strcmp(s, "right") == 0) {
        *d = DIR_RIGHT;
        return true;
    }
    return false;
}

bool parse_cycle_direction(char *s, cycle_dir_t *d)
{
    if (strcmp(s, "prev") == 0) {
        *d = DIR_PREV;
        return true;
    } else if (strcmp(s, "next") == 0) {
        *d = DIR_NEXT;
        return true;
    }
    return false;
}

bool parse_skip_client(char *s, skip_client_t *k)
{
    if (s == NULL || strcmp(s, "--skip-none") == 0) {
        *k = SKIP_NONE;
        return true;
    } else if (strcmp(s, "--skip-floating") == 0) {
        *k = SKIP_FLOATING;
        return true;
    } else if (strcmp(s, "--skip-tiled") == 0) {
        *k = SKIP_TILED;
        return true;
    }
    return false;
}

bool parse_fence_move(char *s, fence_move_t *m)
{
    if (strcmp(s, "push") == 0) {
        *m = MOVE_PUSH;
        return true;
    } else if (strcmp(s, "pull") == 0) {
        *m = MOVE_PULL;
        return true;
    }
    return false;
}

desktop_t *find_desktop(char *name)
{
    desktop_t *d = desk_head;
    while (d != NULL) {
        if (strcmp(d->name, name) == 0)
            return d;
        d = d->next;
    }
    return NULL;
}

void add_desktop(char *name)
{
    desktop_t *d = make_desktop(name);
    desk_tail->next = d;
    d->prev = desk_tail;
    desk_tail = d;
}

void alternate_desktop(void)
{
    if (last_desk == NULL)
        return;
    desktop_t *tmp = desk;
    desk = last_desk;
    last_desk = tmp;
    select_desktop(desk);
}
