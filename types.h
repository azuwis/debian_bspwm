#ifndef _TYPES_H
#define _TYPES_H

typedef enum {
    LAYOUT_TILED,
    LAYOUT_MAX
} layout_t;

typedef enum {
    LAYER_TILING,
    LAYER_FLOATING
} layer_t;

typedef enum {
    TYPE_HORIZONTAL,
    TYPE_VERTICAL
} split_type_t;

typedef enum {
    MODE_ROTATE_CWISE,
    MODE_ROTATE_CCWISE
} split_mode_t;

typedef enum {
    FIRST_CHILD,
    SECOND_CHILD
} pair_polarity_t;

typedef enum {
    DIR_LEFT,
    DIR_UP,
    DIR_RIGHT,
    DIR_DOWN
} split_dir_t;

typedef enum {
    STRATEGY_REPLACE,
    STRATEGY_PAIR
} insertion_strategy_t;

typedef struct {
    xcb_window_t win;
    bool floating;
    bool fullscreen;
    bool urgent;
    bool locked;
} Client;

typedef struct {
    split_type_t split_type;
    double split_ratio;
    xcb_rectangle_t rectangle;
    pair_polarity_t pair_polarity;
    struct Node *first_child;
    struct Node *second_child;
    struct Node *parent;
    Client *client; /* equals NULL except for leaves */
} Node;

typedef struct {
    Node *prev;
} NodeFocusHistory;

typedef struct {
    Node *first;
    Node *focus;
    NodeFocusHistory *focus_history;
} Layer;

typedef Layer TilingLayer;
typedef Layer FloatingLayer;

typedef struct {
    char *name;
    Layer tiling_layer;
    Layer floating_layer;
    layer_t selected_layer;
    layout_t tiling_layout;
    insertion_strategy_t insertion_strategy;
    split_mode_t split_mode;
    struct Desktop *previous;
    struct Desktop *next;
} Desktop;

#endif
