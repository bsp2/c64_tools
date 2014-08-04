
TARGET=    $(TARGET_BASENAME).out
TARGET_MAP=$(TARGET_BASENAME).map

HDR+= $(XDC_CFG)cfg.h

XDC_OBJ= \
	$(XDC_CFG)cfg_c.o64 \
	$(XDC_CFG)cfg.o64

OBJ+= $(XDC_OBJ)


.PHONY: bin
bin: core.out

$(XDC_CFG)cfg_c.c:	$(XDC_CFG).tcf ../tci/platform.tci ../tci/dm3730.tci ../tci/areas.tci
#	$(TCONF) -g $(XDC_CFG).tcf
	$(TCONF) $(XDC_CFG).tcf


core.out: $(OBJ) $(XDC_CFG)cfg_c.c
	echo $(OBJ)
	$(CC) $(LDFLAGS) \
		$(LIBDIR) \
		--reread_libs --rom_model \
		-o $(TARGET) \
		-m $(TARGET_MAP) \
		-l$(XDC_CFG)cfg.cmd \
		$(LIB_PRE) \
		$(OBJ) \
		$(LIB) \
		$(LIB_POST) \
		link_core.cmd 
	@ls -ld $(TARGET)
	@echo "$(TARGET) core DSP image build finished at `$(DATE)`"


CLEAN_RULES+= clean_core

clean_core:
	$(RM) $(TARGET)
	$(RM) $(TARGET_MAP)

clean_xdc:
	$(RM) $(XDC_CFG)*.h
	$(RM) $(XDC_CFG)*.c
	$(RM) $(XDC_CFG)*.h62
	$(RM) $(XDC_CFG)*.s62
	$(RM) $(XDC_CFG)*.cmd
	$(RM) $(XDC_CFG).cdb
