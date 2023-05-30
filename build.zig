const std = @import("std");
const libiup = @import("cpank/ext/iup/build.zig");

pub fn exists(filename : []const u8) !bool {
    const f  = try std.fs.cwd().openFile(filename, .{});
    
    defer f.close();

    _ = try f.stat();

    return true;
    
    
}

pub fn build(b: *std.Build) void {
    

    const cflags = [_][]const u8{ "-Wall", "-std=c11" };

    const sourceToRun = "sample/unicode_escape.pank";

    //Core of Cpank
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

    //Standard Library
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

    //External Library
    var srcExt = [_][]const u8{
        "cpank/ext/baurinum/absneg.c",
        "cpank/ext/baurinum/addsub.c",
        "cpank/ext/baurinum/baurinum.c",
        "cpank/ext/baurinum/compare.c",
        "cpank/ext/baurinum/getter.c",
        "cpank/ext/baurinum/setter.c",
        "cpank/ext/baurinum/utility.c",
    };

    var srcGui = [_][]const u8{
        "gui/gui.c",
    };

    var guiFlags = [_][]const u8{};

    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // the default cli pankti interpreter
    const exe = b.addExecutable(.{ .name = "pankti", .target = target, .optimize = optimize });
    exe.linkLibC();
    exe.linkSystemLibrary("m");
    exe.defineCMacro("MODE_BENGALI", "");
    
    // the static library of pankti interpreter with exposed `run_code(...)`
    const lib = b.addStaticLibrary(.{ .name = "pankti", .optimize = optimize, .target = target });
    lib.linkLibC();
    lib.defineCMacro("MODE_BENGALI", "");
    lib.defineCMacro("APILIB", "");


    // the gui editor with pankti core integrated
    const guiexe = b.addExecutable(.{ .name = "panktiw", .target = target, .optimize = optimize });
    guiexe.linkLibC();
    guiexe.addCSourceFiles(&srcGui, &guiFlags);
    guiexe.defineCMacro("MODE_BENGALI", "");
    guiexe.defineCMacro("APILIB", "lib");
   
    // The Iup Library
    const ui = libiup.addIup(b, target, std.builtin.Mode.ReleaseFast);
    
    //CORE
    exe.addCSourceFiles(&srcCore, &cflags);
    lib.addCSourceFiles(&srcCore, &cflags);

    //STDLIB
    exe.addCSourceFiles(&srcStdlib, &cflags);
    lib.addCSourceFiles(&srcStdlib, &cflags);

    //External
    exe.addCSourceFiles(&srcExt, &cflags);
    lib.addCSourceFiles(&srcExt, &cflags);

    guiexe.linkLibrary(lib);
    guiexe.linkLibrary(ui);

    // Add Icons and Version Information on Windows
    if (target.isWindows()) {
        exe.addObjectFile("cpankproj/cpankproj.res.obj");
        guiexe.addObjectFile("cpankproj/cpankproj.res.obj");
    }

    // Prevents the gui variant to open a console
    guiexe.subsystem = .Windows;

    //Create a symbolic link 
    //Creates compatibility with existing Makefile based build steps
    const create_exe_shortcut = b.addSystemCommand(&[_][]const u8{
        "ln",
        "-s",
        "zig-out/bin/pankti",
        "./"
    });

    const create_gui_shortcut = b.addSystemCommand(&[_][]const u8{
        "ln",
        "-s",
        "zig-out/bin/panktiw",
        "./"
    });
    
    //Run The built cli aginst `sourceToRun`
    const run_cmd_run = b.addSystemCommand(&[_][]const u8{
        "zig-out/bin/pankti",
        sourceToRun,
    });

    //Run python based unittest
    const test_command = b.addSystemCommand(&[_][]const u8{
        "python",
        "-m",
        "unittest",
        "-v",
    });

    //Build CLI Executable
    var cli_build_step = b.step("x", "Build Executable 'pankti'");
    const install_cli_exe = b.addInstallArtifact(exe);
    cli_build_step.dependOn(&install_cli_exe.step);


    const cli_link_exists = exists("pankti") catch false;
    if (target.isLinux() and !cli_link_exists) {
        cli_build_step.dependOn(&create_exe_shortcut.step);
    }

    //Build GUI Executable
    const gui_build_step = b.step("g", "Build Gui 'panktiw'");
    const install_gui_exe = b.addInstallArtifact(guiexe);
    gui_build_step.dependOn(&install_gui_exe.step);

    const gui_link_exists = exists("panktiw") catch false;
    if (target.isLinux() and !gui_link_exists) {
        gui_build_step.dependOn(&create_gui_shortcut.step);
    }

    //Build API Static Library
    const lib_build_step = b.step("lib", "Build Static Library 'libpankti.a'");
    const install_api_lib = b.addInstallArtifact(lib);
    lib_build_step.dependOn(&install_api_lib.step);

    const run_code_step = b.step("run", "Build and Run Sample");
    run_code_step.dependOn(&exe.step);
    run_code_step.dependOn(&run_cmd_run.step);

    const test_step = b.step("test", "Run test using python");
    test_step.dependOn(&exe.step);
    test_step.dependOn(&test_command.step);
    
    b.default_step = cli_build_step;
    

    b.installArtifact(exe);
}
