
lib_LIBRARIES = libcommand.a

LDFLAGS = -L$(top_srcdir)/src/elflib -L$(top_srcdir)/src/utils
INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/elflib \
			  -I$(top_srcdir)/src/utils \
			  -I/usr/include/libxml2

CFLAGS = -g 

libcommand_a_SOURCES = \
builtins.c \
builtins.h \
commands.c \
commands.h \
help.c \
set.c \
show.c \
show_elf.c \
map.c \
funcmap.c \
testcommands.c
