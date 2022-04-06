#!/usr/bin/env make

CC:= gcc
TARGET:= etapa5
LEX_IN:= scanner.l
LEX_OUT:= lex.yy.c
YACC_IN:= parser.y
YACC_OUT:= y.tab.c
SOURCE:= $(wildcard *.c) $(LEX_OUT) $(YACC_OUT)
OBJ:= $(SOURCE:.c=.o)
HEADERS:= lex.yy.h y.tab.h

.PHONY: all clean

all: release

release: CFLAGS:= -Wall -O2 -DNDEBUG
release: $(TARGET)

debug: CFLAGS:= -Wall -O0 -ggdb3
debug: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(LEX_OUT) lex.yy.h: $(LEX_IN)
	lex -d --header-file=$(@:.c=.h) $<

$(YACC_OUT) y.tab.h: $(YACC_IN)
	yacc -vtd $<

clean:
	rm -f $(OBJ) $(YACC_OUT) $(LEX_OUT) $(HEADERS) y.output

