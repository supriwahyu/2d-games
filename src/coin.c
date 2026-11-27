#include "gd_helper.h"
#include "game_state.h"
#include <math.h>
#include <stdlib.h>

typedef struct {
    GDExtensionObjectPtr self;
    Vec2 base_pos;
    float time;
    bool collected;
} CoinData;

static void coin_ready(GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_ret) {
    (void)p_args;
    (void)r_ret;
    CoinData *d = (CoinData *)p_instance;
    d->base_pos = gd_get_position(d->self);
}

static void coin_process(GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_ret) {
    (void)r_ret;
    CoinData *d = (CoinData *)p_instance;
    float delta = p_args ? (float)(*(const double *)p_args[0]) : 0.016f;

    if (d->collected) {
        return;
    }

    d->time += delta * 4.0f;
    Vec2 pos = d->base_pos;
    pos.y += sinf(d->time) * 4.0f;
    gd_set_position(d->self, pos);

    gd_array bodies = gd_get_overlapping_bodies(d->self);
    int count = gd_array_size(&bodies);
    for (int i = 0; i < count; i++) {
        GDExtensionObjectPtr body = gd_array_get_object(&bodies, i);
        if (body && gd_is_in_group(body, "player")) {
            d->collected = true;
            g_state.coins += 1;
            g_state.score += 100;
            gd_queue_free(d->self);
            break;
        }
    }
    gd_array_destroy(&bodies);
}

static GDExtensionObjectPtr coin_create(void *p_userdata, GDExtensionBool p_notify_postinitialize) {
    (void)p_userdata;
    CoinData *d = (CoinData *)gd_alloc(sizeof(CoinData));
    d->self = gd_construct_base("Area2D", "Coin", d, p_notify_postinitialize);
    return d->self;
}

static void coin_free(void *p_userdata, GDExtensionClassInstancePtr p_instance) {
    (void)p_userdata;
    gd_free(p_instance);
}

static GDExtensionClassCallVirtual coin_get_virtual(void *p_userdata, GDExtensionConstStringNamePtr p_name, uint32_t p_hash) {
    (void)p_userdata;
    (void)p_hash;
    if (gd_str_name_eq(p_name, "_ready")) return &coin_ready;
    if (gd_str_name_eq(p_name, "_process")) return &coin_process;
    return NULL;
}

void coin_register(void) {
    gd_register_class("Coin", "Area2D", &coin_create, &coin_free, &coin_get_virtual);
}
