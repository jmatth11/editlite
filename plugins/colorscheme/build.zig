const std = @import("std");

// Although this function looks imperative, note that its job is to
// declaratively construct a build graph that will be executed by an external
// runner.
pub fn build(b: *std.Build) void {
    // Standard target options allows the person running `zig build` to choose
    // what target to build for. Here we do not override the defaults, which
    // means any target is allowed, and the default is native. Other options
    // for restricting supported target set are available.
    const target = b.standardTargetOptions(.{});

    // Standard optimization options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall. Here we do not
    // set a preferred release mode, allowing the user to decide how to optimize.
    const optimize = b.standardOptimizeOption(.{});

    const linkage = b.option(std.builtin.LinkMode, "linkage", "Link mode for utf8-zig library") orelse .dynamic;
    const libModules = b.addModule("colorscheme", .{
        .root_source_file = b.path("src/root.zig"),
        .pic = true,
        .target = target,
        .optimize = optimize,
    });
    const lib = b.addLibrary(.{
        .name = "colorscheme",
        .root_module = libModules,
        .linkage = linkage,
    });

    // utf8 library
    lib.addIncludePath(b.path("../../deps/utf8-zig/src/"));
    // scribe library
    lib.addIncludePath(b.path("../../deps/scribe/header/"));
    // add array template lib
    lib.addIncludePath(b.path("../../deps/array_template/"));

    // allow editlite files to be included
    lib.addIncludePath(b.path("../../src/"));

    lib.linkSystemLibrary("SDL2");

    // tree-sitter
    //lib.addIncludePath("./tree-sitter/lib/include/");
    //lib.addLibraryPath("./tree-sitter/");
    //lib.linkLibrary("tree-sitter");

    // include lib c
    lib.linkLibC();

    // This declares intent for the library to be installed into the standard
    // location when the user invokes the "install" step (the default step when
    // running `zig build`).
    b.installArtifact(lib);

    // Creates a step for unit testing. This only builds the test executable
    // but does not run it.
    const tests_mod = b.createModule(.{
        .root_source_file = b.path("src/test_root.zig"),
        .target = target,
        .optimize = optimize,
    });
    const lib_unit_tests = b.addTest(.{
        .name = "test",
        .root_module = tests_mod,
    });

    const run_lib_unit_tests = b.addRunArtifact(lib_unit_tests);

    // Similar to creating the run step earlier, this exposes a `test` step to
    // the `zig build --help` menu, providing a way for the user to request
    // running the unit tests.
    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&run_lib_unit_tests.step);
}
