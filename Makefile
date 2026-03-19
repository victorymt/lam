lam: z.c constructor.c ast.c lam.y lam.l
	bison -vdty lam.y
	flex lam.l
	gcc -o lam constructor.c ast.c y.tab.c lex.yy.c z.c

clean:
	rm lam
