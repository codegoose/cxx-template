# Install **TDM-GCC**

> TDM-GCC is a compiler suite for Windows. It combines the most recent stable release of the GCC compiler, a few patches for Windows-friendliness, and the free and open-source MinGW.org or MinGW-w64 runtime APIs, to create a more lightweight open-source alternative to Microsoft’s compiler and platform SDK. It’s not quite like other compilers in a few ways. Most importantly, it changes the default GCC runtime libraries to be statically linked and use a shared memory region for handling exceptions.

* https://jmeubank.github.io/tdm-gcc/

#### **Add binaries to the `PATH` variable**

```
C:/TDM-GCC-64/x86_64-w64-mingw32/bin
C:/TDM-GCC-64/bin
```

#### **Check version**

```
$ g++ --version
```

# Install **Scoop**

> Scoop installs programs from the command line with a minimal amount of friction.

* https://scoop.sh/

#### **Allow remote scripts**

```
Set-ExecutionPolicy RemoteSigned -scope CurrentUser
```

#### **Install**

```
iwr -useb get.scoop.sh | iex
```

# Install **CMake**, **Ninja** and **Conan**

> **CMake** is an open-source, cross-platform family of tools designed to build, test and package software. CMake is used to control the software compilation process using simple platform and compiler independent configuration files, and generate native makefiles and workspaces that can be used in the compiler environment of your choice.

* https://cmake.org/

> **Ninja** is a small build system with a focus on speed. It differs from other build systems in two major respects: it is designed to have its input files generated by a higher-level build system, and it is designed to run builds as fast as possible.

* https://ninja-build.org/

> **Conan** is a MIT-licensed, Open Source package manager for C and C++ development, allowing development teams to easily and efficiently manage their packages and dependencies across platforms and build systems.

* https://conan.io/

#### **Install**

```
$ scoop install cmake ninja conan
```

#### **Check versions**

```
$ cmake --version
$ ninja --version
$ conan --version
```

#### **Prepare Conan profile**

> If you are using GCC compiler >= 5.1, Conan will set the `compiler.libcxx` to the old ABI for backwards compatibility. Recent gcc versions will compile the example by default with the new ABI and linking will fail without further customization of your cmake configuration. You can avoid this with the following commands:

* https://docs.conan.io/en/latest/getting_started.html

```https://jmeubank.github.io/tdm-gcc/
$ conan profile new default --detect
$ conan profile update settings.compiler.libcxx=libstdc++11 default
```

Explicitly specify `CC` and `CXX` to resolve some potential build issues:

```
$ conan profile update env.CONAN_CMAKE_FIND_ROOT_PATH=x86_64-w64-mingw32 default
$ conan profile update env.CC=x86_64-w64-mingw32-gcc default
$ conan profile update env.CXX=x86_64-w64-mingw32-g++ default
```

* https://github.com/conan-io/conan/issues/6032 *Compiling OpenSSL with MinGW on Windows*
* https://github.com/openssl/openssl/issues/8268 *Windows MinGW(x86/x86_64) building DSO not possible anymore*

# Prepare build folder

From the project root directory:

```
$ mkdir build
$ cd build
$ conan install .. --build=missing
$ cmake -G Ninja ..
```

Your `.conan` folder will likely become huge at some point as it'll hang on to build tools, sources and such in case they're needed later. You can reduce the size of this folder by running this cleanup command:

```
conan remove * -b -s -f
```

# Build/rebuild

From within the build folder:

```
$ ninja
```

# Install **VSCode**

> **Visual Studio Code** combines the simplicity of a code editor with what developers need for their core edit-build-debug cycle. It provides comprehensive code editing, navigation, and understanding support along with lightweight debugging, a rich extensibility model, and lightweight integration with existing tools.

* https://code.visualstudio.com/

```
$ scoop install vscode
```

Install these extensions:

* C/C++ by Microsoft (`ms-vscode.cpptools`)