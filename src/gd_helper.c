#include "gd_helper.h"
#include "game_state.h"
#include <string.h>

const godot_gdnative_core_api_struct *g_api = NULL;
const godot_gdnative_ext_nativescript_api_struct *g_nativescript_api = NULL;

GameState g_state = {
    .score = 0,
    .coins = 0,
    .lives = 3,
    .level_won = false,
    .game_over = false,
};

void game_state_reset(void) {
    g_state.score = 0;
    g_state.coins = 0;
    g_state.lives = 3;
    g_state.level_won = false;
    g_state.game_over = false;
}

godot_object *gd_get_singleton(const char *name) {
    return g_api->godot_global_get_singleton((char *)name);
}

static godot_string make_gd_string(const char *cstr) {
    godot_string s;
    g_api->godot_string_new(&s);
    g_api->godot_string_parse_utf8(&s, cstr);
    return s;
}

bool gd_is_action_pressed(const char *action) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("Input", "is_action_pressed");
    godot_object *input = gd_get_singleton("Input");
    godot_string s = make_gd_string(action);
    const void *args[1] = { &s };
    godot_bool res = GODOT_FALSE;
    g_api->godot_method_bind_ptrcall(mb, input, args, &res);
    g_api->godot_string_destroy(&s);
    return res != GODOT_FALSE;
}

bool gd_is_action_just_pressed(const char *action) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("Input", "is_action_just_pressed");
    godot_object *input = gd_get_singleton("Input");
    godot_string s = make_gd_string(action);
    const void *args[1] = { &s };
    godot_bool res = GODOT_FALSE;
    g_api->godot_method_bind_ptrcall(mb, input, args, &res);
    g_api->godot_string_destroy(&s);
    return res != GODOT_FALSE;
}

godot_object *gd_get_node(godot_object *self, const char *path) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("Node", "get_node");
    godot_string s = make_gd_string(path);
    godot_node_path np;
    g_api->godot_node_path_new(&np, &s);
    const void *args[1] = { &np };
    godot_object *res = NULL;
    g_api->godot_method_bind_ptrcall(mb, self, args, &res);
    g_api->godot_node_path_destroy(&np);
    g_api->godot_string_destroy(&s);
    return res;
}

godot_object *gd_get_tree(godot_object *self) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("Node", "get_tree");
    godot_object *res = NULL;
    g_api->godot_method_bind_ptrcall(mb, self, NULL, &res);
    return res;
}

void gd_reload_scene(godot_object *self) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("SceneTree", "reload_current_scene");
    godot_object *tree = gd_get_tree(self);
    if (!tree) return;
    godot_variant ret = g_api->godot_method_bind_call(mb, tree, NULL, 0, NULL);
    g_api->godot_variant_destroy(&ret);
}

void gd_queue_free(godot_object *self) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("Node", "queue_free");
    g_api->godot_method_bind_ptrcall(mb, self, NULL, NULL);
}

void gd_add_to_group(godot_object *self, const char *group) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("Node", "add_to_group");
    godot_string s = make_gd_string(group);
    godot_variant v;
    g_api->godot_variant_new_string(&v, &s);
    const godot_variant *args[1] = { &v };
    godot_variant ret = g_api->godot_method_bind_call(mb, self, args, 1, NULL);
    g_api->godot_variant_destroy(&ret);
    g_api->godot_variant_destroy(&v);
    g_api->godot_string_destroy(&s);
}

bool gd_is_in_group(godot_object *obj, const char *group) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("Node", "is_in_group");
    if (!obj) return false;
    godot_string s = make_gd_string(group);
    const void *args[1] = { &s };
    godot_bool res = GODOT_FALSE;
    g_api->godot_method_bind_ptrcall(mb, obj, args, &res);
    g_api->godot_string_destroy(&s);
    return res != GODOT_FALSE;
}

Vec2 gd_get_position(godot_object *node2d) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("Node2D", "get_position");
    Vec2 out = { 0.0f, 0.0f };
    if (!node2d) return out;
    godot_vector2 pos;
    g_api->godot_vector2_new(&pos, 0.0f, 0.0f);
    g_api->godot_method_bind_ptrcall(mb, node2d, NULL, &pos);
    out.x = g_api->godot_vector2_get_x(&pos);
    out.y = g_api->godot_vector2_get_y(&pos);
    return out;
}

void gd_set_position(godot_object *node2d, Vec2 pos) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("Node2D", "set_position");
    if (!node2d) return;
    godot_vector2 gv;
    g_api->godot_vector2_new(&gv, pos.x, pos.y);
    const void *args[1] = { &gv };
    g_api->godot_method_bind_ptrcall(mb, node2d, args, NULL);
}

typedef struct {
    char path[128];
    godot_variant var;
    godot_object *obj;
} TexCacheEntry;

#define MAX_TEX_CACHE 32
static TexCacheEntry g_tex_cache[MAX_TEX_CACHE];
static int g_tex_cache_count = 0;

godot_object *gd_load_texture(const char *res_path) {
    for (int i = 0; i < g_tex_cache_count; i++) {
        if (strcmp(g_tex_cache[i].path, res_path) == 0) {
            return g_tex_cache[i].obj;
        }
    }

    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("_ResourceLoader", "load");
    godot_object *loader = gd_get_singleton("ResourceLoader");
    if (!mb || !loader) return NULL;

    godot_string s = make_gd_string(res_path);
    godot_variant v;
    g_api->godot_variant_new_string(&v, &s);
    const godot_variant *args[1] = { &v };
    godot_variant ret = g_api->godot_method_bind_call(mb, loader, args, 1, NULL);
    godot_object *obj = g_api->godot_variant_as_object(&ret);
    g_api->godot_variant_destroy(&v);
    g_api->godot_string_destroy(&s);

    if (obj && g_tex_cache_count < MAX_TEX_CACHE) {
        TexCacheEntry *e = &g_tex_cache[g_tex_cache_count++];
        strncpy(e->path, res_path, sizeof(e->path) - 1);
        e->path[sizeof(e->path) - 1] = '\0';
        e->var = ret; /* Keep variant alive so Reference count stays >= 1 */
        e->obj = obj;
    } else {
        g_api->godot_variant_destroy(&ret);
    }
    return obj;
}

void gd_sprite_set_texture(godot_object *sprite, godot_object *texture) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("Sprite", "set_texture");
    if (!sprite || !texture) return;
    const void *args[1] = { texture };
    g_api->godot_method_bind_ptrcall(mb, sprite, args, NULL);
}

void gd_sprite_set_flip_h(godot_object *sprite, bool flip) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("Sprite", "set_flip_h");
    if (!sprite) return;
    godot_bool b = flip ? GODOT_TRUE : GODOT_FALSE;
    const void *args[1] = { &b };
    g_api->godot_method_bind_ptrcall(mb, sprite, args, NULL);
}

void gd_label_set_text(godot_object *label, const char *text) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("Label", "set_text");
    if (!label) return;
    godot_string s = make_gd_string(text);
    const void *args[1] = { &s };
    g_api->godot_method_bind_ptrcall(mb, label, args, NULL);
    g_api->godot_string_destroy(&s);
}

void gd_canvas_set_visible(godot_object *item, bool visible) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("CanvasItem", "set_visible");
    if (!item) return;
    godot_bool b = visible ? GODOT_TRUE : GODOT_FALSE;
    const void *args[1] = { &b };
    g_api->godot_method_bind_ptrcall(mb, item, args, NULL);
}

Vec2 gd_move_and_slide(godot_object *body, Vec2 vel, Vec2 up) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("KinematicBody2D", "move_and_slide");
    godot_vector2 g_vel, g_up;
    g_api->godot_vector2_new(&g_vel, vel.x, vel.y);
    g_api->godot_vector2_new(&g_up, up.x, up.y);
    godot_variant v_vel, v_up;
    g_api->godot_variant_new_vector2(&v_vel, &g_vel);
    g_api->godot_variant_new_vector2(&v_up, &g_up);
    const godot_variant *args[2] = { &v_vel, &v_up };
    godot_variant ret = g_api->godot_method_bind_call(mb, body, args, 2, NULL);
    godot_vector2 g_out = g_api->godot_variant_as_vector2(&ret);
    Vec2 out = {
        g_api->godot_vector2_get_x(&g_out),
        g_api->godot_vector2_get_y(&g_out),
    };
    g_api->godot_variant_destroy(&ret);
    g_api->godot_variant_destroy(&v_vel);
    g_api->godot_variant_destroy(&v_up);
    return out;
}

bool gd_is_on_floor(godot_object *body) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("KinematicBody2D", "is_on_floor");
    godot_bool res = GODOT_FALSE;
    g_api->godot_method_bind_ptrcall(mb, body, NULL, &res);
    return res != GODOT_FALSE;
}

bool gd_is_on_wall(godot_object *body) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("KinematicBody2D", "is_on_wall");
    godot_bool res = GODOT_FALSE;
    g_api->godot_method_bind_ptrcall(mb, body, NULL, &res);
    return res != GODOT_FALSE;
}

bool gd_is_on_ceiling(godot_object *body) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("KinematicBody2D", "is_on_ceiling");
    godot_bool res = GODOT_FALSE;
    g_api->godot_method_bind_ptrcall(mb, body, NULL, &res);
    return res != GODOT_FALSE;
}

godot_array gd_get_overlapping_bodies(godot_object *area) {
    static godot_method_bind *mb = NULL;
    if (!mb) mb = g_api->godot_method_bind_get_method("Area2D", "get_overlapping_bodies");
    godot_array arr;
    g_api->godot_array_new(&arr);
    g_api->godot_method_bind_ptrcall(mb, area, NULL, &arr);
    return arr;
}

float gd_variant_to_float(const godot_variant *v) {
    return (float)g_api->godot_variant_as_real(v);
}
