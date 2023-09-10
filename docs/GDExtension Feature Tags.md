# Godot Engine and GDExtension Feature Tags

Reference: [Godot Docs: Feature tags § Default features](https://docs.godotengine.org/en/stable/tutorials/export/feature_tags.html#default-features)

## Appendix: Output Structure
* `bin/`
  * `<architecture>/`
    * `<platform>/`
      (incompatibilities are more likely across different arches than platforms)
      * `debug.lib`
      * `release.lib`
      * `<libruby>`
      * additional Ruby dependency libs

## Platforms
* windows
* macos
* linuxbsd
  * linux
  * bsd
* android
* ios
* uwp

## Architectures
* x86
  * x86_64
  * x86_32
* arm
  * arm64
  * arm32
* riscv
  * rv64
* ppc
  * ppc64
  * ppc32
* wasm
  * wasm32
  * wasm64

## Build Types
* ~~editor~~ (we currently fall it back to ‘debug’)
* debug
* release

## Float Precision
* ~~single (default Godot Engine config)~~ (we currently fall it back to ‘double’)
* double
