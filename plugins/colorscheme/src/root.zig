const std = @import("std");

const editlite = @cImport({
    @cInclude("SDL2/SDL.h");
    @cInclude("types/plugin_interface_types.h");
    @cInclude("types/display_type.h");
    @cInclude("types/registered_functions.h");
    @cInclude("types/page_types.h");
});

const parser = @import("parser.zig");

const name = "Colorscheme";
const temp_file = "colorscheme.";
const exec_cmd = "tree-sitter highlight ";

var alloc: std.mem.Allocator = undefined;
var highlight_data: parser.parse_results = undefined;
var render_state_idx: usize = 0;

fn slice_from_str(str: [*:0]const u8) []u8 {
    return @constCast(std.mem.span(str));
}

fn calculate_highlights(d: *editlite.display) void {
    const cur_buffer = d.viewable_page_buffer.?(d, null);
    defer d.viewable_page_buffer_free.?(@constCast(&cur_buffer));
    // TODO maybe replace with editlite given local directory
    const app_dir = std.fs.getAppDataDir(alloc, "editlite") catch |err| {
        std.debug.print("{s}: error getting app data directory -- \"{}\"", .{ name, err });
        return;
    };
    defer alloc.free(app_dir);
    std.fs.makeDirAbsolute(app_dir) catch {};
    const paths: [2][]u8 = [_][]u8{ app_dir, slice_from_str(temp_file) };
    const new_path = std.fs.path.join(alloc, &paths) catch |err| {
        std.debug.print("{s}: error joining paths for temp file -- \"{}\"", .{ name, err });
        return;
    };
    defer alloc.free(new_path);
    const code_file = std.fs.createFileAbsolute(new_path, .{ .truncate = true }) catch |err| {
        std.debug.print("{s}: error creating temp file -- \"{}\"", .{ name, err });
        return;
    };
    _ = code_file.write(std.mem.span(cur_buffer.buffer.buffer)) catch |err| {
        std.debug.print("{s}: error writing temp file -- \"{}\"", .{ name, err });
        return;
    };
    defer std.fs.deleteFileAbsolute(new_path) catch {};
    const run_params = [_][]u8{ slice_from_str(exec_cmd), new_path };
    const results = std.process.Child.run(.{
        .allocator = alloc,
        .argv = &run_params,
    }) catch |err| {
        std.debug.print("{s}: error executing child process tree-sitter -- \"{}\"", .{ name, err });
        return;
    };
    defer alloc.free(results.stdout);
    defer alloc.free(results.stderr);
    if (highlight_data.parsed) {
        highlight_data.release_results();
    }
    highlight_data.parse(results.stdout) catch |err| {
        std.debug.print("{s}: error processing tree-sitter results -- \"{}\"", .{ name, err });
        return;
    };
}

fn insert_event(d: *editlite.display, e: *const editlite.SDL_Event) void {
    _ = e;
    calculate_highlights(d);
}
fn render_glyph(cd: *editlite.character_display) void {
    if (cd.glyph == null) {
        return;
    }
    if (highlight_data.results.items.len == 0) {
        return;
    }
    const cur_highlight: parser.highlight_info = highlight_data.results.items[render_state_idx];
    const cur_color = cur_highlight.color;
    const col_end = cur_highlight.col + cur_highlight.text.?.len;
    if (cd.row == cur_highlight.row and cd.col >= cur_highlight.col and cd.col < col_end) {
        if (editlite.SDL_SetTextureColorMod(cd.glyph, cur_color.r, cur_color.g, cur_color.b) == -1) {
            std.debug.print("texture color mod not supported.\n", .{});
        }
    }
    if (cd.row > cur_highlight.row or cd.col >= col_end) {
        render_state_idx += 1;
    }
    if (render_state_idx >= highlight_data.results.items.len) {
        render_state_idx = 0;
    }
}
fn page_change(d: *editlite.display) void {
    calculate_highlights(d);
}

export fn setup(pi: *editlite.plugin_interface) bool {
    const insert_func: editlite.registry_entry = .{
        .callback = @constCast(&insert_event),
        .type = editlite.REG_INSERT,
    };
    pi.dispatch.?(pi, editlite.DISPATCH_REGISTER_FUNCTION, @constCast(&insert_func));
    const render_func: editlite.registry_entry = .{
        .callback = @constCast(&render_glyph),
        .type = editlite.REG_RENDER_GLYPH,
    };
    pi.dispatch.?(
        pi,
        editlite.DISPATCH_REGISTER_FUNCTION,
        @constCast(&render_func),
    );
    const page_func: editlite.registry_entry = .{
        .callback = @constCast(&page_change),
        .type = editlite.REG_PAGE_CHANGE,
    };
    pi.dispatch.?(pi, editlite.DISPATCH_REGISTER_FUNCTION, @constCast(&page_func));
    alloc = std.heap.page_allocator;
    highlight_data = parser.parse_results.init(alloc);
    return true;
}

export fn action(pi: *editlite.plugin_interface) bool {
    std.debug.print("action called\n", .{});
    pi.dispatch.?(pi, editlite.DISPATCH_NORMAL, null);
    return true;
}

export fn get_display_prompt(out: *[*:0]const u8) void {
    out.* = name.ptr;
}

export fn cleanup(pi: *editlite.plugin_interface) bool {
    highlight_data.deinit();
    _ = pi;
    return true;
}
