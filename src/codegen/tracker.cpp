#include "tracker.hpp"

void tracker::init()
{
    this->depth = 0;
    this->entry_set = false;
}

bool tracker::register_var()
{
    return false;
}

bool tracker::request_entry()
{
    if(this->entry_set)
    {
        return false;
    }
    else
    {
        this->entry_set = true;
        return true;
    }
}