workspace(name = "trudppp")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

git_repository(
    name = "googletest",
    remote = "https://github.com/google/googletest",
    tag = "release-1.8.1",
)

TEOCCL_BUILD = """
cc_library(
    name = "teoccl",
    srcs = [
        "src/teoccl/array_list.c",
        "src/teoccl/hash.c",
        "src/teoccl/map.c",
        "src/teoccl/memory.c",
        "src/teoccl/queue.c",
    ],
    hdrs = [
        "include/teoccl/array_list.h",
        "include/teoccl/hash.h",
        "include/teoccl/map.h",
        "include/teoccl/memory.h",
        "include/teoccl/queue.h",
    ],
    includes = ["include"],
    visibility = ["//visibility:public"],
)
"""

new_git_repository(
    name = "teoccl",
    build_file_content = TEOCCL_BUILD,
    remote = "https://github.com/Si1ver/teoccl.git",
    branch = "develop",
)

TEOBASE_BUILD = """
cc_library(
    name = "teobase",
    srcs = [
        "src/teobase/logging.c",
        "src/teobase/mutex.c",
        "src/teobase/socket.c",
        "src/teobase/time.c",
    ],
    hdrs = [
        "include/teobase/api.h",
        "include/teobase/logging.h",
        "include/teobase/mutex.h",
        "include/teobase/platform.h",
        "include/teobase/socket.h",
        "include/teobase/time.h",
        "include/teobase/windows.h",
    ],
    includes = ["include"],
    visibility = ["//visibility:public"],
)
"""

new_git_repository(
    name = "teobase",
    build_file_content = TEOBASE_BUILD,
    remote = "https://github.com/Si1ver/teobase.git",
    branch = "master",
)
