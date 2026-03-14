#lang racket

(define Closure
  (lambda (arg body env)
    (list arg body env)))

(define closure-x
  (lambda (closure)
    (car closure)))

(define closure-body
  (lambda (closure)
    (cadr closure)))

(define closure-env
  (lambda (closure)
    (caddr closure)))

(define entend-env
  (lambda (var val env)
    (cons
     (cons var val)
     env)))

;; ((v . 1) . ())

(define lookup
  (lambda (x env)
    (cond ((empty? x) '())
	  ((eq? (car (car env)) x) (cdr (car env)))
	  (#t
	   (lookup x (cdr env))))))

(define interpreter
  (lambda (exp env)
    (match exp
      [(? number? x) x]
      [(? symbol? x) (lookup x env)]
      [`(lambda (,x) ,body) (Closure x body env)]
      [`(,op ,e1 ,e2)
       (let ((v1 (interpreter e1 env))
	     (v2 (interpreter e2 env)))
	 (match op
	   ['+ (+ v1 v2)]
	   ['- (- v1 v2)]
	   ['* (* v1 v2)]
	   ['/ (/ v1 v2)]))]
      [`(,fun-p ,e)
       (let ((fun (interpreter fun-p env)))
	 (let ((arg (closure-x fun))
	       (body (closure-body fun))
	       (env (closure-env fun)))
	   (let ((new-env (entend-env arg (interpreter e env) env)))
	     (interpreter body new-env))))])))

(interpreter '(+ 1 2) '())
(interpreter '((lambda (x) (+ x 1)) 3) '())
