# The symbol tracker
As with most compilers & programming languages, Chirp compiler needs to track various names declared by the user program.
It needs to resolve identifiers to variables, parameters, namespaces, functions, types (TODO), etc.
It also needs to track various attributes and properties of symbols, like full access path (if available), whether it is global, etc.
All of that is provided and managed by the symbol tracker.

## Symbols
Symbols keep a record of various program entities. The complete list is as follows:
- Top scope - The root scope of the whole program
- Global variables - Variables that live in the top scope or in namespace scope
- Global functions
- Namespaces
- Nested scopes - Usually unnamed entities that live within some other entity's scope, like compound block scopes.

Every symbol has a parent symbol (except the root symbol, which has none), an optional local name (name that is accessible in the symbol's scope), and an optional global name (which is the path used to access the symbol from global scope).
A symbol can also be tied to a particular AST node which introduced it, so further information can be obtained (like type, kind of symbol, etc.).

## Scope
A scope defines a subset of visible symbols that are referencable at a particular point in a program. No two symbols of the same name (unless they're describing the same symbol) can exist within the same scope's local set.

A scope's local set specifies symbols and hides (temporary exclusions of symbols from the scope), which are unique to that scope. A name defined in this set is not visible outside of it, but it's visible in subscopes defined within this scope (unless a subscope hides it).

A name shadowing occurs when a scope defines a symbol (or lack therof, i.e. a hide) with the same name as a name in an enclosing scope.

Scopes are tied to symbols (which can be unnamed and local). This allows for lookup within a scope when a name is encountered.

## Name lookup
There are two kinds of lookup: unqualified and qualified. An unqualified lookup usually occurs when an identefier is to be resolved.
Qualified lookup occurs when a name is to be resolved in the scope of another symbol.

When a qualified identifier is looked up, the first part of the indentifier is resolved by means of unqualified lookup, and then the following parts are resolved in scope of the previous resolved symbol by qualified lookup.

### Unqualified lookup
When an unqualified lookup occurs, the following steps are taken. All scopes, starting at the current nested scope, are examined in the order of less and less nested, for the searched name. When a symbol is found (or a lack of one is explicitly defined in a scope), the lookup stops, and the result (whether a symbol was found or not) is returned. When the search reaches the end of list of scopes, the lookup fails with no symbol returned.

### Qualified lookup
Qualified lookup considers only the scope within it occurs. When no name is defined, lookup fails with no symbol. When a name is found, the result (a symbol or not) becomes the result of the lookup.

# Tracker API
The tracker tracks all symbols used in the file currently processed. The first symbol corresponds to the root of the program syntax tree.

Creating and binding symbols is done with these instance methods:

```c++
symbol* decl_sym();
symbol* decl_sym(identifier const& name, decl& target);
```
This method creates a new symbol, and optionally assigns a name (makes it named: see `has_name`) and a target node it.

```c++
bool bind_sym(symbol* sym);
```
This method binds a symbol to the current scope. It returns true on success. On failure, returns false & reports the proper diagnostics where appropiate.

Looking up symbols is done through following instance methods:

```c++
symbol* find_sym_cur(identifier const& name);
```
This is a low-level function that only searches the current scope. If the symbol is not found within the current scope, returns null.

```c++
symbol* lookup_sym(identifier const& name);
symbol* lookup_sym_qual(qual_identifier const& name);
```
These two methods perform unqualified and qualified lookup, respectively, on identifiers. The first one doesn't report diagnostics on failure, but the second one does.

```c++
symbol* lookup_decl_sym(decl const& decl_scope, identifier const& name);
```
This low-level function performs a qualified lookup inside the given symbol to find a name. It considers only the scope of provided symbol. It reports diagnostics on failure.

These functions deal with scopes:

```c++
void push_scope(symbol* sym);
```
This method creates and enters a new scope, described by the provided symbol.

```c++
void pop_scope();
```
This method exits current nested scope and goes back to the one directly embedding it. Exiting the main program (global) scope is undefined.

## Symbol attributes
Each symbol has a following set of attributes
| Name | Type | Default value | Description |
| --- | --- | --- | :-- |
| `has_name` | `bool` | `false` | Has a `name` |
| `is_global` | `bool` | `true` | Lives in global scope (can be potentially exported) |
| `has_storage` | `bool` | `false` | Defines a concrete entity that exists in produced object code (variables, functions) |
| `is_entry` | `bool` | `false` | Is an entry declaration |
| `is_scope` | `bool` | `false` | Defines a scope (see: [Scope](#scope)) |
