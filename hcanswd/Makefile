.PHONY: libhcan++ libhcandata

include ../ARCH.inc

OBJ = main.o

LD_FLAGS = -Wl,--export-dynamic \
			-L ../libhcandata -lhcandata \
			-L ../libhcan++ -lhcan++ \
			-lboost_program_options \
			-lboost_regex \
			`pkg-config libxml++-2.6 --libs`

ifeq ($(ARCH),i386)	
	CXX = g++
else	
	CXX = arm-linux-gnu-g++	
endif

INC = -I. -I.. -I../libhcan++ -I../libhcandata -I../include
CXXFLAGS = -Wall -Werror -O2 $(INC) -g

all: libhcan++ libhcandata hcanswd

libhcan++:
	cd ../libhcan++ && $(MAKE)

libhcandata:
	cd ../libhcandata && $(MAKE)

hcanswd: $(OBJ) libhcan++ libhcandata
	$(CXX) -o $@ $(OBJ) $(LD_FLAGS) 
	test -d ./binary_$(ARCH) || mkdir binary_$(ARCH)
	mv $@ binary_$(ARCH)

clean:
	rm -f *.o
	cd binary_$(ARCH); sudo rm -f hcanswd
	#sudo rm -f -R binary_$(ARCH)

ctags:
	ctags *.cc *.h ../libhcan++/*.cc ../libhcan++/*.h ../libhcandata/*.cc ../libhcandata/*.h

install:
	cd binary_$(ARCH); install hcanswd /usr/sbin

dep:
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

include .depend

