# Update
Instructions for updating 21.1.0 to 21.1.4.

```sh
# Check out this repository.
git clone git@github.com:qis/llvm llvm && cd llvm
git remote add upstream https://github.com/llvm/llvm-project

# Get changes.
git branch -a
git switch 21.1.0
git diff HEAD^ HEAD > ../unwrap-21.1.0.diff

# Update master.
git switch master
git fetch upstream llvmorg-21.1.4:refs/tags/llvmorg-21.1.4
git reset --hard llvmorg-21.1.4 && git gc --prune=now
git push -uf origin master

# Create branch.
git switch -c 21.1.4

# Apply and review changes.
git apply ../unwrap-21.1.0.diff
git diff HEAD

# Build and run tests.
cmake --preset debug && \
cmake --preset release

ninja -C build/debug lld clang clangd runtimes && \
ninja -C build/release lld clang clangd runtimes

ninja -C build/debug unwrap-test-run
ninja -C build/release unwrap-test-run

# Commit changes and push branch.
git add . && git commit -m "Apply unwrap patches"
git push -u origin 21.1.4

# Update readme.
git switch unwrap
vim -p readme.md update.md
git add readme.md update.md && git commit -m "Update readme"
git push
```

<!--
Alternative commands:
* `git diff HEAD^ HEAD` is equivalent to `git diff $(git log --skip=1 -n 1 --format=%h)`
* `git diff HEAD` is equivalent to `git diff $(git log -n 1 --format=%h)`
-->
