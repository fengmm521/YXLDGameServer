
descend: $(DIRS)
	@for i in $(DIRS); \
	do \
		make -C $$i release; \
		if [ $$? -ne 0 ] ; then exit 1 ; fi \
	done
