#ifndef GD_HELPER_H
#define GD_HELPER_H

#include <gdextension_interface.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct { uint8_t data[8]; } gd_string;
typedef struct { uint8_t data[8]; } gd_string_name;
typedef struct { uint8_t data[8]; } gd_node_path;
typedef struct { uint8_t data[8]; } gd_array;
typedef struct { uint8_t data[24]; } gd_variant;

typedef struct {
    float x;
    float y;
} Vec2;

/* Initialization */
void gd_helper_init_procs(GDExtensionInterfaceGetProcAddress get_proc_address, GDExtensionClassLibraryPtr library);
void gd_helper_init_scene(void);
void gd_helper_deinit_scene(void);

/* Class registration */
void gd_register_class(const char *name,
                       const char *parent,
                       GDExtensionClassCreateInstance2 create_fn,
                       GDExtensionClassFreeInstance free_fn,
                       GDExtensionClassGetVirtual2 get_virtual_fn);
GDExtensionObjectPtr gd_construct_base(const char *parent_name, const char *class_name, void *instance_data, GDExtensionBool notify_postinitialize);
void *gd_alloc(size_t size);
void gd_free(void *ptr);
bool gd_str_name_eq(GDExtensionConstStringNamePtr a, const char *b);

/* Input helpers */
bool gd_is_action_pressed(const char *action);
bool gd_is_action_just_pressed(const char *action);

/* Node / Node2D helpers */
GDExtensionObjectPtr gd_get_node(GDExtensionObjectPtr self, const char *path);
GDExtensionObjectPtr gd_get_tree(GDExtensionObjectPtr self);
void gd_reload_scene(GDExtensionObjectPtr self);
void gd_queue_free(GDExtensionObjectPtr self);
void gd_add_to_group(GDExtensionObjectPtr self, const char *group);
bool gd_is_in_group(GDExtensionObjectPtr obj, const char *group);

Vec2 gd_get_position(GDExtensionObjectPtr node2d);
void gd_set_position(GDExtensionObjectPtr node2d, Vec2 pos);

/* Sprite2D / UI helpers */
GDExtensionObjectPtr gd_load_texture(const char *res_path);
void gd_sprite_set_texture(GDExtensionObjectPtr sprite, GDExtensionObjectPtr texture);
void gd_sprite_set_flip_h(GDExtensionObjectPtr sprite, bool flip);
void gd_label_set_text(GDExtensionObjectPtr label, const char *text);
void gd_canvas_set_visible(GDExtensionObjectPtr item, bool visible);

/* Physics helpers */
Vec2 gd_move_and_slide(GDExtensionObjectPtr body, Vec2 vel, Vec2 up);
bool gd_is_on_floor(GDExtensionObjectPtr body);
bool gd_is_on_wall(GDExtensionObjectPtr body);
bool gd_is_on_ceiling(GDExtensionObjectPtr body);

/* Area2D / Array helpers */
gd_array gd_get_overlapping_bodies(GDExtensionObjectPtr area);
int gd_array_size(gd_array *arr);
GDExtensionObjectPtr gd_array_get_object(gd_array *arr, int index);
void gd_array_destroy(gd_array *arr);

#endif
