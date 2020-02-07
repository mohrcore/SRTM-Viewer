#pragma once

#include <fstream>

template <typename E, typename U>
struct TaggedUnion {
    E type;
    U content;
    TaggedUnion()
        : content { nullptr } {}
    TaggedUnion(E t, U c)
        : type(t), content(c) {}
};

size_t getStreamSize(std::istream & stream);