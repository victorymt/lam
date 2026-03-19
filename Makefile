lam: z.c constructor.c ast.c
	gcc -o lam z.c constructor.c ast.c

clean:
	rm lam
