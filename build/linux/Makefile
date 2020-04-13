#
CXX = g++
LINK = g++
CXXFLAGS2=-Wall -O2 -I../../src -I../../src/fks
SRCDIR=../../src
OBJS=abx.o AbxConvFmt.o AbxFiles.o AbxMsgStr.o AbxMsgStrJp.o abxmt.o fks_io.o fks_dirent.o fks_misc.o fks_path.o fks_perfcnt.o fks_time.o

.SUFFIXES: .cpp .c .o

.cpp.o:
    $(CXX) $(CXXFLAGS) $(CXXFLAGS2) -o $@ $<

.c.o:
    $(CXX) $(CXXFLAGS) $(CXXFLAGS2) -o $@ $<

all: abx

abx: $(OBJS)
    $(LINK) -o $@ $^

abx.o: $(SRCDIR)/abx.cpp $(SRCDIR)/AbxConvFmt.hpp $(SRCDIR)/AbxFiles.hpp $(SRCDIR)/AbxMsgStr.hpp

AbxConvFmt.o: $(SRCDIR)/AbxConvFmt.cpp

AbxFiles.o: $(SRCDIR)/AbxFiles.cpp

AbxMsgStr.o: $(SRCDIR)/AbxMsgStr.cpp

AbxMsgStrJp.o: $(SRCDIR)/AbxMsgStrJp.cpp

abxmt.o: $(SRCDIR)/abxmt.cpp

fks_io.o: $(SRCDIR)/fks/fks_io.c

fks_dirent.o: $(SRCDIR)/fks/fks_dirent.c

fks_misc.o: $(SRCDIR)/fks/fks_misc.c

fks_path.o: $(SRCDIR)/fks/fks_path.c

fks_perfcnt.o: $(SRCDIR)/fks/fks_perfcnt.c

fks_time.o: $(SRCDIR)/fks/fks_time.c

