<p align="center"> <img src="https://raw.githubusercontent.com/mahdiware/myLanguage/master/img/icon.jpg" alt="Interpreter-Language" title=Logo" align="center"> </p>

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

## Version
 v1.0:
- Added the print() function.
- Created the example/ directory for testing.
- Applied the MakeFile.
- Fixed bugs in the print() function and added support for printing strings only, integers only, and both.
- Added a variable to store a string.
- Created a new function called 'input' that inserts and updates variable values.

v1.1:
- Updated the input() function and applied it to integers only.
- Added the varstore type to the enum: 'VAR_INT, VAR_STR, VAR_FLT'.
- Expanded support to include floating-point numbers, whereas previously only integers were supported.
- Added the stringToFloat() function in run.c to convert strings to floats and return a float value.

## Sample Output

To start the interpreter, specify the file that contains the code.

Once the file is opened and memory-mapped, the lexer begins its work. The tokens are outputted to standard output, appearing in alternating colors (green and yellow) to clearly indicate the start and end of each token.

If the lexing process succeeds (all tokens are recognized), the parser starts. During each shift or reduce operation, a single line is outputted showing

 the current contents of the parse stack. Non-terminals are displayed in yellow, while terminals are displayed in green. If the parsing process is successful, the parse stack should contain a single non-terminal called "Unit".

The interpreter then starts execution from the root of the tree (which is always "Unit"), executing the tree produced by the parser.


for example these code: 
```
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
```

and excused the code
```bash
$ make
$ ./lang examples/do-while.txt
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
