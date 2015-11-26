

include $(top_srcdir)/Makefile.common

EXTRA_DIST = \
	example/aadvark.cc 		\
	example/aadvark.h		\
	example/Doxyfile		\
	example/xml/aadvark_8cc.xml	\
	example/xml/aadvark_8h.xml	\
	example/xml/classAadvark.xml	\
	example/xml/combine.xslt	\
	example/xml/compound.xsd	\
	example/xml/index.xml		\
	example/xml/index.xsd

if PYTHON
utilspythondir = $(grpythondir)/doxyxml

TESTS = 				\
	run_tests

nobase_utilspython_PYTHON = \
	__init__.py \
	base.py \
	doxyindex.py \
	text.py \
	generated/__init__.py \
	generated/index.py \
	generated/indexsuper.py \
	generated/compound.py \
	generated/compoundsuper.py
endif
