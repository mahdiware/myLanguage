<p align="center">
<img src="assets/images/logo.png" height="90px" alt="Mw Programming Language" title="Mw Programming Language">
</p>

**Mw** is a powerful, dynamically typed, lightweight, embeddable programming language written in C without any external dependencies (except for stdlib). It is a class-based concurrent scripting language with a modern [Swift](https://github.com/apple/swift) like syntax.

**Mw** supports procedural programming, object-oriented programming, functional programming and data-driven programming. Thanks to special built-in methods, it can also be used as a prototype-based programming language.

**Mw** has been developed from scratch for the [Creo](https://creolabs.com) project in order to offer an easy way to write portable code for the iOS and Android platforms. It is written in portable C code that can be compiled on any platform using a C99 compiler. The VM code is about 4K lines long, the multipass compiler code is about 7K lines and the shared code is about 3K lines long. The compiler and virtual machine combined, add less than 200KB to the executable on a 64 bit system.

> Comments in the C code make it easy to read and understand.

## What Mw code looks like

```lua
function example(){
	print("Hello ", "World!");
}

example();
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
