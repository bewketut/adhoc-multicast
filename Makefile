top_srcdir      := ..
srcdir          := .
prefix          := /usr/
exec_prefix     := ${prefix}
libdir          := /usr/lib
libexecdir      := ${exec_prefix}/libexec
xtlibdir        := ${libexecdir}/xtables
xtables_CFLAGS  := 

CC              := gcc
CL		:= clang
CCLD            := ${CC}
LCFLAGS          := -g -O2
LDFLAGS         := 
regular_CFLAGS  := -D_LARGEFILE_SOURCE=1 -D_LARGE_FILES -D_FILE_OFFSET_BITS=64 	-D_REENTRANT -Wall -Waggregate-return -Wmissing-declarations 	-Wmissing-prototypes -Wredundant-decls -Wshadow -Wstrict-prototypes 	-Winline -pipe -DXTABLES_LIBDIR=\"${xtlibdir}\"
kinclude_CFLAGS :=  -I /lib/modules/`uname -r`/build/include
AM_CFLAGS      := ${regular_CFLAGS} -I${top_srcdir}/include ${xtables_CFLAGS} ${kinclude_CFLAGS}
AM_DEPFLAGS     = -Wp,-MMD,$(@D)/.$(@F).d,-MT,$@

obj-m += xt_MCAST.o #	compat_xtables.o 
all: lib 
	make -C	/lib/modules/`uname -r`/build M=`pwd`
mcmds: mcastcmds-nt3.c
	${CCLD} ${regular_CFLAGS} -o mcastcmds mcastcmds-nt3.c
	${CL} ${regular_CFLAGS} -o mcastcmdsfast mcastcmds-nt3.c
clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean
	rm -rf libxt_MCAST.so
install:
	cp *.so  $(xtlibdir) 
	cp *.ko /lib/modules/`uname -r`/extra

lib:	libxt_MCAST.so  
lib%.so: lib%.oo
	${CCLD} ${AM_LDFLAGS} -shared ${LDFLAGS} -o $@ $<;

lib%.oo: ${srcdir}/lib%.c
	${CC} ${AM_DEPFLAGS} ${AM_CFLAGS} -D_INIT=lib$*_init -DPIC -fPIC ${LCFLAGS} -o $@ -c $<;


