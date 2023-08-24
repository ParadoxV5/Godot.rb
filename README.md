~~Move aside, [🐍](https://github.com/touilleMan/godot-python/tree/godot4-meson), there’s a new sheriff in town!~~

# Godot.rb – CRuby in Godot Engine [![justforfunnoreally.dev badge](https://img.shields.io/badge/justforfunnoreally-dev-9ff)](https://justforfunnoreally.dev)


## Features

### ☑️️ Current
[TBA]

### 🔜 Planned
* Other platforms – Only Windows, [RubyInstaller](https://rubyinstaller.org) non-Devkit + MinGW tested currently.
* [TBA]

### 🧊 Anti-Features
[TBA]


## Building

### GitHub Actions CI (Coming Soon)

[The GitHub Actions workflow](.github/workflows/build_and_test.yml)
automatically builds and tests relevant commits on all supported platforms and architectures, or you can also
[manually activate the CI](https://docs.github.com/actions/using-workflows/manually-running-a-workflow).
When it concludes, the CI stores all successive builds in in one convenient
[download](https://docs.github.com/actions/managing-workflow-runs/downloading-workflow-artifacts).
These downloads are distributable (suitable for [publishing](../../releases)) –
libraries for all platform-architectures (that built successfully) included,
and development-only files (e.g., [C source files](src/)) are not included.

## Building locally

As you may’ve guessed, you need [CRuby Ruby](https://www.ruby-lang.org) and a basic set of C compiler toolkit.

Then, run the following in the project root.

```shell
bundle config set --local without 'development'
  # ❗ ONLY if you have RubyInstaller but no MSYS,
  # or don’t want to check/get additional development utilities in general.
 
bundle install
bundle exec rake
```

This builds Godot.rb for your device’s platform-architecture – and only that platform-architecture pair. Moreover,
there are also project files that are only for development but extraneous for publishing, such as [the C sources](src/).
So while you can beta test the work in Godot locally, you’d need more polishing before it’s good for publishing.


## License

### [`include/godot/`](include/godot/)

Copyright (c) 2014-present Godot Engine contributors. https://godotengine.org/license/

### Bundled 3rd-party libraries in [`bin/`](bin/) (in builds)

#### “libruby” – [CRuby](https://www.ruby-lang.org)
[custom terms](https://github.com/ruby/ruby/blob/v3_2_2/COPYING#L25-L30).

### The rest of this project

Copyright 2023 ParadoxV5

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

> http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
