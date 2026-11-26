#include "gd_helper.h"
#include "game_state.h"
#include <math.h>
#include <stdlib.h>

typedef struct {
    Vec2 velocity;
    Vec2 spawn_pos;
    float coyote_timer;
    float jump_buffer_timer;
    float anim_timer;
    float invuln_timer;
    int anim_frame;
    bool facing_left;
    bool initialized;
    godot_object *sprite;
    godot_object *tex_idle;
    godot_object *tex_run1;
    godot_object *tex_run2;
    godot_object *tex_jump;
} PlayerData;

void *player_constructor(godot_object *instance, void *method_data) {
    (void)instance;
    (void)method_data;
    PlayerData *d = (PlayerData *)calloc(1, sizeof(PlayerData));
    return d;
}

void player_destructor(godot_object *instance, void *method_data, void *user_data) {
    (void)instance;
    (void)method_data;
    free(user_data);
}

godot_variant player_ready(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args) {
    (void)method_data;
    (void)num_args;
    (void)args;
    PlayerData *d = (PlayerData *)user_data;

    gd_add_to_group(instance, "player");
    d->spawn_pos = gd_get_position(instance);
    d->sprite = gd_get_node(instance, "Sprite");
    d->tex_idle = gd_load_texture("res://assets/player_idle.png");
    d->tex_run1 = gd_load_texture("res://assets/player_run1.png");
    d->tex_run2 = gd_load_texture("res://assets/player_run2.png");
    d->tex_jump = gd_load_texture("res://assets/player_jump.png");
    d->initialized = true;

    godot_variant ret;
    g_api->godot_variant_new_nil(&ret);
    return ret;
}

static void player_die_or_respawn(godot_object *instance, PlayerData *d) {
    g_state.lives -= 1;
    if (g_state.lives <= 0) {
        g_state.lives = 0;
        g_state.game_over = true;
        d->velocity.x = 0.0f;
        d->velocity.y = 0.0f;
    } else {
        gd_set_position(instance, d->spawn_pos);
        d->velocity.x = 0.0f;
        d->velocity.y = 0.0f;
        d->invuln_timer = 1.5f;
    }
}

godot_variant player_physics_process(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args) {
    (void)method_data;
    PlayerData *d = (PlayerData *)user_data;
    float delta = (num_args > 0) ? gd_variant_to_float(args[0]) : 0.016f;

    if (g_state.game_over || g_state.level_won) {
        if (gd_is_action_just_pressed("restart")) {
            game_state_reset();
            gd_reload_scene(instance);
        }
        godot_variant ret;
        g_api->godot_variant_new_nil(&ret);
        return ret;
    }

    if (g_state.player_bounce_pending) {
        g_state.player_bounce_pending = false;
        d->velocity.y = -420.0f;
    }

    if (g_state.player_hurt_pending) {
        g_state.player_hurt_pending = false;
        if (d->invuln_timer <= 0.0f) {
            player_die_or_respawn(instance, d);
        }
    }

    if (d->invuln_timer > 0.0f) {
        d->invuln_timer -= delta;
        if (d->sprite) {
            bool vis = ((int)(d->invuln_timer * 15.0f) % 2) == 0;
            gd_canvas_set_visible(d->sprite, vis);
        }
    } else if (d->sprite) {
        gd_canvas_set_visible(d->sprite, true);
    }

    /* Horizontal movement */
    float dir = 0.0f;
    if (gd_is_action_pressed("move_left")) dir -= 1.0f;
    if (gd_is_action_pressed("move_right")) dir += 1.0f;

    bool sprinting = gd_is_action_pressed("sprint");
    float max_speed = sprinting ? 310.0f : 220.0f;
    float accel = 1400.0f;
    float friction = 1600.0f;

    if (dir != 0.0f) {
        d->velocity.x += dir * accel * delta;
        if (d->velocity.x > max_speed) d->velocity.x = max_speed;
        if (d->velocity.x < -max_speed) d->velocity.x = -max_speed;
        d->facing_left = (dir < 0.0f);
    } else {
        if (d->velocity.x > 0.0f) {
            d->velocity.x -= friction * delta;
            if (d->velocity.x < 0.0f) d->velocity.x = 0.0f;
        } else if (d->velocity.x < 0.0f) {
            d->velocity.x += friction * delta;
            if (d->velocity.x > 0.0f) d->velocity.x = 0.0f;
        }
    }

    /* Coyote time & jump buffer */
    bool on_floor = gd_is_on_floor(instance);
    if (on_floor) {
        d->coyote_timer = 0.10f;
    } else {
        d->coyote_timer -= delta;
    }

    if (gd_is_action_just_pressed("jump")) {
        d->jump_buffer_timer = 0.12f;
    } else {
        d->jump_buffer_timer -= delta;
    }

    if (d->jump_buffer_timer > 0.0f && d->coyote_timer > 0.0f) {
        d->velocity.y = -520.0f;
        d->coyote_timer = 0.0f;
        d->jump_buffer_timer = 0.0f;
    }

    /* Variable jump height */
    if (!gd_is_action_pressed("jump") && d->velocity.y < -180.0f) {
        d->velocity.y += 1800.0f * delta;
    }

    /* Gravity */
    d->velocity.y += 1250.0f * delta;
    if (d->velocity.y > 750.0f) d->velocity.y = 750.0f;

    Vec2 up = { 0.0f, -1.0f };
    d->velocity = gd_move_and_slide(instance, d->velocity, up);

    /* Pit fall check */
    Vec2 pos = gd_get_position(instance);
    if (pos.y > 720.0f) {
        player_die_or_respawn(instance, d);
    }

    /* Sprite animation */
    if (d->sprite) {
        gd_sprite_set_flip_h(d->sprite, d->facing_left);
        if (!on_floor) {
            gd_sprite_set_texture(d->sprite, d->tex_jump);
        } else if (fabsf(d->velocity.x) > 15.0f) {
            d->anim_timer += delta * (sprinting ? 14.0f : 10.0f);
            if (d->anim_timer >= 1.0f) {
                d->anim_timer = 0.0f;
                d->anim_frame = 1 - d->anim_frame;
            }
            gd_sprite_set_texture(d->sprite, d->anim_frame ? d->tex_run1 : d->tex_run2);
        } else {
            gd_sprite_set_texture(d->sprite, d->tex_idle);
        }
    }

    godot_variant ret;
    g_api->godot_variant_new_nil(&ret);
    return ret;
}
