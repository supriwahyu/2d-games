#include "gd_helper.h"
#include "game_state.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    godot_object *stats_label;
    godot_object *banner_label;
} HudData;

void *hud_constructor(godot_object *instance, void *method_data) {
    (void)instance;
    (void)method_data;
    return calloc(1, sizeof(HudData));
}

void hud_destructor(godot_object *instance, void *method_data, void *user_data) {
    (void)instance;
    (void)method_data;
    free(user_data);
}

godot_variant hud_ready(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args) {
    (void)method_data;
    (void)num_args;
    (void)args;
    HudData *d = (HudData *)user_data;
    d->stats_label = gd_get_node(instance, "StatsLabel");
    d->banner_label = gd_get_node(instance, "BannerLabel");
    if (d->banner_label) {
        gd_canvas_set_visible(d->banner_label, false);
    }
    godot_variant ret;
    g_api->godot_variant_new_nil(&ret);
    return ret;
}

godot_variant hud_process(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args) {
    (void)instance;
    (void)method_data;
    (void)num_args;
    (void)args;
    HudData *d = (HudData *)user_data;

    if (d->stats_label) {
        char buf[128];
        snprintf(buf, sizeof(buf), "SCORE: %06d    COINS: %02d    LIVES: %d",
                 g_state.score, g_state.coins, g_state.lives);
        gd_label_set_text(d->stats_label, buf);
    }

    if (d->banner_label) {
        if (g_state.level_won) {
            gd_label_set_text(d->banner_label, "COURSE CLEAR!  PRESS R TO PLAY AGAIN");
            gd_canvas_set_visible(d->banner_label, true);
        } else if (g_state.game_over) {
            gd_label_set_text(d->banner_label, "GAME OVER!  PRESS R TO RESTART");
            gd_canvas_set_visible(d->banner_label, true);
        } else {
            gd_canvas_set_visible(d->banner_label, false);
        }
    }

    godot_variant ret;
    g_api->godot_variant_new_nil(&ret);
    return ret;
}
