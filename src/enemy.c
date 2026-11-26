#include "gd_helper.h"
#include "game_state.h"
#include <stdlib.h>

typedef struct {
    Vec2 velocity;
    float dir;
    float anim_timer;
    float squash_timer;
    int anim_frame;
    bool squashed;
    godot_object *sprite;
    godot_object *hitbox;
    godot_object *tex_walk1;
    godot_object *tex_walk2;
    godot_object *tex_squashed;
} EnemyData;

void *enemy_constructor(godot_object *instance, void *method_data) {
    (void)instance;
    (void)method_data;
    EnemyData *d = (EnemyData *)calloc(1, sizeof(EnemyData));
    d->dir = -1.0f;
    return d;
}

void enemy_destructor(godot_object *instance, void *method_data, void *user_data) {
    (void)instance;
    (void)method_data;
    free(user_data);
}

godot_variant enemy_ready(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args) {
    (void)method_data;
    (void)num_args;
    (void)args;
    EnemyData *d = (EnemyData *)user_data;

    d->sprite = gd_get_node(instance, "Sprite");
    d->hitbox = gd_get_node(instance, "Hitbox");
    d->tex_walk1 = gd_load_texture("res://assets/enemy_walk1.png");
    d->tex_walk2 = gd_load_texture("res://assets/enemy_walk2.png");
    d->tex_squashed = gd_load_texture("res://assets/enemy_squashed.png");

    godot_variant ret;
    g_api->godot_variant_new_nil(&ret);
    return ret;
}

godot_variant enemy_physics_process(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args) {
    (void)method_data;
    EnemyData *d = (EnemyData *)user_data;
    float delta = (num_args > 0) ? gd_variant_to_float(args[0]) : 0.016f;

    if (g_state.game_over || g_state.level_won) {
        godot_variant ret;
        g_api->godot_variant_new_nil(&ret);
        return ret;
    }

    if (d->squashed) {
        d->squash_timer -= delta;
        if (d->squash_timer <= 0.0f) {
            gd_queue_free(instance);
        }
        godot_variant ret;
        g_api->godot_variant_new_nil(&ret);
        return ret;
    }

    d->velocity.x = d->dir * 75.0f;
    d->velocity.y += 1100.0f * delta;
    if (d->velocity.y > 600.0f) d->velocity.y = 600.0f;

    Vec2 up = { 0.0f, -1.0f };
    d->velocity = gd_move_and_slide(instance, d->velocity, up);

    if (gd_is_on_wall(instance)) {
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
        godot_array bodies = gd_get_overlapping_bodies(d->hitbox);
        int count = g_api->godot_array_size(&bodies);
        Vec2 my_pos = gd_get_position(instance);
        for (int i = 0; i < count; i++) {
            godot_variant v = g_api->godot_array_get(&bodies, i);
            godot_object *body = g_api->godot_variant_as_object(&v);
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
            g_api->godot_variant_destroy(&v);
        }
        g_api->godot_array_destroy(&bodies);
    }

    godot_variant ret;
    g_api->godot_variant_new_nil(&ret);
    return ret;
}
