ACLOCAL_AMFLAGS = -I m4

# Library.
lib_LTLIBRARIES = libsnappy.la libsnappystream.la
libsnappy_la_SOURCES = snappy.cc snappy-sinksource.cc snappy-stubs-internal.cc snappy-c.cc
libsnappy_la_LDFLAGS = -version-info $(SNAPPY_LTVERSION)

libsnappystream_la_SOURCES = snappy-stream.cc
libsnappystream_la_LDFLAGS = -version-info $(SNAPPY_LTVERSION)
libsnappystream_la_LIBADD = libsnappy.la

include_HEADERS = snappy.h snappy-sinksource.h snappy-stubs-public.h snappy-c.h snappy-stream.h
noinst_HEADERS = snappy-internal.h snappy-stubs-internal.h snappy-test.h

# Unit tests and benchmarks.
snappy_unittest_CPPFLAGS = $(gflags_CFLAGS) $(GTEST_CPPFLAGS)
snappy_unittest_SOURCES = snappy_unittest.cc snappy-test.cc
snappy_unittest_LDFLAGS = $(GTEST_LDFLAGS)
snappy_unittest_LDADD = libsnappy.la $(UNITTEST_LIBS) $(gflags_LIBS) $(GTEST_LIBS)
TESTS = snappy_unittest
noinst_PROGRAMS = $(TESTS)

bin_PROGRAMS = snap
snap_SOURCES = snap.cc
snap_LDFLAGS = $(GTEST_LDFLAGS)
snap_LDADD = libsnappy.la libsnappystream.la $(gflags_LIBS)

bin_SCRIPTS = unsnap sncat

EXTRA_DIST = autogen.sh testdata/alice29.txt testdata/asyoulik.txt testdata/baddata1.snappy testdata/baddata2.snappy testdata/baddata3.snappy testdata/cp.html testdata/fields.c testdata/geo.protodata testdata/grammar.lsp testdata/house.jpg testdata/html testdata/html_x_4 testdata/kennedy.xls testdata/kppkn.gtb testdata/lcet10.txt testdata/mapreduce-osdi-1.pdf testdata/plrabn12.txt testdata/ptt5 testdata/sum testdata/urls.10K testdata/xargs.1 
dist_doc_DATA = ChangeLog COPYING INSTALL NEWS README format_description.txt

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
