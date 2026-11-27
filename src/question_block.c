#include "gd_helper.h"
#include "game_state.h"
#include <stdlib.h>

typedef struct {
    GDExtensionObjectPtr self;
    bool used;
    float bump_timer;
    Vec2 base_pos;
    GDExtensionObjectPtr sprite;
    GDExtensionObjectPtr hit_area;
    GDExtensionObjectPtr tex_used;
} QuestionBlockData;

static void qblock_ready(GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_ret) {
    (void)p_args;
    (void)r_ret;
    QuestionBlockData *d = (QuestionBlockData *)p_instance;
    d->base_pos = gd_get_position(d->self);
    d->sprite = gd_get_node(d->self, "Sprite2D");
    d->hit_area = gd_get_node(d->self, "BottomHit");
    d->tex_used = gd_load_texture("res://assets/block_used.png");
}

static void qblock_process(GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_ret) {
    (void)r_ret;
    QuestionBlockData *d = (QuestionBlockData *)p_instance;
    float delta = p_args ? (float)(*(const double *)p_args[0]) : 0.016f;

    if (d->bump_timer > 0.0f) {
        d->bump_timer -= delta;
        float offset = (d->bump_timer > 0.08f) ? -6.0f : 0.0f;
        Vec2 pos = d->base_pos;
        pos.y += offset;
        gd_set_position(d->self, pos);
    }

    if (!d->used && d->hit_area) {
        gd_array bodies = gd_get_overlapping_bodies(d->hit_area);
        int count = gd_array_size(&bodies);
        for (int i = 0; i < count; i++) {
            GDExtensionObjectPtr body = gd_array_get_object(&bodies, i);
            if (body && gd_is_in_group(body, "player")) {
                d->used = true;
                d->bump_timer = 0.16f;
                g_state.coins += 1;
                g_state.score += 200;
                if (d->sprite && d->tex_used) {
                    gd_sprite_set_texture(d->sprite, d->tex_used);
                }
                break;
            }
        }
        gd_array_destroy(&bodies);
    }
}

static GDExtensionObjectPtr qblock_create(void *p_userdata, GDExtensionBool p_notify_postinitialize) {
    (void)p_userdata;
    QuestionBlockData *d = (QuestionBlockData *)gd_alloc(sizeof(QuestionBlockData));
    d->self = gd_construct_base("StaticBody2D", "QuestionBlock", d, p_notify_postinitialize);
    return d->self;
}

static void qblock_free(void *p_userdata, GDExtensionClassInstancePtr p_instance) {
    (void)p_userdata;
    gd_free(p_instance);
}

static GDExtensionClassCallVirtual qblock_get_virtual(void *p_userdata, GDExtensionConstStringNamePtr p_name, uint32_t p_hash) {
    (void)p_userdata;
    (void)p_hash;
    if (gd_str_name_eq(p_name, "_ready")) return &qblock_ready;
    if (gd_str_name_eq(p_name, "_process")) return &qblock_process;
    return NULL;
}

void qblock_register(void) {
    gd_register_class("QuestionBlock", "StaticBody2D", &qblock_create, &qblock_free, &qblock_get_virtual);
}
