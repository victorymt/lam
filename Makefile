OBJS = constructor.o ast.o y.tab.o lex.yy.o
CFLAGS = -Wall -Wextra -g -Isrc -I.

# bison + flex 生成到当前目录（根目录）
GEN_Y = y.tab.c y.tab.h y.output
GEN_L = lex.yy.c

all: lam test

lam: src/z.c $(OBJS)
	gcc $(CFLAGS) -o lam $(OBJS) src/z.c

test: src/test.c $(OBJS)
	gcc $(CFLAGS) -o test $(OBJS) src/test.c

# bison / flex 输入在 src/，输出到当前目录
$(GEN_Y): src/lam.y
	bison -vdty src/lam.y

$(GEN_L): src/lam.l $(GEN_Y)
	flex src/lam.l

y.tab.o: y.tab.c src/ast.h src/constructor.h
	gcc $(CFLAGS) -c y.tab.c

lex.yy.o: lex.yy.c y.tab.h
	gcc $(CFLAGS) -c lex.yy.c

constructor.o: src/constructor.c src/constructor.h src/ast.h
	gcc $(CFLAGS) -c src/constructor.c

ast.o: src/ast.c src/ast.h
	gcc $(CFLAGS) -c src/ast.c

clean:
	rm -f lam test *.o y.* lex.yy.c
