#include "gd_helper.h"
#include "game_state.h"

/* Player */
void *player_constructor(godot_object *instance, void *method_data);
void player_destructor(godot_object *instance, void *method_data, void *user_data);
godot_variant player_ready(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args);
godot_variant player_physics_process(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args);

/* Enemy */
void *enemy_constructor(godot_object *instance, void *method_data);
void enemy_destructor(godot_object *instance, void *method_data, void *user_data);
godot_variant enemy_ready(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args);
godot_variant enemy_physics_process(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args);

/* Coin */
void *coin_constructor(godot_object *instance, void *method_data);
void coin_destructor(godot_object *instance, void *method_data, void *user_data);
godot_variant coin_ready(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args);
godot_variant coin_process(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args);

/* QuestionBlock */
void *qblock_constructor(godot_object *instance, void *method_data);
void qblock_destructor(godot_object *instance, void *method_data, void *user_data);
godot_variant qblock_ready(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args);
godot_variant qblock_process(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args);

/* Flagpole */
void *flagpole_constructor(godot_object *instance, void *method_data);
void flagpole_destructor(godot_object *instance, void *method_data, void *user_data);
godot_variant flagpole_process(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args);

/* GameHUD */
void *hud_constructor(godot_object *instance, void *method_data);
void hud_destructor(godot_object *instance, void *method_data, void *user_data);
godot_variant hud_ready(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args);
godot_variant hud_process(godot_object *instance, void *method_data, void *user_data, int num_args, godot_variant **args);

static void register_method(void *handle, const char *cls, const char *name,
                            GDCALLINGCONV godot_variant (*fn)(godot_object *, void *, void *, int, godot_variant **)) {
    godot_method_attributes attr = { GODOT_METHOD_RPC_MODE_DISABLED };
    godot_instance_method m = { fn, NULL, NULL };
    g_nativescript_api->godot_nativescript_register_method(handle, cls, name, attr, m);
}

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *p_options) {
    g_api = p_options->api_struct;
    for (unsigned int i = 0; i < g_api->num_extensions; i++) {
        if (g_api->extensions[i]->type == GDNATIVE_EXT_NATIVESCRIPT) {
            g_nativescript_api = (const godot_gdnative_ext_nativescript_api_struct *)g_api->extensions[i];
            break;
        }
    }
}

void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *p_options) {
    (void)p_options;
    g_api = NULL;
    g_nativescript_api = NULL;
}

void GDN_EXPORT godot_nativescript_init(void *p_handle) {
    godot_instance_create_func create = { NULL, NULL, NULL };
    godot_instance_destroy_func destroy = { NULL, NULL, NULL };

    /* Player */
    create.create_func = player_constructor;
    destroy.destroy_func = player_destructor;
    g_nativescript_api->godot_nativescript_register_class(p_handle, "Player", "KinematicBody2D", create, destroy);
    register_method(p_handle, "Player", "_ready", player_ready);
    register_method(p_handle, "Player", "_physics_process", player_physics_process);

    /* Enemy */
    create.create_func = enemy_constructor;
    destroy.destroy_func = enemy_destructor;
    g_nativescript_api->godot_nativescript_register_class(p_handle, "Enemy", "KinematicBody2D", create, destroy);
    register_method(p_handle, "Enemy", "_ready", enemy_ready);
    register_method(p_handle, "Enemy", "_physics_process", enemy_physics_process);

    /* Coin */
    create.create_func = coin_constructor;
    destroy.destroy_func = coin_destructor;
    g_nativescript_api->godot_nativescript_register_class(p_handle, "Coin", "Area2D", create, destroy);
    register_method(p_handle, "Coin", "_ready", coin_ready);
    register_method(p_handle, "Coin", "_process", coin_process);

    /* QuestionBlock */
    create.create_func = qblock_constructor;
    destroy.destroy_func = qblock_destructor;
    g_nativescript_api->godot_nativescript_register_class(p_handle, "QuestionBlock", "StaticBody2D", create, destroy);
    register_method(p_handle, "QuestionBlock", "_ready", qblock_ready);
    register_method(p_handle, "QuestionBlock", "_process", qblock_process);

    /* Flagpole */
    create.create_func = flagpole_constructor;
    destroy.destroy_func = flagpole_destructor;
    g_nativescript_api->godot_nativescript_register_class(p_handle, "Flagpole", "Area2D", create, destroy);
    register_method(p_handle, "Flagpole", "_process", flagpole_process);

    /* GameHUD */
    create.create_func = hud_constructor;
    destroy.destroy_func = hud_destructor;
    g_nativescript_api->godot_nativescript_register_class(p_handle, "GameHUD", "CanvasLayer", create, destroy);
    register_method(p_handle, "GameHUD", "_ready", hud_ready);
    register_method(p_handle, "GameHUD", "_process", hud_process);
}
