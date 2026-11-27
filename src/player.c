#include "gd_helper.h"
#include "game_state.h"
#include <math.h>
#include <stdlib.h>

typedef struct {
    GDExtensionObjectPtr self;
    Vec2 velocity;
    Vec2 spawn_pos;
    float coyote_timer;
    float jump_buffer_timer;
    float anim_timer;
    float invuln_timer;
    int anim_frame;
    bool facing_left;
    bool initialized;
    GDExtensionObjectPtr sprite;
    GDExtensionObjectPtr tex_idle;
    GDExtensionObjectPtr tex_run1;
    GDExtensionObjectPtr tex_run2;
    GDExtensionObjectPtr tex_jump;
} PlayerData;

static void player_ready(GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_ret) {
    (void)p_args;
    (void)r_ret;
    PlayerData *d = (PlayerData *)p_instance;

    gd_add_to_group(d->self, "player");
    d->spawn_pos = gd_get_position(d->self);
    d->sprite = gd_get_node(d->self, "Sprite2D");
    d->tex_idle = gd_load_texture("res://assets/player_idle.png");
    d->tex_run1 = gd_load_texture("res://assets/player_run1.png");
    d->tex_run2 = gd_load_texture("res://assets/player_run2.png");
    d->tex_jump = gd_load_texture("res://assets/player_jump.png");
    d->initialized = true;
}

static void player_die_or_respawn(PlayerData *d) {
    g_state.lives -= 1;
    if (g_state.lives <= 0) {
        g_state.lives = 0;
        g_state.game_over = true;
        d->velocity.x = 0.0f;
        d->velocity.y = 0.0f;
    } else {
        gd_set_position(d->self, d->spawn_pos);
        d->velocity.x = 0.0f;
        d->velocity.y = 0.0f;
        d->invuln_timer = 1.5f;
    }
}

static void player_physics_process(GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_ret) {
    (void)r_ret;
    PlayerData *d = (PlayerData *)p_instance;
    float delta = p_args ? (float)(*(const double *)p_args[0]) : 0.016f;

    if (g_state.game_over || g_state.level_won) {
        if (gd_is_action_just_pressed("restart")) {
            game_state_reset();
            gd_reload_scene(d->self);
        }
        return;
    }

    if (g_state.player_bounce_pending) {
        g_state.player_bounce_pending = false;
        d->velocity.y = -420.0f;
    }

    if (g_state.player_hurt_pending) {
        g_state.player_hurt_pending = false;
        if (d->invuln_timer <= 0.0f) {
            player_die_or_respawn(d);
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
    bool on_floor = gd_is_on_floor(d->self);
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
    d->velocity = gd_move_and_slide(d->self, d->velocity, up);

    /* Pit fall check */
    Vec2 pos = gd_get_position(d->self);
    if (pos.y > 720.0f) {
        player_die_or_respawn(d);
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
}

static GDExtensionObjectPtr player_create(void *p_userdata, GDExtensionBool p_notify_postinitialize) {
    (void)p_userdata;
    PlayerData *d = (PlayerData *)gd_alloc(sizeof(PlayerData));
    d->self = gd_construct_base("CharacterBody2D", "Player", d, p_notify_postinitialize);
    return d->self;
}

static void player_free(void *p_userdata, GDExtensionClassInstancePtr p_instance) {
    (void)p_userdata;
    gd_free(p_instance);
}

static GDExtensionClassCallVirtual player_get_virtual(void *p_userdata, GDExtensionConstStringNamePtr p_name, uint32_t p_hash) {
    (void)p_userdata;
    (void)p_hash;
    if (gd_str_name_eq(p_name, "_ready")) return &player_ready;
    if (gd_str_name_eq(p_name, "_physics_process")) return &player_physics_process;
    return NULL;
}

void player_register(void) {
    gd_register_class("Player", "CharacterBody2D", &player_create, &player_free, &player_get_virtual);
}
