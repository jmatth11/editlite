const std = @import("std");

const c = @cImport({
    @cInclude("SDL2/SDL.h");
    @cInclude("types/plugin_interface_types.h");
    @cInclude("types/display_type.h");
    @cInclude("types/registered_functions.h");
});

const name = "Colorscheme";

fn insert_event(d: *c.display, e: *const c.SDL_Event) void {
    _ = d;
    _ = e;
    std.debug.print("insert event happened\n", .{});
}
fn render_glyph(cd: *c.character_display) void {
    if (cd.glyph == null) {
        return;
    }
    if (c.SDL_SetTextureColorMod(cd.glyph, 100, 0, 0) == -1) {
        std.debug.print("texture color mod not supported.\n", .{});
    }
}
fn page_change(d: *c.display) void {
    _ = d;
    std.debug.print("page change event happened\n", .{});
}

export fn setup(pi: *c.plugin_interface) bool {
    const insert_func: c.registry_entry = .{
        .callback = @constCast(&insert_event),
        .type = c.REG_INSERT,
    };
    pi.dispatch.?(pi, c.DISPATCH_REGISTER_FUNCTION, @constCast(&insert_func));
    const render_func: c.registry_entry = .{
        .callback = @constCast(&render_glyph),
        .type = c.REG_RENDER_GLYPH,
    };
    pi.dispatch.?(
        pi,
        c.DISPATCH_REGISTER_FUNCTION,
        @constCast(&render_func),
    );
    const page_func: c.registry_entry = .{
        .callback = @constCast(&page_change),
        .type = c.REG_PAGE_CHANGE,
    };
    pi.dispatch.?(pi, c.DISPATCH_REGISTER_FUNCTION, @constCast(&page_func));
    return true;
}

export fn action(pi: *c.plugin_interface) bool {
    std.debug.print("action called\n", .{});
    pi.dispatch.?(pi, c.DISPATCH_NORMAL, null);
    return true;
}

export fn get_display_prompt(out: *[*:0]const u8) void {
    out.* = name.ptr;
}

export fn cleanup(pi: *c.plugin_interface) bool {
    _ = pi;
    return true;
}
