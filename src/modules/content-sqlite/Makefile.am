AM_CFLAGS = \
	$(WARNING_CFLAGS) \
	$(CODE_COVERAGE_CFLAGS)

AM_LDFLAGS = \
	$(CODE_COVERAGE_LIBS)

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src/include \
	-I$(top_builddir)/src/common/libflux \
	$(ZMQ_CFLAGS) $(SQLITE_CFLAGS) \
	$(LZ4_CFLAGS)

fluxmod_LTLIBRARIES = content-sqlite.la

content_sqlite_la_SOURCES = \
	content-sqlite.c

content_sqlite_la_LDFLAGS = $(fluxmod_ldflags) -module
content_sqlite_la_LIBADD = \
		$(top_builddir)/src/common/libcontent/libcontent.la \
		$(top_builddir)/src/common/libflux-internal.la \
		$(top_builddir)/src/common/libflux-core.la \
		$(ZMQ_LIBS) $(SQLITE_LIBS) $(LZ4_LIBS)
