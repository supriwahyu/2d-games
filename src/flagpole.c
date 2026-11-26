#include "gd_helper.h"
#include "game_state.h"
#include <stdlib.h>

typedef struct {
    bool triggered;
} FlagpoleData;

void *flagpole_constructor(godot_object *instance, void *method_data) {
    (void)instance;
    (void)method_data;
    return calloc(1, sizeof(FlagpoleData));
}

void flagpole_destructor(godot_object *instance, void *method_data, void *user_data) {
    (void)instance;
    (void)method_data;
    free(user_data);
}

godot_variant flagpole_process(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args) {
    (void)method_data;
    (void)num_args;
    (void)args;
    FlagpoleData *d = (FlagpoleData *)user_data;

    if (!d->triggered && !g_state.level_won && !g_state.game_over) {
        godot_array bodies = gd_get_overlapping_bodies(instance);
        int count = g_api->godot_array_size(&bodies);
        for (int i = 0; i < count; i++) {
            godot_variant v = g_api->godot_array_get(&bodies, i);
            godot_object *body = g_api->godot_variant_as_object(&v);
            if (body && gd_is_in_group(body, "player")) {
                d->triggered = true;
                g_state.level_won = true;
                g_state.score += 1000;
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
