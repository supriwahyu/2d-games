#include "gd_helper.h"
#include "game_state.h"
#include <math.h>
#include <stdlib.h>

typedef struct {
    Vec2 base_pos;
    float time;
    bool collected;
} CoinData;

void *coin_constructor(godot_object *instance, void *method_data) {
    (void)instance;
    (void)method_data;
    return calloc(1, sizeof(CoinData));
}

void coin_destructor(godot_object *instance, void *method_data, void *user_data) {
    (void)instance;
    (void)method_data;
    free(user_data);
}

godot_variant coin_ready(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args) {
    (void)method_data;
    (void)num_args;
    (void)args;
    CoinData *d = (CoinData *)user_data;
    d->base_pos = gd_get_position(instance);
    godot_variant ret;
    g_api->godot_variant_new_nil(&ret);
    return ret;
}

godot_variant coin_process(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args) {
    (void)method_data;
    CoinData *d = (CoinData *)user_data;
    float delta = (num_args > 0) ? gd_variant_to_float(args[0]) : 0.016f;

    if (d->collected) {
        godot_variant ret;
        g_api->godot_variant_new_nil(&ret);
        return ret;
    }

    d->time += delta * 4.0f;
    Vec2 pos = d->base_pos;
    pos.y += sinf(d->time) * 4.0f;
    gd_set_position(instance, pos);

    godot_array bodies = gd_get_overlapping_bodies(instance);
    int count = g_api->godot_array_size(&bodies);
    for (int i = 0; i < count; i++) {
        godot_variant v = g_api->godot_array_get(&bodies, i);
        godot_object *body = g_api->godot_variant_as_object(&v);
        if (body && gd_is_in_group(body, "player")) {
            d->collected = true;
            g_state.coins += 1;
            g_state.score += 100;
            gd_queue_free(instance);
            g_api->godot_variant_destroy(&v);
            break;
        }
        g_api->godot_variant_destroy(&v);
    }
    g_api->godot_array_destroy(&bodies);

    godot_variant ret;
    g_api->godot_variant_new_nil(&ret);
    return ret;
}
