#ifndef GD_HELPER_H
#define GD_HELPER_H

#include <gdnative_api_struct.gen.h>
#include <stdbool.h>
#include <stddef.h>

extern const godot_gdnative_core_api_struct *g_api;
extern const godot_gdnative_ext_nativescript_api_struct *g_nativescript_api;

/* Singleton helpers */
godot_object *gd_get_singleton(const char *name);

/* Input helpers */
bool gd_is_action_pressed(const char *action);
bool gd_is_action_just_pressed(const char *action);

/* Node / Node2D helpers */
godot_object *gd_get_node(godot_object *self, const char *path);
godot_object *gd_get_tree(godot_object *self);
void gd_reload_scene(godot_object *self);
void gd_queue_free(godot_object *self);
void gd_add_to_group(godot_object *self, const char *group);
bool gd_is_in_group(godot_object *obj, const char *group);

typedef struct {
    float x;
    float y;
} Vec2;

Vec2 gd_get_position(godot_object *node2d);
void gd_set_position(godot_object *node2d, Vec2 pos);

/* Sprite / UI helpers */
godot_object *gd_load_texture(const char *res_path);
void gd_sprite_set_texture(godot_object *sprite, godot_object *texture);
void gd_sprite_set_flip_h(godot_object *sprite, bool flip);
void gd_label_set_text(godot_object *label, const char *text);
void gd_canvas_set_visible(godot_object *item, bool visible);

/* Physics helpers */
Vec2 gd_move_and_slide(godot_object *body, Vec2 vel, Vec2 up);
bool gd_is_on_floor(godot_object *body);
bool gd_is_on_wall(godot_object *body);
bool gd_is_on_ceiling(godot_object *body);
godot_array gd_get_overlapping_bodies(godot_object *area);

/* Variant helpers */
float gd_variant_to_float(const godot_variant *v);

#endif
