## About

This is a customizable and expandable lexical analyzer, parser, and interpreter for a simplistic, imperative language with similarities to C. It also serves as an educational resource for comprehending lexical analysis and parsing.

## How it Operates

The lexical analyzer generates a token list from the input (a memory-mapped file with `PROT_READ` permissions). Each token is produced by consuming a character from the input and querying each token function to determine if it matches. These token functions have internal states and can return `STS_ACCEPT`, `STS_REJECT`, or `STS_HUNGRY` based on the consumed character. The lexer continues reading characters until all functions return `STS_REJECT` (resetting their internal state), and then the accepted token is determined by searching for an `STS_ACCEPT` from the previous iteration.

Essentially, this algorithm follows the "maximal munch" approach.

The parser takes the token list and generates a parse tree. It does so by continuously shifting tokens from the input to the parse stack and then reducing any matching suffix of the stack into a non-terminal, following the grammar rules. The grammar is defined as a static array of structs, where each struct represents a rule. When a rule matches a stack suffix, a reduction is performed. The reduction creates a level of child nodes (symbols that matched the rule) and they become children of a new non-terminal symbol on the stack (the left-hand side of the matching rule).

In essence, this is a shift-reduce, bottom-up parser. Due to the parser lacking state and decision tables, a few additional workarounds are implemented to handle operator precedence and if-elif-else chains.

The interpreter is straightforward. It begins at the top of the parse tree and traverses down through the child nodes, executing statements and evaluating expressions. Any warnings that occur during program execution are written to standard error with a prefix of `warn:`.

## The Language

* Control-flow statements (curly braces are mandatory):
  * `if (Expr) { N✕Stmt } elif (Expr) { N✕Stmt } else { N✕Stmt }`
  * `while (Expr) { N✕Stmt }`
  * `do { N✕Stmt } while (Expr);`

* Variable and array assignment (integers only, `Name` is equivalent to `Name[0]`):
  * `Name = Expr;`
  * `Name[Expr] = Expr;`

* Printing to standard output (integers only):
  * `print "Placeholder: " Expr;`
  * `print Expr;`

* Parenthesized expressions (integers only):
  * `(Expr)`

* Binary expressions (between two integers):
  * `Expr OP Expr`, where `OP` can be `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `&&`, or `||`

* Unary expressions (integers only):
  * `OP Expr`, where `OP` can be `-`, `+`, or `!`

* Ternary expression (integers only):
  * `Expr ? Expr : Expr`

* Line and block comments:
  * `// line comment`
  * `/* block comment */`

## Sample Output

To start the interpreter, specify the file that contains the code.

Once the file is opened and memory-mapped, the lexer begins its work. The tokens are outputted to standard output, appearing in alternating colors (green and yellow) to clearly indicate the start and end of each token.

If the lexing process succeeds (all tokens are recognized), the parser starts. During each shift or reduce operation, a single line is outputted showing

 the current contents of the parse stack. Non-terminals are displayed in yellow, while terminals are displayed in green. If the parsing process is successful, the parse stack should contain a single non-terminal called "Unit".

The interpreter then starts execution from the root of the tree (which is always "Unit"), executing the tree produced by the parser.

```
$ make
$ ./lang examples/do-while.txt
number = 0;

do {
    if (number % 2 == 0 || number % 4 == 0) {
        print "2️⃣> " number;
    } elif (number % 5 == 0) {
        print "1️⃣> " number;
    } else {
        print "🔘> " number;
    }

    number = number + 1;
} while (number <= 10);

*** Parsing ***
Shift: ^
Shift: ^ number
Shift: ^ number =
Shift: ^ number = 0
Red19: ^ number = Atom
Red20: ^ number = Expr
Shift: ^ number = Expr ;
Red05: ^ Assn
Red02: ^ Stmt
Shift: ^ Stmt do
Shift: ^ Stmt do {
Shift: ^ Stmt do { if
Shift: ^ Stmt do { if (
Shift: ^ Stmt do { if ( number
Red18: ^ Stmt do { if ( Atom
Red20: ^ Stmt do { if ( Expr
Shift: ^ Stmt do { if ( Expr %
Shift: ^ Stmt do { if ( Expr % 2
Red19: ^ Stmt do { if ( Expr % Atom
Red20: ^ Stmt do { if ( Expr % Expr
Red39: ^ Stmt do { if ( Bexp
Red22: ^ Stmt do { if ( Expr
Shift: ^ Stmt do { if ( Expr ==
Shift: ^ Stmt do { if ( Expr == 0
Red19: ^ Stmt do { if ( Expr == Atom
Red20: ^ Stmt do { if ( Expr == Expr
Red27: ^ Stmt do { if ( Bexp
Red22: ^ Stmt do { if ( Expr
Shift: ^ Stmt do { if ( Expr ||
Shift: ^ Stmt do { if ( Expr || number
Red18: ^ Stmt do { if ( Expr || Atom
Red20: ^ Stmt do { if ( Expr || Expr
Shift: ^ Stmt do { if ( Expr || Expr %
Shift: ^ Stmt do { if ( Expr || Expr % 4
Red19: ^ Stmt do { if ( Expr || Expr % Atom
Red20: ^ Stmt do { if ( Expr || Expr % Expr
Red39: ^ Stmt do { if ( Expr || Bexp
Red22: ^ Stmt do { if ( Expr || Expr
Shift: ^ Stmt do { if ( Expr || Expr ==
Shift: ^ Stmt do { if ( Expr || Expr == 0
Red19: ^ Stmt do { if ( Expr || Expr == Atom
Red20: ^ Stmt do { if ( Expr || Expr == Expr
Red27: ^ Stmt do { if ( Expr || Bexp
Red22: ^ Stmt do { if ( Expr || Expr
Red34: ^ Stmt do { if ( Bexp
Red22: ^ Stmt do { if ( Expr
Shift: ^ Stmt do { if ( Expr )
Red26: ^ Stmt do { if Pexp
Red21: ^ Stmt do { if Expr
Shift: ^ Stmt do { if Expr {
Shift: ^ Stmt do { if Expr { print
Shift: ^ Stmt do { if Expr { print "2️⃣> "
Shift: ^ Stmt do { if Expr { print "2️⃣> " number
Red18: ^ Stmt do { if Expr { print "2️⃣> " Atom
Red20: ^ Stmt do { if Expr { print "2️⃣> " Expr
Shift: ^ Stmt do { if Expr { print "2️⃣> " Expr ;
Red08: ^ Stmt do { if Expr { Prnt
Red03: ^ Stmt do { if Expr { Stmt
Shift: ^ Stmt do { if Expr { Stmt }
Red13: ^ Stmt do { Cond
Shift: ^ Stmt do { Cond elif
Shift: ^ Stmt do { Cond elif (
Shift: ^ Stmt do { Cond elif ( number
Red18: ^ Stmt do { Cond elif ( Atom
Red20: ^ Stmt do { Cond elif ( Expr
Shift: ^ Stmt do { Cond elif ( Expr %
Shift: ^ Stmt do { Cond elif ( Expr % 5
Red19: ^ Stmt do { Cond elif ( Expr % Atom
Red20: ^ Stmt do { Cond elif ( Expr % Expr
Red39: ^ Stmt do { Cond elif ( Bexp
Red22: ^ Stmt do { Cond elif ( Expr
Shift: ^ Stmt do { Cond elif ( Expr ==
Shift: ^ Stmt do { Cond elif ( Expr == 0
Red19: ^ Stmt do { Cond elif ( Expr == Atom
Red20: ^ Stmt do { Cond elif ( Expr == Expr
Red27: ^ Stmt do { Cond elif ( Bexp
Red22: ^ Stmt do { Cond elif ( Expr
Shift: ^ Stmt do { Cond elif ( Expr )
Red26: ^ Stmt do { Cond elif Pexp
Red21: ^ Stmt do { Cond elif Expr
Shift: ^ Stmt do { Cond elif Expr {
Shift: ^ Stmt do { Cond elif Expr { print
Shift: ^ Stmt do { Cond elif Expr { print "1️⃣> "
Shift: ^ Stmt do { Cond elif Expr { print "1️⃣> " number
Red18: ^ Stmt do { Cond elif Expr { print "1️⃣> " Atom
Red20: ^ Stmt do { Cond elif Expr { print "1️⃣> " Expr
Shift: ^ Stmt do { Cond elif Expr { print "1️⃣> " Expr ;
Red08: ^ Stmt do { Cond elif Expr { Prnt
Red03: ^ Stmt do { Cond elif Expr { Stmt
Shift: ^ Stmt do { Cond elif Expr { Stmt }
Red14: ^ Stmt do { Cond Elif
Shift: ^ Stmt do { Cond Elif else
Shift: ^ Stmt do { Cond Elif else {
Shift: ^ Stmt do { Cond Elif else { print
Shift: ^ Stmt do { Cond Elif else { print "🔘> "
Shift: ^ Stmt do { Cond Elif else { print "🔘> " number
Red18: ^ Stmt do { Cond Elif else { print "🔘> " Atom
Red20: ^ Stmt do { Cond Elif else { print "🔘> " Expr
Shift: ^ Stmt do { Cond Elif else { print "🔘> " Expr ;
Red08: ^ Stmt do { Cond Elif else { Prnt
Red03: ^ Stmt do { Cond Elif else { Stmt
Shift: ^ Stmt do { Cond Elif else { Stmt }
Red15: ^ Stmt do { Cond Elif Else
Red10: ^ Stmt do { Ctrl
Red04: ^ Stmt do { Stmt
Shift: ^ Stmt do { Stmt number
Shift: ^ Stmt do { Stmt number =
Shift: ^ Stmt do { Stmt number = number
Red18: ^ Stmt do { Stmt number = Atom
Red20: ^ Stmt do { Stmt number = Expr
Shift: ^ Stmt do { Stmt number = Expr +
Shift: ^ Stmt do { Stmt number = Expr + 1
Red19: ^ Stmt do { Stmt number = Expr + Atom
Red20: ^ Stmt do { Stmt number = Expr + Expr
Red35: ^ Stmt do { Stmt number = Bexp
Red22: ^ Stmt do { Stmt number = Expr
Shift: ^ Stmt do { Stmt number = Expr ;
Red05: ^ Stmt do { Stmt Assn
Red02: ^ Stmt do { Stmt Stmt
Shift: ^ Stmt do { Stmt Stmt }
Shift: ^ Stmt do { Stmt Stmt } while
Shift: ^ Stmt do { Stmt Stmt } while (
Shift: ^ Stmt do { Stmt Stmt } while ( number
Red18: ^ Stmt do { Stmt Stmt } while ( Atom
Red20: ^ Stmt do { Stmt Stmt } while ( Expr
Shift: ^ Stmt do { Stmt Stmt } while ( Expr <=
Shift: ^ Stmt do { Stmt Stmt } while ( Expr <= 10
Red19: ^ Stmt do { Stmt Stmt } while ( Expr <= Atom
Red20: ^ Stmt do { Stmt Stmt } while ( Expr <= Expr
Red31: ^ Stmt do { Stmt Stmt } while ( Bexp
Red22: ^ Stmt do { Stmt Stmt } while ( Expr
Shift: ^ Stmt do { Stmt Stmt } while ( Expr )
Red26: ^ Stmt do { Stmt Stmt } while Pexp
Red21: ^ Stmt do { Stmt Stmt } while Expr
Shift: ^ Stmt do { Stmt Stmt } while Expr ;
Red16: ^ Stmt Dowh
Red11: ^ Stmt Ctrl
Red04: ^ Stmt Stmt
Shift: ^ Stmt Stmt $
Red01: Unit
ACCEPT Unit

*** Running ***
2️⃣> 0
🔘> 1
2️⃣> 2
🔘> 3
2️⃣> 4
1️⃣> 5
2️⃣> 6
🔘> 7
2️⃣> 8
🔘> 9
2️⃣> 10
```