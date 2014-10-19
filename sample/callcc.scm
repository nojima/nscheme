((lambda ()

  (+ 5
    (call-with-current-continuation
      (lambda (k)
        (+ 100 (k (+ 20 30))))))

))
