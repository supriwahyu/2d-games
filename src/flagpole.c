#include "gd_helper.h"
#include "game_state.h"
#include <stdlib.h>

typedef struct {
    GDExtensionObjectPtr self;
    bool triggered;
} FlagpoleData;

static void flagpole_process(GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_ret) {
    (void)p_args;
    (void)r_ret;
    FlagpoleData *d = (FlagpoleData *)p_instance;

    if (!d->triggered && !g_state.level_won && !g_state.game_over) {
        gd_array bodies = gd_get_overlapping_bodies(d->self);
        int count = gd_array_size(&bodies);
        for (int i = 0; i < count; i++) {
            GDExtensionObjectPtr body = gd_array_get_object(&bodies, i);
            if (body && gd_is_in_group(body, "player")) {
                d->triggered = true;
                g_state.level_won = true;
                g_state.score += 1000;
                break;
            }
        }
        gd_array_destroy(&bodies);
    }
}

static GDExtensionObjectPtr flagpole_create(void *p_userdata, GDExtensionBool p_notify_postinitialize) {
    (void)p_userdata;
    FlagpoleData *d = (FlagpoleData *)gd_alloc(sizeof(FlagpoleData));
    d->self = gd_construct_base("Area2D", "Flagpole", d, p_notify_postinitialize);
    return d->self;
}

static void flagpole_free(void *p_userdata, GDExtensionClassInstancePtr p_instance) {
    (void)p_userdata;
    gd_free(p_instance);
}

static GDExtensionClassCallVirtual flagpole_get_virtual(void *p_userdata, GDExtensionConstStringNamePtr p_name, uint32_t p_hash) {
    (void)p_userdata;
    (void)p_hash;
    if (gd_str_name_eq(p_name, "_process")) return &flagpole_process;
    return NULL;
}

void flagpole_register(void) {
    gd_register_class("Flagpole", "Area2D", &flagpole_create, &flagpole_free, &flagpole_get_virtual);
}
