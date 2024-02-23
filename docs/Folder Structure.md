# Folder and File Structure

* *`doc/`* – output folder for YARD, which covers both Godot.rb itself and generated glue code
* **`docs/`** – additional, mostly technical documentation
* `ext/` – Godot.rb compile sources
  * `godot_rb/` – Godot.rb C sources
  * `include/`
    * `godot/` – [submodule of Godot Engine GDExtension header and data](https://github.com/ParadoxV5/godot-headers)
    * `godot_rb.h` – public header for Godot.rb in case anyone wanna write an extension
* `generated/`
  * **`*.rb`** – wrapper Ruby code for GDScript classes and functions
  * `godot.rbs` – glue RBS for the glue Ruby (*might* become non-deletable in the future)
  * `godot_rb.gdextension` – the GDExtension config file that you can copy/symlink in your projects
  * **`godot_rb.so`** – the GDExtension shared library that Godot Engine loads
  * **`*.so`/`*.dll`** – Symlinks to CRuby shared libraries
* `lib/`
  * **`godot_rb/`** – Godot.rb Ruby library
  * `godot_rb.rb` – in case someone skipped tutorial and went straight to `require 'godot_rb'`
* `sig/` – RBS for the Godot.rb library (for devs’ reference; users likely won’t need these)
* *`tmp/`* – workspace for `rake-compiler`

Once built, only entries in **strong** (and their subentries) are mandatory for Godot.rb to work.
