# Chirp Language Reference

This is simple language reference, to vaguely describe most/all of the features of the language, a specification should be written soon. 

# Entry point

> This feature is heavily subject to change (*02-16-2021*)

Unlike in most other languages, the entry point in chirp isn't a function called main, instead it is declared with the keyword ``entry``. The entry point can have or not have parameters, this is specificed by adding or not adding ``(parameters)`` after the entry point.

```ch
# Is valid
entry{...}

# Is also valid
entry(){...}
```

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