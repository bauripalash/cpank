const std = @import("std");

pub fn build(b: *std.Build) void {
    var srcCore = [_][]const u8{
        "cpank/api.c",
        "cpank/bn.c",
        "cpank/builtins.c",
        "cpank/compiler.c",
        "cpank/debug.c",
        "cpank/errmsg.c",
        "cpank/htable.c",
        "cpank/instruction.c",
        "cpank/lexer.c",
        "cpank/mem.c",
        "cpank/obj.c",
        "cpank/openfile.c",
        "cpank/runfile.c",
        "cpank/utils.c",
        "cpank/value.c",
        "cpank/vm.c",
        "cpank/main.c",
    };

    var srcStdlib = [_][]const u8{
        "cpank/stdlib/array.c",
        "cpank/stdlib/big.c",
        "cpank/stdlib/bnmath.c",
        "cpank/stdlib/bnos.c",
        "cpank/stdlib/common.c",
        "cpank/stdlib/file.c",
        "cpank/stdlib/map.c",
        "cpank/stdlib/math.c",
        "cpank/stdlib/os.c",
        "cpank/stdlib/stdlib.c",
        "cpank/stdlib/string.c",
    };

    var srcExt = [_][]const u8{
        "cpank/ext/baurinum/absneg.c",
        "cpank/ext/baurinum/addsub.c",
        "cpank/ext/baurinum/baurinum.c",
        "cpank/ext/baurinum/compare.c",
        "cpank/ext/baurinum/getter.c",
        "cpank/ext/baurinum/setter.c",
        "cpank/ext/baurinum/utility.c",
    };

    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{ .name = "pankti", .target = target, .optimize = optimize });
    const lib = b.addStaticLibrary(.{ .name = "pankti", .optimize = optimize, .target = target });

    exe.linkLibC();
    lib.linkLibC();

    exe.defineCMacro("MODE_BENGALI", "");
    lib.defineCMacro("MODE_BENGALI", "");
    lib.defineCMacro("APILIB", "");

    //CORE
    exe.addCSourceFiles(&srcCore, &.{});
    lib.addCSourceFiles(&srcCore, &.{});

    //STDLIB
    exe.addCSourceFiles(&srcStdlib, &.{});
    lib.addCSourceFiles(&srcStdlib, &.{});

    //lib.addCSourceFiles(files: []const []const u8, flags: []const []const u8)

    //External
    exe.addCSourceFiles(&srcExt, &.{});
    lib.addCSourceFiles(&srcExt, &.{});

    exe.linkSystemLibrary("m");

    b.installArtifact(exe);
    b.installArtifact(lib);
}
