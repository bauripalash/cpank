const std = @import("std");

const srcdir = struct {
    fn getSrcDir() []const u8 {
        return std.fs.path.dirname(@src().file).?;
    }
}.getSrcDir();

pub fn get_build_inst(b: *std.Build, target: std.zig.CrossTarget, optimize: std.builtin.Mode) *std.Build.Step.Compile {
    var srcCommon = [_][]const u8{
        srcdir ++ "/common/areaevents.c",
        srcdir ++ "/common/attribute.c",
        srcdir ++ "/common/attrlist.c",
        srcdir ++ "/common/attrstr.c",
        srcdir ++ "/common/control.c",
        srcdir ++ "/common/debug.c",
        srcdir ++ "/common/matrix.c",
        srcdir ++ "/common/opentype.c",
        srcdir ++ "/common/shouldquit.c",
        srcdir ++ "/common/table.c",
        srcdir ++ "/common/tablemodel.c",
        srcdir ++ "/common/tablevalue.c",
        srcdir ++ "/common/userbugs.c",
        srcdir ++ "/common/utf.c",
    };

    var srcUnix = [_][]const u8{
        srcdir ++ "/unix/alloc.c",
        srcdir ++ "/unix/area.c",
        srcdir ++ "/unix/attrstr.c",
        srcdir ++ "/unix/box.c",
        srcdir ++ "/unix/button.c",
        srcdir ++ "/unix/cellrendererbutton.c",
        srcdir ++ "/unix/checkbox.c",
        srcdir ++ "/unix/child.c",
        srcdir ++ "/unix/colorbutton.c",
        srcdir ++ "/unix/combobox.c",
        srcdir ++ "/unix/control.c",
        srcdir ++ "/unix/datetimepicker.c",
        srcdir ++ "/unix/debug.c",
        srcdir ++ "/unix/draw.c",
        srcdir ++ "/unix/drawmatrix.c",
        srcdir ++ "/unix/drawpath.c",
        srcdir ++ "/unix/drawtext.c",
        srcdir ++ "/unix/editablecombo.c",
        srcdir ++ "/unix/entry.c",
        srcdir ++ "/unix/fontbutton.c",
        srcdir ++ "/unix/fontmatch.c",
        srcdir ++ "/unix/form.c",
        srcdir ++ "/unix/future.c",
        srcdir ++ "/unix/graphemes.c",
        srcdir ++ "/unix/grid.c",
        srcdir ++ "/unix/group.c",
        srcdir ++ "/unix/image.c",
        srcdir ++ "/unix/label.c",
        srcdir ++ "/unix/main.c",
        srcdir ++ "/unix/menu.c",
        srcdir ++ "/unix/multilineentry.c",
        srcdir ++ "/unix/opentype.c",
        srcdir ++ "/unix/progressbar.c",
        srcdir ++ "/unix/radiobuttons.c",
        srcdir ++ "/unix/separator.c",
        srcdir ++ "/unix/slider.c",
        srcdir ++ "/unix/spinbox.c",
        srcdir ++ "/unix/stddialogs.c",
        srcdir ++ "/unix/tab.c",
        srcdir ++ "/unix/table.c",
        srcdir ++ "/unix/tablemodel.c",
        srcdir ++ "/unix/text.c",
        srcdir ++ "/unix/util.c",
        srcdir ++ "/unix/window.c",
    };

    var unixSysLibs = [_][]const u8{"gtk+-3.0"};

    var srcWindows = [_][]const u8{
        srcdir ++ "/windows/alloc.cpp",
        srcdir ++ "/windows/area.cpp",
        srcdir ++ "/windows/areadraw.cpp",
        srcdir ++ "/windows/areaevents.cpp",
        srcdir ++ "/windows/areascroll.cpp",
        srcdir ++ "/windows/areautil.cpp",
        srcdir ++ "/windows/attrstr.cpp",
        srcdir ++ "/windows/box.cpp",
        srcdir ++ "/windows/button.cpp",
        srcdir ++ "/windows/checkbox.cpp",
        srcdir ++ "/windows/colorbutton.cpp",
        srcdir ++ "/windows/colordialog.cpp",
        srcdir ++ "/windows/combobox.cpp",
        srcdir ++ "/windows/container.cpp",
        srcdir ++ "/windows/control.cpp",
        srcdir ++ "/windows/d2dscratch.cpp",
        srcdir ++ "/windows/datetimepicker.cpp",
        srcdir ++ "/windows/debug.cpp",
        srcdir ++ "/windows/draw.cpp",
        srcdir ++ "/windows/drawmatrix.cpp",
        srcdir ++ "/windows/drawpath.cpp",
        srcdir ++ "/windows/drawtext.cpp",
        srcdir ++ "/windows/dwrite.cpp",
        srcdir ++ "/windows/editablecombo.cpp",
        srcdir ++ "/windows/entry.cpp",
        srcdir ++ "/windows/events.cpp",
        srcdir ++ "/windows/fontbutton.cpp",
        srcdir ++ "/windows/fontdialog.cpp",
        srcdir ++ "/windows/fontmatch.cpp",
        srcdir ++ "/windows/form.cpp",
        srcdir ++ "/windows/graphemes.cpp",
        srcdir ++ "/windows/grid.cpp",
        srcdir ++ "/windows/group.cpp",
        srcdir ++ "/windows/image.cpp",
        srcdir ++ "/windows/init.cpp",
        srcdir ++ "/windows/label.cpp",
        srcdir ++ "/windows/main.cpp",
        srcdir ++ "/windows/menu.cpp",
        srcdir ++ "/windows/multilineentry.cpp",
        srcdir ++ "/windows/opentype.cpp",
        srcdir ++ "/windows/parent.cpp",
        srcdir ++ "/windows/progressbar.cpp",
        srcdir ++ "/windows/radiobuttons.cpp",
        srcdir ++ "/windows/separator.cpp",
        srcdir ++ "/windows/sizing.cpp",
        srcdir ++ "/windows/slider.cpp",
        srcdir ++ "/windows/spinbox.cpp",
        srcdir ++ "/windows/stddialogs.cpp",
        srcdir ++ "/windows/tab.cpp",
        srcdir ++ "/windows/table.cpp",
        srcdir ++ "/windows/tabledispinfo.cpp",
        srcdir ++ "/windows/tabledraw.cpp",
        srcdir ++ "/windows/tableediting.cpp",
        srcdir ++ "/windows/tablemetrics.cpp",
        srcdir ++ "/windows/tabpage.cpp",
        srcdir ++ "/windows/text.cpp",
        srcdir ++ "/windows/utf16.cpp",
        srcdir ++ "/windows/utilwin.cpp",
        srcdir ++ "/windows/window.cpp",
        srcdir ++ "/windows/winpublic.cpp",
        srcdir ++ "/windows/winutil.cpp",
    };

    var WindowsSysLibs = [_][]const u8{ "user32", "kernel32", "gdi32", "comctl32", "uxtheme", "msimg32", "comdlg32", "d2d1", "dwrite", "ole32", "oleaut32", "olecc", "uuid", "windowscodecs" };

    const libui = b.addStaticLibrary(.{
        .name = "ui",
        .target = target,
        .optimize = optimize,
    });

    libui.linkLibC();

    libui.addCSourceFiles(&srcCommon, &.{});

    if (target.isLinux()) {
        libui.addCSourceFiles(&srcUnix, &.{});
        for (unixSysLibs) |value| {
            libui.linkSystemLibrary(value);
        }
    } else if (target.isWindows()) {
        libui.addCSourceFiles(&srcWindows, &.{});
        for (WindowsSysLibs) |value| {
            libui.linkSystemLibrary(value);
        }

        libui.linkLibCpp();
        libui.disable_sanitize_c = true;
        libui.defineCMacro("_Z_BUILD", "1");
    }

    //b.installArtifact(libui);
    return libui;
}
