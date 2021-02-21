/*
Contains an enum for each base datatypes, could be in ast.hpp, but
since it's not an ast_node, I prefer having it here
*/
#pragma once

// Almost all of the keywords here, are also keywords in C++ 
// so I for each of them put a _ before.
// Also I'm not sure how we're gonna do classes now that all types are in this enum
// uhm... this a problem for future me xd

enum class dtypename
{
    _int,
    _float,
    _double,
    _char,
    _byte,
    _bool,
    _none
};

enum class dtypemod
{
    _ptr,
    _signed,
    _unsigned,
    _const,
    _func
};