//
// Created by dell_nicolas on 07/06/24.
//

#ifndef MEINCANVAS_EVENT_HPP
#define MEINCANVAS_EVENT_HPP


#include <condition_variable>
#include <SDL2/SDL.h>

#include "../main_prog/data.hpp"


class Event_queue {
private:
    SDL_Event last_event{};
    bool new_event_available = false;
    std::condition_variable cond_var;

public:
    Event_queue() = default;
    ~Event_queue() = default;

    void poll_events();
    void get_event(SDL_Event* event, bool &quit);
    void notify_all();
};


#endif //MEINCANVAS_EVENT_HPP
