<p align="center">
<img src="assets/images/logo.png" height="90px" alt="Mw Programming Language" title="Mw Programming Language">
</p>

**Mw** is a powerful, dynamically typed, lightweight, embeddable programming language written in C without any external dependencies (except for stdlib).

**Mw** This is a customizable and expandable lexical analyzer, parser, and interpreter for a simplistic, imperative language with similarities to C.

> Comments in the C code make it easy to read and understand.

## What Mw code looks like

```mahdiware
number = 0;

do {
    print("Hello ", "World!\n");
    
    number = number + 1;
} while (number <= 10);
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
  * `print("Placeholder: ");`
  * `print("Placeholder: ", Expr);`
  * `print(Expr);`

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
