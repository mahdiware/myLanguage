## The Language

* Control-flow statements (curly braces are mandatory):
  * `if (Expr) { N✕Stmt } elif (Expr) { N✕Stmt } else { N✕Stmt }`
    ```
	if (2 == 2){
		print "true";
	} else {
		print "false";
	}
    ```
  * `while (Expr) { N✕Stmt }`
    ```
	i = 0;
	while (i < 10) {
    	print "i: " i;
    	i = i + 1;
	}
    ```
  * `do { N✕Stmt } while (Expr);`
    ```
	i = 0;
	do {
    	print "i: " i;
    	i = i + 1;
	} while (i < 10);
    ```
* Variable and array assignment (`Name` is equivalent to `Name[0]`):
  * `Name = Expr;`
    ```
	number = 2+3;
    ```
  * `Name = Strl;`
    ```
	name = "Mahdi";
    ```
  * `Name[Expr] = Expr;`
    ```
	Number[2] = 30;
    ```
  * `Name[Expr] = Strl;`  It is under maintenance 

* Printing to standard output:
  * `print "Placeholder: ";`
    ```
	print "Hello World!";
    ```
  * `print "Placeholder: " Expr;`
    ```
	print "Happy new year: " 2023;
    ```
  * `print Expr;`
    ```
	print 2+76-7*2/7;
    ```

* Parenthesized expressions:
  * `(Expr)`
    ```
	(2+3)
    ```
  * `(Strl)`
    ```
	("Hello")
    ```

* Binary expressions (between two integers):
  * `Expr OP Expr`, where `OP` can be `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `&&`, or `||`
    ```
	n1 = 2+3-1*3;
	n2 = n1 == 43;
    ```
* Unary expressions (integers only):
  * `OP Expr`, where `OP` can be `-`, `+`, or `!`
    ```
	n1 = -70;
	n2 = !70;
    ```
* Ternary expression (integers only):
  * `Expr ? Expr : Expr`
    ```
	a = 10;
	b = 20;
	num = a == b ? 100  : 1 ;
    ```
* Line and block comments:
  * `// line comment`
    ```
	// this is line comment
	print "Hello World!";
    ```
  * `/* block comment */`
    ```
	/*
	block comment
	*/
	print  "Hello World!";
    ```
