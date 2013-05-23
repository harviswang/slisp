/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * HLG 
 */


/* This is a just an array pointer, that contains some basic lisp functions
 * It would be load using mmap at startup, unless -b flag is specified 
 * Your are able to put any function you want, just be sure that the function 
 * works :). You could also obey the format:
 * "(defun fact(n)
 *     (cond ((= n 0) 1)
 *      (t
 *       (* (fact (- n 1)) n))))\n"
 */

char *s[] = {
	"(defun first (x) (car x))\n",
	"(defun second (x) (car (cdr x)))\n",
	"(defun third (x) (car (cdr (cdr x))))\n",
	"(defun fourth (x) (nth 3 x))\n",
	"(defun fifth (x) (nth 4 x))\n",
	"(defun sixth (x) (nth 5 x))\n",
	"(defun seventh (x) (nth 6 x))\n",
	"(defun eighth (x) (nth 7 x))\n",
	"(defun ninth (x) (nth 8 x))\n",
	"(defun tenth (x) (nth 9 x))\n",
	
	"(defun caar (x) (car (car x)))\n",
	"(defun cadr (x) (car (cdr x)))\n",
	"(defun cdar (x) (cdr (car x)))\n",
	"(defun cddr (x) (cdr (cdr x)))\n",
	"(defun caaar (x) (car (car (car x))))\n",
	"(defun caadr (x) (car (car (cdr x))))\n",
	"(defun cadar (x) (car (cdr (car x))))\n",
	"(defun caddr (x) (car (cdr (cdr x))))\n",
	"(defun cdaar (x) (cdr (car (car x))))\n",
	"(defun cdadr (x) (cdr (car (cdr x))))\n",
	"(defun cddar (x) (cdr (cdr (car x))))\n",
	"(defun cdddr (x) (cdr (cdr (cdr x))))\n",
	"(defun caaaar (x) (car (car (car (car x)))))\n",
	"(defun caaadr (x) (car (car (car (cdr x)))))\n",
	"(defun caadar (x) (car (car (cdr (car x)))))\n",
	"(defun caaddr (x) (car (car (cdr (cdr x)))))\n",
	"(defun cadaar (x) (car (cdr (car (car x)))))\n",
	"(defun cadadr (x) (car (cdr (car (cdr x)))))\n",
	"(defun caddar (x) (car (cdr (cdr (car x)))))\n",
	"(defun cadddr (x) (car (cdr (cdr (cdr x)))))\n",
	"(defun cdaaar (x) (cdr (car (car (car x)))))\n",
	"(defun cdaadr (x) (cdr (car (car (cdr x)))))\n",
	"(defun cdadar (x) (cdr (car (cdr (car x)))))\n",
	"(defun cdaddr (x) (cdr (car (cdr (cdr x)))))\n",
	"(defun cddaar (x) (cdr (cdr (car (car x)))))\n",
	"(defun cddadr (x) (cdr (cdr (car (cdr x)))))\n",
	"(defun cdddar (x) (cdr (cdr (cdr (car x)))))\n",
	"(defun cddddr (x) (cdr (cdr (cdr (cdr x)))))\n",
	
	"(defun fact-help(n ac)\n",
	"  (cond ((< n 2) ac) (t (fact-help (- n 1) (* n ac)))))\n",
	
	"(defun factorial(n)\n",
	"\"returns the factorial of a number n*n-1*n-2...*1\"\n",
	"  (fact-help n 1))\n",	

	"(defun last* (l)\n",
	"\"returns the last element of a list\"\n",
	"  (cond ((eq (cdr l) nil) (car l))\n",
	"	(t (last* (cdr l)))))\n",

	"(defun assert (exp)\n",
	"\"asserts that expression is true\"\n",
	"  (or (eval exp)\n",
	"      (princ \"assertion failed: \" exp \" (:= \" (eval exp) \")\n\")))\n",
	
	"(defun asserteq (exp1 exp2)\n",
	"\"asserts that 2 expressions are evaluaded to the same value (i.e: equal)\"\n",
	"  (or (eq (eval exp1) (eval exp2))\n",
	"      (princ \"assertion failed: \" exp1 \" (:= \" (eval exp1) \") == \"\n",
	"	     exp2 \" (:= \" (eval exp2) \")\n\")))\n",
	
	"(defun assertn (exp)\n",
	"\"asserts that the negation of an expression is true\"\n",
	"  (assert (list 'not exp)))\n",
	
	"(defun nth (n l)\n",
	"\"returns the Nth element of list L (N counts from zero).\"\n",
	"  (cond ((atom l) l)\n",
	"	((eq n 0) (car l))\n",
	"	(t (nth (- n 1) (cdr l)))))\n",
	
	
	"(defun cond* (x)\n",
	"\"A reimplementation of `cond' using a list.\"\n",
	"  (cond ((set 'cond-r (eval (caar x)))\n",
	"	 (cond ((cdar x) (progn* (cdar x)))\n",
	"	       (t cond-r)))\n",
	"	((cdr x) (cond* (cdr x)))\n",
	"	(t nil)))\n",
	
	"(defun butlast(l)\n",
	"\"Returns the list but the last element.\"\n",
	"  (cond ((eq (cdr l) nil) nil)\n",
	"        (t (cons (car l) (butlast (cdr l))))))\n",
	
	"(defun incr (x)\n",
	"	(+ x 1))\n",
	
	"(defun decr (x)\n",
	"	(- x 1))\n",
	
	"(defun member (x a)\n",
	"\"Returns t if x is a member of a\"\n",
	"    (cond ((null a) nil)\n",
	"        ((equal (car a) x) t)\n",
	"        (t (member x (cdr a)))))\n",
	
	"(defun union (a b)\n",
	"\"Returns the  elements of a and b (union set)\"\n",
	"    (cond ((null a) b)\n",
	"        ((member (car a) b) (union (cdr a) b))\n",
	"        (t (cons (car a) (union (cdr a) b)))))\n",

	"(defun get-names(x)\n",
	"\"argument must be a assoc list ((n_0 v_0) (n_1 v_1) ...)\nReturns all n_i\"\n",
	"    (cond ((eq x nil) nil)",
	"           (t (cons (caar x) (get-names (cdr x))))))\n",	

	"(defun get-values(x)\n",
	"\"argument must be a assoc list ((n_0 v_0) (n_1 v_1) ...)\nReturns all v_i\"\n",
	"    (cond ((eq x nil) nil)",
	"           (t (cons (cadar x) (get-values (cdr x))))))\n",	

	"(defun unpair(x)\n",
	"\"Unasociate an association list\"\n",
	"    (list (get-names x) (get-values x)))\n",

	"(defun intersection (a b)\n",
	"\"Returns the common  elements between a and b (intersection set)\"\n",
	"    (cond ((null a) nil)\n",
	"        ((member (car a) b)\n",
	"            (cons (car a) (intersection (cdr a) b)))\n",
	"        (t (intersection (cdr a) b))))\n",
	
	"(defun progn* (x)\n",
	"\"A reimplementation of `progn' using a list.\"\n",
	"    (cond ((cdr x) (eval (car x)) (progn* (cdr x)))\n",
	"	  (t (eval (car x)))))\n",
	
	"(defun subst (new old l)\n",
	"\"Substitutes occurrences of OLD with NEW in L.\"\n",
	"  (cond ((atom l) (cond ((eq l old) new) (t l)))\n",
	"	(t (cons (subst new old (car l))\n",
	"		 (subst new old (cdr l))))))\n",

	"(defun subst* (as l)\n",
	"\"First arg is an assoc list.((v_0 n_0) .. (v_n n_n)).\n subst every v_i in l for n_i values\n\"\n",
	"   (cond ((eq as nil) l)\n",
	"         (t (subst* (cdr as)\n",
	"                (subst (car (car as)) (car (cdr (car as)))\n",
	"          l)))))\n",
	
	"(defun equal* (x y)\n",
	"\"Compare two s-expressions for equality.\"\n",
	"  (or (and (atom x) (atom y) (eq x y))\n",
	"      (and (not (atom x))\n",
	"	   (not (atom y))\n",
	"	   (equal (car x) (car y))\n",
	"	   (equal (cdr x) (cdr y)))))\n",
	
	"(defun eval-by-subst(as function)\n",
	"\"eval one function by substitution\n\"\n",
	"(eval (subst* as (car (cdr (cdr function))))))\n",
	
	"(defun append (x y)\n",
	"\"Create a new list containing the elements of x and y.\"\n",
	"  (cond ((null x) y)\n",
	"	(t (cons (car x) (append (cdr x) y)))))\n",
	
	"(defun among (x y)\n",
	"\"McCarthy's `among' function (returns t or nil).\"\n",
	"  (and (not (null y))\n",
	"       (or (equal x (car y)) (among x (cdr y)))))\n",
	
	"(defun pop (a) \"Take away the car of a\" (setq a (cdr a)))\n",
	
	"(defun push (n a) \"Put n at car of a\" (setq a (cons n a)))\n",
	
	"(defun assoc (x y)\n",
	"\"Association list selector function.\ny is a list of the form ((u1 v1) ... (uN vN))\nx is one of the u's (an atom)\n here we return the pair (u v) as in most modern Lisps\"\n",
	"  (cond ((null y) nil)\n",
	"	((eq (caar y) x) (car y))\n",
	"		(t (assoc x (cdr y)))))\n",

	"(defun sign(x)\n",
	"\"If x<0 1 x=0 0 x>0 1\"\n",
	"  (cond ((< x 0) -1)\n",
	"	 ((> x 0) 1)\n",
	"	 (t 0)))\n",
	
	"(defun rplaca (x y)\n",
	"\"Replace car of x by y\"\n",
	"  (cons y (cdr x)))\n",
	
	"(defun rplacd (x y)\n",
	"\"Replace cdr of x by y\"\n",
	"  (cons (car x) y))\n",
	
	"(defun alt(x)\n",
	"  (cond ((or (null x)\n",
	"	      (null (cdr x))) x)\n"
	"	 (t (cons (car x) (alt (cddr x))))))\n",

	"(defun assoc* (x y)\n",
	"\"McCarthy assoc function.would return vN instead of (uN vN)\"\n",
	"   (cond ((eq (caar y) x) (cadar y))\n",
	"      (t (assoc* x (cdr y)))))\n",
	
	"(defun try-substs (substitutes x)\n",
	"  (cond ((null substitutes) x)\n",
        "    ((eq x (caar substitutes)) (cdar substitutes))\n",
    	"     (t (try-substs (cdr substitutes) x))))\n",

	"(defun atom-sublis (substitutes pattern)\n",
	"  (cond ((null pattern) nil)\n",
	"       ((atom pattern) (try-substs substitutes pattern))\n",
	"        (t (cons\n",
        "     		(atom-sublis substitutes (car pattern))\n",
        "     		(atom-sublis substitutes (cdr pattern))))))\n",
	
	"(defun sublis (x y)\n",
	"\"x is an association list.\n(sublis x y) substitutes the values in x for the keys in y.\"\n",
	"  (cond ((atom y) (_sublis x y))\n",
	"	(t (cons (sublis x (car y)) (sublis x (cdr y))))))\n",
	
	"(defun _sublis (x z)\n",
	"  (cond ((null x) z)\n",
	"	((eq (caar x) z) (cadar x))\n",
	"	(t (_sublis (cdr x) z))))\n",

	"(defun remove (e l)\n",
	"\"remove an element from a list.\"\n",
	"  (cond ((null l) nil)\n",
	"	((equal e (car l)) (remove e (cdr l)))\n",
	"	(t (cons (car l) (remove e (cdr l))))))\n",
	
	"(defun succ (x y)\n",
	"\"find the successor of the atom x in y.\"\n",
	"  (cond ((or (null y) (null (cdr y))) nil)\n",
	"	((eq (car y) x) (cadr y))\n",
	"	(t (succ x (cdr y)))))\n",
	
	"(defun pred (x y)\n",
	"\"find the predecessor of the atom x in y.\"\n",
	"  (cond ((or (null y) (null (cdr y))) nil)\n",
	"	((eq (cadr y) x) (car y))\n",
	"	(t (pred x (cdr y)))))\n",
	
	"(defun before (x y)\n",
	"\"Return the elements in x up to y.\"\n",
	"  (cond ((atom x) nil)\n",
	"	((null (cdr x)) nil)\n",
	"	((equal (car x) y) nil)\n",
	"	((equal (cadr x) y) (cons (car x) nil))\n",
	"	(t (cons (car x) (before (cdr x) y)))))\n",
	
	"(defun after (x y)\n",
	"  (cond ((atom x) nil)\n",
	"	((equal (car x) y) (cdr x))\n",
	"	(t (after (cdr x) y))))\n",
	
	"(defun ldifference (in out)\n",
	"  (cond ((null in) nil)\n",
	"	((member (car in) out) (ldifference (cdr in) out))\n",
	"	(t (cons (car in) (ldifference (cdr in) out)))))\n",
	
	"(defun subsetp (a b)\n",
	"  (cond ((null a) t)\n",
	"	((member (car a) b) (subsetp (cdr a) b))\n",
	"	(t nil)))\n",
	
	"(defun samesetp (a b)\n",
	"	(and (subsetp a b) (subsetp b a)))\n",
	
	"(defun count-anywhere (item tree)\n",
	"  (cond ((eq item tree) 1)\n",
	"        ((atom tree) 0)\n",
	"        (t (+ (count-anywhere item (first tree))\n",
	"              (count-anywhere item (rest tree))))))\n",
	
	"(defun dot-product (a b)\n",
	"  (if (or (null a) (null b))\n",
	"      0\n",
	"      (+ (* (first a) (first b))\n",
	"         (dot-product (rest a) (rest b)))))\n",
	
	"(defun number-and-negation (x)\n",
	"\"Returns a list with x and -x\"\n",
	"  (if (numberp x)\n",
	"      (list x (- x))\n",
	"      nil))\n",
	
	"(defun apply (f exp)\n",
	"   (cond ((null exp) nil)\n",
	"         (t (eval (cons f exp)))))\n",

	"(defun maplist (exp f)\n",
	"\"Apply to (car of) exp the function f\"\n",
	"   (cond ((null exp) nil)\n",
	"         (t (cons (apply f exp)\n",
	"            (maplist (cdr exp) f)))))\n",
	
	"(defun sum-h (var from to expr)\n",
	" (cond ((eq from to) nil)\n",
	"	(t\n",
	"	 (cons (subst from var expr)\n",
	" 	       (sum-h var (+ 1 from) to expr)))))\n",

	"(defun sum (var from to expr)\n",
	"\"Expands sum for var from to expression.\n(sum 'x 1 3 '(/ 1 x)\n(+ (/ 1 1) (/ 1 2) (/ 1 3))\"\n",
	" (append\n",
	"   (list '+)\n",
	"   (sum-h var from to expr)))\n",

	"(defun mul (var from to expr)\n",
	"\"Expands mul for var from to expression.\n(mul 'x 1 3 '(/ 1 x)\n(* (/ 1 1) (/ 1 2) (/ 1 3))\"\n",
	" (append\n",
	"   (list '*)\n",
	"   (sum-h var from to expr)))\n",

	"(defun mapcar (op a b)\n",
	"\"Apply op to a and b (a,b are lists)\"\n",
	"  (eval (append (list op) (append a b))))\n",

	"(defun count-atoms (exp)\n",
	"\"Returns number of atoms of exp\"\n",
	"  (cond ((null exp) 0)\n",
	"        ((atom exp) 1)\n",
	"        (t (+ (count-atoms (first exp))\n",
	"              (count-atoms (rest exp))))))\n",
	
	"(defun head (n li)\n",
	"\"Returns head of li at given n pos\"\n",
	"   (cond ((eq n 0) nil)\n",
	"         (t (cons (car li) (head (- n 1) (cdr li))))))\n",

	"(defun tail (n li)\n",
	"\"Returns tail of li at given n pos\"\n",
	"   (cond ((eq n 0) li)\n",
	"         (t (tail (- n 1) (cdr li)))))\n",
	" "
};
