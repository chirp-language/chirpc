#pragma once

#include <memory>

namespace bonk
{
    class value
    {
        public:
        value();

        value(int t, void* d);

        // Copy constructor
        value(const value&);

        ~value();

        friend std::ostream& operator<<(std::ostream&, const value&);

        // 0 - error
        // 1 - none
        // 2 - list
        // 3 - int
        // 4 - string
        // 5 - bool

        void* data;
        int type;
    };
}