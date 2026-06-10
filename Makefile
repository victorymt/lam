OBJS = constructor.o ast.o y.tab.o lex.yy.o
CFLAGS = -Wall -Wextra -g

# bison + flex 生成 .c/.h 文件
GEN_Y = y.tab.c y.tab.h y.output
GEN_L = lex.yy.c

all: lam test

lam: z.c $(OBJS)
	gcc $(CFLAGS) -o lam $(OBJS) z.c

test: test.c $(OBJS)
	gcc $(CFLAGS) -o test $(OBJS) test.c

# 统一生成：先 bison 再 flex
$(GEN_Y): lam.y
	bison -vdty lam.y

$(GEN_L): lam.l $(GEN_Y)
	flex lam.l

y.tab.o: y.tab.c ast.h constructor.h
	gcc $(CFLAGS) -c y.tab.c

lex.yy.o: lex.yy.c y.tab.h
	gcc $(CFLAGS) -c lex.yy.c

constructor.o: constructor.c constructor.h ast.h
	gcc $(CFLAGS) -c constructor.c

ast.o: ast.c ast.h
	gcc $(CFLAGS) -c ast.c

clean:
	rm -f lam test *.o y.* lex.yy.c
