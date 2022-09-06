#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <map> // for new_list_from_map
#include "../shared/array_view.hpp"

namespace bonk
{
    enum value_type : unsigned char
    {
        error_t = 0,
        none_t,
        list_t,
        int_t,
        string_t,
        bool_t,
    };

    class value
    {
        public:

        value(const value&) = delete;

        ~value();

        friend std::ostream& operator<<(std::ostream&, const value&);

        value_type type;
    };

    using handle = std::unique_ptr<value>;

    class int_value : public value
    {
        public:
        long value;
    };

    class bool_value : public value
    {
        public:
        bool value;
    };

    class string_value : public value
    {
        public:
        size_t length;

        std::string_view data() const
        {
            return { reinterpret_cast<const char*>(&length) + sizeof length, length };
        }
    };

    using string = std::unique_ptr<string_value>;

    class list_value : public value
    {
        public:
        size_t length;

        array_view<const handle> elements() const
        {
            return { reinterpret_cast<const handle*>(reinterpret_cast<const char*>(&length) + sizeof length), length };
        }

        array_view<const string> names() const
        {
            const string* p = reinterpret_cast<const string*>(reinterpret_cast<const char*>(&length) + sizeof length + sizeof(handle) * length);
            if (*p)
                return { p, length };
            return {};
        }

        const value* lookup_name(std::string_view key) const;
        const value* lookup_index(size_t index) const;
    };

    using list = std::unique_ptr<list_value>;

    handle new_int(long value);
    string new_string(std::string_view string);
    handle get_true();
    handle get_false();
    // Values are moved-from, but not names, if present
    list new_list(array_view<handle> values, array_view<std::string> names = {});
    list new_list_from_map(std::map<std::string, handle>&& map);
}
