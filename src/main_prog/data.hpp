//
// Created by dell_nicolas on 31/05/24.
//

#pragma once

#ifndef MEINCANVAS_DATA_HPP
#define MEINCANVAS_DATA_HPP

#include <string>
#include <mutex>

namespace Holy_Mutex
{
    extern std::mutex mtx;
}

struct Data
{
    int window_width = 800;
    int window_height = 600;

    const std::string prog_name = "Canvas";
};

enum class Command_option
{
    NO_FPS_LIMIT,

    NONE
};

class Color
{
public:
    int r, g, b;

    // To create personalized colors : Color myColor = Color(r, g, b);
    Color(int r, int g, int b) : r(r), g(g), b(b) {}

};

#endif //MEINCANVAS_DATA_HPP
