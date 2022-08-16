#!/bin/bash -xe

function exe() {
    bazel build //:exe
    rm -rf exe
    cp bazel-bin/exe .
}

function test() {
    bazel test --test_output=all //:test
}

function srv() {
    bazel build //:udp_srv
    rm -rf udp_srv
    cp bazel-bin/udp_srv .
}
function cli() {
    bazel build //:udp_cli
    rm -rf udp_cli
    cp bazel-bin/udp_cli .
}

$@