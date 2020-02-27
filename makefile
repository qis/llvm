MAKEFLAGS += --no-print-directory

all: llvm

# =================================================================================================
# llvm
# =================================================================================================

src/llvm:
	@git clone --depth 1 https://github.com/llvm/llvm-project src/llvm
	@cmake -P src/llvm.cmake

bin/clang: src/llvm
	@cmake -GNinja -Wno-dev \
	  -DCMAKE_BUILD_TYPE=Release \
	  -DCMAKE_INSTALL_PREFIX="$(CURDIR)" \
	  -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
	  -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld;lldb;polly;openmp;compiler-rt;libunwind;libcxxabi;libcxx" \
	  -DLLVM_TARGETS_TO_BUILD="X86" \
	  -DLLVM_ENABLE_BACKTRACES=OFF \
	  -DLLVM_ENABLE_UNWIND_TABLES=OFF \
	  -DLLVM_ENABLE_WARNINGS=OFF \
	  -DLLVM_INCLUDE_BENCHMARKS=OFF \
	  -DLLVM_INCLUDE_EXAMPLES=OFF \
	  -DLLVM_INCLUDE_TESTS=OFF \
	  -DLLVM_INCLUDE_DOCS=OFF \
	  -DCLANG_ENABLE_ARCMT=OFF \
	  -DCLANG_ENABLE_STATIC_ANALYZER=OFF \
	  -DCLANG_DEFAULT_STD_C="c11" \
	  -DCLANG_DEFAULT_STD_CXX="cxx20" \
	  -DCLANG_DEFAULT_CXX_STDLIB="libc++" \
	  -DCLANG_DEFAULT_UNWINDLIB="none" \
	  -DCLANG_DEFAULT_RTLIB="compiler-rt" \
	  -DCLANG_DEFAULT_LINKER="lld" \
	  -DCLANG_PLUGIN_SUPPORT=OFF \
	  -DOPENMP_ENABLE_LIBOMPTARGET=OFF \
	  -DCOMPILER_RT_BUILD_SANITIZERS=OFF \
	  -DCOMPILER_RT_BUILD_LIBFUZZER=OFF \
	  -DCOMPILER_RT_BUILD_PROFILE=OFF \
	  -DCOMPILER_RT_BUILD_XRAY=OFF \
	  -DCOMPILER_RT_INCLUDE_TESTS=OFF \
	  -DLIBUNWIND_ENABLE_SHARED=OFF \
	  -DLIBUNWIND_ENABLE_STATIC=ON \
	  -DLIBUNWIND_USE_COMPILER_RT=ON \
	  -DLIBCXXABI_ENABLE_SHARED=OFF \
	  -DLIBCXXABI_ENABLE_STATIC=ON \
	  -DLIBCXXABI_ENABLE_STATIC_UNWINDER=ON \
	  -DLIBCXXABI_USE_COMPILER_RT=ON \
	  -DLIBCXXABI_USE_LLVM_UNWINDER=ON \
	  -DLIBCXX_ENABLE_SHARED=OFF \
	  -DLIBCXX_ENABLE_STATIC=ON \
	  -DLIBCXX_HAS_MUSL_LIBC=ON \
	  -DLIBCXX_USE_COMPILER_RT=ON \
	  -DLIBCXX_INCLUDE_BENCHMARKS=OFF \
	  -DLIBCXX_ENABLE_PARALLEL_ALGORITHMS=ON \
	  -B build/llvm src/llvm/llvm
	@ninja -C build/llvm \
	  install-LTO \
	  install-lld-stripped \
	  install-lldb-stripped \
	  install-clang-stripped \
	  install-clang-format-stripped \
	  install-clang-resource-headers \
	  install-llvm-ar-stripped \
	  install-llvm-nm-stripped \
	  install-llvm-objdump-stripped \
	  install-llvm-ranlib-stripped \
	  install-llvm-strip-stripped \
	  install-compiler-rt-stripped \
	  install-compiler-rt-headers-stripped \
	  install-unwind-stripped \
	  install-cxxabi-stripped \
	  install-cxx-stripped
	@ln -fs clang bin/cc
	@ln -fs clang++ bin/c++
	@ln -fs clang-cpp bin/cpp

llvm: bin/clang include/pstl

# =================================================================================================
# pstl
# =================================================================================================

include/pstl: include/tbb
	@cmake -GNinja -Wno-dev \
	  -DCMAKE_BUILD_TYPE=Release \
	  -DCMAKE_INSTALL_PREFIX="$(CURDIR)" \
	  -DCMAKE_C_COMPILER="$(CURDIR)/bin/clang" \
	  -DCMAKE_CXX_COMPILER="$(CURDIR)/bin/clang++" \
	  -DPSTL_PARALLEL_BACKEND="tbb" \
	  -B build/pstl src/llvm/pstl
	@ninja -C build/pstl install

# =================================================================================================
# tbb
# =================================================================================================

src/tbb.tar.gz:
	@curl -L https://github.com/intel/tbb/archive/v2020.1.tar.gz -o src/tbb.tar.gz

src/tbb: src/tbb.tar.gz
	@mkdir -p src/tbb
	@tar xf src/tbb.tar.gz -C src/tbb --strip-components 1
	@cd src/tbb && patch -p1 < $(CURDIR)/tbb/fix-comparison-operator.patch
	@cd src/tbb && patch -p1 < $(CURDIR)/tbb/fix-static-build.patch
	@cd src/tbb && patch -p1 < $(CURDIR)/tbb/fix-warnings.patch
	@cp tbb/CMakeLists.txt tbb/TBBConfig.cmake.in src/tbb/

include/tbb: src/tbb
	@cmake -GNinja -Wno-dev \
	  -DCMAKE_BUILD_TYPE=Release \
	  -DCMAKE_INSTALL_PREFIX="$(CURDIR)" \
	  -DCMAKE_CXX_STANDARD="20" \
	  -DCMAKE_CXX_EXTENSIONS=ON \
	  -DCMAKE_CXX_COMPILER="$(CURDIR)/bin/clang++" \
	  -DCMAKE_CXX_FLAGS_INIT="-fasm -fPIC -D_DEFAULT_SOURCE=1 -Wno-deprecated-volatile" \
	  -DWITH_PSTL=ON \
	  -B build/tbb src/tbb
	@ninja -C build/tbb install

.PHONY: all llvm
