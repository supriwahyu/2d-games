#include "gd_helper.h"
#include "game_state.h"
#include <stdlib.h>

typedef struct {
    GDExtensionObjectPtr self;
    Vec2 velocity;
    float dir;
    float anim_timer;
    float squash_timer;
    int anim_frame;
    bool squashed;
    GDExtensionObjectPtr sprite;
    GDExtensionObjectPtr hitbox;
    GDExtensionObjectPtr tex_walk1;
    GDExtensionObjectPtr tex_walk2;
    GDExtensionObjectPtr tex_squashed;
} EnemyData;

static void enemy_ready(GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_ret) {
    (void)p_args;
    (void)r_ret;
    EnemyData *d = (EnemyData *)p_instance;

    d->sprite = gd_get_node(d->self, "Sprite2D");
    d->hitbox = gd_get_node(d->self, "Hitbox");
    d->tex_walk1 = gd_load_texture("res://assets/enemy_walk1.png");
    d->tex_walk2 = gd_load_texture("res://assets/enemy_walk2.png");
    d->tex_squashed = gd_load_texture("res://assets/enemy_squashed.png");
}

static void enemy_physics_process(GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_ret) {
    (void)r_ret;
    EnemyData *d = (EnemyData *)p_instance;
    float delta = p_args ? (float)(*(const double *)p_args[0]) : 0.016f;

    if (g_state.game_over || g_state.level_won) {
        return;
    }

    if (d->squashed) {
        d->squash_timer -= delta;
        if (d->squash_timer <= 0.0f) {
            gd_queue_free(d->self);
        }
        return;
    }

    d->velocity.x = d->dir * 75.0f;
    d->velocity.y += 1100.0f * delta;
    if (d->velocity.y > 600.0f) d->velocity.y = 600.0f;

    Vec2 up = { 0.0f, -1.0f };
    d->velocity = gd_move_and_slide(d->self, d->velocity, up);

    if (gd_is_on_wall(d->self)) {
        d->dir = -d->dir;
    }

    /* Walk animation */
    d->anim_timer += delta * 6.0f;
    if (d->anim_timer >= 1.0f) {
        d->anim_timer = 0.0f;
        d->anim_frame = 1 - d->anim_frame;
        if (d->sprite) {
            gd_sprite_set_texture(d->sprite, d->anim_frame ? d->tex_walk1 : d->tex_walk2);
        }
    }

    /* Check player collision via Hitbox Area2D */
    if (d->hitbox) {
        gd_array bodies = gd_get_overlapping_bodies(d->hitbox);
        int count = gd_array_size(&bodies);
        Vec2 my_pos = gd_get_position(d->self);
        for (int i = 0; i < count; i++) {
            GDExtensionObjectPtr body = gd_array_get_object(&bodies, i);
            if (body && gd_is_in_group(body, "player")) {
                Vec2 p_pos = gd_get_position(body);
                if (p_pos.y < my_pos.y - 12.0f) {
                    /* Stomped from above! */
                    d->squashed = true;
                    d->squash_timer = 0.35f;
                    g_state.score += 100;
                    g_state.player_bounce_pending = true;
                    if (d->sprite && d->tex_squashed) {
                        gd_sprite_set_texture(d->sprite, d->tex_squashed);
                    }
                } else {
                    /* Hit player from side */
                    g_state.player_hurt_pending = true;
                }
            }
        }
        gd_array_destroy(&bodies);
    }
}

static GDExtensionObjectPtr enemy_create(void *p_userdata, GDExtensionBool p_notify_postinitialize) {
    (void)p_userdata;
    EnemyData *d = (EnemyData *)gd_alloc(sizeof(EnemyData));
    d->dir = -1.0f;
    d->self = gd_construct_base("CharacterBody2D", "Enemy", d, p_notify_postinitialize);
    return d->self;
}

static void enemy_free(void *p_userdata, GDExtensionClassInstancePtr p_instance) {
    (void)p_userdata;
    gd_free(p_instance);
}

static GDExtensionClassCallVirtual enemy_get_virtual(void *p_userdata, GDExtensionConstStringNamePtr p_name, uint32_t p_hash) {
    (void)p_userdata;
    (void)p_hash;
    if (gd_str_name_eq(p_name, "_ready")) return &enemy_ready;
    if (gd_str_name_eq(p_name, "_physics_process")) return &enemy_physics_process;
    return NULL;
}

void enemy_register(void) {
    gd_register_class("Enemy", "CharacterBody2D", &enemy_create, &enemy_free, &enemy_get_virtual);
}
