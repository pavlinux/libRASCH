include ../../Makefile.incl

all	:: Makelib job
	$(Q)make -f Makelib
	$(Q)cp librasch_sci.so ./sci_files
	$(Q)cp loader.sce ./sci_files/loader.sce

Makelib : ../matlab/ra_matlab.c ../matlab/ra_matlab.h builder.sce
	$(Q)cp ../matlab/ra_matlab.c ./ra_scilab.c
	$(Q)cp ../matlab/ra_matlab.h ./ra_matlab.h
	$(Q)$(SCILAB_PATH)/util/scidem $(SCILAB_PATH) builder.sce builder.dia

job	: 
	$(Q)make -f Makelib

tests	: all
	make -f Makelib tests 

clean	::
	@echo $(INDENT)clean Scilab interface
	$(Q)$(RM) -f -r *.o *.lo *.la *.so *.dll *.sl *.a *.dia so_locations
	$(Q)$(RM) -f -r .libs 
	$(Q)$(RM) -f Makelib 

distclean:: clean 
	$(Q)$(RM) -f *.dia  *_gateway.c 

message:
	@echo "------------------------------------------";
	@echo "Start Scilab in sci_files directory";
	@echo "At Scilab prompt, enter:";
	@echo "-->exec loader.sce";
	@echo "------------------------------------------";

