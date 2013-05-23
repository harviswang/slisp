;;
;; sum from i=a to b
;; f(i) = f(a) + ... + f(b)
;;

(defun sum(term a next b)
  (if (> a b)
      0
      (+ (term a) (sum term (next a) next b))))


;; nice 
(defun cube(x)
  (* x x x))

;;
;; Simple integral:
;; S from a, to b of f = [f(a+ dx/2) + f(a + dx + dx/2) +
;;                        f(a + 2dx + dx/2) + ... ]dx
;;

(defun integral(f a b dx)
  (labels ((add-dx (x) (+ x dx)))
    (* (sum f (+ a (/ dx 2.0)) add-dx b)
       dx)))
;;
;; Simpson Integration rule:
;; h/3[y_0 + 4y_1 + 2y_2 + 4y_3 + 2y_4 + ... + 2y_n-2 + 4y_n-1 + y_n ]
;; (/ (* h (+ y_0 (* 4 y_1) (* 2 y_2) ...)) 3)
;;
(defun simpson(f a b n)
  (let ((h (/ (- b a) n))) ;; h = (b-a)/n;
    (labels ((next-t (x) (+ x (* 2.0 h)))) ;; next-t compute next x
      (* (+ (f a) (f b) (* 2.0 (sum f (+ a (* 2.0 h)) next-t b))
	    (* 4.0 (sum f (+ a h) next-t b)))
	 (/ h 3.0)))))