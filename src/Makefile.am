SUBDIRS = elflib utils commands

bin_PROGRAMS = elfcrunch

noinst_PROGRAMS = testprog
testprog_SOURCES = testprog.c

CFLAGS = -g 
elfcrunch_LDFLAGS = -L$(srcdir)/elflib   \
						  -L$(srcdir)/utils    \
						  -L$(srcdir)/commands \
						  -L/usr/local/include

INCLUDES = -I$(srcdir)/elflib      \
			  -I/usr/include/libxml2  \
			  -I$(srcdir)/utils 		  \
			  -I$(srcdir)/commands

elfcrunch_LDADD = -lxml2 -lssl -lelflib -lutils -lcommand -ldisasm

elfcrunch_SOURCES = \
common.h \
functions.c \
functions.h \
globals.h \
main.c \
objects.c \
objects.h \
target.c \
target.h \
edisasm.h \
edisasm.c \
xmlelflib.c \
disasm_dump.c \
disasm_dump.h \
vars.c \
vars.h \
pltmap.c \
pltmap.h \
plt.c \
plt.h \
elfobjs.c \
elfobjs.h \
gendot.c \
refmap.c \
refmap.h \
mapfunc.c \
mapfunc.h \
xmlelflib.h

elfcrunch_DEPENDENCIES = $(srcdir)/elflib/libelflib.a \
								 $(srcdir)/utils/libutils.a   \
								 $(srcdir)/commands/libcommand.a
