
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

#lib utils begin
cc_library(
    name = "utils",
    srcs = glob([
            "src/utils/*.cc"
    ]),
    hdrs = glob([
            "src/utils/*.h",
    ]),
    visibility = ["//visibility:public"],
)
#lib end

#lib jw begin
cc_library(
    name = "jw",
    srcs = glob([
            "src/jw/*.cc"
    ]),
    hdrs = glob([
            "src/jw/*.h",
    ]),
    visibility = ["//visibility:public"],
)
#lib end

#lib tmp begin
cc_library(
    name = "tmp",
    srcs = glob([
            "src/tmp/*.cc"
    ]),
    hdrs = glob([
            "src/tmp/*.h",
    ]),
    visibility = ["//visibility:public"],
)
#lib end

cc_test(
  name = "test",
  size = "small",
  srcs = ["src/test/test.cc"],
  deps = [
    ":tmp",
    "@com_google_googletest//:gtest_main",
    
    ],
)

cc_binary(
  name = "exe",
  srcs = glob(["src/main.cc"]),
  linkopts = [ "-lpthread" ] ,
  deps = [
    ":cc_proto_lib",
    #":libName",
    #"@com_github_libevent_libevent//:event",
    "@com_github_tencent_rapidjson//:rapidjson",
    ":utils",
    ":jw",
    ":tmp",
    ],
)

cc_binary(
  name = "udp_srv",
  srcs = glob(["src/udp/srv.cc"]),
  linkopts = [ "-lpthread" ] ,
  deps = [
    "@com_github_tencent_rapidjson//:rapidjson",
    ],
)

cc_binary(
  name = "udp_cli",
  srcs = glob(["src/udp/client.cc"]),
  linkopts = [ "-lpthread" ] ,
  deps = [
    "@com_github_tencent_rapidjson//:rapidjson",
    ],
)