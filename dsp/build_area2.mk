#
# build 4 variants of an area2 (256k) overlay DSP image.
#
#  each variant uses a different base address, i.e.
#    area2_1: 0x86200000
#    area2_2: 0x86240000
#    area2_2: 0x86280000
#    area2_3: 0x862C0000
#
#  (also see ../tci/config.tci)
#

CFLAGS+= --mem_model:const=far
CFLAGS+= --mem_model:data=far

TARGET1=    $(TARGET_BASENAME)_area2_1.out
TARGET1_MAP=$(TARGET_BASENAME)_area2_1.map

TARGET2=    $(TARGET_BASENAME)_area2_2.out
TARGET2_MAP=$(TARGET_BASENAME)_area2_2.map

TARGET3=    $(TARGET_BASENAME)_area2_3.out
TARGET3_MAP=$(TARGET_BASENAME)_area2_3.map

TARGET4=    $(TARGET_BASENAME)_area2_4.out
TARGET4_MAP=$(TARGET_BASENAME)_area2_4.map

LINKERARGS= $(LDFLAGS) \
		$(LIBDIR) \
		--reread_libs --rom_model \
		$(LIB_PRE) \
		$(OBJ) \
		$(LIB) \
		$(LIB_POST) \

.PHONY: bin
bin: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)


$(TARGET1): $(OBJ)

	$(CC) $(LINKERARGS) \
		-o $(TARGET1) \
		-m $(TARGET1_MAP) \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/pre.cmd \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/link_area2_1.cmd \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/post.cmd 
	@echo "$(TARGET1) DSP area2 overlay build finished at `$(DATE)`"

	@ls -ld $(TARGET1)


$(TARGET2): $(OBJ)
	$(CC) $(LINKERARGS) \
		-o $(TARGET2) \
		-m $(TARGET2_MAP) \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/pre.cmd \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/link_area2_2.cmd \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/post.cmd 
	@echo "$(TARGET2) DSP area2 overlay build finished at `$(DATE)`"

	@ls -ld $(TARGET2)


$(TARGET3): $(OBJ)
	$(CC) $(LINKERARGS) \
		-o $(TARGET3) \
		-m $(TARGET3_MAP) \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/pre.cmd \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/link_area2_3.cmd \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/post.cmd 
	@echo "$(TARGET3) DSP area2 overlay build finished at `$(DATE)`"

	@ls -ld $(TARGET3)

$(TARGET4): $(OBJ)
	$(CC) $(LINKERARGS) \
		-o $(TARGET4) \
		-m $(TARGET4_MAP) \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/pre.cmd \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/link_area2_4.cmd \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/post.cmd 
	@echo "$(TARGET4) DSP area2 overlay build finished at `$(DATE)`"

	@ls -ld $(TARGET4)


CLEAN_RULES+= clean_area1_4

clean_area1_4:
	echo "cleaning"
	$(RM) $(TARGET1)
	$(RM) $(TARGET1_MAP)
	$(RM) $(TARGET2)
	$(RM) $(TARGET2_MAP)
	$(RM) $(TARGET3)
	$(RM) $(TARGET3_MAP)
	$(RM) $(TARGET4)
	$(RM) $(TARGET4_MAP)
