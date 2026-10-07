# NameGenerator

This is a small, dependency-free C++20 name generator and web demo I built for fun while using GitHub
Copilot to test its capabilities and learn about AI agent development. It generates deterministic
names from a seed, a name kind, and a broad sound profile. Please try it out and share feedback by
[creating an issue in the GitHub repository](https://github.com/cyberpunk42/NameGenerator/issues).

`NameKind` identifies the caller-defined use of a name, and `NameProfile` selects its broad sound
profile. The Quenya- and Sindarin-inspired profiles use invented syllable combinations informed by
phonological descriptions; they are not translations. English, French, and German profiles use broad
sound patterns informed by the references below. Orcish, Gnomish, Infernal, and Abyssal are original,
genre-inspired sound profiles rather than representations of a particular published language. The
Cthulhu Mythos profile recombines fragments adapted from Mythos nomenclature and can produce
recognizable canonical names. The Fae-inspired profile uses original syllable combinations informed
by historical Celtic fair-folk traditions; it does not represent a single language or copy names from
modern fiction.

## Attributions and bibliography

### Phonological references

- Tolkien, J. R. R. *The Lord of the Rings*. Appendices E and F. George Allen & Unwin, 1954-1955.
	The Quenya- and Sindarin-inspired profiles draw on the broad sound descriptions; they do not
	implement either language.
- Roach, Peter. *English Phonetics and Phonology: A Practical Course*. 4th ed. Cambridge University
	Press, 2009.
- Tranel, Bernard. *The Sounds of French: An Introduction*. Cambridge University Press, 1987.
- Wiese, Richard. *The Phonology of German*. Clarendon Press, 1996.
- Keightley, Thomas. *The Fairy Mythology: Illustrative of the Romance and Superstition of Various
	Countries*. Revised and enlarged edition, 1870. [Project Gutenberg eBook 41006](https://www.gutenberg.org/ebooks/41006).
- Evans-Wentz, W. Y. *The Fairy-Faith in Celtic Countries*. 1911. [Project Gutenberg eBook 34853](https://www.gutenberg.org/ebooks/34853).

These works inform general phonological tendencies only. The generator's syllable inventories are
original combinations, not copied examples or a substitute for linguistic analysis. The folklore
references inform broad cultural context, not a universal system of Fae names.

### Cthulhu Mythos references

- Lovecraft, H. P. "The Call of Cthulhu." *Weird Tales*, February 1928. [Text](https://www.hplovecraft.com/writings/texts/fiction/cc.aspx).
- Lovecraft, H. P. "The Dunwich Horror." *Weird Tales*, April 1929. [Text](https://www.hplovecraft.com/writings/texts/fiction/dh.aspx).
- Lovecraft, H. P. "The Whisperer in Darkness." *Weird Tales*, August 1931. [Text](https://www.hplovecraft.com/writings/texts/fiction/wid.aspx).

These stories inform the Mythos profile's fragment choices. That profile is an homage for
procedural naming, not an attempt to extend or define Mythos canon.

## Build and test

Requires CMake 3.24+, Ninja, and a C++20 compiler.

```powershell
cmake --preset release
cmake --build --preset release
ctest --preset release
```

## WebAssembly demo and GitHub Pages

The Vue demo runs the C++ generator compiled to WebAssembly, so it does not need a local server-side
API. Install and activate Emscripten 6.0.11, then from the repository root:

```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force
. "$HOME\emsdk\emsdk_env.ps1"
npm.cmd --prefix web install
npm.cmd --prefix web run dev
```

The `predev` script builds the wasm module before Vite starts. To create a production bundle, run
`npm.cmd --prefix web run build`. The `wasm` CMake preset can also be configured and built directly.

GitHub Actions deploys the static app to Pages when changes are pushed to `main`, or when the Pages
workflow is run manually. The deployment serves the Vue app and its wasm module under the repository
path.

## Use from another CMake project

Add this project with `add_subdirectory` and link the exported build-tree target:

```cmake
add_subdirectory(path/to/NameGenerator)
target_link_libraries(my_app PRIVATE NameGenerator::NameGenerator)
```

For an installed library, use `find_package(NameGenerator CONFIG REQUIRED)` and link the same
target. Install it with `cmake --install build/release --prefix <install-prefix>`.

## License

This project is licensed under the Apache License 2.0. See [LICENSE](LICENSE) for the full text.
