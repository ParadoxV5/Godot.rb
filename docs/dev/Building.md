# Building

## GitHub Actions CI (Coming Soon)

[The GitHub Actions workflow](../.github/workflows/workflow.yml)
automatically builds and tests relevant commits on all supported platforms and architectures, or you can also
[manually activate the CI](https://docs.github.com/actions/using-workflows/manually-running-a-workflow).

When it concludes, the CI stores all successive builds in one convenient
[download](https://docs.github.com/actions/managing-workflow-runs/downloading-workflow-artifacts).

These downloads are distributable (suitable for [publishing](https://github.com/ParadoxV5/Godot.rb/releases)) –
libraries for all platform-architectures (that built successfully) included,
and development-only files (e.g., [C source files](../src/)) are not included.

## Building locally

> Note: These instructions were written to be future-proof.
> In practice, only Windows, RubyInstaller no-Devkit + MinGW is currently tested.

As you may’ve guessed, you need [CRuby Ruby](https://www.ruby-lang.org) and a basic `gcc` set.
Specifically, you need `gcc` to compile `godot_rb.c` to a shared library that links with the Ruby shared library
(i.e., `libruby.so`), which came from building Ruby with `--enable-shared` `./configure`d.
RubyInstaller for Windows includes the Ruby shared library (it’s a `.dll` because Macrohard);
check your contents for other prebuild packages.

The `Rakefile` has all the building steps. Ruby [bundles](https://github.com/ruby/ruby/blob/HEAD/gems/bundled_gems)
[the `rake` gem](https://github.com/ruby/rake) unless you’ve uninstalled it for whatëver defiance.

* For most platforms (read: Linux), your building workflow is the same as that of any Rake project.
  ```shell
  bundle install
  bundle exec rake
  ```

* For Windows, [RubyInstaller2](https://rubyinstaller.org) + Devkit comes with [MSYS2](https://www.msys2.org)
  integration, allowing *you* to use the same Linux steps above.

* RubyInstaller without MSYS2 (neither Devkit nor separately installed) plus a separate `gcc`
  (such as [MinGW](https://www.mingw-w64.org)) also do; but without MSYS2,
  the Linux/MSYS2 commands above will certainly fail, and you also miss out utilities for auxiliary development tasks.
  To substitute those Linux/MSYS2 commands:
  ```shell
  rake --version
  # Confirm the Rake version is suitable as recommended by the `Gemfile`
  rake
  ```

This builds Godot.rb for your device’s platform-architecture – and only that platform-architecture pair.
It also (if necessary) symlinks `libruby` and additional dependencies instead of duplicating them.
(As such, you need admin elevation on Windows if you don’t have
[symlink permissions](https://learn.microsoft.com/windows/security/threat-protection/security-policy-settings/create-symbolic-links))
