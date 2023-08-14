#include <ruby.gdextension.h>

// API, uninitialized
struct ruby_gdextension ruby_gdextension = {
  NULL,
  NULL,
  false
};

void ruby_gdextension_setup(__attribute__((unused)) void* userdata, GDExtensionInitializationLevel p_level) {
  switch(p_level) {
    case GDEXTENSION_INITIALIZATION_CORE:
      if(ruby_setup())
        /* Error */;
      else
        ruby_gdextension.ruby_setup = true;
    case GDEXTENSION_INITIALIZATION_SERVERS:
    case GDEXTENSION_INITIALIZATION_SCENE:
    default:
      break; // Do nothing
  }
}

void ruby_gdextension_cleanup(__attribute__((unused)) void* userdata, GDExtensionInitializationLevel p_level) {
  if(ruby_gdextension.ruby_setup == 0)
    switch(p_level) {
      case GDEXTENSION_INITIALIZATION_SCENE:
      case GDEXTENSION_INITIALIZATION_SERVERS:
      case GDEXTENSION_INITIALIZATION_CORE:
      default:
        break; // Do nothing
    }
}

GDExtensionBool ruby_gdextension_main(
  GDExtensionInterfaceGetProcAddress p_get_proc_address,
  GDExtensionClassLibraryPtr p_library,
  GDExtensionInitialization* r_initialization
) {
  // Set up de/initialization functions
  r_initialization->minimum_initialization_level = GDEXTENSION_INITIALIZATION_CORE;
  r_initialization->initialize = &ruby_gdextension_setup;
  r_initialization->deinitialize = &ruby_gdextension_cleanup;
  // Save GDExtension API
  r_initialization->userdata = &ruby_gdextension;
  ruby_gdextension.get_proc_address = p_get_proc_address;
  ruby_gdextension.library = p_library;
  // Success
  return true;
}
