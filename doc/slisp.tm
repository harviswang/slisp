<TeXmacs|1.0.4.3>

<style|article>

<\body>
  <assign|tmstrong|<macro|1|<with|font-series|bold|<arg|1>>>><assign|tmop|<macro|1|<ensuremath|<with|math-font-family|rm|<arg|1>>>>><assign|emdash|<macro|--->>

  <surround| \ ||<doc-data|<doc-title|SLISP 2.x
  DOCUMENTATION>|<doc-author-data|<author-name|>|<\author-address>
    HLG
  </author-address>>|<doc-date|<date|>>>>

  <section|About Lisp>

  LISP (LISt Processor) was developed for the IBM 704 computer by the AI
  group at MIT.

  LISP was invented by John McCarthy in 1958 while he was at MIT. You could
  get more information about how LISP was designed by reading ``Recursive
  Functions of Symbolic Expression and ther Computation by Machine, Part I''

  LISP is a dynamic typed language and it basic datatype are lists.

  You could get more information at

  \;

  \;

  <section|What SLISP pretend to be>

  SLISP is a simple LISP interpreter desing for fun, and for learn the basics
  of LISP, not just programming in LISP also seeing the source code, and
  trying to get deeper comprehension about LISP TDAs, functions, and so on.

  The second point is the most interesting one because you can learn a lot of
  lisp without using it. If you take a compilable language such as C, then
  you have to refer to the C compiler, lets say: GCC. Then when you look at
  the code, you understand a few thousands of lines, but don`t get what C
  language is about.

  The main idea of SLISP now is to bring to the user a clear source code, so
  they can use in any CS course to understand how LISP could be implemented,
  modifying it, and programming in lisp at the same time.

  SLISP is programmed in C. Why?

  <\itemize>
    <item>We need a language that can be compiled so we can get an executable
    interpreter.

    <item>We need a plataform-portable language.

    <item>We need also, a well know, well documented programming language.
  </itemize>

  Besides that C is, as far I know, one of the most studied and used
  imperative programming language ever.

  \;

  \;

  <section|A little more about how SLISP works, and its made>

  <subsubsection|TDA>

  It`s a linked list. Basically a lisp object is made by a type, a value
  (identifier, integer, double, string, a cons-cell). As you can see at
  slisp.h source, an object looks like this:

  <strong|<strong|<strong|>>>type = { T,NIL,integer,double,string,identifier,cons-cell
  }

  gc = { 0,1 } refers to the garbage collector flag, if it`s set or not.

  doc = ``string'' (Nice to comment code)

  value

  ptr _ next _ object

  Value means a `real` value. So if the object-type is integer, then the
  object-value would be a natural number.

  <subsubsection|FUNCTIONS>

  Well, all functions are of the same type, because we need to `group` all
  functions so we can easily call them, and as we have our defined object,
  functions may not expect nothing else. Lets be more clear in this point.

  <\with|mode|math>
    Let think about sum function, wich can be used in lisp as:

    (+ a<rsub|0>,a<rsub|1>,\<ldots\>a<rsub|n>)

    \;

    It is clear that we can define an group ARGS =
    {a<rsub|0>,a<rsub|1>,\<ldots\>,a<rsub|n>} but the \|ARGS\| (size, or
    number of elements) is unknow at compile time.
  </with>

  \;

  We can see our first problem: How many arguments our function must expect?

  The answer is `unknown`. The solution is to pack, every argument in one
  single object, we can do it because our TDA is potencially infinite (I mean
  a list would contain from 1 elemnent to <with|mode|math|\<infty\>>). So we
  decide to pack all our arguments into one single list.

  \;

  Now, in our C code, we just expect one argument, a list, with n elements.

  Inside functions, we can deal with that just by looking over the list.
  Because all of this we expect that our functions looks like:

  \;

  function(objectp arguments);

  \;

  Now,what should a function must return? Let see another example:

  <\with|mode|math>
    \;

    (<op|s*e*t*q> a \ (\<ast\>123))
  </with>

  \;

  By this statement we are saying

  <\with|mode|math>
    a = 1\<ast\> 2* \<ast\> 3

    \;

    So let X = {x<rsub|0>,x<rsub|1>,\<ldots\>,x<rsub|n>} to be a set, and
    f(X, \<Theta\>) be a function thats returns the evluation of:

    (\<Theta\> x<rsub|0>, x<rsub|1>, \<ldots\>, x<rsub|n>)

    \;

    In this case, \<Theta\> is multiplication

    \;
  </with>

  \;

  The object `a` it's clearly a object, so our mult function must return an
  object. Now we now that we must return an object.

  We can say a little bit more about this:

  <\with|mode|math>
    \;
  </with>

  <\with|mode|math>
    Let f and g be two C functions, and f returns a value of type i \ \ while
    g returns an object type.

    Both g and f arguments are objects (as we defined before).

    \;

    Now Lets h be another function defined as:

    \;

    h = f ( g (object)) \ \ \ \ Where h argument and return a value of type
    object.

    \;

    It is obvious that h can<lprime|`>t exite, because:

    g: object \<rightarrow\> object

    f: object \<rightarrow\> i

    h: object \<rightarrow\> object

    \;

    So let e,a,c \ \<in\>objects, e = g(a) , c = f(e).

    \;

    But, we have just said that c is an object, while f(e) is returning a
    value of type i, so the only way to make this assignment is i = object,
    by this both groups i and object are equals.
  </with>

  \;

  So, a function is defined by:

  \;

  object function (object)

  \;

  Lets go on. We saw that there's exists a + (plus) function. It's imposible
  to define a function named as + in C, and in many other languages. So, we
  need a correspondency (think as a hash) between LISP function name and C
  function name.

  \;

  <with|mode|math|f:>lisp-function-name <with|mode|math|\<rightarrow\>>C-function-name

  i.e:

  <with|mode|math|f(+)\<rightarrow\>>F _ add

  \;

  We now go into function definition. There's no difference between a list
  and a function, in fact, as every type of lisp is a list, atom, or
  cons-cell, a function is just a list, that evaluaded returns an atom or a
  cons-cell. So defining a function is like:

  <\with|mode|math>
    \;

    (<op|d*e*f*u*n><op|F*n*a*m*e>(<op|a*r*g><rsub|0><op|a*r*g><rsub|1>\<ldots\><op|a*r*g><rsub|n>)(<op|b*o*d*y>))
  </with>

  \;

  This statement creates an object, as know Fname, which value is a list
  containing both args and body. But the question is: How we can distinguish
  between a function and a list? Well, we are going to go for that answer.

  When we assing or (functionally speaking) letting a variable a value, we
  can refer a value by it`s `identifier'. So:

  <\with|mode|math>
    \;

    x=3
  </with>

  x+4=7

  \;

  Think a little bit about that. We first let x to be 3 then add 4 to x to
  get 7. Why this is possible? Because of assignment or variable-binding.

  So the really question must be: How we can distinguish between a function
  and an identifier?.

  We'll do a little trick. First, we create a list called L. Then the first
  element of the list would be an identifier called `lambda'. So our list L
  looks like:

  \;

  L = (lambda)

  \;

  Then, we have to put the arguments, and the body. So this drive us to:

  \;

  L = (append '(lambda) (list '(arguments) '(body)))

  \;

  Finally we set this to the object Fname. If we asked Fname to the
  interpreter it would return:

  \;

  (lambda (arguments) (body))

  \;

  One may ask himself: How we can recognise a function from another, if all
  looks the same?.

  Well the answer is that besides we assing the `lambda` to all functions
  this is just to distinguish functions from common identifiers, to
  distinguish between functions, we just have to look over our objects. Here
  is an example:

  \;

  <\with|mode|math>
    \<gg\> (defun sum (a b) (+ a b))

    \<Rightarrow\> (lambda (a b) (+ a b))

    \<gg\> (defun \ \ mul (a b) ( \<ast\> a b))

    \<Rightarrow\> (lambda (a b) (+ a b))

    \<gg\> (setq value 32)

    \<Rightarrow\> 32

    \;
  </with>

  \;

  We look over our objects linked list, and try to find the `sum' identifier
  somewhere, if we find it. As we can see it's value is a list. But, the
  first element of sum is an identifier, `lambda' so we assume that it must
  be a function. Then we took the arguments ( (2 3)) and then, call the C
  function (i.e: F _ add) with arguments.

  So, what happens if someone tries:

  \;

  <\with|mode|math>
    \<gg\> (setq a <rprime|'>(lambda (a \ \ b) (+ a \ \ b)))
  </with>

  \;

  Then, a is no other thing that a function :).

  With C defined functions we don't have to deal with this, so we return to
  our previous point:

  The correspondency between lisp-named functions and C-named functions.

  Suppouse that there exists such <with|mode|math|f> that for every
  lisp-named functions, it's returns the C-named function. What else we
  need?. Well, we need a table containing the lisp-named functions and the
  C-named ones.

  We can do something like:

  \;

  <with|mode|math|C>-<with|mode|math|named>-<with|mode|math|function>
  <with|mode|math|><with|mode|math|\<Longrightarrow\>>
  <with|mode|math|LISP>-<with|mode|math|named>-<with|mode|math|function>

  \;

  In C:

  \;

  <\verbatim>
    typedef struct {

    \ \ \ \ \ \ \ \ char *lisp-function-name;

    \ \ \ \ \ \ \ \ objectp (*func) (objectp args);

    }
  </verbatim>

  \;

  We put an function pointer, and not a name, because when we now what
  function we are calling, we can just call it and expect a result (in the
  best case).

  \;

  One more point is evaluation. SLISP is programed in C. C is an imperative
  programming language. By this we can state that the evaluation is `eager`.

  <\verbatim>
    int f(int a, int b)

    {

    \ \ \ \ \ return a+3;

    }

    \;

    int g()

    {

    \ \ \ \ \ return g() + 1;

    }

    \;

    int main()

    {

    \ \ \ \ \ return f(1,g());

    }
  </verbatim>

  \;

  If we run this code, we can see that will never stop. The reason is that as
  C uses eager evaluation will eval every argument, and then apply the
  function rule. Let be more clear:

  \;

  <\with|mode|math>
    F:\<bbb-R\> \<rightarrow\> \<bbb-R\>

    double: \<bbb-R\> \<rightarrow\>\<bbb-R\>

    \;

    F(a,b) = a + b;

    double(x) = 2*x
  </with>

  \;

  With eager evaluation:

  \;

  <\with|mode|math>
    eval(F(double(2), double(3))) =\ 

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(double(2))
    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(2\<ast\>*2)
    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(4)
    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =4
    (NF)

    eval(F(4,double(3))) \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(double(3))
    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(2
    \<ast\> 3) \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =\ 

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(6)
    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =
    6 (NF)

    eval(F(4,6)) \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(4+6)
    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =

    \ eval(10) \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =
    10 (NF)

    \;
  </with>

  With lazy evalutation:

  \;

  <\with|mode|math>
    eval(F(double(2),double(3))) \ \ \ \ \ \ \ \ \ \ \ \ \ \ =

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(double(2) +
    double(3)) =

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(double(2))
    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =\ 

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(2*\<ast\>2)
    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(4)
    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =4
    (N F)

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(4 +
    double(3)) \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(double(3))
    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(2
    \<ast\> 3) \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =\ 

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(6)
    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =6
    \ (NF)

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ eval(4+6)
    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =

    eval(10) \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ =10
    (NF)
  </with>

  <htab|5mm>

  NF Stands for : Normal Form (a value)

  \;

  You see, lazy evalution first apply the function rule, and then reduce the
  arguments. If the is a non-used argument, by lazy evaluation you don`t need
  to eval it.

  \;

  In our example return(F(1,g())); as the second parameter of the F function
  is not needed then, function would return 4 (1 + 3);

  \;

  Lazy evaluation, it not just the statement: ``If an argument is not needed
  then is not evalled''.

  Lazy evaluation, is applying first the outermost function rule, and then
  going from outside to inside evaluating. As contraire, eager evaluation
  ,evals from inside to outside.

  \;

  This is related to graph theory and redex (EXpression REDuction).

  \;

  What SLISP does, is just to don`t eval what is not needed.

  One might ask:\ 

  \;

  Another interesing feature is the possibility of using some kind of
  \ <with|mode|math|\<lambda\> expressions.>

  For example:

  <\with|mode|math>
    (\<lambda\> ((arg<rsub|0> arg<rsub|1> \<ldots\> arg<rsub|n>) (expr))
    val<rsub|0> val<rsub|1>\<ldots\>val<rsub|n>)
  </with>

  \;

  One can easly see that defining lambda as this way, is the same as binding.\ 

  LET syntax is:

  <\with|mode|math>
    (let ((var<rsub|0> val<rsub|0>) (var<rsub|1>
    val<rsub|1>)\<ldots\>(var<rsub|n> val<rsub|n>)) (expr))
  </with>

  One can easly convert a lambda expression into a let expression by:

  \;

  <\with|mode|math>
    (\<lambda\> ((args) (expr)) (values)

    bindings = (pair args values)

    bindings = ((arg<rsub|0> val<rsub|0>) (arg<rsub|1> val<rsub|1>) \<ldots\>
    (arg<rsub|n> val<rsub|n>))

    (let bindigns expr)
  </with>

  \;

  So that's main reason for using this kind of lambda notation.

  \;

  Now consider the following scheme code:

  \;

  <\verbatim>
    (define (integral f a b dx)

    \ \ \ (define (add-dx x) (+ x dx))

    \ \ \ (* (sum f (+ a (/ dx 2.0)) add-dx b)

    \ \ \ \ \ \ dx))
  </verbatim>

  \;

  Sum is just:

  \;

  <with|mode|math|<big|sum><rsup|b><rsub|i=n> f(i) = f(a)+\<ldots\> + f(b)>

  \;

  Now... we are defining a new function inside another function, but, add-dx
  function is only visible in integral function context, so, outside
  integral, add-dx doesn't exists.

  I'll do something to simulate that behavoir:

  \;

  <\verbatim>
    (defun integral(f a b dx)

    \ \ \ (let ((add-dx '(lambda (x) (+ x dx))))

    \ \ \ \ \ \ \ (* (sum f (+ a (/ dx 2.0)) add-dx b)

    \ \ \ \ \ \ \ \ \ \ \ dx)))
  </verbatim>

  Basically, we bind the variable add-dx to <with|mode|math|<rprime|'>(lambda
  (x) (+ x dx))> so interpreter would use it as a function.

  \;

  To avoid the addition of <with|mode|math|<rprime|'>(lambda \<ldots\>. )> in
  the let form, there is a `labels' functions wich allow us to create those
  `functions bindings'.

  So, the labels syntax is:

  \;

  <with|mode|math|(labels ((func<rsub|0> (params) (body)) (func<rsub|1>
  (params) (body)) \<ldots\> ) expr<rsub|0> expr<rsub|1> expr<rsub|n>) >

  Integral \ could be created as:

  \;

  <\verbatim>
    (defun integral(f a b dx)

    \ \ \ \ (labels ((add-dx (x) (+ x \ dx)))

    \ \ \ \ \ \ \ \ \ \ \ (* (sum f (+ a (/ dx 2.0)) add-dx b)

    \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ dx)))
  </verbatim>

  \;

  Some readers could note that in add-dx case, add-dx is using an element,
  that is not `defined' (dx). The reason of this, is that functions have
  their own `space'. And by this, we could do:

  <\with|mode|math>
    \<gg\>(setq a \ 3)

    \<gg\> (defun b(a) (+ a a))

    \<gg\> (b \ 2)

    \<Rightarrow\> 4
  </with>

  \;

  Formally, functions do a routine before evaluating itselfs. First, take the
  arguments (in this case 'a') and save the original value. Then, set 'a' the
  argument value, in this case 2. Then evaluates the body, so every function
  in slisp know that a = 2. Then, before returns, it restore the values or
  the value, (a = 3) and returns the result.

  \;

  The same with let, and labels. They use the same strategy.. and that is
  called: `scoping'. For a function-body the elements has the value of their
  arguments, and for the `environment' the values are those whose user set.

  \;

  One might think: ``if there no exist such setq,setf, and destructive
  functions, then function evaluation would be faster''

  Yes, totally true.

  \;

  So basically that's all about functions.

  <section|<samp|>INPUT-OUTPUT>

  \;

  Basically you can eval files by:

  <with|mode|math|# slisp file.lisp>

  If thre is an error trying to evaluate the file, then you will be noticed.

  \;

  If you want to use the file and continue with the interpreter you can add
  the -i flag to get:

  <\with|mode|math>
    #slisp -i file.lisp

    ;; Loaded file file.lisp

    \<gg\>
  </with>

  About working with files the main idea is to provide the user with a set of
  built-in functions:

  <\with|mode|math>
    fopen

    fclose

    fread

    fwrite

    fseek

    ftell

    frewind

    \;
  </with>

  This functions, besides the name, are similar to the libc functions.

  You can read the function-manual to lear how to use them.

  \;

  <section|About another things>

  <subsection|Recognising (lexing, parsing)>

  \;

  First of all we have to define all data-types so we can difference one from
  another:

  \;

  If you are usring Slisp with GMP then:

  cstring \ \ \ \ = `` * ''

  ngmp \ \ \ \ \ \ = ?(+\|-)([0-9]+) ?(.[0-9]+)

  \;

  If you dont:

  cstring \ \ \ \ = `` * ''

  <op|integer> \ \ \ \ = ?(+\|-)([0-9]+)

  <op|double> \ \ \ \ = ?(+\|-)([0-9]+).([0-9]+)

  <op|idenfitifer >= ([0-9]+)? ([a-Z]+)([0-9][a-Z]+)?

  \;

  Now, in SLISP with gmp support strings, doubles and integers, are
  considerer as string, then functions check if is a numeric or not string.

  If you are not using gmp, then:

  \;

  Integers and doubles:

  If there is a plus (+) or minus (-) sign:

  If there is an integer, we read it and store into a buffer, until we find a
  non integer caracter.

  \;

  When we find a non integer character there are two possible cases:

  a) The char is a `.` (dot)

  b) The char is alpha ([a-Z]+)

  \;

  a) We read the token, until we find a non-numeric character, and return
  double. If is like:

  <with|mode|math|12.>

  We append a 0 so, we get

  <with|mode|math|12.0>

  \;

  b) We take away that char, and start reading again, as it was an
  identifier. So

  <\with|mode|math>
    12aw23a
  </with>

  Is an identifier.

  \;

  Identifiers:

  If there is a character continue reading until is a valid character (nor
  `(' ')' for example).

  \;

  Strings:

  If there is a `` read until we find a '' character.

  \;

  To get more information just see lexer.c file.

  \;

  <subsection|What is the main difference between a cons-cell and a list?>

  \;

  Well, first of all, you can consider a cons-cell as a value, just as an
  integer, or double.

  In fact, a list is made up cons-cell.

  A cons-cell value could be represented as follow:

  \;

  [ <with|mode|math|<op|V*A*L*U*E><rsub|1>> \|
  <with|mode|math|<op|V*A*L*U*E><rsub|2>> ] ----\<gtr\> NIL

  \;

  While a list of two elements is represented as:

  [ <with|mode|math|<op|V*A*L*U*E><rsub|1>> \| * ] ----\<gtr\> [
  <with|mode|math|<op|V*A*L*U*E><rsub|2>> \| * ] <emdash>\<gtr\> NIL

  \;

  Every element of a list, has a value, and a pointer to the next element, or
  nil.

  \;

  Where (*) represents a pointer.

  A cons-cell is represented by LISP as:

  <\with|mode|math>
    (A . B)

    \;

    \<gg\> (car <rprime|'>(A \ . B))

    \<Rightarrow\> A

    \<gg\> (cdr <rprime|'>(A . B))

    \<Rightarrow\> B
  </with>

  \;

  But, a list is represented by LISP as:

  <\with|mode|math>
    (A B)

    \;

    \<gg\> (car <rprime|'>(A B))

    \<Rightarrow\> A

    \<gg\> (cdr <rprime|'>(A B))

    \<Rightarrow\> (B)
  </with>

  \;

  The rest of the list '(A B) is a list which only element is B (and nil).

  Speaking as implementation:

  <\verbatim>
    \;

    struct {

    \ \ \ \ \ \ \ \ \ object car;

    \ \ \ \ \ \ \ \ \ object cdr;

    } cons-cell;

    \;

    struct {

    \ \ \ \ \ \ \ \ \ object car;

    \ \ \ \ \ \ \ \ \ list \ \ cdr;

    \ } list;

    \;
  </verbatim>

  We could create a cons-cell as follow:\ 

  \;

  <\verbatim>
    object create_cons_cell(object args)\ 

    {

    \ \ \ \ \ object a, value1, value2;

    \ \ \ \ \ value1 = eval(car(args));

    \ \ \ \ \ value2 = eval(car(cdr(args));

    \ \ \ \ \ 

    \ \ \ \ \ a = new_object(OBJ_CONS);

    \ \ \ \ \ a-\<gtr\>value.c.car = value1;

    \ \ \ \ \ a-\<gtr\>value.c.cdr = value2;

    \;

    \ \ \ \ \ return a;

    }

    \;
  </verbatim>

  If we want a list we have to:

  \;

  <\verbatim>
    object create_list(object args)

    {

    \ \ \ \ \ object a, value1, value2;

    \ \ \ \ \ value1 = eval(car(args));

    \ \ \ \ \ value2 = eval(car(cdr(args)));

    \;

    \ \ \ \ \ a = new_object(OBJ_CONS);

    \ \ \ \ \ a-\<gtr\>value.c.car = value1.

    \ \ \ \ \ a-\<gtr\>value.c.cdr = new_object(OBJ_CONS);

    \ \ \ \ \ a-\<gtr\>value.c.cdr-\<gtr\>value.c.car = value2;

    \;

    \ \ \ \ \ return a;

    \;

    }
  </verbatim>

  \;

  <subsection|I can declare `a' as an identifier and as a function!>

  \;

  This is because we can recognize between an identifier, an a function.

  When we are looking for objects in memory (as you can see in object.c) we
  do it by comparing the object-type, and in case it maches, then, we compare
  the name.

  \;

  Suppouse you have a <with|mode|math|a=2> and
  <with|mode|math|b=<rprime|''><op|h*e*l*l*o><op|w*o*r*l*d><rprime|''>> if
  you want to get <with|mode|math|a> value, the parser would call a function
  that look for an object which type is integer and its value is 2, on the
  other hand, in you want to get <with|mode|math|b> value, the parser would
  call a functoin that look for an object which type is string and its value
  is ''hello world''.

  \;

  <subsection|What are T and NIL?>

  \;

  Well, T and NIL are the only two identifiers that are objects too. So T
  value is T, and NIL value is NIL.

  \;

  T and NIL are too, as integers. A value-type.

  So one could state:

  <\with|mode|math>
    a=t

    b = nil
  </with>

  To create T and NIL we just do:

  \;

  <\verbatim>
    object t = new_object(T);

    object nil = new_object(NIL):

    \;
  </verbatim>

  \;

  <subsection|There are different types of lists?>

  \;

  A list could be always represented as follows:

  - The head of a list.

  - The tail of a list.

  \;

  So, by this we are saying that every list has a head and a tail. What head
  or tail are doesn`t matter. In fact, head or tail, could be:

  \;

  - Another list.

  - An atom. (integer,double,string,identifier)

  - A cons-cell. ( (a . b) ) where a,b are atoms.

  \;

  One might say: It`s hard to see what `is the composition' of a list, when
  some function deals with it.

  That's true. I get your point, and I will gave you an example:

  \;

  <\verbatim>
    (defun get-names (x)

    \ \ \ \ (cond ((eq x nil) nil)

    \ \ \ \ \ \ \ (t (cons (caar x) (get-names (cdr x))))))
  </verbatim>

  \;

  In this case, get-names is expecting which is called (an association list)
  which can be represented as follow:

  <\with|mode|math>
    If A is an association list, then the structure of A is similar to:

    \;

    A = ( (name<rsub|0> value<rsub|0>) (name<rsub|1> value<rsub|0>) \<ldots\>
    (name<rsub|n> value<rsub|n>))
  </with>

  \;

  One can easly see that `car' of A is: <with|mode|math|(name<rsub|0>
  value<rsub|0>)> and `cdr' of A is: <with|mode|math|((name<rsub|1>
  value<rsub|1>) \<ldots\> )>

  So, by that, caar will take car <with|mode|math|(name<rsub|0> value<rsub|0
  >) >and that would be: <with|mode|math|name<rsub|0>>.

  \;

  But, we are not solving the problem. The real solution is to comment how
  function works. One may put a description as:

  ;; get-names expect an association list <with|mode|math|((a<rsub|0>
  v<rsub|0>)(a<rsub|1> v<rsub|1>) \<ldots\>(a<rsub|n> v<rsub|n>))>

  ;; and would return a list contanining <with|mode|math|a<rsub|i> \ \ \ 0
  \<leq\> i \<leq\> n>

  \;

  <subsection|What about documenting?>

  \;

  Yes... you can document functions, becasue when you comment functions, and
  you are working in the interpreter you can`t see them. So there is a way.

  <with|mode|math|(defun function (args) <rprime|''>comment<rprime|''>
  (body))>

  So... ``comment'' would be copied into function-\<gtr\>doc value.

  This is a case when comment is usefull:

  \;

  (defun sum(var from to expr) ``Expand summatory of `expr' for 'var' from
  `from' to 'to' ``

  \ \ (append (list '+) (sum-h var from \ to expr)))

  \;

  By typing:

  \;

  <\with|mode|math>
    \<gg\> (doc sum)
  </with>

  sum: Expand summatory of `expr' for `var' from `from' to `to'

  <with|mode|math|\<Rightarrow\> t>

  \;

  If the function is not commented you can comment it by:

  <with|mode|math|\<gg\> (makedoc func<rsub|>
  \ <rprime|''>comment<rprime|''>)>

  \;

  You can only comment functions.

  <section|Let's talk about how to programming SLISP>

  \;

  Well, as you can see code is pretty clear, and easy to understand. But
  there some interesting things.\ 

  <\enumerate-roman>
    <item>Why some functions are declared as __inline__?

    The point of making a function "inline" is to hint to the compiler that
    it is worth making some form of extra effort to call the function faster
    than it would otherwise - generally by substituting the code of the
    function into its caller. As well as eliminating the need for a call and
    return sequence, it might allow the compiler to perform certain
    optimizations between the body of both functions.

    Those functions that are declared as __inline__ are common used by the
    program, so it makes the program faster (don't decrement complexity, just
    Constant)

    \;

    <item>Can I add another data-types to the object?

    Yes, you can, but depends of the data-type you'll need to modify
    functions related so they can work with your data-type. Formally:

    <\with|mode|math>
      \;

      Let F \ be the group of \ functions that work with integers number.

      \ So F = \ {f<rsub|0>, f<rsub|1>,\<ldots\>,f<rsub|n>}.
      \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \<forall\> f<rsub|i> \<in\> F \ \ with 0
      \<leq\> i \<leq\> n \ \ 

      f<rsub|i> : D \<rightarrow\> D

      \;

      We don<lprime|`>t have to change the Domain of the function. We just
      have to add code that recognize

      this new data-type so the function know how to deal with it.

      And of course, we have to add the new data-type in D.
    </with>

    <item>Is the any garbage collector?

    Yes and is very easy to understand how it works by reading the code, and
    maybe you only need that, as this text doesn't pretend to be a lisp, or
    gc, tutorial or reference. But, I willl give you a little idea about how
    it's works. Let's try to understand the following code:

    \;

    <\with|mode|math>
      \<gg\> *(setq l (list 1 2 3 4 5 6 7 8 9))

      \<Rightarrow\> (1 2 3 4 56 7 8 9)

      \<gg\> (setq l (list 1 2 3))
    </with>

    \;

    `l` could be represented as a list of 10 objects, which types are
    integers. So this could be:

    <with|mode|math|L = [OBJ<rsub|0>\|1]\<rightarrow\> [OBJ<rsub|1>\|2]
    \<rightarrow\> \<ldots\> \<rightarrow\>[OBJ<rsub|8>\| \ 9 ]
    \<rightarrow\> nil>

    \;

    So, we have now, 9 new objects defined in memory, we could think as nil
    as is has been already defined.

    If we take a look at what (&show-objects) ouptut is we can verify this.

    Now by second input (setq l (list 12 3)) We are redefining `l` as:

    <with|mode|math|L= [OBJ<rsub|0>\|1] \<rightarrow\> [OBJ<rsub|1>\|2]
    \<rightarrow\> [OBJ<rsub|2>\|3] \<rightarrow\> nil>

    \;

    So, there is when gc comes and recycle and unreference inaccesible
    objects. By this I mean that gc will `remove` (actually it's free objects
    memory) those unreferenced objects.

    \;
  </enumerate-roman>

  \;
</body>

<\references>
  <\collection>
    <associate|auto-1|<tuple|1|1>>
    <associate|auto-10|<tuple|5.3|8>>
    <associate|auto-11|<tuple|5.4|8>>
    <associate|auto-12|<tuple|5.5|?>>
    <associate|auto-13|<tuple|5.6|?>>
    <associate|auto-14|<tuple|6|?>>
    <associate|auto-2|<tuple|2|1>>
    <associate|auto-3|<tuple|3|1>>
    <associate|auto-4|<tuple|1|1>>
    <associate|auto-5|<tuple|2|2>>
    <associate|auto-6|<tuple|4|5>>
    <associate|auto-7|<tuple|5|6>>
    <associate|auto-8|<tuple|5.1|7>>
    <associate|auto-9|<tuple|5.2|7>>
  </collection>
</references>

<\auxiliary>
  <\collection>
    <\associate|toc>
      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|1<space|2spc>About
      Lisp> <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-1><vspace|0.5fn>

      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|2<space|2spc>What
      SLISP pretend to be> <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-2><vspace|0.5fn>

      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|3<space|2spc>A
      little more about how SLISP works, and its made>
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-3><vspace|0.5fn>

      <with|par-left|<quote|3fn>|1<space|2spc>TDA
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-4>>

      <with|par-left|<quote|3fn>|2<space|2spc>FUNCTIONS
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-5>>

      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|4<space|2spc><with|font-family|<quote|ss>|>INPUT-OUTPUT>
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-6><vspace|0.5fn>

      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|5<space|2spc>About
      another things> <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-7><vspace|0.5fn>

      <with|par-left|<quote|1.5fn>|5.1<space|2spc>Recognising (lexing,
      parsing) <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-8>>

      <with|par-left|<quote|1.5fn>|5.2<space|2spc>What is the main difference
      between a cons-cell and a list? <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-9>>

      <with|par-left|<quote|1.5fn>|5.3<space|2spc>I can declare `a' as an
      identifier and as a function! <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-10>>

      <with|par-left|<quote|1.5fn>|5.4<space|2spc>What are T and NIL?
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-11>>

      <with|par-left|<quote|1.5fn>|5.5<space|2spc>There are different types
      of lists? <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-12>>

      <with|par-left|<quote|1.5fn>|5.6<space|2spc>What about documenting?
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-13>>

      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|6<space|2spc>Let's
      talk about how to programming SLISP>
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <pageref|auto-14><vspace|0.5fn>
    </associate>
  </collection>
</auxiliary>