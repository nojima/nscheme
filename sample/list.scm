((lambda ()

  (define fold-right
    (lambda (op init seq)
      (define rec
        (lambda (xs)
          (if (null? xs)
              init
              (op (car xs) (rec (cdr xs))))))
      (rec seq)))

  (define map
    (lambda (f ls)
      (fold-right (lambda (x y) (cons (f x) y)) '() ls)))

  (define length
    (lambda (ls)
      (fold-right (lambda (_ r) (+ r 1)) 0 ls)))

  (length '(1 2 3 4 5 6 7))

  (map (lambda (x) (- 100 x)) '(10 20 30))
))
