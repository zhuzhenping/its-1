# SUBDIRS:=$(shell ls -l . |awk '/^d/ {print $NF}')

SUBDIRS = main
prj:
	@for subdir in $(SUBDIRS); \
	do \
		( cd $$subdir && make ) || exit 1; \
	done

clean:
	@for subdir in $(SUBDIRS); \
	do \
		( cd $$subdir && make clean) || exit 1; \
	done
