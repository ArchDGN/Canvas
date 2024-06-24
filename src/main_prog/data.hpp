//
// Created by dell_nicolas on 31/05/24.
//

#pragma once

#ifndef MEINCANVAS_DATA_HPP
#define MEINCANVAS_DATA_HPP

#include <string>
#include <mutex>


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


namespace Quit_Mutex
{
    extern std::mutex mtx;
}
namespace Threads_Mutex
{
    extern std::mutex mtx;
}
namespace Video_Mutex
{
    extern std::mutex mtx;
}
namespace Event_Mutex
{
    extern std::mutex mtx;
}
namespace Button_Mutex
{
    extern std::mutex mtx;
}

#endif //MEINCANVAS_DATA_HPP
