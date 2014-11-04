((lambda ()

  (define even (lambda (n)
    (if (= n 0)
        #t
        (odd (- n 1)))))

  (define odd (lambda (n)
    (if (= n 0)
        #f
        (even (- n 1)))))

  (print (odd 1))
  (print (odd 2))
  (print (odd 3))
  (print (odd 4))
  (print (odd 5))
  (print (odd 6))
  (print (odd 7))

))
