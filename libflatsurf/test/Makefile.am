check_PROGRAMS = length_along_triangulation vector_longlong interval_exchange_transformation delaunay saddle_connections vector_exactreal saddle_connections_benchmark cereal permutation flat_triangulation_combinatorial

TESTS = $(check_PROGRAMS)

saddle_connections_SOURCES = saddle_connections.test.cc main.hpp surfaces.hpp
saddle_connections_benchmark_SOURCES = saddle_connections.benchmark.cc main.hpp surfaces.hpp
vector_exactreal_SOURCES = vector_exactreal.test.cc main.hpp
delaunay_SOURCES = delaunay.test.cc main.hpp
interval_exchange_transformation_SOURCES = interval_exchange_transformation.test.cc main.hpp surfaces.hpp
vector_longlong_SOURCES = vector_longlong.test.cc main.hpp
length_along_triangulation_SOURCES = length_along_triangulation.test.cc main.hpp surfaces.hpp
cereal_SOURCES = cereal.test.cc main.hpp surfaces.hpp
permutation_SOURCES = permutation.test.cc main.hpp
flat_triangulation_combinatorial_SOURCES = flat_triangulation_combinatorial.test.cc main.hpp

# We vendor the header-only library Cereal (serialization with C++ to be able
# to run the tests even when cereal is not installed.
# Cereal is basically not packaged in any distribution since it is a
# header-only library that people always vendor into their source tree.
EXTRA_DIST = external/cereal/include

@VALGRIND_CHECK_RULES@

AM_CPPFLAGS = -I $(srcdir)/../src -I $(builddir)/../src -isystem $(srcdir)/external/cereal/include
AM_LDFLAGS = $(builddir)/../src/libflatsurf.la -lgtest -lbenchmark -lrt -lpthread
# we use gmpxx & flint through exact-real's arb/arf wrappers; since we include
# gmpxx.h, we need to link in gmp since it depends on it
AM_LDFLAGS += -lgmpxx -lgmp -lflint

# Run tests serially so we can see the output without having to check the log file explicitly.
# AUTOMAKE_OPTIONS = serial-tests
