
# SUBDIRS:=$(shell ls -l . |awk '/^d/ {print $NF}')
	
SUBDIRS = test common
prj:
	@for subdir in $(SUBDIRS); \
	do \
		( cd $$subdir && make ) || exit 1; \
	done
