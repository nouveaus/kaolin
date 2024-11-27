## Introduction

[CMake](https://cmake.org/) does not build the project. It is used to generate the files used by
your desired build tool (Ninja, Unix Makefiles, Xcode, etc.).

## TL;DR (For the Impatient)

1. Get the source:
   ```
   $ git clone https://github.com/nouveaus/kaolin
   ```

2. Run CMake:
   ```
   $ cmake -S kaolin -B build -G Ninja
   ```

3. Build the project:
   ```
   $ cmake --build build
   ```

## Build system requirements

The host system must have the following dependencies:

- CMake, at least version 3.28
  - A CMake generator, for example Ninja. To see a list of the available generators on your system, execute
  ```
  $ cmake --help
  ```

## Toolchain requirements

## Configuring

Run CMake to configure your build, e.g. `cmake -B build -G Ninja`.

Extra configuration options may be passed here with `-DVARIABLE=value`.

## Building

```
$ cmake --build build
```

## Testing
