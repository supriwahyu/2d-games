#include "gd_helper.h"
#include <string.h>

static GDExtensionClassLibraryPtr g_library = NULL;

/* Core GDExtension procs */
static GDExtensionInterfaceMemAlloc gd_mem_alloc = NULL;
static GDExtensionInterfaceMemFree gd_mem_free = NULL;
static GDExtensionInterfaceStringNewWithUtf8Chars gd_string_new_utf8 = NULL;
static GDExtensionInterfaceStringNameNewWithUtf8Chars gd_string_name_new_utf8 = NULL;
static GDExtensionInterfaceVariantGetPtrConstructor gd_variant_get_ptr_constructor = NULL;
static GDExtensionInterfaceVariantGetPtrDestructor gd_variant_get_ptr_destructor = NULL;
static GDExtensionInterfaceVariantGetPtrOperatorEvaluator gd_variant_get_ptr_op_eval = NULL;
static GDExtensionInterfaceVariantGetPtrBuiltinMethod gd_variant_get_ptr_builtin_method = NULL;
static GDExtensionInterfaceGetVariantToTypeConstructor gd_get_var_to_type_ctor = NULL;
static GDExtensionInterfaceVariantGetType gd_variant_get_type = NULL;
static GDExtensionInterfaceVariantDestroy gd_variant_destroy = NULL;
static GDExtensionInterfaceGlobalGetSingleton gd_global_get_singleton = NULL;
static GDExtensionInterfaceClassdbGetMethodBind gd_classdb_get_method_bind = NULL;
static GDExtensionInterfaceClassdbRegisterExtensionClass6 gd_classdb_register_class6 = NULL;
static GDExtensionInterfaceClassdbConstructObject3 gd_classdb_construct_object3 = NULL;
static GDExtensionInterfaceObjectSetInstance gd_object_set_instance = NULL;
static GDExtensionInterfaceObjectMethodBindPtrcall gd_object_ptrcall = NULL;

/* Builtin helpers */
static GDExtensionPtrDestructor dtor_string = NULL;
static GDExtensionPtrDestructor dtor_string_name = NULL;
static GDExtensionPtrDestructor dtor_node_path = NULL;
static GDExtensionPtrDestructor dtor_array = NULL;
static GDExtensionPtrConstructor ctor_node_path_from_string = NULL;
static GDExtensionPtrOperatorEvaluator op_string_name_equal = NULL;
static GDExtensionPtrBuiltInMethod bm_array_size = NULL;
static GDExtensionPtrBuiltInMethod bm_array_get = NULL;
static GDExtensionTypeFromVariantConstructorFunc var_to_obj = NULL;

/* Method binds */
static GDExtensionMethodBindPtr mb_object_notification = NULL;
static GDExtensionMethodBindPtr mb_input_is_action_pressed = NULL;
static GDExtensionMethodBindPtr mb_input_is_action_just_pressed = NULL;
static GDExtensionMethodBindPtr mb_node_get_node = NULL;
static GDExtensionMethodBindPtr mb_node_get_tree = NULL;
static GDExtensionMethodBindPtr mb_node_queue_free = NULL;
static GDExtensionMethodBindPtr mb_node_add_to_group = NULL;
static GDExtensionMethodBindPtr mb_node_is_in_group = NULL;
static GDExtensionMethodBindPtr mb_scenetree_reload_current_scene = NULL;
static GDExtensionMethodBindPtr mb_node2d_get_position = NULL;
static GDExtensionMethodBindPtr mb_node2d_set_position = NULL;
static GDExtensionMethodBindPtr mb_resource_loader_load = NULL;
static GDExtensionMethodBindPtr mb_sprite2d_set_texture = NULL;
static GDExtensionMethodBindPtr mb_sprite2d_set_flip_h = NULL;
static GDExtensionMethodBindPtr mb_label_set_text = NULL;
static GDExtensionMethodBindPtr mb_canvas_set_visible = NULL;
static GDExtensionMethodBindPtr mb_charbody2d_set_velocity = NULL;
static GDExtensionMethodBindPtr mb_charbody2d_get_velocity = NULL;
static GDExtensionMethodBindPtr mb_charbody2d_set_up_direction = NULL;
static GDExtensionMethodBindPtr mb_charbody2d_move_and_slide = NULL;
static GDExtensionMethodBindPtr mb_charbody2d_is_on_floor = NULL;
static GDExtensionMethodBindPtr mb_charbody2d_is_on_wall = NULL;
static GDExtensionMethodBindPtr mb_charbody2d_is_on_ceiling = NULL;
static GDExtensionMethodBindPtr mb_area2d_get_overlapping_bodies = NULL;

/* Singletons */
static GDExtensionObjectPtr g_input_singleton = NULL;
static GDExtensionObjectPtr g_resource_loader_singleton = NULL;

static GDExtensionMethodBindPtr get_method(const char *cls, const char *method, GDExtensionInt hash) {
    gd_string_name sn_cls, sn_method;
    gd_string_name_new_utf8(&sn_cls, cls);
    gd_string_name_new_utf8(&sn_method, method);
    GDExtensionMethodBindPtr mb = gd_classdb_get_method_bind(&sn_cls, &sn_method, hash);
    dtor_string_name(&sn_method);
    dtor_string_name(&sn_cls);
    return mb;
}

static GDExtensionObjectPtr get_singleton(const char *name) {
    gd_string_name sn;
    gd_string_name_new_utf8(&sn, name);
    GDExtensionObjectPtr obj = gd_global_get_singleton(&sn);
    dtor_string_name(&sn);
    return obj;
}

void gd_helper_init_procs(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr library) {
    g_library = library;
    gd_mem_alloc = (GDExtensionInterfaceMemAlloc)(void *)p_get_proc_address("mem_alloc");
    gd_mem_free = (GDExtensionInterfaceMemFree)(void *)p_get_proc_address("mem_free");
    gd_string_new_utf8 = (GDExtensionInterfaceStringNewWithUtf8Chars)(void *)p_get_proc_address("string_new_with_utf8_chars");
    gd_string_name_new_utf8 = (GDExtensionInterfaceStringNameNewWithUtf8Chars)(void *)p_get_proc_address("string_name_new_with_utf8_chars");
    gd_variant_get_ptr_constructor = (GDExtensionInterfaceVariantGetPtrConstructor)(void *)p_get_proc_address("variant_get_ptr_constructor");
    gd_variant_get_ptr_destructor = (GDExtensionInterfaceVariantGetPtrDestructor)(void *)p_get_proc_address("variant_get_ptr_destructor");
    gd_variant_get_ptr_op_eval = (GDExtensionInterfaceVariantGetPtrOperatorEvaluator)(void *)p_get_proc_address("variant_get_ptr_operator_evaluator");
    gd_variant_get_ptr_builtin_method = (GDExtensionInterfaceVariantGetPtrBuiltinMethod)(void *)p_get_proc_address("variant_get_ptr_builtin_method");
    gd_get_var_to_type_ctor = (GDExtensionInterfaceGetVariantToTypeConstructor)(void *)p_get_proc_address("get_variant_to_type_constructor");
    gd_variant_get_type = (GDExtensionInterfaceVariantGetType)(void *)p_get_proc_address("variant_get_type");
    gd_variant_destroy = (GDExtensionInterfaceVariantDestroy)(void *)p_get_proc_address("variant_destroy");
    gd_global_get_singleton = (GDExtensionInterfaceGlobalGetSingleton)(void *)p_get_proc_address("global_get_singleton");
    gd_classdb_get_method_bind = (GDExtensionInterfaceClassdbGetMethodBind)(void *)p_get_proc_address("classdb_get_method_bind");
    gd_classdb_register_class6 = (GDExtensionInterfaceClassdbRegisterExtensionClass6)(void *)p_get_proc_address("classdb_register_extension_class6");
    gd_classdb_construct_object3 = (GDExtensionInterfaceClassdbConstructObject3)(void *)p_get_proc_address("classdb_construct_object3");
    gd_object_set_instance = (GDExtensionInterfaceObjectSetInstance)(void *)p_get_proc_address("object_set_instance");
    gd_object_ptrcall = (GDExtensionInterfaceObjectMethodBindPtrcall)(void *)p_get_proc_address("object_method_bind_ptrcall");
}

void gd_helper_init_scene(void) {
    dtor_string = gd_variant_get_ptr_destructor(GDEXTENSION_VARIANT_TYPE_STRING);
    dtor_string_name = gd_variant_get_ptr_destructor(GDEXTENSION_VARIANT_TYPE_STRING_NAME);
    dtor_node_path = gd_variant_get_ptr_destructor(GDEXTENSION_VARIANT_TYPE_NODE_PATH);
    dtor_array = gd_variant_get_ptr_destructor(GDEXTENSION_VARIANT_TYPE_ARRAY);
    ctor_node_path_from_string = gd_variant_get_ptr_constructor(GDEXTENSION_VARIANT_TYPE_NODE_PATH, 2);
    op_string_name_equal = gd_variant_get_ptr_op_eval(
        GDEXTENSION_VARIANT_OP_EQUAL,
        GDEXTENSION_VARIANT_TYPE_STRING_NAME,
        GDEXTENSION_VARIANT_TYPE_STRING_NAME
    );
    var_to_obj = gd_get_var_to_type_ctor(GDEXTENSION_VARIANT_TYPE_OBJECT);

    gd_string_name sn_size, sn_get;
    gd_string_name_new_utf8(&sn_size, "size");
    gd_string_name_new_utf8(&sn_get, "get");
    bm_array_size = gd_variant_get_ptr_builtin_method(GDEXTENSION_VARIANT_TYPE_ARRAY, &sn_size, 3173160232);
    bm_array_get = gd_variant_get_ptr_builtin_method(GDEXTENSION_VARIANT_TYPE_ARRAY, &sn_get, 708700221);
    dtor_string_name(&sn_get);
    dtor_string_name(&sn_size);

    mb_object_notification = get_method("Object", "notification", 4023243586);
    mb_input_is_action_pressed = get_method("Input", "is_action_pressed", 1558498928);
    mb_input_is_action_just_pressed = get_method("Input", "is_action_just_pressed", 1558498928);
    mb_node_get_node = get_method("Node", "get_node", 2734337346);
    mb_node_get_tree = get_method("Node", "get_tree", 2958820483);
    mb_node_queue_free = get_method("Node", "queue_free", 3218959716);
    mb_node_add_to_group = get_method("Node", "add_to_group", 3683006648);
    mb_node_is_in_group = get_method("Node", "is_in_group", 2619796661);
    mb_scenetree_reload_current_scene = get_method("SceneTree", "reload_current_scene", 166280745);
    mb_node2d_get_position = get_method("Node2D", "get_position", 3341600327);
    mb_node2d_set_position = get_method("Node2D", "set_position", 743155724);
    mb_resource_loader_load = get_method("ResourceLoader", "load", 3358495409);
    mb_sprite2d_set_texture = get_method("Sprite2D", "set_texture", 4051416890);
    mb_sprite2d_set_flip_h = get_method("Sprite2D", "set_flip_h", 2586408642);
    mb_label_set_text = get_method("Label", "set_text", 83702148);
    mb_canvas_set_visible = get_method("CanvasItem", "set_visible", 2586408642);
    mb_charbody2d_set_velocity = get_method("CharacterBody2D", "set_velocity", 743155724);
    mb_charbody2d_get_velocity = get_method("CharacterBody2D", "get_velocity", 3341600327);
    mb_charbody2d_set_up_direction = get_method("CharacterBody2D", "set_up_direction", 743155724);
    mb_charbody2d_move_and_slide = get_method("CharacterBody2D", "move_and_slide", 2240911060);
    mb_charbody2d_is_on_floor = get_method("CharacterBody2D", "is_on_floor", 36873697);
    mb_charbody2d_is_on_wall = get_method("CharacterBody2D", "is_on_wall", 36873697);
    mb_charbody2d_is_on_ceiling = get_method("CharacterBody2D", "is_on_ceiling", 36873697);
    mb_area2d_get_overlapping_bodies = get_method("Area2D", "get_overlapping_bodies", 3995934104);

    g_input_singleton = get_singleton("Input");
    g_resource_loader_singleton = get_singleton("ResourceLoader");
}

void gd_helper_deinit_scene(void) {
    g_input_singleton = NULL;
    g_resource_loader_singleton = NULL;
}

void gd_register_class(const char *name,
                       const char *parent,
                       GDExtensionClassCreateInstance2 create_fn,
                       GDExtensionClassFreeInstance free_fn,
                       GDExtensionClassGetVirtual2 get_virtual_fn) {
    gd_string_name sn_name, sn_parent;
    gd_string_name_new_utf8(&sn_name, name);
    gd_string_name_new_utf8(&sn_parent, parent);

    GDExtensionClassCreationInfo6 info;
    memset(&info, 0, sizeof(info));
    info.is_exposed = 1;
    info.create_instance_func = create_fn;
    info.free_instance_func = free_fn;
    info.get_virtual_func = get_virtual_fn;

    gd_classdb_register_class6(g_library, &sn_name, &sn_parent, &info);
    dtor_string_name(&sn_parent);
    dtor_string_name(&sn_name);
}

GDExtensionObjectPtr gd_construct_base(const char *parent_name, const char *class_name, void *instance_data, GDExtensionBool notify_postinitialize) {
    gd_string_name sn_parent, sn_class;
    gd_string_name_new_utf8(&sn_parent, parent_name);
    gd_string_name_new_utf8(&sn_class, class_name);

    GDExtensionObjectPtr obj = gd_classdb_construct_object3(&sn_parent);
    gd_object_set_instance(obj, &sn_class, instance_data);

    if (notify_postinitialize && mb_object_notification) {
        GDExtensionInt what = 0; /* NOTIFICATION_POSTINITIALIZE */
        GDExtensionBool reversed = 0;
        const GDExtensionConstTypePtr args[2] = { &what, &reversed };
        gd_object_ptrcall(mb_object_notification, obj, args, NULL);
    }

    dtor_string_name(&sn_class);
    dtor_string_name(&sn_parent);
    return obj;
}

void *gd_alloc(size_t size) {
    void *p = gd_mem_alloc(size);
    if (p) memset(p, 0, size);
    return p;
}

void gd_free(void *ptr) {
    if (ptr) gd_mem_free(ptr);
}

bool gd_str_name_eq(GDExtensionConstStringNamePtr a, const char *b) {
    gd_string_name sn_b;
    gd_string_name_new_utf8(&sn_b, b);
    uint8_t eq = 0;
    op_string_name_equal(a, &sn_b, &eq);
    dtor_string_name(&sn_b);
    return eq != 0;
}

bool gd_is_action_pressed(const char *action) {
    if (!g_input_singleton || !mb_input_is_action_pressed) return false;
    gd_string_name sn;
    gd_string_name_new_utf8(&sn, action);
    uint8_t exact = 0;
    uint8_t ret = 0;
    const GDExtensionConstTypePtr args[2] = { &sn, &exact };
    gd_object_ptrcall(mb_input_is_action_pressed, g_input_singleton, args, &ret);
    dtor_string_name(&sn);
    return ret != 0;
}

bool gd_is_action_just_pressed(const char *action) {
    if (!g_input_singleton || !mb_input_is_action_just_pressed) return false;
    gd_string_name sn;
    gd_string_name_new_utf8(&sn, action);
    uint8_t exact = 0;
    uint8_t ret = 0;
    const GDExtensionConstTypePtr args[2] = { &sn, &exact };
    gd_object_ptrcall(mb_input_is_action_just_pressed, g_input_singleton, args, &ret);
    dtor_string_name(&sn);
    return ret != 0;
}

GDExtensionObjectPtr gd_get_node(GDExtensionObjectPtr self, const char *path) {
    if (!self || !mb_node_get_node) return NULL;
    gd_string s;
    gd_node_path np;
    gd_string_new_utf8(&s, path);
    const GDExtensionConstTypePtr np_args[1] = { &s };
    ctor_node_path_from_string(&np, np_args);

    GDExtensionObjectPtr ret = NULL;
    const GDExtensionConstTypePtr args[1] = { &np };
    gd_object_ptrcall(mb_node_get_node, self, args, &ret);

    dtor_node_path(&np);
    dtor_string(&s);
    return ret;
}

GDExtensionObjectPtr gd_get_tree(GDExtensionObjectPtr self) {
    if (!self || !mb_node_get_tree) return NULL;
    GDExtensionObjectPtr ret = NULL;
    gd_object_ptrcall(mb_node_get_tree, self, NULL, &ret);
    return ret;
}

void gd_reload_scene(GDExtensionObjectPtr self) {
    GDExtensionObjectPtr tree = gd_get_tree(self);
    if (!tree || !mb_scenetree_reload_current_scene) return;
    GDExtensionInt err = 0;
    gd_object_ptrcall(mb_scenetree_reload_current_scene, tree, NULL, &err);
}

void gd_queue_free(GDExtensionObjectPtr self) {
    if (!self || !mb_node_queue_free) return;
    gd_object_ptrcall(mb_node_queue_free, self, NULL, NULL);
}

void gd_add_to_group(GDExtensionObjectPtr self, const char *group) {
    if (!self || !mb_node_add_to_group) return;
    gd_string_name sn;
    gd_string_name_new_utf8(&sn, group);
    uint8_t persistent = 0;
    const GDExtensionConstTypePtr args[2] = { &sn, &persistent };
    gd_object_ptrcall(mb_node_add_to_group, self, args, NULL);
    dtor_string_name(&sn);
}

bool gd_is_in_group(GDExtensionObjectPtr obj, const char *group) {
    if (!obj || !mb_node_is_in_group) return false;
    gd_string_name sn;
    gd_string_name_new_utf8(&sn, group);
    uint8_t ret = 0;
    const GDExtensionConstTypePtr args[1] = { &sn };
    gd_object_ptrcall(mb_node_is_in_group, obj, args, &ret);
    dtor_string_name(&sn);
    return ret != 0;
}

Vec2 gd_get_position(GDExtensionObjectPtr node2d) {
    Vec2 v = { 0.0f, 0.0f };
    if (!node2d || !mb_node2d_get_position) return v;
    gd_object_ptrcall(mb_node2d_get_position, node2d, NULL, &v);
    return v;
}

void gd_set_position(GDExtensionObjectPtr node2d, Vec2 pos) {
    if (!node2d || !mb_node2d_set_position) return;
    const GDExtensionConstTypePtr args[1] = { &pos };
    gd_object_ptrcall(mb_node2d_set_position, node2d, args, NULL);
}

GDExtensionObjectPtr gd_load_texture(const char *res_path) {
    if (!g_resource_loader_singleton || !mb_resource_loader_load) return NULL;
    gd_string s_path, s_hint;
    gd_string_new_utf8(&s_path, res_path);
    gd_string_new_utf8(&s_hint, "");
    GDExtensionInt cache_mode = 1; /* CACHE_MODE_REUSE */
    GDExtensionObjectPtr ret = NULL;
    const GDExtensionConstTypePtr args[3] = { &s_path, &s_hint, &cache_mode };
    gd_object_ptrcall(mb_resource_loader_load, g_resource_loader_singleton, args, &ret);
    dtor_string(&s_hint);
    dtor_string(&s_path);
    return ret;
}

void gd_sprite_set_texture(GDExtensionObjectPtr sprite, GDExtensionObjectPtr texture) {
    if (!sprite || !mb_sprite2d_set_texture) return;
    const GDExtensionConstTypePtr args[1] = { &texture };
    gd_object_ptrcall(mb_sprite2d_set_texture, sprite, args, NULL);
}

void gd_sprite_set_flip_h(GDExtensionObjectPtr sprite, bool flip) {
    if (!sprite || !mb_sprite2d_set_flip_h) return;
    uint8_t f = flip ? 1 : 0;
    const GDExtensionConstTypePtr args[1] = { &f };
    gd_object_ptrcall(mb_sprite2d_set_flip_h, sprite, args, NULL);
}

void gd_label_set_text(GDExtensionObjectPtr label, const char *text) {
    if (!label || !mb_label_set_text) return;
    gd_string s;
    gd_string_new_utf8(&s, text);
    const GDExtensionConstTypePtr args[1] = { &s };
    gd_object_ptrcall(mb_label_set_text, label, args, NULL);
    dtor_string(&s);
}

void gd_canvas_set_visible(GDExtensionObjectPtr item, bool visible) {
    if (!item || !mb_canvas_set_visible) return;
    uint8_t v = visible ? 1 : 0;
    const GDExtensionConstTypePtr args[1] = { &v };
    gd_object_ptrcall(mb_canvas_set_visible, item, args, NULL);
}

Vec2 gd_move_and_slide(GDExtensionObjectPtr body, Vec2 vel, Vec2 up) {
    if (!body || !mb_charbody2d_move_and_slide) return vel;
    const GDExtensionConstTypePtr args_vel[1] = { &vel };
    gd_object_ptrcall(mb_charbody2d_set_velocity, body, args_vel, NULL);

    const GDExtensionConstTypePtr args_up[1] = { &up };
    gd_object_ptrcall(mb_charbody2d_set_up_direction, body, args_up, NULL);

    uint8_t collided = 0;
    gd_object_ptrcall(mb_charbody2d_move_and_slide, body, NULL, &collided);

    Vec2 out = { 0.0f, 0.0f };
    gd_object_ptrcall(mb_charbody2d_get_velocity, body, NULL, &out);
    return out;
}

bool gd_is_on_floor(GDExtensionObjectPtr body) {
    if (!body || !mb_charbody2d_is_on_floor) return false;
    uint8_t ret = 0;
    gd_object_ptrcall(mb_charbody2d_is_on_floor, body, NULL, &ret);
    return ret != 0;
}

bool gd_is_on_wall(GDExtensionObjectPtr body) {
    if (!body || !mb_charbody2d_is_on_wall) return false;
    uint8_t ret = 0;
    gd_object_ptrcall(mb_charbody2d_is_on_wall, body, NULL, &ret);
    return ret != 0;
}

bool gd_is_on_ceiling(GDExtensionObjectPtr body) {
    if (!body || !mb_charbody2d_is_on_ceiling) return false;
    uint8_t ret = 0;
    gd_object_ptrcall(mb_charbody2d_is_on_ceiling, body, NULL, &ret);
    return ret != 0;
}

gd_array gd_get_overlapping_bodies(GDExtensionObjectPtr area) {
    gd_array arr;
    memset(&arr, 0, sizeof(arr));
    if (!area || !mb_area2d_get_overlapping_bodies) return arr;
    gd_object_ptrcall(mb_area2d_get_overlapping_bodies, area, NULL, &arr);
    return arr;
}

int gd_array_size(gd_array *arr) {
    if (!arr || !bm_array_size) return 0;
    GDExtensionInt sz = 0;
    bm_array_size(arr, NULL, &sz, 0);
    return (int)sz;
}

GDExtensionObjectPtr gd_array_get_object(gd_array *arr, int index) {
    if (!arr || !bm_array_get) return NULL;
    GDExtensionInt idx = index;
    const GDExtensionConstTypePtr args[1] = { &idx };
    gd_variant v;
    memset(&v, 0, sizeof(v));
    bm_array_get(arr, args, &v, 1);

    GDExtensionObjectPtr obj = NULL;
    if (gd_variant_get_type(&v) == GDEXTENSION_VARIANT_TYPE_OBJECT) {
        var_to_obj(&obj, &v);
    }
    gd_variant_destroy(&v);
    return obj;
}

void gd_array_destroy(gd_array *arr) {
    if (arr && dtor_array) {
        dtor_array(arr);
    }
}
