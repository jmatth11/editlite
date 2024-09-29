const std = @import("std");

pub const escape_code: u8 = 0o33;
pub const foreground: u8 = 38;
pub const background: u8 = 48;
pub const flag_256: u8 = 5;
const ansi_group_limit: u8 = 5;

pub const ansi_error = error{
    invalid_format,
    error_parsing,
    unsupported_format,
};

pub const color = struct {
    r: u8,
    g: u8,
    b: u8,
    foreground: bool,
    bold: bool,
    use_color: bool,
};

pub const ansi_results = struct {
    bytes_read: u8,
    color: color,
    reset: bool,
};

const ansi_flags = struct {
    flag_256: bool,
};

fn empty_ansi_flag() ansi_flags {
    return ansi_flags{
        .flag_256 = false,
    };
}

fn load_256_value(n: i16, r: *color) void {
    if (n <= 0x0F) {
        const c = ansi_standard_colors.color_from_ansi(@intCast(n));
        r.r = c.r;
        r.g = c.g;
        r.b = c.b;
    } else if (n <= 0xE7) {
        const float_n: f32 = @floatFromInt(n);
        const index_r: f32 = ((float_n - 16.0) / 36.0);
        if (index_r > 0) r.r = @intFromFloat(55.0 + index_r * 40.0);
        const index_g: f32 = (@mod((float_n - 16.0), 36.0) / 6.0);
        if (index_g > 0) r.g = @intFromFloat(55.0 + index_g * 40.0);
        const index_b: f32 = (@mod((float_n - 16.0), 6.0));
        if (index_b > 0) r.b = @intFromFloat(55.0 + index_b * 40.0);
    } else if (n <= 0xFF) {
        const gray_color: u8 = (@as(u8, @intCast(n)) - 232) * 10 + 8;
        r.r = gray_color;
        r.g = gray_color;
        r.b = gray_color;
    }
}

fn assign_ansi_values(nums: [ansi_group_limit]u8, groups_parsed: u8, result: *ansi_results) ansi_error!void {
    switch (groups_parsed) {
        1 => {
            const num = nums[0];
            if (num == 0) {
                result.reset = true;
                result.color.use_color = false;
            }
            if (num == 1) {
                result.color.bold = true;
                result.color.use_color = false;
            }
        },
        3 => {
            if (nums[0] == foreground or nums[0] == background) {
                result.color.foreground = (nums[0] == foreground);
            }
            if (nums[1] == flag_256) {
                load_256_value(nums[2], &result.color);
            }
            result.color.use_color = true;
        },
        else => return ansi_error.unsupported_format,
    }
}

pub fn parse_color_at(str: []u8, idx: usize) ansi_error!ansi_results {
    return try parse_color(str[idx..]);
}

pub fn parse_color(str: []u8) ansi_error!ansi_results {
    var result: ansi_results = .{
        .bytes_read = 0,
        .color = .{
            .r = 0,
            .g = 0,
            .b = 0,
            .foreground = undefined,
            .bold = false,
            .use_color = false,
        },
        .reset = false,
    };
    if (str.len < 4) return ansi_error.invalid_format;
    if (str[0] != escape_code or str[1] != '[') return ansi_error.invalid_format;
    var process = true;
    var str_starting_idx: u8 = 2;
    var parsed_number: u8 = 0;
    var groupings: [ansi_group_limit]u8 = undefined;
    while (process) {
        var buf: [4]u8 = undefined;
        var idx: u8 = 0;
        while (idx < buf.len) : (idx += 1) {
            const cur_idx = str_starting_idx + idx;
            if (cur_idx >= str.len) return ansi_error.invalid_format;
            const cur_char = str[cur_idx];
            if (cur_char == 'm') {
                process = false;
                break;
            }
            if (cur_char == ';') break;
            buf[idx] = cur_char;
        }
        str_starting_idx += idx + 1;
        parsed_number += 1;
        if (parsed_number >= ansi_group_limit) return ansi_error.invalid_format;
        const num: u8 = std.fmt.parseInt(u8, buf[0..idx], 10) catch return ansi_error.error_parsing;
        groupings[(parsed_number - 1)] = num;
    }
    try assign_ansi_values(groupings, parsed_number, &result);
    result.bytes_read = str_starting_idx;
    return result;
}

pub const ansi_standard_colors = struct {
    const black: color = .{
        .r = 0,
        .g = 0,
        .b = 0,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const white: color = .{
        .r = 255,
        .g = 255,
        .b = 255,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const red: color = .{
        .r = 255,
        .g = 0,
        .b = 0,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const lime: color = .{
        .r = 0,
        .g = 255,
        .b = 0,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const blue: color = .{
        .r = 0,
        .g = 0,
        .b = 255,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const yellow: color = .{
        .r = 255,
        .g = 255,
        .b = 0,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const cyan: color = .{
        .r = 0,
        .g = 255,
        .b = 255,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const magenta: color = .{
        .r = 255,
        .g = 0,
        .b = 255,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const silver: color = .{
        .r = 192,
        .g = 192,
        .b = 192,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const gray: color = .{
        .r = 128,
        .g = 128,
        .b = 128,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const maroon: color = .{
        .r = 128,
        .g = 0,
        .b = 0,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const olive: color = .{
        .r = 128,
        .g = 128,
        .b = 0,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const green: color = .{
        .r = 0,
        .g = 128,
        .b = 0,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const purple: color = .{
        .r = 128,
        .g = 0,
        .b = 128,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const teal: color = .{
        .r = 0,
        .g = 128,
        .b = 128,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };
    const navy: color = .{
        .r = 0,
        .g = 0,
        .b = 128,
        .foreground = true,
        .bold = false,
        .use_color = true,
    };

    pub fn color_from_ansi(n: u8) color {
        return switch (n) {
            0 => ansi_standard_colors.black,
            1 => ansi_standard_colors.maroon,
            2 => ansi_standard_colors.green,
            3 => ansi_standard_colors.olive,
            4 => ansi_standard_colors.navy,
            5 => ansi_standard_colors.purple,
            6 => ansi_standard_colors.teal,
            7 => ansi_standard_colors.silver,
            8 => ansi_standard_colors.gray,
            9 => ansi_standard_colors.red,
            10 => ansi_standard_colors.lime,
            11 => ansi_standard_colors.yellow,
            12 => ansi_standard_colors.blue,
            13 => ansi_standard_colors.magenta,
            14 => ansi_standard_colors.cyan,
            15 => ansi_standard_colors.white,
            else => ansi_standard_colors.white,
        };
    }
};
