
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; These are some really basic tests of SIOD that I wrote for the LLVM
;; testsuite... later we should replace these with code that actually does
;; something, I guess. -Brian

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Some standard-Scheme compatibility routines
(define else #t)
(define (display x) 
	(cond ((number? x) (puts (number->string x)))
		  ((string? x) (puts x))
		  ((symbol? x) (puts x))
		  (else (err "can't display errobj" x))))
(define (newline) (puts "\n"))
(define (cadddr x) (caddr (cdr x)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Property lists like in LISP.
(define *properties* '())
(define (get table key)
	(let ((result (assoc (cons key table) *properties*)))
		(if result
			(cadr result)
			#f)))
(define (put table key value)
     (let* ((real-key (cons key table))
	        (result (assoc real-key *properties*)))
		(if result
			(set-car! (cdr result) value)
			(set! *properties* (cons (list real-key value) *properties*)))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; simple test code for proplist code, presented above
(define (simple-proplist-test)
	(put 'answer 'value 42)
	(display "the value of (get 'answer 'value) is ")
	(display (get 'answer 'value))
	(newline)
	(put 'answer 'value 'xyzzy)
	(display "the value of (get 'answer 'value) is ")
	(display (get 'answer 'value))
	(newline))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; recursively count down from COUNTER, printing out the numbers
(define (print-countdown counter)
	(if (equal? counter 0)
		#t
		(begin
			(display counter) 
			(newline)
            (print-countdown (- counter 1)))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; print out whether VALUE is positive, zero, or negative
(define (print-signum value)
    (display value)
    (display " is ")
	(display (cond ((> value 0) "positive")
	               ((< value 0) "negative")
	               (else "zero")))
    (newline))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Fibonacci test

(define (fib n)
  (if (< n 2)
     n
     (+ (fib (- n 1)) (fib (- n 2)))))

(define (fib-test)
  (display "the 33rd Fibonacci number is ")
  (display (fib 33))
  (newline))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Run some simple tests
(print-countdown 10)
(print-signum -42)
(print-signum 0)
(print-signum 42)
(simple-proplist-test)
(fib-test)
