
licenses(["notice"])  # Apache 2

cc_library(
    name = "llhttp",
    srcs = [
        "src/api.c",
        "src/http.c",
        "src/llhttp.c",
    ],
    defines = ["LLHTTP_STRICT_MODE"],
    hdrs = ["include/llhttp.h"],
    includes = ["include"],
    visibility = ["//visibility:public"],
)
