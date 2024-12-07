const std = @import("std");
const ansi = @import("ansi_color.zig");

pub const highlight_info = struct {
    alloc: std.mem.Allocator,
    text: ?[]u8,
    color: ansi.color,
    row: usize,
    col: usize,

    pub fn init(alloc: std.mem.Allocator) highlight_info {
        return highlight_info{
            .alloc = alloc,
            .text = null,
            .color = undefined,
            .row = 0,
            .col = 0,
        };
    }

    pub fn copy_text(self: *highlight_info, text: []u8) !void {
        self.text = try self.alloc.alloc(u8, text.len);
        @memcpy(self.text.?, text);
    }

    pub fn deinit(self: *highlight_info) void {
        if (self.text) |text| {
            self.alloc.free(text);
        }
    }
};

pub const parse_results = struct {
    alloc: std.mem.Allocator,
    results: std.ArrayList(highlight_info),
    parsed: bool,

    pub fn init(alloc: std.mem.Allocator) parse_results {
        return parse_results{
            .alloc = alloc,
            .results = std.ArrayList(highlight_info).init(alloc),
            .parsed = false,
        };
    }

    pub fn parse(self: *parse_results, data: []u8) !void {
        var row: usize = 0;
        var col: usize = 0;
        var idx: usize = 0;
        var cur_highlight: ?highlight_info = null;
        var capturing_idx: ?usize = null;
        while (idx < data.len) {
            const cur_char = data[idx];
            if (cur_char == '\n') {
                row += 1;
                col = 0;
                idx += 1;
                continue;
            }
            if (cur_char == ansi.escape_code) {
                const ansi_color = try ansi.parse_color_at(data, idx);
                if (ansi_color.reset and cur_highlight != null) {
                    try cur_highlight.?.copy_text(data[capturing_idx.?..idx]);
                    try self.results.append(cur_highlight.?);
                    capturing_idx = null;
                    cur_highlight = null;
                } else if (cur_highlight == null and ansi_color.color.use_color) {
                    cur_highlight = highlight_info.init(self.alloc);
                    cur_highlight.?.color = ansi_color.color;
                    cur_highlight.?.row = row;
                    cur_highlight.?.col = col;
                }
                idx += ansi_color.bytes_read;
                if (capturing_idx == null and ansi_color.color.use_color) {
                    capturing_idx = idx;
                }
                continue;
            }
            idx += 1;
            col += 1;
        }
        self.parsed = true;
    }

    pub fn release_results(self: *parse_results) void {
        var idx: usize = 0;
        const len: usize = self.results.items.len;
        while (idx < len) : (idx += 1) {
            var hi = self.results.pop();
            hi.deinit();
        }
        self.parsed = false;
    }

    pub fn deinit(self: *parse_results) void {
        self.release_results();
        self.results.deinit();
    }
};
