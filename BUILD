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
        "src/trudppp/callbacks.cpp",
        "src/trudppp/channel.cpp",
        "src/trudppp/connection.cpp",
        "src/trudppp/packet.cpp",
        "src/trudppp/serialized_packet.cpp",
        "src/trudppp/serialized_packet.hpp",
        "src/trudppp/service.cpp",
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
        "include/trudppp/callbacks.hpp",
        "include/trudppp/channel.hpp",
        "include/trudppp/connection.hpp",
        "include/trudppp/constants.hpp",
        "include/trudppp/packet.hpp",
        "include/trudppp/service.hpp",
    ],
    includes = ["include", "src"],
)

cc_test(
    name = "header_inclusion_test",
    copts = select({
        ":windows": ["/std:c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
    deps = [":trudppp", "@teobase//:teobase", "@teoccl//:teoccl"],
    linkopts = select({
        ":windows": [],
        "//conditions:default": ["-pthread"],
    }),
    size = "small",
    srcs = [
        "tests/header_inclusion/header_inclusion_test.cpp",
        "tests/header_inclusion/include_callbacks_hpp_test.cpp",
        "tests/header_inclusion/include_channel_hpp_test.cpp",
        "tests/header_inclusion/include_connection_hpp_test.cpp",
        "tests/header_inclusion/include_constants_hpp_test.cpp",
        "tests/header_inclusion/include_packet_hpp_test.cpp",
        "tests/header_inclusion/include_service_hpp_test.cpp",
    ],
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
    size = "small",
    srcs = [
        "tests/unit_tests/packet_serialization_test.cpp",
        "tests/unit_tests/connection_events_test.cpp",
        "tests/unit_tests/service_test.cpp",
        "src/trudppp/serialized_packet.hpp",
    ],
    includes = ["src"],
)