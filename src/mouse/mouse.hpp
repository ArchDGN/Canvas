//
// Created by dell_nicolas on 31/05/24.
//

#ifndef MEINCANVAS_MOUSE_HPP
#define MEINCANVAS_MOUSE_HPP


#include <SDL2/SDL.h>
#include <array>
#include <memory>

#include "../event_handler_for_multi_threads/event.hpp"

class Mouse
{
private:
    std::array<bool, 3> click_statue{};

    int _mouse_x{};
    int _mouse_y{};

    SDL_Event *_event{};

    //std::unique_ptr<Event_queue> *m_event_handler;

public:
    Mouse() = delete;
    explicit Mouse(SDL_Event *event);
    ~Mouse() = default;

    void return_position(int *x, int *y);
    void update_position();

    bool left_button_pressed();
    bool middle_button_pressed();
    bool right_button_pressed();

    bool left_button_released();
    bool middle_button_released();
    bool right_button_released();

    int mouse_wheel();

    bool get_click_statue(int code);
};


#endif //MEINCANVAS_MOUSE_HPP
