#include <godot_rb.h>
#include "gdextension_interface.h"

/**
  * This repurposes some elements similar to godot_rb_cVariants.
    * `GDEXTENSION_VARIANT_TYPE_NIL`..`GDEXTENSION_VARIANT_TYPE_FLOAT` ➡ unassigned
  * `GDEXTENSION_VARIANT_TYPE_STRING`.. ➡ corresponding T_DATA info
  
  `void* data` holds `godot_variant_get_ptr_constructor(type_id, 1)`
*/
extern rb_data_type_t godot_rb_cVariant_types[GDEXTENSION_VARIANT_TYPE_VARIANT_MAX];

// Loaded in [`variant_types.c`](variant_types.c.erb)
extern GDExtensionInterfaceVariantGetPtrConstructor     godot_variant_get_ptr_constructor;
extern GDExtensionInterfaceStringNameNewWithLatin1Chars godot_string_name_new_with_latin1_chars;
extern GDExtensionInterfaceObjectMethodBindPtrcall      godot_object_method_bind_ptrcall;
extern GDExtensionInterfaceGlobalGetSingleton           godot_global_get_singleton;
extern GDExtensionInterfaceClassdbGetMethodBind         godot_classdb_get_method_bind;

void Init_variant_types();
void Init_variant();
