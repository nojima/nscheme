((lambda ()

  (define sum (lambda (first last)
    (define iter (lambda (i ret)
      (if (eq? i last)
          (+ ret last)
          (iter (+ i 1) (+ ret i)))))
    (iter first 0)))

  (sum 1 100)

))
