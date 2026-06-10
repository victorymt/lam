;; Lambda 演算解释器 —— Chez Scheme 实现
;;
;; 运行： chez --script r.ss

(define Closure
  (lambda (arg body env)
    (list arg body env)))

(define closure-x     (lambda (c) (car c)))
(define closure-body  (lambda (c) (cadr c)))
(define closure-env   (lambda (c) (caddr c)))

;; extend-env :: var -> val -> env -> env
(define extend-env
  (lambda (var val env)
    (cons (cons var val) env)))

;; lookup :: symbol -> env -> value
(define lookup
  (lambda (x env)
    (cond
     ((null? env) (error 'lookup "unknown symbol: ~s" x))
     ((eq? (caar env) x) (cdar env))
     (else (lookup x (cdr env))))))

;; extend-lst-env :: ((var val) ...) -> env -> env
(define extend-lst-env
  (lambda (lst env)
    (if (null? lst)
        env
        (extend-lst-env (cdr lst)
                        (extend-env (caar lst) (cadar lst) env)))))

;; pretty-print :: exp -> void
(define pretty-print
  (lambda (exp)
    (define indent
      (lambda (n) (do ((i 0 (+ i 1))) ((= i n)) (display "  "))))
    (let walk ((exp exp) (depth 0))
      (cond
       ((number? exp)
        (indent depth) (display exp) (newline))
       ((symbol? exp)
        (indent depth) (display exp) (newline))
       ((and (list? exp) (= (length exp) 3)
             (eq? (car exp) 'lambda)
             (list? (cadr exp)) (= (length (cadr exp)) 1))
        (indent depth) (display "(lambda (")
        (display (car (cadr exp))) (display ")") (newline)
        (walk (caddr exp) (+ depth 1))
        (indent depth) (display ")") (newline))
       ((and (list? exp) (= (length exp) 3) (eq? (car exp) 'let))
        (indent depth) (display "(let ") (display (cadr exp)) (newline)
        (walk (caddr exp) (+ depth 1))
        (indent depth) (display ")") (newline))
       ((and (list? exp) (= (length exp) 3)
             (memq (car exp) '(+ - * /)))
        (indent depth) (display "(") (display (car exp)) (newline)
        (walk (cadr exp) (+ depth 1))
        (walk (caddr exp) (+ depth 1))
        (indent depth) (display ")") (newline))
       ((and (list? exp) (= (length exp) 2))
        (indent depth) (display "(") (newline)
        (walk (car exp) (+ depth 1))
        (walk (cadr exp) (+ depth 1))
        (indent depth) (display ")") (newline))
       (else
        (indent depth) (display "<unknown: ") (display exp)
        (display ">") (newline))))))

;; interpreter :: exp -> env -> value
(define interpreter
  (lambda (exp env)
    (cond
     ((number? exp) exp)
     ((symbol? exp) (lookup exp env))
     ((and (list? exp) (= (length exp) 3)
           (eq? (car exp) 'lambda)
           (list? (cadr exp)) (= (length (cadr exp)) 1))
      (Closure (car (cadr exp)) (caddr exp) env))
     ((and (list? exp) (= (length exp) 3) (eq? (car exp) 'let))
      (let ((new-env (extend-lst-env (cadr exp) env)))
        (interpreter (caddr exp) new-env)))
     ((and (list? exp) (= (length exp) 3)
           (memq (car exp) '(+ - * /)))
      (let ((v1 (interpreter (cadr exp) env))
            (v2 (interpreter (caddr exp) env)))
        (case (car exp)
          ((+) (+ v1 v2))
          ((-) (- v1 v2))
          ((*) (* v1 v2))
          ((/) (quotient v1 v2)))))
     ((and (list? exp) (= (length exp) 2))
      (let ((fun (interpreter (car exp) env)))
        (let ((arg (closure-x fun))
              (body (closure-body fun))
              (env (closure-env fun)))
          (let ((new-env (extend-env arg
                                     (interpreter (cadr exp) env)
                                     env)))
            (interpreter body new-env)))))
     (else (error 'interpreter "unknown expression: ~s" exp)))))

;; ── 测试 ──

(define pass 0)
(define fail 0)

(define-syntax test
  (syntax-rules ()
    ((_ description actual expected)
     (let ((a actual) (e expected))
       (if (equal? a e)
           (begin (set! pass (+ pass 1))
                  (display "  ") (display description)
                  (display (make-string (- 50 (string-length description)) #\space))
                  (display "OK") (newline))
           (begin (set! fail (+ fail 1))
                  (display "  ") (display description)
                  (display (make-string (- 50 (string-length description)) #\space))
                  (display "FAIL  got ") (display a)
                  (display ", expected ") (display e) (newline)))))))

(newline)
(display "── 测试 ──") (newline) (newline)

(test "数值字面量"          (interpreter 42 '()) 42)
(test "加法 (+ 1 2)"       (interpreter '(+ 1 2) '()) 3)
(test "减法 (- 5 3)"       (interpreter '(- 5 3) '()) 2)
(test "乘法 (* 3 4)"       (interpreter '(* 3 4) '()) 12)
(test "除法 (/ 10 3)"      (interpreter '(/ 10 3) '()) 3)
(test "((lambda (x) (+ x 1)) 2)"   (interpreter '((lambda (x) (+ x 1)) 2) '()) 3)
(test "((lambda (x) (+ x 1)) 3)"   (interpreter '((lambda (x) (+ x 1)) 3) '()) 4)
(test "((lambda (x) (* x x)) 7)"   (interpreter '((lambda (x) (* x x)) 7) '()) 49)
(test "嵌套闭包 1 2" (interpreter '(((lambda (x) (lambda (y) (+ x y))) 1) 2) '()) 3)
(test "嵌套闭包 5 6" (interpreter '(((lambda (x) (lambda (y) (+ x y))) 5) 6) '()) 11)
(test "恒等 ((lambda (x) x) 99)"   (interpreter '((lambda (x) x) 99) '()) 99)
(test "let 单绑定"         (interpreter '(let ((x 1)) 1) '()) 1)
(test "let 捕获变量"       (interpreter '(let ((x 1)) ((lambda (y) (+ y x)) 1)) '()) 2)
(test "lambda 返回 closure"
      (list? (interpreter '(lambda (x) x) '()))
      #t)
(test "let 多绑定"         (interpreter '(let ((x 1) (y 2)) (+ x y)) '()) 3)
(test "let 三绑定"         (interpreter '(let ((a 1) (b 2) (c 3)) (+ a (+ b c))) '()) 6)

(newline)
(display "── pretty-print 示例 ──") (newline) (newline)
(pretty-print '(((lambda (x) (lambda (y) (+ x y))) 1) 2))
(newline)
(display "结果: ")
(display (interpreter '(((lambda (x) (lambda (y) (+ x y))) 1) 2) '()))
(newline) (newline)
(display "合计: ") (display pass) (display " 通过, ") (display fail) (display " 失败")
(newline)

;; 非零退出码表示失败
(exit fail)
