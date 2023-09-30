// List of GDExtension Interface functions neither yet utilized nor found unnecessary //

// Based on [`gdextension_interface.h`](include/godot/gdextension_interface.h):
// Copyright (c) 2014-present Godot Engine contributors. https://godotengine.org/license/

#include "../../include/godot/gdextension_interface.h"

/* INTERFACE */
GDExtensionInterfaceGetGodotVersion get_godot_version;

/* INTERFACE: Memory */
GDExtensionInterfaceMemRealloc mem_realloc;

/* INTERFACE: Variant */
GDExtensionInterfaceVariantCallStatic variant_call_static;
GDExtensionInterfaceVariantEvaluate variant_evaluate;
GDExtensionInterfaceVariantIterInit variant_iter_init;
GDExtensionInterfaceVariantIterNext variant_iter_next;
GDExtensionInterfaceVariantIterGet variant_iter_get;
GDExtensionInterfaceVariantHash variant_hash;
GDExtensionInterfaceVariantRecursiveHash variant_recursive_hash;
GDExtensionInterfaceVariantHashCompare variant_hash_compare;
GDExtensionInterfaceVariantStringify variant_stringify;
GDExtensionInterfaceVariantGetConstantValue variant_get_constant_value;
GDExtensionInterfaceVariantGetPtrUtilityFunction variant_get_ptr_utility_function;

/* INTERFACE: FileAccess Utilities */
GDExtensionInterfaceFileAccessStoreBuffer file_access_store_buffer;
GDExtensionInterfaceFileAccessGetBuffer file_access_get_buffer;

/* INTERFACE: WorkerThreadPool Utilities */
GDExtensionInterfaceWorkerThreadPoolAddNativeGroupTask worker_thread_pool_add_native_group_task;
GDExtensionInterfaceWorkerThreadPoolAddNativeTask worker_thread_pool_add_native_task;

/* INTERFACE: Object */
GDExtensionInterfaceObjectCastTo object_cast_to;
GDExtensionInterfaceObjectGetInstanceId object_get_instance_id;
