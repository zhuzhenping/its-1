
# SUBDIRS:=$(shell ls -l . |awk '/^d/ {print $NF}')
	
SUBDIRS = test 
prj:
	@for subdir in $(SUBDIRS); \
	do \
		( cd $$subdir && make ) || exit 1; \
	done
