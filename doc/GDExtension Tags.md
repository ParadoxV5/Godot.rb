## Output Structure
* `bin/`
  * `<architecture>/`
    * `<platform>/`
      (incompatibilities are more likely across different arches than platforms)
      * `debug.<ext>`
      * `release.<ext>`
      * `<libruby>`
      * additional Ruby dependency libs

### Platforms
* windows
* macos
* linuxbsd
  * linux
  * bsd
* android
* ios
* uwp

### Architectures
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

### Build Types
* editor
* debug
* release
