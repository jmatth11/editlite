const std = @import("std");

const alloc = std.heap.smp_allocator;

fn build_plugins(
    b: *std.Build,
    optimize: std.builtin.OptimizeMode,
    target: std.Build.ResolvedTarget,
) !void {
    var arena: std.heap.ArenaAllocator = .init(alloc);
    defer arena.deinit();
    const plugin_directory = try std.fs.cwd().openDir("plugins", .{ .iterate = true });
    var plugin_iterator = plugin_directory.iterate();
    while (try plugin_iterator.next()) |entry| {
        if (entry.kind == .directory) {
            const dir = try std.fmt.allocPrint(
                arena.allocator(),
                "plugins/{s}",
                .{entry.name},
            );
            const lib = b.addLibrary(.{
                .name = entry.name,
                .root_module = try createModule(b, optimize, target, dir),
                .linkage = .dynamic,
            });
            lib.linkLibC();
            b.installArtifact(lib);
        }
    }
}

fn createModule(
    b: *std.Build,
    optimize: std.builtin.OptimizeMode,
    target: std.Build.ResolvedTarget,
    dir: []const u8,
) !*std.Build.Module {
    var arena: std.heap.ArenaAllocator = .init(alloc);
    defer arena.deinit();
    const src_dir = try std.fs.cwd().openDir(dir, .{
        .iterate = true,
    });
    var walker = try src_dir.walk(arena.allocator());
    defer walker.deinit();
    var files_array: std.array_list.Managed([]const u8) = .init(arena.allocator());
    defer files_array.deinit();
    while (try walker.next()) |entry| {
        if (entry.kind == .file) {
            if (std.mem.endsWith(u8, entry.basename, ".c")) {
                const name = try std.fs.path.join(arena.allocator(), &.{
                    dir,
                    entry.path,
                });
                try files_array.append(name);
            }
        }
    }
    const files: []const []const u8 = try files_array.toOwnedSlice();
    const flags: []const []const u8 = &.{
        "-Wall",
        "-Wextra",
        "-O2",
        "-std=gnu11",
        "-march=native",
    };
    const module = b.createModule(.{
        .pic = true,
        .target = target,
        .optimize = optimize,
    });
    module.link_libc = true;
    if (!std.mem.eql(u8, "src", dir)) {
        module.addIncludePath(b.path(dir));
    }
    module.addIncludePath(b.path("./src"));
    module.addIncludePath(b.path("./deps/utf8-zig/headers/"));
    module.addIncludePath(b.path("./deps/scribe/header/"));
    module.addIncludePath(b.path("./deps/tomlc99/"));
    module.addIncludePath(b.path("./deps/array_template/"));
    module.linkSystemLibrary("sdl2", .{ .needed = true });
    module.linkSystemLibrary("SDL2_ttf", .{ .needed = true });
    module.addObjectFile(b.path("./deps/tomlc99/libtoml.a"));
    module.addObjectFile(b.path("./deps/scribe/zig-out/lib/libscribe.a"));
    module.addObjectFile(b.path("./deps/utf8-zig/zig-out/lib/libutf8-zig.a"));
    module.addCSourceFiles(.{
        .language = .c,
        .files = files,
        .flags = flags,
    });
    return module;
}

pub fn build(b: *std.Build) !void {
    const optimize = b.standardOptimizeOption(.{});
    const nativeTarget = b.standardTargetOptions(.{});
    const nativeLib = b.addExecutable(.{
        .name = "editlite",
        .root_module = try createModule(b, optimize, nativeTarget, "src"),
    });
    nativeLib.linkLibC();
    b.installArtifact(nativeLib);
    try build_plugins(b, optimize, nativeTarget);
}
