#pragma once

#include <string>

struct Texture {
    unsigned int id = 0; // Initialize id to 0
    std::string type;
    std::string path;

    // Optionally, you can also provide a default constructor if needed
    Texture() : id(0) {} // This explicit constructor is not necessary given the in-class initializer above
};

