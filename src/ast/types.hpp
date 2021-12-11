/*
Contains an enum for each base datatypes, could be in ast.hpp, but
since it's not an ast_node, I prefer having it here
*/
#pragma once

#include <vector>

// Almost all of the keywords here, are also keywords in C++ 
// so I for each of them put a _ before.
// Also I'm not sure how we're gonna do classes now that all types are in this enum
// uhm... this a problem for future me xd

enum class dtypename
{
    _char,
    _byte,
    _int,
    _long,
    _float,
    _double,
    _bool,
    _none,
};

enum class dtypemod : unsigned char
{
    _ptr,
    _signed,
    _unsigned,
    _const,
    _func,
};

// Primary class of a type (influenced by both the base type and modifiers)
enum class dtypeclass
{
    _none,
    _int, // Integral / character
    _float,
    _bool,
    _ptr,
    _func,
};

enum class exprcat
{
    unset, // unknown/unassigned type
    lval, // lvalue, i.e. has memory location
    rval, // rvalue, i.e. a pure value (not tied to any object, can be used as an operand)
    error, // result of an invalid operation
};

struct basic_type
{
    // Type modifiers 'exttp' are stored in reverse order of declaration, for easier manipulation
    // For example, `ptr unsigned char` -> basic_type { .basetp = _char, .exttp = [_unsigned, _ptr] }
    dtypename basetp;
    std::vector<dtypemod> exttp;
    // The `basetp` is considered to be the most nested specifier
    // All valid types must hold the following invariants
    // - `unsigned` & `signed` modifiers may only be applied directly to integral types (see dtypeclass `int`), except for `char`
    // - `unsigned` & `signed` modifiers may appear at most once, and are mutually exclusive
    // - consecutive `const` modifiers cannot appear in the list
    // - `func` & `const` modifiers cannot be applied directly to a `func` specifier
    // - `const` cannot be applied to a `none` type, unless preceeded by a `ptr`
    // - `none` cannot be the top specifier (least nested), unless as return type of a function

    basic_type(dtypename t = dtypename::_none)
        : basetp(t) {}

    dtypeclass to_class() const
    {
        for (auto m : exttp)
        {
            if (m == dtypemod::_ptr)
                return dtypeclass::_ptr;
            if (m == dtypemod::_func)
                return dtypeclass::_func;
        }
        switch (basetp)
        {
            case dtypename::_int:
            case dtypename::_long:
            case dtypename::_char:
            case dtypename::_byte:
                return dtypeclass::_int;
            case dtypename::_float:
            case dtypename::_double:
                return dtypeclass::_float;
            case dtypename::_bool:
                return dtypeclass::_bool;
            case dtypename::_none:
                return dtypeclass::_none;
        }
    }

    bool has_modifier_front(dtypemod mod) const
    {
        return !exttp.empty() and exttp.front() == mod;
    }

    bool has_modifier_back(dtypemod mod) const
    {
        return !exttp.empty() and exttp.back() == mod;
    }

    bool operator==(basic_type const& o) const
    {
        return basetp == o.basetp and exttp == o.exttp;
    }

    bool operator!=(basic_type const& o) const
    {
        return !operator==(o);
    }
};
