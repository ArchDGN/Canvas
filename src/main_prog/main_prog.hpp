// File: Main_prog.cpp

#ifndef MEINCANVAS_MAIN_PROG_HPP
#define MEINCANVAS_MAIN_PROG_HPP

#include <SDL2/SDL.h>

#include <iostream>
#include <thread>
#include <memory>

#include "data.hpp"
#include "../draw/draw_on_screen.hpp"
#include "../mouse/mouse.hpp"
#include "../buttons/buttons.hpp"
#include "../threads_workers/threads_workers.hpp"
#include "../event_handler_for_multi_threads/event.hpp"
#include "../video/video.hpp"

namespace Mein_canvas {


    class Main_prog {

    private:
        int *m_window_width{};
        int *m_window_height{};
        const std::string *m_prog_name{};

        SDL_Event m_event{};
        bool m_quit;

        SDL_Window *m_prog_window;
        SDL_Renderer *m_renderer;
        SDL_Rect m_rect{};

        Data m_common_data;

        std::unique_ptr<Event_queue> m_event_control;
        std::unique_ptr<Draw_on_screen> m_draw_on_window;
        std::unique_ptr<Mouse> m_mouse_control;
        std::unique_ptr<Buttons> m_button_control;
        std::unique_ptr<ThreadsWorkers> m_threads_workers;
        std::unique_ptr<Video> m_video;

    private:
        static void limit_fps_of(bool &quit, float fps, const std::function<void()> &function);
        static void limit_fps_of(bool &quit, const std::function<void()> &function);

        static void get_error(const std::string &error, const std::string &error_log, int code);

        void display_on_screen();
        void set_up_main_workers();

    public:
        Main_prog();

        void run();

        ~Main_prog();

    };

}   // Mein_canvas


#endif //MEINCANVAS_MAIN_PROG_HPP
