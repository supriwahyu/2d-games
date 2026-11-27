#include "gd_helper.h"
#include "game_state.h"

/* Class registration functions */
void player_register(void);
void enemy_register(void);
void coin_register(void);
void qblock_register(void);
void flagpole_register(void);
void hud_register(void);

GameState g_state = {
    .score = 0,
    .coins = 0,
    .lives = 3,
    .level_won = false,
    .game_over = false,
    .player_bounce_pending = false,
    .player_hurt_pending = false
};

void game_state_reset(void) {
    g_state.score = 0;
    g_state.coins = 0;
    g_state.lives = 3;
    g_state.level_won = false;
    g_state.game_over = false;
    g_state.player_bounce_pending = false;
    g_state.player_hurt_pending = false;
}

static void initialize_mario_module(void *p_userdata, GDExtensionInitializationLevel p_level) {
    (void)p_userdata;
    if (p_level != GDEXTENSION_INITIALIZATION_SCENE) {
        return;
    }

    gd_helper_init_scene();
    player_register();
    enemy_register();
    coin_register();
    qblock_register();
    flagpole_register();
    hud_register();
}

static void deinitialize_mario_module(void *p_userdata, GDExtensionInitializationLevel p_level) {
    (void)p_userdata;
    if (p_level != GDEXTENSION_INITIALIZATION_SCENE) {
        return;
    }
    gd_helper_deinit_scene();
}

GDExtensionBool mario_gdextension_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                       GDExtensionClassLibraryPtr p_library,
                                       GDExtensionInitialization *r_initialization) {
    gd_helper_init_procs(p_get_proc_address, p_library);

    r_initialization->minimum_initialization_level = GDEXTENSION_INITIALIZATION_SCENE;
    r_initialization->userdata = NULL;
    r_initialization->initialize = &initialize_mario_module;
    r_initialization->deinitialize = &deinitialize_mario_module;

    return 1;
}
