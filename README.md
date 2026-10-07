# NameGenerator

A dependency-free C++20 library for deterministic procedural names. `NameKind` identifies the
caller-defined use of a name, and `NameProfile` selects its broad sound profile. The Quenya- and
Sindarin-inspired profiles use original syllable fragments informed by phonological descriptions;
they do not generate either constructed language or copy its vocabulary. Orcish, Gnomish, Infernal,
Abyssal, and Cthulhu Mythos-inspired profiles similarly use original sound fragments, not canonical
character or creature names.

## Build and test

Requires CMake 3.24+, Ninja, and a C++20 compiler.

```powershell
cmake --preset release
cmake --build --preset release
ctest --preset release
```

## Windows demo UI

The Vue demo calls the same C++ generator through a local command-line bridge. From the repository
root, open a Visual Studio Developer PowerShell (or another shell with the MSVC environment loaded),
then build the bridge and start the Vite app:

```powershell
cmake --preset release
cmake --build --preset release --target name_generator_cli
npm.cmd --prefix web install
npm.cmd --prefix web run dev
```

Open the local URL printed by Vite. Change the seed, name kind, sound profile, or batch size to
inspect repeatable outputs. Node.js and an MSVC C++20 toolchain are required.

## Use from another CMake project

Add this project with `add_subdirectory` and link the exported build-tree target:

```cmake
add_subdirectory(path/to/NameGenerator)
target_link_libraries(my_app PRIVATE NameGenerator::NameGenerator)
```

For an installed library, use `find_package(NameGenerator CONFIG REQUIRED)` and link the same
target. Install it with `cmake --install build/release --prefix <install-prefix>`.
