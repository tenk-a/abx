#
OPTS='-DNDEBUG -DFKS_USE_MSC_JP -DENABLE_MT_X'
c_cmpl() {
  gcc -c -o $1 -O2 -I../../src -I../../src/fks ${OPTS} $2
}
cxx_cmpl() {
  g++ -c -o $1 -O2 -I../../src -I../../src/fks ${OPTS} $2
}
rm *.o
rm abx
SRCDIR='../../src'
cxx_cmpl abx.o 		${SRCDIR}/abx.cpp
cxx_cmpl AbxConvFmt.o	${SRCDIR}/AbxConvFmt.cpp
cxx_cmpl AbxFiles.o 	${SRCDIR}/AbxFiles.cpp
cxx_cmpl AbxMsgStr.o	${SRCDIR}/AbxMsgStr.cpp
cxx_cmpl AbxMsgStrJp.o	${SRCDIR}/AbxMsgStrJp.cpp
cxx_cmpl abxmt.o 	${SRCDIR}/abxmt.cpp
c_cmpl   fks_path.o 	${SRCDIR}/fks/fks_path.c
c_cmpl   fks_io.o 	${SRCDIR}/fks/fks_io.c
c_cmpl   fks_io_mbs.o 	${SRCDIR}/fks/fks_io_mbs.c
c_cmpl   fks_mbc.o 	${SRCDIR}/fks/fks_mbc.c
c_cmpl   fks_dirent.o 	${SRCDIR}/fks/fks_dirent.c
cxx_cmpl fks_misc.o 	${SRCDIR}/fks/fks_misc.c
c_cmpl   fks_perfcnt.o 	${SRCDIR}/fks/fks_perfcnt.c
c_cmpl   fks_time.o 	${SRCDIR}/fks/fks_time.c
OBJS='abx.o AbxConvFmt.o AbxFiles.o AbxMsgStr.o abxmt.o fks_io.o fks_io_mbs.o fks_dirent.o fks_misc.o fks_path.o fks_perfcnt.o fks_time.o AbxMsgStrJp.o'
LIBS=-lpthread
g++ -o abx ${OBJS} ${LIBS}
rm *.o
