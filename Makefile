all:
	make -C sub
	GENDEV=/opt/gendev make -f /opt/gendev/sgdk/mkfiles/makefile.gen clean all
dirs:
	mkdir -p out
	mkdir -p sub/out
