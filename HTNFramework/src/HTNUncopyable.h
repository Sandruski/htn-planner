#pragma once

// Base class to prevent copying
class HTNUncopyable
{
protected:
    // Make constructor and destructor protected to allow construction and destruction
    HTNUncopyable()  = default;
    ~HTNUncopyable() = default;

private:
    // Make copy constructor and copy assignment operator private to prevent copying
    HTNUncopyable(const HTNUncopyable& inOther);
    HTNUncopyable& operator=(const HTNUncopyable& inOther);
};