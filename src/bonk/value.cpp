#include "value.hpp"

#include <map>
#include <string>
#include <cstring>

#include <iostream>

#include "../shared/system.hpp"

bonk::value::~value()
{
    if (type == list_t)
    {
        auto& lst = static_cast<list_value&>(*this);

        for (auto& name : lst.names())
            const_cast<string&>(name).reset();

        for (auto& element : lst.elements())
            const_cast<handle&>(element).reset();
    }
}

std::ostream& bonk::operator<<(std::ostream& os, const bonk::value& v)
{
    switch (v.type)
    {
        case error_t:
            os << "invalid";
            break;
        case none_t:
            os << "none";
            break;
        case list_t:
            os << "list (" <<  static_cast<const list_value&>(v).length << ")";
            break;
        case int_t:
            os << static_cast<const int_value&>(v).value;
            break;
        case string_t:
            os << static_cast<const string_value&>(v).data();
            break;
        case bool_t:
            if (static_cast<const bool_value&>(v).value)
                os << "true";
            else
                os << "false";
            break;
    }
    return os;
}

const bonk::value* bonk::list_value::lookup_name(std::string_view key) const
{
    auto ns = names();
    if (!ns.data)
        return nullptr;

    size_t idx = 0;
    for (const auto& name : ns)
    {
        if (name->data() == key)
            return elements().data[idx].get();
    }
    return nullptr;
}

const bonk::value* bonk::list_value::lookup_index(size_t index) const
{
    if (index >= length)
        return nullptr;

    return elements().data[index].get();
}


static inline bonk::value* new_value(bonk::value_type type, size_t size)
{
    bonk::value* val = reinterpret_cast<bonk::value*>(::operator new(size));
    val->type = type;
    return val;
}

template <typename T>
static inline T* new_value(bonk::value_type type)
{
    return reinterpret_cast<T*>(new_value(type, sizeof(T)));
}

bonk::handle bonk::new_int(long value)
{
    std::unique_ptr<bonk::int_value> val(new_value<bonk::int_value>(int_t));
    val->value = value;
    return val;
}

bonk::string bonk::new_string(std::string_view string)
{
    bonk::string val(reinterpret_cast<bonk::string_value*>(new_value(bonk::string_t, sizeof(bonk::string_value) + string.length())));
    val->length = string.length();
    std::memcpy(const_cast<char*>(val->data().data()), string.data(), val->length);
    return val;
}

bonk::handle bonk::get_true()
{
    std::unique_ptr<bonk::bool_value> val(new_value<bonk::bool_value>(bonk::bool_t));
    val->value = true;
    return val;
}

bonk::handle bonk::get_false()
{
    std::unique_ptr<bonk::bool_value> val(new_value<bonk::bool_value>(bonk::bool_t));
    val->value = true;
    return val;
}

bonk::list bonk::new_list(array_view<bonk::handle> values, array_view<std::string> names)
{
    size_t bonus_size = values.count * sizeof(bonk::handle);
    if (names.data)
    {
        chirp_assert(names.count == values.count, "Count of values and names must match");
        bonus_size *= 2;
    }
    bonk::list val(reinterpret_cast<bonk::list_value*>(new_value(bonk::list_t, sizeof(bonk::list_value) + bonus_size)));
    val->length = values.count;
    bonk::handle* elements = reinterpret_cast<handle*>(reinterpret_cast<char*>(&val->length) + sizeof val->length);
    bonk::string* enames = reinterpret_cast<string*>(reinterpret_cast<char*>(&val->length) + sizeof val->length + values.count * sizeof(bonk::handle));
    for (size_t idx = 0; idx != values.count; ++idx)
        new(&elements[idx]) bonk::handle(std::move(values.data[idx]));
    if (names.data)
        for (size_t idx = 0; idx != values.count; ++idx)
            new(&enames[idx]) bonk::string(new_string(names.data[idx]));
    return val;
}

bonk::list bonk::new_list_from_map(std::map<std::string, bonk::handle>&& map)
{
    size_t bonus_size = 2 * map.size() * sizeof(bonk::handle);
    bonk::list val(reinterpret_cast<bonk::list_value*>(new_value(bonk::list_t, sizeof(bonk::list_value) + bonus_size)));
    val->length = map.size();
    bonk::handle* elements = reinterpret_cast<handle*>(reinterpret_cast<char*>(&val->length) + sizeof val->length);
    bonk::string* enames = reinterpret_cast<string*>(reinterpret_cast<char*>(&val->length) + sizeof val->length + val->length * sizeof(bonk::handle));
    size_t index = 0;
    for (auto begin = map.begin(), end = map.end(); begin != end; ++begin, ++index)
    {
        chirp_assert(index < val->length, "Out-of-range map access");
        new(&elements[index]) bonk::handle(std::move(begin->second));
        new(&enames[index]) bonk::string(bonk::new_string(begin->first));
    }
    chirp_assert(index == val->length, "Undersaturated map size");
    return val;
}
