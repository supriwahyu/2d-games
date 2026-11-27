#include "gd_helper.h"
#include "game_state.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    GDExtensionObjectPtr self;
    GDExtensionObjectPtr stats_label;
    GDExtensionObjectPtr banner_label;
    GDExtensionObjectPtr restart_anchor;
} HudData;

static void hud_ready(GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_ret) {
    (void)p_args;
    (void)r_ret;
    HudData *d = (HudData *)p_instance;
    d->stats_label = gd_get_node(d->self, "StatsLabel");
    d->banner_label = gd_get_node(d->self, "BannerLabel");
    d->restart_anchor = gd_get_node(d->self, "RestartAnchor");
    if (d->banner_label) {
        gd_canvas_set_visible(d->banner_label, false);
    }
    if (d->restart_anchor) {
        gd_canvas_set_visible(d->restart_anchor, false);
    }
}

static void hud_process(GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_ret) {
    (void)p_args;
    (void)r_ret;
    HudData *d = (HudData *)p_instance;

    if (d->stats_label) {
        char buf[128];
        snprintf(buf, sizeof(buf), "SCORE: %06d    COINS: %02d    LIVES: %d",
                 g_state.score, g_state.coins, g_state.lives);
        gd_label_set_text(d->stats_label, buf);
    }

    bool show_end = g_state.level_won || g_state.game_over;
    if (d->banner_label) {
        if (g_state.level_won) {
            gd_label_set_text(d->banner_label, "COURSE CLEAR!  TAP RETRY OR PRESS R");
            gd_canvas_set_visible(d->banner_label, true);
        } else if (g_state.game_over) {
            gd_label_set_text(d->banner_label, "GAME OVER!  TAP RETRY OR PRESS R");
            gd_canvas_set_visible(d->banner_label, true);
        } else {
            gd_canvas_set_visible(d->banner_label, false);
        }
    }
    if (d->restart_anchor) {
        gd_canvas_set_visible(d->restart_anchor, show_end);
    }
}

static GDExtensionObjectPtr hud_create(void *p_userdata, GDExtensionBool p_notify_postinitialize) {
    (void)p_userdata;
    HudData *d = (HudData *)gd_alloc(sizeof(HudData));
    d->self = gd_construct_base("CanvasLayer", "GameHUD", d, p_notify_postinitialize);
    return d->self;
}

static void hud_free(void *p_userdata, GDExtensionClassInstancePtr p_instance) {
    (void)p_userdata;
    gd_free(p_instance);
}

static GDExtensionClassCallVirtual hud_get_virtual(void *p_userdata, GDExtensionConstStringNamePtr p_name, uint32_t p_hash) {
    (void)p_userdata;
    (void)p_hash;
    if (gd_str_name_eq(p_name, "_ready")) return &hud_ready;
    if (gd_str_name_eq(p_name, "_process")) return &hud_process;
    return NULL;
}

void hud_register(void) {
    gd_register_class("GameHUD", "CanvasLayer", &hud_create, &hud_free, &hud_get_virtual);
}
