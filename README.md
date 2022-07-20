# clang-issue

This repository provides minimal code to demomstrate possible issue with _Clang 13.0.1 with MSVC-like command-line_

The code is taken from https://github.com/BinomialLLC/basis_universal/blob/9009cae5407ef54f5030c1d0fabb1c2919c418bb/encoder/basisu_kernels_sse.cpp (modified)

Clang generates code that leads to segfault at runtine (most of the time).

RelWithDebInfo configuration is affected only.

Possible workaround is to make the function `extract_x86_extended_flags` _inline_
