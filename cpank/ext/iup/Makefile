CC:=clang 
CFLAGS:=-Isrc/ -Iinclude/ -Isrcgl/ -Isrc/gtk/ -Isrc/mot $(shell pkg-config -cflags gtk+-3.0 gdk-3.0 gl)
BUILDDIR:=build
LDFLAGS:=$(shell pkg-config -libs gtk+-3.0 gdk-3.0) -lm
SRC:=$(shell find src -maxdepth 1 -name "*.c")
SRC:=srcgl/iup_glcanvas_x.c src/gtk/iupgtk_button.c src/gtk/iupgtk_button.c src/gtk/iupgtk_calendar.c src/gtk/iupgtk_canvas.c src/gtk/iupgtk_clipboard.c src/gtk/iupgtk_common.c src/gtk/iupgtk_dialog.c src/gtk/iupgtk_dragdrop.c src/gtk/iupgtk_draw_cairo.c src/gtk/iupgtk_filedlg.c src/gtk/iupgtk_focus.c src/gtk/iupgtk_font.c src/gtk/iupgtk_fontdlg.c src/gtk/iupgtk_frame.c src/gtk/iupgtk_frame.c src/gtk/iupgtk_globalattrib.c src/gtk/iupgtk_help.c src/gtk/iupgtk_image.c src/gtk/iupgtk_info.c src/gtk/iupgtk_image.c src/gtk/iupgtk_info.c src/gtk/iupgtk_key.c src/gtk/iupgtk_label.c src/gtk/iupgtk_list.c src/gtk/iupgtk_loop.c src/gtk/iupgtk_menu.c src/gtk/iupgtk_messagedlg.c src/gtk/iupgtk_open.c src/gtk/iupgtk_progressbar.c src/gtk/iupgtk_str.c src/gtk/iupgtk_tabs.c src/gtk/iupgtk_text.c src/gtk/iupgtk_timer.c src/gtk/iupgtk_tips.c src/gtk/iupgtk_toggle.c src/gtk/iupgtk_val.c
OBJS:=$(SRC:%=$(BUILDDIR)/%.o)
TARGET:=libiup.a
# src/gtk/iupgtk_tree.c
all: $(TARGET)

x: $(TARGET)
	$(CC) a.c ./libiup.a -o ax $(shell pkg-config --libs gtk+-3.0 gdk-3.0) -lXext -lX11 -lm

y: $(TARGET)
	$(CC) a.c ./zig-out/lib/libiup.a -o ay $(shell pkg-config --libs gtk+-3.0 gdk-3.0) -lXext -lX11 -lm

$(TARGET): $(OBJS)
	ar -rcs $@ $(OBJS)

$(BUILDDIR)/%.c.o: %.c 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

#all:
#	clang -Wall -g3 src/*.c -Iinclude/ -Isrc/src/gtk -Isrcgl/ -lm $(shell pkg-config -libs gtk+-3.0 gdk-3.0)
