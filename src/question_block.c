#include "gd_helper.h"
#include "game_state.h"
#include <stdlib.h>

typedef struct {
    bool used;
    float bump_timer;
    Vec2 base_pos;
    godot_object *sprite;
    godot_object *hit_area;
    godot_object *tex_used;
} QuestionBlockData;

void *qblock_constructor(godot_object *instance, void *method_data) {
    (void)instance;
    (void)method_data;
    return calloc(1, sizeof(QuestionBlockData));
}

void qblock_destructor(godot_object *instance, void *method_data, void *user_data) {
    (void)instance;
    (void)method_data;
    free(user_data);
}

godot_variant qblock_ready(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args) {
    (void)method_data;
    (void)num_args;
    (void)args;
    QuestionBlockData *d = (QuestionBlockData *)user_data;
    d->base_pos = gd_get_position(instance);
    d->sprite = gd_get_node(instance, "Sprite");
    d->hit_area = gd_get_node(instance, "BottomHit");
    d->tex_used = gd_load_texture("res://assets/block_used.png");

    godot_variant ret;
    g_api->godot_variant_new_nil(&ret);
    return ret;
}

godot_variant qblock_process(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args) {
    (void)method_data;
    QuestionBlockData *d = (QuestionBlockData *)user_data;
    float delta = (num_args > 0) ? gd_variant_to_float(args[0]) : 0.016f;

    if (d->bump_timer > 0.0f) {
        d->bump_timer -= delta;
        float offset = (d->bump_timer > 0.08f) ? -6.0f : 0.0f;
        Vec2 pos = d->base_pos;
        pos.y += offset;
        gd_set_position(instance, pos);
    }

    if (!d->used && d->hit_area) {
        godot_array bodies = gd_get_overlapping_bodies(d->hit_area);
        int count = g_api->godot_array_size(&bodies);
        for (int i = 0; i < count; i++) {
            godot_variant v = g_api->godot_array_get(&bodies, i);
            godot_object *body = g_api->godot_variant_as_object(&v);
            if (body && gd_is_in_group(body, "player")) {
                d->used = true;
                d->bump_timer = 0.16f;
                g_state.coins += 1;
                g_state.score += 200;
                if (d->sprite && d->tex_used) {
                    gd_sprite_set_texture(d->sprite, d->tex_used);
                }
                g_api->godot_variant_destroy(&v);
                break;
            }
            g_api->godot_variant_destroy(&v);
        }
        g_api->godot_array_destroy(&bodies);
    }

    godot_variant ret;
    g_api->godot_variant_new_nil(&ret);
    return ret;
}
