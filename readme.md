# LLVM
[LLVM](https://llvm.org/) toolchain for [Alpine Linux](https://alpinelinux.org/).

Install dependencies.

```sh
sudo apk add binutils fortify-headers linux-headers libc-dev
sudo apk add curl git make nasm ninja nodejs npm patch perl pkgconf python sqlite swig z3
sudo apk add build-base libxml2-dev z3-dev libedit-dev ncurses-dev xz-dev
```

Install [CMake](https://cmake.org/).

```sh
sudo apk add cmake curl-dev openssl-dev
wget https://github.com/Kitware/CMake/releases/download/v3.16.4/cmake-3.16.4.tar.gz
tar xf cmake-3.16.4.tar.gz
cmake -GNinja -Wno-dev \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/cmake \
  -DCMAKE_C_COMPILER=gcc \
  -DCMAKE_CXX_COMPILER=g++ \
  -DCMAKE_USE_SYSTEM_CURL=ON \
  -B cmake-build cmake-3.16.4
ninja -C cmake-build
sudo ninja -C cmake-build install
sudo apk del cmake curl-dev openssl-dev
```

Add `/opt/cmake/bin` to the `PATH` environment variable.

```sh
export PATH="/opt/cmake/bin:${PATH}"
cmake --version
```

Create and take ownership of the `/opt/llvm` directory.

```sh
sudo mkdir -p /opt/llvm
sudo chown `id -un`:`id -gn` /opt/llvm
```

Clone this repository.

```sh
git clone git@github.com:qis/llvm /opt/llvm
```

Build toolchain.

```sh
make -C /opt/llvm
```

Remove dependencies (optional).

```sh
sudo apk del build-base libxml2-dev z3-dev libedit-dev ncurses-dev xz-dev
```

Add `/opt/llvm/bin` to the `PATH` environment variable.

```sh
export PATH="/opt/llvm/bin:${PATH}"
clang --version
```
