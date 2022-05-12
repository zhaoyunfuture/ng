
#proto begin
load(
    "@rules_proto//proto:defs.bzl",
    "proto_library",
)

cc_library(
    name = "cc_proto_lib",
    deps = [":cc_proto"],
)

cc_proto_library(
    name = "cc_proto",
    deps = [":src_proto"],
)

proto_library(
    name = "src_proto",
    srcs = glob([
        "proto/*.proto",
    ]),

    visibility = ["//visibility:public"],
)
#proto end

#lib begin
cc_library(
    name = "libName",
    srcs = glob([
            #"path_to_project/*.c"
    ]),
    hdrs = glob([
            #"path_to_project/*.h",
    ]),
    visibility = ["//visibility:public"],
)
#lib end

cc_library(
    name = "libevent",
    deps = ["@com_github_libevent_libevent//:event"],
    visibility = ["//visibility:public"],
)

cc_test(
  name = "test",
  size = "small",
  srcs = ["src/test.cc"],
  deps = ["@com_google_googletest//:gtest_main"],
)

cc_binary(
  name = "exe",
  srcs = glob(["src/main.cc"]),
  deps = [
    #"@com_google_googletest//:gtest_main",
    ":cc_proto_lib",
    #":libName",
    ":libevent",
    ],
)
