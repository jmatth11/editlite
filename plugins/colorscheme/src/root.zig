const std = @import("std");
const testing = std.testing;

const sdl = @cImport({
    @cInclude("SDL2/SDL.h");
});

const editlite = @cImport({
    @cInclude("types/plugin_interface_types.h");
    @cInclude("types/display_type.h");
    @cInclude("types/registered_functions.h");
});

const name = "Colorscheme";

export fn insert_event(d: *editlite.display, e: *const sdl.SDL_Event) void {
    _ = d;
    _ = e;
    std.debug.print("insert event happened\n", .{});
}
export fn render_glyph(cd: *editlite.character_display) void {
    _ = cd;
}
export fn page_change(d: *editlite.display) void {
    _ = d;
    std.debug.print("page change event happened\n", .{});
}

//var insert_event_ptr: *anyopaque = @ptrCast(&insert_event);
//var render_glyph_ptr: *anyopaque = @ptrCast(&render_glyph);
//var page_change_ptr: *anyopaque = @ptrCast(&page_change);

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

//export fn render(d: *editlite.display, dim: *editlite.display_dim) bool {
//
//    return true;
//}
//
//export fn event(e: *sdl.SDL_Event, d: *editlite.display, dim: *editlite.display_dim) bool {
//
//    return true;
//}
//
export fn cleanup(pi: *editlite.plugin_interface) bool {
    _ = pi;
    return true;
}
