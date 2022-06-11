# Ignore Gen

Offline .gitignore generator based on [gitignore.io](https://www.toptal.com/developers/gitignore)

## Usage

### Standard form

```sh
ignore-gen [OPTIONS] <gitignore-keys>
```

### List available keys

```sh
ignore-gen --list-keys
```

### Create .gitignore file

```sh
ignore-gen --output=".gitignore" c++ meson linux macos
```

## Build

### Requirements

- Meson build system
- C++ compiler with C++17 support
- fmt >= 8.1
- Sqlite3 >= 3.36

### Development build

```sh
meson setup builddir
meson compile -C builddir
```

### Production build

```sh
meson setup -Dbuildtype=release -Dpackage_build=true builddir
meson compile -C builddir
cd builddir && sudo ninja install
```
