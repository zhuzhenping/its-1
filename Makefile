
SUBDIRS=sln

$(SUBDIRS):ECHO
  make -C $@