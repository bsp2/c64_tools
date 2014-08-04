#
# build area3 (1MB) overlay DSP image.
#
#  (also see ../tci/config.tci)
#

CFLAGS+= --mem_model:const=far
CFLAGS+= --mem_model:data=far

TARGET5=    $(TARGET_BASENAME)_area3.out
TARGET5_MAP=$(TARGET_BASENAME)_area3.map

LINKERARGS= $(LDFLAGS) \
		$(LIBDIR) \
		--reread_libs --rom_model \
		$(LIB_PRE) \
		$(OBJ) \
		$(LIB) \
		$(LIB_POST) \

.PHONY: bin
bin: $(TARGET5)


$(TARGET5): $(OBJ)
	$(CC) $(LINKERARGS) \
		-o $(TARGET5) \
		-m $(TARGET5_MAP) \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/pre.cmd \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/link_area3.cmd \
		$(C64_TOOLS_DSP_ROOT)/linker_scripts/post.cmd 
	@echo "$(TARGET5) DSP area3 overlay build finished at `$(DATE)`"

	@ls -ld $(TARGET5)


CLEAN_RULES+= clean_area3

clean_area3:
	$(RM) $(TARGET5)
	$(RM) $(TARGET5_MAP)
