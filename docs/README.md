<p align="center">
<img src="assets/images/logo.png" height="90px" alt="Mw Programming Language" title="Mw Programming Language">
</p>

**Mw** is a powerful, dynamically typed, lightweight, embeddable programming language written in C without any external dependencies (except for stdlib).

**Mw** is a customizable and expandable lexical analyzer, parser, and interpreter for a simplistic, imperative language with similarities to C. It also serves as an educational resource for comprehending lexical analysis and parsing.

> Comments in the C code make it easy to read and understand.

## What Mw code looks like

```lua
number = 0;

function example(){
	do {
	    if (number % 2 == 0 || number % 4 == 0) {
	        print "2️⃣> " number;
	    } elif (number % 5 == 0) {
	        print "5️⃣> " number;
	    } else {
	        print "🔘> " number;
	    }
	    
	    number = number + 1;
	} while (number <= 10);
}

example();
print "\n\n";
number[1] = 10;

print "number[1] is: " + number[1];
print "This program is working\n";
 ```

## Features
* Control-flow statements (curly braces are mandatory):
  * `if (Expr) { N✕Stmt } elif (Expr) { N✕Stmt } else { N✕Stmt }`
  * `while (Expr) { N✕Stmt }`
  * `do { N✕Stmt } while (Expr);`

* Variable and array assignment (`Name` is equivalent to `Name[0]`):
  * `Name = Expr;`
  * `Name = Strl;`
  * `Name[Expr] = Expr;`
  * `Name[Expr] = Strl;`  It is under maintenance 

* Printing to standard output:
  * `print "Placeholder: ";`
  * `print "Placeholder: " Expr;`
  * `print Expr;`

* Parenthesized expressions:
  * `(Expr)`
  * `(Strl)`

* Binary expressions (between two integers):
  * `Expr OP Expr`, where `OP` can be `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `&&`, or `||`

* Unary expressions (integers only):
  * `OP Expr`, where `OP` can be `-`, `+`, or `!`

* Ternary expression (integers only):
  * `Expr ? Expr : Expr`

* Line and block comments:
  * `// line comment`
  * `/* block comment */`
