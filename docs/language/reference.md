# Chirp Language Reference

This is simple language reference, to vaguely describe most/all of the features of the language, I will write a specification sometime.

# Entry point

> This feature is heavily subject to change (*02-16-2021*)

Unlike in most other languages, the entry point in chirp isn't a function called main, instead it is declared with the keyword ``entry``. The entry point can have or not have parameters, this is specificed by adding or not adding ``(parameters)`` after the entry point.

```ch
# Is valid
entry{...}

# Is also valid
entry(){...}
```

# Datatypes & Values

> This section is basicaly certain to change

The datatypes in Chirp are very similar to those in C.

| Keyword | Size |
| --- | --- | --- |
| ``int`` | 4 bytes |
| ``float`` | 4 bytes |
| ``double`` | 8 bytes |
| ``char`` | 1 byte |
| ``byte`` | 1 byte |
| ``bool`` | 1 byte |
| ``none`` | 0 byte |

The none datatype is used to represent that there is no datatype, it cannot be used alone. It has to be used with either the ``func`` or ``ptr`` keyword.

The modifiers are the following. Some of these modifiers can be used alone

| Keyword | Standalone | Effect |
| --- | --- | --- |
| ``ptr`` | Yes | Makes it into the size of ptr |
| ``signed`` | No | Default state of variables, make them signed |
| ``unsigned`` | No | Makes the variable's data unsigned |
| ``func`` | No | Is used both to declare a function, and when declaring a function pointer|

Values in Chirps are as follow

| Keyword | Description |
| --- | --- |
|``false``|Boolean value equivalent to 0|
|``true``|Boolean value equivalent to 1|


# Variables

Declaring a variable in Chirp is done like this:
``datatype: identifier``

Defining on the other hand is done like this
``identifier = value``

Example: 
```
int: a = 123
char: b = 'b'
char ptr: c = "chirp" # C-Style string
```

### Pointers

Chirp is very verbose, and so are pointers. When declaring a pointer in chirp, you must use the ptr keyword in the datatype. So for a generic pointer it would br ``ptr: a``, or for a char pointer it would be ``char ptr: a``.

> This part is also like very subject to change

Pointer reference and dereferencing is very verbose. It is done using the ``ref`` and ``deref`` keyword.

```ch
int: a = 123
int ptr: b = ref a
int: c = deref b
```

### Casting

Casting is done using the ``as`` keyword.

```ch
ptr: data = mem.alloc(4)
int: a = deref(data) as(int)
```

> On the ``ref``,``deref``, and ``as``, and be used with or without a ``()`` as they are operators, not functions.

## Functions

Functions are defined with the ``func`` keyword and then the data type. If the function has no return value, it's datatype should be ``none``.

> Having it so if the function has no return value, it would only have to have the ``func`` keyword, is a feature that might get implemented.

Unlike with variables, the datatype in the function is **NOT** followed by a ``:``(semicolon).

```
func int foo(int: a, int: b)
{
    ret a + b
}
```

## Ret

The ``ret`` keyword is exactly what ``return`` does in C.

## Pointer to a function

Just like in C, you can create a pointer to a function in chirp. Chirp being much more verbose.

```ch
func int foo()
{
    return 123;
}

entry
{
    func int ptr: fun = ref foo
    deref fun()
}
```

A function ptr needs to have a ``func`` keyword, which in this case acts as a datatype instead of a keyword (subject to change).