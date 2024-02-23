~~*Move aside, [Py](https://github.com/touilleMan/godot-python/tree/godot4-meson), there’s a new sheriff in town!*~~

# Godot.rb – Ruby in [Godot Engine](https://godotengine.org) [![justforfunnoreally.dev badge](https://img.shields.io/badge/justforfunnoreally-dev-9ff)](https://justforfunnoreally.dev)

> ### Caution: This project is still developing.
> 
> While the Ruby land designs are mostly set, the C side code are all still experiments.
> Even the core components see occasional rewrites.
> 
> Moreöver, [the lead maintainer](https://github.com/ParadoxV5) is a C novice,
> so thank you early adopters for beïng prepared to encounter and report segfaults and core dumps 😅.


## Getting Started

1. As betaware, only Windows and [RubyInstaller2](https://rubyinstaller.org) __without Devkit__ is tested currently.
   It also needs [MinGW](https://www.mingw-w64.org) in the `PATH`. Things will go unexpected with the MSYS2 Devkit.
   * Support for Devkit and Linux are planned, but not for Windows going through WSL
     (but Linux support means yes for Godot Engine and Ruby both contained in WSL).
   
2. I might publish this project to RubyGems in the future.
   For now, download/clone this repo to a consistent place (i.e., not the Downloads folder) and run:
   ```sh
   gem install mingw-make
   gem install rake rake-compiler json
   rake
   rake clean # optional, delete leftovers
   ```
   If you don’t have [symlink permissions][],
   consider admin elevation for the `rake` line so the script symlinks Ruby DLLs rather than duplicating them.
   
3. Copy-paste or symlink `generated/godot_rb.gdextension` in your Godot Project
   (Unlike nearly all GDExtensions, this one uses absolute paths so multiple projects can share the same install.)

4. Have fun with [_real_ singletons](https://ruby-doc.org/current/syntax/modules_and_classes_rdoc.html#label-Singleton+Classes)! 💎
   (No ideas on exporting projects currently…)

<details>
<summary><h4>A couple of reminders for Godot.rb development</h4></summary>

* The [`Rakefile`](Rakefile) is for building, not setting up…

* `bundle install` fails on RubyInstaller no-Devkit because of the `Gemfile` app `steep`’s transitive dependency `ffi`.
  Gem insists to use the platform-independent version of the `ffi` gem, which doesn’t know about
 `bin/ruby_builtin_dlls/libffi-8.dll` and must compile `libffi` from scratch with `libffi` own `Makefile`.
  I’m unable to work around this problem one way or another (`#help-wanted`).

* RubyInstaller Ruby utilizes the new Universal C Runtime, but Godot Engine still uses the old MS VC++ Runtime,
  for they support (on a best-effort basis) the minority who refuse to upgrade their outdated Windows versions.
  See [godotengine/godot-proposals#9111][] regarding the situations. Fortunately, while mixing runtimes is unsupported,
  the only known issue is that Ruby cannot print to console (only `Godot#print` and etc. can).

</details>

[symlink permissions]: https://learn.microsoft.com/windows/security/threat-protection/security-policy-settings/create-symbolic-links
[godotengine/godot-proposals#9111]: https://github.com/godotengine/godot-proposals/discussions/9111


## Features

### ☑️️ Current
[TBA]

### 📝 Beta
[TBA]

### 🔜 Planned
[TBA]

### ⏳ Backlog
[TBA]

### 🧊 Anti-Features
[TBA]


## License

### [`include/godot/`](include/godot/)

Copyright (c) 2014-present Godot Engine contributors. https://godotengine.org/license/

### The rest of this project

Copyright 2024 ParadoxV5

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

> http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
