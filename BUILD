config_setting(
    name = "windows",
    constraint_values = ["@bazel_tools//platforms:windows"],
)

cc_library(
    name = "trudppp",
    copts = select({
        ":windows": ["/std:c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
    deps = ["@teobase//:teobase", "@teoccl//:teoccl"],
    srcs = [
        "src/trudppp/packet.cpp",
        "src/trudppp/serialized_packet.cpp",
        "src/trudppp/serialized_packet.hpp",
        "src/packet.h",
        "src/packet_legacy.cpp",
        "src/packet_queue.cpp",
        "src/packet_queue.h",
        "src/trudp.cpp",
        "src/trudp.h",
        "src/trudp_api.h",
        "src/trudp_channel.cpp",
        "src/trudp_channel.h",
        "src/trudp_const.h",
        "src/trudp_ev.c",
        "src/trudp_ev.h",
        "src/trudp_options.cpp",
        "src/trudp_options.h",
        "src/trudp_receive_queue.cpp",
        "src/trudp_receive_queue.h",
        "src/trudp_send_queue.cpp",
        "src/trudp_send_queue.h",
        "src/trudp_stat.cpp",
        "src/trudp_stat.h",
        "src/trudp_utils.cpp",
        "src/trudp_utils.h",
        "src/udp.cpp",
        "src/udp.h",
        "src/utils.h",
        "src/utils_r.h",
        "src/write_queue.cpp",
        "src/write_queue.h",
    ],
    hdrs = [
        "include/trudppp/constants.hpp",
        "include/trudppp/packet.hpp",
    ],
    includes = ["include", "src"],
)

cc_test(
    name = "include_constants_test",
    copts = select({
        ":windows": ["/std:c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
    deps = [":trudppp", "@teobase//:teobase", "@teoccl//:teoccl"],
    linkopts = select({
        ":windows": [],
        "//conditions:default": ["-pthread"],
    }),
    srcs = ["tests/header_inclusion/include_constants_hpp.cpp"],
)

cc_test(
    name = "include_packet_test",
    copts = select({
        ":windows": ["/std:c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
    deps = [":trudppp", "@teobase//:teobase", "@teoccl//:teoccl"],
    linkopts = select({
        ":windows": [],
        "//conditions:default": ["-pthread"],
    }),
    srcs = ["tests/header_inclusion/include_packet_hpp.cpp"],
)

cc_test(
    name = "packet_serialization_test",
    copts = select({
        ":windows": ["/std:c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
    deps = [":trudppp", "@teobase//:teobase", "@teoccl//:teoccl", "@googletest//:gtest_main"],
    linkopts = select({
        ":windows": [],
        "//conditions:default": ["-pthread"],
    }),
    srcs = [
        "tests/packet/packet_serialization.cpp",
        "src/trudppp/serialized_packet.hpp",
    ],
    includes = ["src"],
)