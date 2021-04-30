Chirp v1 specification draft #0

# Purpose

The purpose of this document is to define the syntax and behaviour of the Chirp
programming language.

# Chirp file
A chirp file is any file containing Chirp source code, and should end with thefile extension `.ch`. `.chirp` is also acceptable, but not preferred.A chirp file is composed of a list of *import statements*, and then a list of*top-level declarations*. A chirp file that is to be run as a program must
contain an `entry` function.

## Encoding
A chirp compiler should be able to read source-code written in *UTF-8 Encoding*.

# Top-Level
The top level are statements which are not within any compound statement (alsorefered as *block statements*). There is a limited number of statements which can be on the top-level, they are entry, import, variable declaration & definitions, function definitions, function declarations, class definitions and externs.

## Dependency

Dependency management is always made on the top-level. Two keywords are reserved for dependency related operations, these are `import` and `export`.

### Exporting

By default, classes and methods are hidden from other file. So to have a class or method be accesible from a file that imports it, it needs to be specified as *exported*, via the ``export`` keyword. An ``export`` keyword can be applied before any type of declarative statement.

### Importing

You can import another file via the import statement. Import statements are formulated as such `import "otherfile.ch"`. The ``import`` keyword must be followed by a string literal containing a relative path to the requested file, the filename of a standard library file can also be written, and the compiler should look through relative files before standard library ones.

## Entry Point

The entry point is defined via the `entry` keyword. Following the `entry` keyword is an optional set of parentheses, and a mandatory block statement. Once all top-level variables are finished being initialized, the entry block begins executing - once the entry block is done executing, all static variables are destroyed and then the program stops executing.


## Top-Level Variable definition

A *top-level variable definition* symbolically declares a variable that can be read at least within the same file. *Top-level variables* are always constant, even though the `const` keyword may not be written.

A *top-level variable definition* uses the same syntax as a normal *variable definition*. A *top-level variable* is initialized (the expression whose value it is set equal to is evaluated) before the *entry* function begins. Its value cannot be changed, and any expression which attempts to change its value is semantically incorrect.

Unlike local variable statements, global variables can be *exported* from one file to another, which makes them usable from files other than those in which they are declared. An *exported* global variable is defined with the `export` keyword before the variable declaration.

## Top-Level Function

A *top-level function definition* is composed of a 

### Reserved words

The following is a list of all words that cannot be used as identifiers:
--

```as
bool
byte
char
class
const
deref
double
else
elif
entry
export
false
float
func
if
import
int
long
none
private
ptr
public
ref
return
signed
true
typename
unsigned
while
```

> This part describe mostly behaviour

# Scopes

Scopes are created??

# Data types

Datatypes are used to describe the size of a object. A datatype always has a typename, and can have type modifiers. Some modifiers such as ``ptr`` can be used standalone, and act as a standalone type. When standalone, ``ptr`` will declare the object as a generic pointer.

Typenames:

| Keyword | Size |
| --- | --- |
| `int` | 4 bytes |
| `float` | 4 bytes |
| `double` | 8 bytes |
| `char` | 1 byte |
| `byte` | 1 byte |
| `bool` | 1 byte |
| `none` | 0 bytes |

The `none` keyword cannot be used within a variable definition. It can only be used in function definitions to specify that a function has no return.

Type modifier:

| Keyword | Standalone | Effect |
| --- | --- | --- |
| `ptr` | Yes | Makes the variable point to an object |
| `signed` | No | Default state of variables, make them signed |
| `unsigned` | No | Makes the variable's data unsigned |
| `const` | No | Makes the variable unmodifiable |
| `func` | No | Is used both to declare a function, and when declaring a function pointer |

The `ptr` data modifier can be used alone to specifiy a generic pointer (like `void*` in C)

The `func` data modifier is both used as the keyword to declare/define a function, and as a specifier when creating function pointers.

# Operator precedence

| Symbol | Precedence | Associativity | Notes |
| --- | --: | :-: | :-- |
| `()` | 0 | | Technically not an operator, but resets precedence |
| `f(...)` | 1 | left-to-right | Function call |
| `as`, `deref`, `ref` | 2 | right-to-left | Unary |
| `*`, `/` | 3 | left-to-right |
| `+`, `-` | 4 | left-to-right |
| `,` | 5 | left-to-right | Not implemented, also required for lists |
