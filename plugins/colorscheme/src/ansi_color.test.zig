const std = @import("std");
const testing = std.testing;

const ansi = @import("ansi_color.zig");

fn gen_escape_code(buf: [*]u8, text: [*:0]const u8, n: usize) void {
    buf[0] = ansi.escape_code;
    var text_idx: usize = 0;
    while (text_idx < n) : (text_idx += 1) {
        buf[text_idx + 1] = text[text_idx];
    }
}

fn gen_escape_code_post_fix(buf: [*]u8, text: [*:0]const u8, buf_n: usize, n: usize) void {
    var buf_idx: usize = 0;
    // pad beginning
    while (buf_idx < buf_n) : (buf_idx += 1) {
        buf[buf_idx] = 'a';
    }
    buf[buf_idx] = ansi.escape_code;
    var text_idx: usize = 0;
    while (text_idx < n) : (text_idx += 1) {
        buf[text_idx + (buf_idx + 1)] = text[text_idx];
    }
}

test "ansi escape code" {
    const test_str = '';
    try testing.expect(test_str == ansi.escape_code);
}

test "parse ansi code 256 value - foreground color" {
    const test_str: [*:0]const u8 = "[38;5;56m";
    var text_buff = [_]u8{0} ** 10;
    gen_escape_code(&text_buff, test_str, 9);
    const result = try ansi.parse_color(&text_buff);
    try testing.expect(result.bytes_read == 10);
    try testing.expect(result.color.foreground == true);
    try testing.expect(result.color.r == 99);
    try testing.expect(result.color.g == 81);
    try testing.expect(result.color.b == 215);
    try testing.expect(result.reset == false);
    try testing.expect(result.color.use_color == true);
}

test "parse ansi code 256 value - foreground standard color" {
    const test_str: [*:0]const u8 = "[38;5;10m";
    var text_buff = [_]u8{0} ** 10;
    gen_escape_code(&text_buff, test_str, 9);
    const result = try ansi.parse_color(&text_buff);
    try testing.expect(result.bytes_read == 10);
    try testing.expect(result.color.foreground == true);
    try testing.expect(result.color.r == 0);
    try testing.expect(result.color.g == 255);
    try testing.expect(result.color.b == 0);
    try testing.expect(result.reset == false);
    try testing.expect(result.color.use_color == true);
}

test "parse ansi code 256 value - background gray" {
    const test_str: [*:0]const u8 = "[48;5;239m";
    var text_buff = [_]u8{0} ** 11;
    gen_escape_code(&text_buff, test_str, 10);
    const result = try ansi.parse_color(&text_buff);
    try testing.expect(result.bytes_read == 11);
    try testing.expect(result.color.foreground == false);
    try testing.expect(result.color.r == 78);
    try testing.expect(result.color.g == 78);
    try testing.expect(result.color.b == 78);
    try testing.expect(result.reset == false);
    try testing.expect(result.color.use_color == true);
}

test "parse ansi code 256 value - reset code" {
    const test_str: [*:0]const u8 = "[0m";
    var text_buff = [_]u8{0} ** 4;
    gen_escape_code(&text_buff, test_str, 4);
    const result = try ansi.parse_color(&text_buff);
    try testing.expect(result.bytes_read == 4);
    try testing.expect(result.color.foreground == false);
    try testing.expect(result.color.r == 0);
    try testing.expect(result.color.g == 0);
    try testing.expect(result.color.b == 0);
    try testing.expect(result.reset == true);
    try testing.expect(result.color.use_color == false);
}

test "parse ansi code 256 value - bold" {
    const test_str: [*:0]const u8 = "[0m";
    var text_buff = [_]u8{0} ** 4;
    gen_escape_code(&text_buff, test_str, 4);
    const result = try ansi.parse_color(&text_buff);
    try testing.expect(result.bytes_read == 4);
    try testing.expect(result.color.foreground == false);
    try testing.expect(result.color.r == 0);
    try testing.expect(result.color.g == 0);
    try testing.expect(result.color.b == 0);
    try testing.expect(result.reset == false);
    try testing.expect(result.color.use_color == false);
    try testing.expect(result.color.bold == true);
}
test "parse ansi code 256 value - at index" {
    const test_str: [*:0]const u8 = "[38;5;56m";
    var text_buff = [_]u8{0} ** 15;
    gen_escape_code_post_fix(&text_buff, test_str, 5, 9);
    const result = try ansi.parse_color_at(&text_buff, 5);
    try testing.expect(result.bytes_read == 10);
    try testing.expect(result.color.foreground == true);
    try testing.expect(result.color.r == 99);
    try testing.expect(result.color.g == 81);
    try testing.expect(result.color.b == 215);
    try testing.expect(result.reset == false);
    try testing.expect(result.color.use_color == false);
}
