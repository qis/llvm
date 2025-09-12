# Unwrap
Adds the `__unwrap__` keyword to `clang(1)`.

## Debug
Instructions for debug builds.

```sh
cmake --preset debug
ninja -C build/debug lld clang clangd runtimes
ninja -C build/debug unwrap-test-run
```

## Release
Instructions for release builds.

```sh
cmake --preset release
ninja -C build/release lld clang clangd runtimes
ninja -C build/release unwrap-test-run
```

<!--
## Initialize
The following commands were used to create this repository.

```sh
mkdir llvm && cd llvm
git init -b master && git config core.filemode false
git remote add upstream https://github.com/llvm/llvm-project
git remote add origin git@github.com:qis/llvm

git fetch upstream llvmorg-20.1.8:refs/tags/llvmorg-20.1.8
git reset --hard llvmorg-20.1.8 && git gc --prune=now
git push -uf origin master

git switch --orphan unwrap
cp ../unwrap.md readme.md
git add readme.md
git commit -m "Add readme"
git push -uf origin unwrap

git switch -c 20.1.8
git apply ../unwrap.diff
git add . && git commit -m "Apply unwrap patches"
git push -uf origin 20.1.8
```
-->

## Upgrade
Instructions for upgrading to the latest LLVM release.

```sh
git clone git@github.com:qis/llvm llvm && cd llvm
git remote add upstream https://github.com/llvm/llvm-project

git branch -a
git switch 20.1.8
git log
git diff 87f0227cb601 > ../unwrap-20.1.8.diff
git switch master
git fetch upstream llvmorg-21.1.0:refs/tags/llvmorg-21.1.0
git reset --hard llvmorg-21.1.0 && git gc --prune=now
git push -uf origin master

git switch -c 21.1.0
git apply ../unwrap-20.1.8.diff

cmake --preset debug && \
cmake --preset release

ninja -C build/debug lld clang clangd runtimes && \
ninja -C build/release lld clang clangd runtimes

ninja -C build/debug unwrap-test-run
ninja -C build/release unwrap-test-run

git add . && git commit -m "Apply unwrap patches"
git diff 3623fe661ae3 > ../unwrap-21.1.0.diff
git push -u origin 21.1.0
```
