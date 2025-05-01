CC = gcc
CFLAGS = -g -ansi -Wall -pedantic

all: assembler

assembler: assembler.o coding_funcs.o file_funcs.o firstpass_input_check.o firstpassfuncs.o macrolist.o preassembler.o secondpass_errors.o symblist.o tables_and_arrays.o wordlist.o secondpassfuncs.o
	$(CC) $(CFLAGS) -o assembler assembler.o coding_funcs.o file_funcs.o firstpass_input_check.o firstpassfuncs.o macrolist.o preassembler.o secondpass_errors.o symblist.o tables_and_arrays.o wordlist.o secondpassfuncs.o

assembler.o: assembler.c assembler.h
	$(CC) $(CFLAGS) -c assembler.c

secondpassfuncs.o: secondpassfuncs.c secondpassfuncs.h
	$(CC) $(CFLAGS) -c secondpassfuncs.c

coding_funcs.o: coding_funcs.c coding_funcs.h
	$(CC) $(CFLAGS) -c coding_funcs.c

file_funcs.o: file_funcs.c file_funcs.h
	$(CC) $(CFLAGS) -c file_funcs.c

firstpass_input_check.o: firstpass_input_check.c firstpass_input_check.h
	$(CC) $(CFLAGS) -c firstpass_input_check.c

firstpassfuncs.o: firstpassfuncs.c firstpassfuncs.h
	$(CC) $(CFLAGS) -c firstpassfuncs.c

macrolist.o: macrolist.c macrolist.h
	$(CC) $(CFLAGS) -c macrolist.c

preassembler.o: preassembler.c preassembler.h
	$(CC) $(CFLAGS) -c preassembler.c

secondpass_errors.o: secondpass_errors.c secondpass_errors.h
	$(CC) $(CFLAGS) -c secondpass_errors.c

symblist.o: symblist.c symblist.h
	$(CC) $(CFLAGS) -c symblist.c

tables_and_arrays.o: tables_and_arrays.c tables_and_arrays.h
	$(CC) $(CFLAGS) -c tables_and_arrays.c

wordlist.o: wordlist.c wordlist.h
	$(CC) $(CFLAGS) -c wordlist.c

clean:
	rm -f assembler *.o
