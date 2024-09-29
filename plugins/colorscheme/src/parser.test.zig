const std = @import("std");
const testing = std.testing;

const parser = @import("parser.zig");
const ansi = @import("ansi_color.zig");

const test_file = @embedFile("testing/test.dat");

fn populate_str(dst: []u8, src: [*]const u8) void {
    var idx: usize = 0;
    while (idx < dst.len) : (idx += 1) {
        dst[idx] = src[idx];
    }
}

fn slice_from_str(str: [*:0]const u8) []u8 {
    return @constCast(std.mem.span(str));
}

test "highlight info copy text" {
    const test_alloc = testing.allocator;
    var hi = parser.highlight_info.init(test_alloc);
    defer hi.deinit();
    const test_text = try test_alloc.alloc(u8, 5);
    defer test_alloc.free(test_text);
    populate_str(test_text, "test!");
    try testing.expect(hi.text == null);
    try hi.copy_text(test_text);
    try testing.expectEqualSlices(u8, hi.text.?, test_text);
}

test "parser results - successful" {
    const test_alloc = testing.allocator;
    const expected: [17]parser.highlight_info = [_]parser.highlight_info{
        parser.highlight_info{
            .text = slice_from_str("function"),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 99,
                .g = 81,
                .b = 215,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 1,
            .col = 0,
        },
        parser.highlight_info{
            .text = slice_from_str("test_fun"),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 66,
                .g = 121,
                .b = 215,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 1,
            .col = 9,
        },
        parser.highlight_info{
            .text = slice_from_str("("),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 78,
                .g = 78,
                .b = 78,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 1,
            .col = 17,
        },
        parser.highlight_info{
            .text = slice_from_str(")"),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 78,
                .g = 78,
                .b = 78,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 1,
            .col = 18,
        },
        parser.highlight_info{
            .text = slice_from_str("{"),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 78,
                .g = 78,
                .b = 78,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 1,
            .col = 20,
        },
        parser.highlight_info{
            .text = slice_from_str("console"),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 0,
                .g = 0,
                .b = 0,
                .foreground = false,
                .bold = true,
                .use_color = false,
            },
            .row = 2,
            .col = 2,
        },
        parser.highlight_info{
            .text = slice_from_str("."),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 78,
                .g = 78,
                .b = 78,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 2,
            .col = 9,
        },
        parser.highlight_info{
            .text = slice_from_str("log"),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 66,
                .g = 121,
                .b = 215,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 2,
            .col = 10,
        },
        parser.highlight_info{
            .text = slice_from_str("("),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 78,
                .g = 78,
                .b = 78,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 2,
            .col = 13,
        },
        parser.highlight_info{
            .text = slice_from_str("\"hello, world!\""),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 68,
                .g = 135,
                .b = 0,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 2,
            .col = 14,
        },
        parser.highlight_info{
            .text = slice_from_str(")"),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 78,
                .g = 78,
                .b = 78,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 2,
            .col = 29,
        },
        parser.highlight_info{
            .text = slice_from_str(";"),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 78,
                .g = 78,
                .b = 78,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 2,
            .col = 30,
        },
        parser.highlight_info{
            .text = slice_from_str("}"),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 78,
                .g = 78,
                .b = 78,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 3,
            .col = 0,
        },
        parser.highlight_info{
            .text = slice_from_str("test_fun"),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 66,
                .g = 121,
                .b = 215,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 5,
            .col = 0,
        },
        parser.highlight_info{
            .text = slice_from_str("("),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 78,
                .g = 78,
                .b = 78,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 5,
            .col = 8,
        },
        parser.highlight_info{
            .text = slice_from_str(")"),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 78,
                .g = 78,
                .b = 78,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 5,
            .col = 9,
        },
        parser.highlight_info{
            .text = slice_from_str(";"),
            .alloc = test_alloc,
            .color = ansi.color{
                .r = 78,
                .g = 78,
                .b = 78,
                .foreground = true,
                .bold = false,
                .use_color = true,
            },
            .row = 5,
            .col = 10,
        },
    };
    const test_text = try test_alloc.alloc(u8, test_file.len);
    defer test_alloc.free(test_text);
    populate_str(test_text, test_file);
    var parser_t = parser.parse_results.init(test_alloc);
    defer parser_t.deinit();
    try testing.expect(parser_t.parsed == false);
    try parser_t.parse(test_text);
    try testing.expect(parser_t.parsed);
    try testing.expect(parser_t.results.items.len == 17);
    for (parser_t.results.items, 0..) |item, idx| {
        const expected_item = expected[idx];
        try testing.expectEqualSlices(u8, expected_item.text.?, item.text.?);
        try testing.expect(expected_item.color.r == item.color.r);
        try testing.expect(expected_item.color.g == item.color.g);
        try testing.expect(expected_item.color.b == item.color.b);
        try testing.expect(expected_item.color.foreground == item.color.foreground);
        try testing.expect(expected_item.color.bold == item.color.bold);
        try testing.expect(expected_item.color.use_color == item.color.use_color);
        try testing.expect(expected_item.row == item.row);
        try testing.expect(expected_item.col == item.col);
    }
}
