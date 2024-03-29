#!/usr/bin/env make

CC:= gcc
TARGET:= etapa6
LEX_IN:= scanner.l
LEX_OUT:= lex.yy.c
YACC_IN:= parser.y
YACC_OUT:= y.tab.c
SOURCE:= $(wildcard *.c) $(LEX_OUT) $(YACC_OUT)
OBJ:= $(SOURCE:.c=.o)
DEPFILES:= $(SOURCE:.c=.d)
DEPFLAGS= -MT $@ -MMD -MP -MF $*.d
TESTSFOLDER:= tests
TESTS:= $(wildcard $(TESTSFOLDER)/*.txt)
TESTSASM:= $(TESTS:.txt=.s)
TESTSEXE:= $(basename $(TESTSASM))

.PHONY: all clean

all: release

release: CFLAGS:= -Wall -O2 -DNDEBUG
release: $(TARGET)

debug: CFLAGS:= -Wall -O0 -ggdb3
debug: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
%.o: %.c %.d
	$(CC) -c $< -o $@ $(DEPFLAGS) $(CFLAGS)

$(LEX_OUT) lex.yy.h: $(LEX_IN)
	lex -d --header-file=$(@:.c=.h) $<

$(YACC_OUT) y.tab.h: $(YACC_IN)
	yacc -vtd $<

$(DEPFILES):
include $(wildcard $(DEPFILES))

clean:
	rm -f $(OBJ) $(YACC_OUT) $(LEX_OUT) $(DEPFILES) y.output $(TESTSASM) $(TESTSEXE)

test: $(TESTSEXE)

$(TESTSFOLDER)/%: $(TESTSFOLDER)/%.s
	$(CC) -o $@ $< -Wa,-gdwarf-2
	@echo "Executing $@:"
	-@./$@
	@echo

.SECONDARY: $(TESTSASM) $(TESTSEXE)

$(TESTSFOLDER)/%.s: $(TESTSFOLDER)/%.txt debug
	@echo
	./$(TARGET) $< $@