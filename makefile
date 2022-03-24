#!/usr/bin/env make

CC:= gcc
CFLAGS:= -Wall -O0 -ggdb3
TARGET:= etapa4
LEX_IN:= scanner.l
LEX_OUT:= lex.yy.c
YACC_IN:= parser.y
YACC_OUT:= y.tab.c
SOURCE:= main.c symbol_table.c syntax_tree.c $(LEX_OUT) $(YACC_OUT)
OBJ:= $(SOURCE:.c=.o)
HEADERS:= lex.yy.h y.tab.h

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

#$(OBJ): %.o: %.c:
#	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c $(HEADERS)
symbol_table.o: symbol_table.c $(HEADERS)
syntax_tree.o: syntax_tree.c $(HEADERS)
lex.yy.o: lex.yy.c $(HEADERS)
y.tab.o: y.tab.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(LEX_OUT) lex.yy.h: $(LEX_IN)
	lex -d --header-file=$(@:.c=.h) $<

$(YACC_OUT) y.tab.h: $(YACC_IN)
	yacc -vtd $<

.PHONY: clean
clean:
	rm $(OBJ) $(YACC_OUT) $(LEX_OUT) $(HEADERS) y.output

