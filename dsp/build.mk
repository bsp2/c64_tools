

.SUFFIXES: .asm .s62 .c .cpp .o64


.asm.o64:
	$(CC) $(CFLAGS) $(OPTFLAGS) $(INC) "$<" --output_file="$@"

.s62.o64:
	$(CC) $(CFLAGS) $(OPTFLAGS) $(INC) "$<" --output_file="$@"

.c.o64:
	$(CC) $(CFLAGS) $(OPTFLAGS) $(INC) "$<" --output_file="$@"

.cpp.o64:
	$(CC) $(CFLAGS) $(OPTFLAGS) $(INC) "$<" --output_file="$@"



realclean: clean
	$(RM) `find . -name \*~`

clean: $(CLEAN_RULES)
	$(RM) $(TARGET) $(OBJ)
