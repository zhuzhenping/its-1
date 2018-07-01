# SUBDIRS:=$(shell ls -l . |awk '/^d/ {print $NF}')

SUBDIRS = common network datalib ctp account main
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
