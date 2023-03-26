# We have both EPOXY_CORE_AVR and EPOXY_CORE_ESP*266 tests here, so we must
# recompile the object files when switching between the two. The easiest way to
# do that, while preserving the *.out file for each test to allow the
# 'runtests' target to suceed, is to perform a 'make clean' before compiling
# each test. It's a bit inefficient but there are only 3 tests in this
# directory.
tests:
	set -e; \
	for i in *Test/Makefile; do \
		echo '==== Making:' $$(dirname $$i); \
		$(MAKE) -C $$(dirname $$i) -j4 clean; \
		$(MAKE) -C $$(dirname $$i) -j4; \
	done

runtests:
	set -e; \
	for i in *Test/Makefile; do \
		echo '==== Running:' $$(dirname $$i); \
		$(MAKE) -C $$(dirname $$i) run; \
	done

clean:
	set -e; \
	for i in *Test/Makefile; do \
		echo '==== Cleaning:' $$(dirname $$i); \
		$(MAKE) -C $$(dirname $$i) clean; \
	done
