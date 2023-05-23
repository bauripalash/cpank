const std = @import("std");

const srcRoot = struct {
    fn getSrcDir() []const u8{
        return std.fs.path.dirname(@src().file).?;
    }
}.getSrcDir();

pub fn addIup(b: *std.Build , target : std.zig.CrossTarget , optimize : std.builtin.Mode) *std.Build.Step.Compile {
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

    var gtkSysLibs = [_][]const u8{"gtk+-3.0" , "gdk-3.0" , "m" , "X11"};
    var gtkSysIncludes = [_][]const u8{"gtk+3.0" , "gdk-3.0"};
    var gtkIncludePath = [_][]const u8{srcdir , srcRoot ++ "/include" , srcRoot ++ "/srcgl" , srcdir ++ "/gtk" , srcdir ++ "/mot"};

    const lib = b.addStaticLibrary(.{.name = "iup" , .target = target , .optimize = optimize});
    

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

    }

    return lib;
    
}
