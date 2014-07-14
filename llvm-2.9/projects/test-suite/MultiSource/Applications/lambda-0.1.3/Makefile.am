SUBDIRS=docs
bin_PROGRAMS = lambda
data_DATA = definitions
lambda_SOURCES=\
lambda.cc \
node.cc \
node.h \
parse.cc \
parse.h \
token_stream.cc \
token_stream.h

lambda_LDADD=-lstdc++
AM_CXXFLAGS=-Wall
AM_LDFLAGS=

EXTRA_DIST=definitions definitions_with_numbers \
fdl.txt gpl.txt

CLEANFILES=lambda

ChangeLog: FORCE
	cvs2cl.pl
FORCE:
