; this program exits with -100
(call-with-current-continuation (lambda (exit)

  (define cont1 '())

  (define foo
    (lambda (a cont2)
      (exit
        (* a
          (call/cc (lambda (cont3)  ; this call/cc returns with -1
            (set! cont1 cont3)
            (cont2 (+ 1 2))))))))

  (cont1
    (- (call/cc (lambda (cont4)  ; this call/cc returns with 3
         (foo 100 cont4)))
       4))

))
