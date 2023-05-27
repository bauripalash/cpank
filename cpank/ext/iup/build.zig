const std = @import("std");

const srcRoot = struct {
    fn getSrcDir() []const u8 {
        return std.fs.path.dirname(@src().file).?;
    }
}.getSrcDir();

pub fn addIup(b: *std.Build, target: std.zig.CrossTarget, optimize: std.builtin.Mode) *std.Build.Step.Compile {
    const srcdir = srcRoot ++ "/src/";
    var srcCore = [_][]const u8{
        srcdir ++ "/iup_animatedlabel.c",
        srcdir ++ "/iup_array.c",
        srcdir ++ "/iup_assert.c",
        srcdir ++ "/iup_attrib.c",
        srcdir ++ "/iup_backgroundbox.c",
        srcdir ++ "/iup_box.c",
        srcdir ++ "/iup_button.c",
        srcdir ++ "/iup.c",
        srcdir ++ "/iup_callback.c",
        srcdir ++ "/iup_cbox.c",
        srcdir ++ "/iup_childtree.c",
        srcdir ++ "/iup_classattrib.c",
        srcdir ++ "/iup_classbase.c",
        srcdir ++ "/iup_class.c",
        srcdir ++ "/iup_classinfo.c",
        srcdir ++ "/iup_config.c",
        srcdir ++ "/iup_detachbox.c",
        srcdir ++ "/iup_dialog.c",
        srcdir ++ "/iup_dlglist.c",
        srcdir ++ "/iup_draw.c",
        srcdir ++ "/iup_dropbutton.c",
        srcdir ++ "/iup_elempropdlg.c",
        srcdir ++ "/iup_expander.c",
        srcdir ++ "/iup_export.c",
        srcdir ++ "/iup_filedlg.c",
        srcdir ++ "/iup_fill.c",
        srcdir ++ "/iup_flatbutton.c",
        srcdir ++ "/iup_flatframe.c",
        srcdir ++ "/iup_flatlabel.c",
        srcdir ++ "/iup_flatlist.c",
        srcdir ++ "/iup_flatscrollbar.c",
        srcdir ++ "/iup_flatscrollbox.c",
        srcdir ++ "/iup_flatseparator.c",
        srcdir ++ "/iup_flattabs.c",
        srcdir ++ "/iup_flattoggle.c",
        srcdir ++ "/iup_flattree.c",
        srcdir ++ "/iup_flatval.c",
        srcdir ++ "/iup_focus.c",
        srcdir ++ "/iup_font.c",
        srcdir ++ "/iup_fontdlg.c",
        srcdir ++ "/iup_frame.c",
        srcdir ++ "/iup_func.c",
        srcdir ++ "/iup_gauge.c",
        srcdir ++ "/iup_getparam.c",
        srcdir ++ "/iup_globalattrib.c",
        srcdir ++ "/iup_globalsdlg.c",
        srcdir ++ "/iup_gridbox.c",
        srcdir ++ "/iup_hbox.c",
        srcdir ++ "/iup_image.c",
        srcdir ++ "/iup_key.c",
        srcdir ++ "/iup_label.c",
        srcdir ++ "/iup_layout.c",
        srcdir ++ "/iup_layoutdlg.c",
        srcdir ++ "/iup_ledlex.c",
        srcdir ++ "/iup_ledparse.c",
        srcdir ++ "/iup_linefile.c",
        srcdir ++ "/iup_link.c",
        srcdir ++ "/iup_list.c",
        srcdir ++ "/iup_loop.c",
        srcdir ++ "/iup_mask.c",
        srcdir ++ "/iup_maskmatch.c",
        srcdir ++ "/iup_maskparse.c",
        srcdir ++ "/iup_menu.c",
        srcdir ++ "/iup_messagedlg.c",
        srcdir ++ "/iup_multibox.c",
        srcdir ++ "/iup_names.c",
        srcdir ++ "/iup_normalizer.c",
        srcdir ++ "/iup_object.c",
        srcdir ++ "/iup_open.c",
        srcdir ++ "/iup_predialogs.c",
        srcdir ++ "/iup_progressbar.c",
        srcdir ++ "/iup_progressdlg.c",
        srcdir ++ "/iup_radio.c",
        srcdir ++ "/iup_recplay.c",
        srcdir ++ "/iup_register.c",
        srcdir ++ "/iup_sbox.c",
        srcdir ++ "/iup_scanf.c",
        srcdir ++ "/iup_scrollbox.c",
        srcdir ++ "/iup_show.c",
        srcdir ++ "/iup_space.c",
        srcdir ++ "/iup_split.c",
        srcdir ++ "/iup_str.c",
        srcdir ++ "/iup_strmessage.c",
        srcdir ++ "/iup_table.c",
        srcdir ++ "/iup_tabs.c",
        srcdir ++ "/iup_text.c",
        srcdir ++ "/iup_thread.c",
        srcdir ++ "/iup_timer.c",
        srcdir ++ "/iup_toggle.c",
        srcdir ++ "/iup_user.c",
        srcdir ++ "/iup_val.c",
        srcdir ++ "/iup_vbox.c",
        srcdir ++ "/iup_zbox.c",
        srcdir ++ "/iup_canvas.c",
        srcdir ++ "/iup_colorbar.c",
        srcdir ++ "/iup_colorbrowser.c",
        srcdir ++ "/iup_colordlg.c",
        srcdir ++ "/iup_colorhsi.c",
        srcdir ++ "/iup_datepick.c",
        srcdir ++ "/iup_dial.c",
        srcdir ++ "/iup_spin.c",
        srcdir ++ "/iup_tree.c",
    };

    var srcGtk = [_][]const u8{
        srcdir ++ "/gtk/iupgtk_button.c",
        srcdir ++ "/gtk/iupgtk_calendar.c",
        srcdir ++ "/gtk/iupgtk_canvas.c",
        srcdir ++ "/gtk/iupgtk_clipboard.c",
        srcdir ++ "/gtk/iupgtk_common.c",
        srcdir ++ "/gtk/iupgtk_dialog.c",
        srcdir ++ "/gtk/iupgtk_dragdrop.c",
        srcdir ++ "/gtk/iupgtk_draw_cairo.c",
        srcdir ++ "/gtk/iupgtk_filedlg.c",
        srcdir ++ "/gtk/iupgtk_focus.c",
        srcdir ++ "/gtk/iupgtk_font.c",
        srcdir ++ "/gtk/iupgtk_fontdlg.c",
        srcdir ++ "/gtk/iupgtk_frame.c",
        srcdir ++ "/gtk/iupgtk_globalattrib.c",
        srcdir ++ "/gtk/iupgtk_help.c",
        srcdir ++ "/gtk/iupgtk_image.c",
        srcdir ++ "/gtk/iupgtk_info.c",
        srcdir ++ "/gtk/iupgtk_image.c",
        srcdir ++ "/gtk/iupgtk_info.c",
        srcdir ++ "/gtk/iupgtk_key.c",
        srcdir ++ "/gtk/iupgtk_label.c",
        srcdir ++ "/gtk/iupgtk_list.c",
        srcdir ++ "/gtk/iupgtk_loop.c",
        srcdir ++ "/gtk/iupgtk_menu.c",
        srcdir ++ "/gtk/iupgtk_messagedlg.c",
        srcdir ++ "/gtk/iupgtk_open.c",
        srcdir ++ "/gtk/iupgtk_progressbar.c",
        srcdir ++ "/gtk/iupgtk_str.c",
        srcdir ++ "/gtk/iupgtk_tabs.c",
        srcdir ++ "/gtk/iupgtk_text.c",
        srcdir ++ "/gtk/iupgtk_timer.c",
        srcdir ++ "/gtk/iupgtk_tips.c",
        srcdir ++ "/gtk/iupgtk_toggle.c",
        srcdir ++ "/gtk/iupgtk_val.c",
        srcdir ++ "/gtk/iupgtk_tree.c",
        srcdir ++ "/mot/iupunix_info.c",
        srcRoot ++ "/srcgl/iup_glcanvas_x.c",
    };

    var srcWin = [_][]const u8{
        srcdir ++ "/win/iupwindows_help.c",
        srcdir ++ "/win/iupwindows_info.c",
        srcdir ++ "/win/iupwindows_main.c",
        srcdir ++ "/win/iupwin_brush.c",
        srcdir ++ "/win/iupwin_button.c",
        srcdir ++ "/win/iupwin_calendar.c",
        srcdir ++ "/win/iupwin_canvas.c",
        srcdir ++ "/win/iupwin_clipboard.c",
        srcdir ++ "/win/iupwin_common.c",
        srcdir ++ "/win/iupwin_datepick.c",
        srcdir ++ "/win/iupwin_dialog.c",
        srcdir ++ "/win/iupwin_dragdrop.c",
        srcdir ++ "/win/iupwin_draw.c",
        srcdir ++ "/win/iupwin_draw_gdi.c",
        srcdir ++ "/win/iupwin_draw_wdl.c",
        srcdir ++ "/win/iupwin_filedlg.c",
        srcdir ++ "/win/iupwin_focus.c",
        srcdir ++ "/win/iupwin_font.c",
        srcdir ++ "/win/iupwin_fontdlg.c",
        srcdir ++ "/win/iupwin_frame.c",
        srcdir ++ "/win/iupwin_globalattrib.c",
        srcdir ++ "/win/iupwin_handle.c",
        srcdir ++ "/win/iupwin_image.c",
        srcdir ++ "/win/iupwin_image_wdl.c",
        srcdir ++ "/win/iupwin_info.c",
        srcdir ++ "/win/iupwin_key.c",
        srcdir ++ "/win/iupwin_label.c",
        srcdir ++ "/win/iupwin_list.c",
        srcdir ++ "/win/iupwin_loop.c",
        srcdir ++ "/win/iupwin_menu.c",
        srcdir ++ "/win/iupwin_messagedlg.c",
        srcdir ++ "/win/iupwin_open.c",
        srcdir ++ "/win/iupwin_progressbar.c",
        srcdir ++ "/win/iupwin_str.c",
        srcdir ++ "/win/iupwin_tabs.c",
        srcdir ++ "/win/iupwin_text.c",
        srcdir ++ "/win/iupwin_timer.c",
        srcdir ++ "/win/iupwin_tips.c",
        srcdir ++ "/win/iupwin_toggle.c",
        srcdir ++ "/win/iupwin_touch.c",
        srcdir ++ "/win/iupwin_tree.c",
        srcdir ++ "/win/iupwin_val.c",
        srcdir ++ "/win/wdl/backend-d2d.c",
        srcdir ++ "/win/wdl/backend-dwrite.c",
        srcdir ++ "/win/wdl/backend-gdix.c",
        srcdir ++ "/win/wdl/backend-wic.c",
        srcdir ++ "/win/wdl/bitblt.c",
        srcdir ++ "/win/wdl/brush.c",
        srcdir ++ "/win/wdl/cachedimage.c",
        srcdir ++ "/win/wdl/canvas.c",
        srcdir ++ "/win/wdl/draw.c",
        srcdir ++ "/win/wdl/fill.c",
        srcdir ++ "/win/wdl/font.c",
        srcdir ++ "/win/wdl/image.c",
        srcdir ++ "/win/wdl/init.c",
        srcdir ++ "/win/wdl/memstream.c",
        srcdir ++ "/win/wdl/misc.c",
        srcdir ++ "/win/wdl/path.c",
        srcdir ++ "/win/wdl/string.c",
        srcdir ++ "/win/wdl/strokestyle.c",
    };

    var gtkSysLibs = [_][]const u8{ "gtk+-3.0", "gdk-3.0", "m", "X11" };
    var gtkSysIncludes = [_][]const u8{ "gtk+3.0", "gdk-3.0" };
    var gtkIncludePath = [_][]const u8{ srcdir, srcRoot ++ "/include", srcRoot ++ "/srcgl", srcdir ++ "/gtk", srcdir ++ "/mot" };
    var winIncludePath = [_][]const u8{ srcdir, srcRoot ++ "/include", srcRoot ++ "/srcgl", srcdir ++ "/win", srcdir ++ "/win/wdl" };
    var winSysLibs = [_][]const u8{
        "gdi32",
        "comdlg32",
        "comctl32",
        "uuid",
        "oleaut32",
        "ole32",
    };

    const lib = b.addStaticLibrary(.{ .name = "iup", .target = target, .optimize = optimize });

    lib.addCSourceFiles(&srcCore, &.{});

    if (target.isLinux()) {
        lib.addCSourceFiles(&srcGtk, &.{});
        for (gtkSysLibs) |value| {
            lib.linkSystemLibrary(value);
        }

        for (gtkSysIncludes) |value| {
            lib.addSystemIncludePath(value);
        }

        for (gtkIncludePath) |value| {
            lib.addIncludePath(value);
        }
    } else if (target.isWindows()) {
        lib.addCSourceFiles(&srcWin, &.{});
        for (winSysLibs) |value| {
            lib.linkSystemLibrary(value);
        }

        for (winIncludePath) |value| {
            lib.addIncludePath(value);
        }

        lib.defineCMacro("USE_NEW_DRAW", "");
        lib.defineCMacro("_WIN32_WINNT", "0x0601");
        lib.defineCMacro("WINVER", "0x0601");
        lib.defineCMacro("COBJMACROS", "");
        lib.defineCMacro("NOTREEVIEW", "");
        lib.defineCMacro("UNICODE", "");
        lib.defineCMacro("_UNICODE", "");
    }
    lib.linkLibC();

    return lib;
}
