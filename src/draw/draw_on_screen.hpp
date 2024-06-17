//
// Created by dell_nicolas on 31/05/24.
//

#ifndef MEINCANVAS_DRAW_ON_SCREEN_HPP
#define MEINCANVAS_DRAW_ON_SCREEN_HPP

#include <SDL2/SDL.h>

#include <algorithm>

#include "../main_prog/data.hpp"

class Draw_on_screen {

private:
    SDL_Renderer *m_renderer;
    SDL_Rect *m_rect;
    int *m_window_width, *m_window_height;

public:
    Draw_on_screen() = delete;
    Draw_on_screen(SDL_Renderer *renderer, SDL_Rect *rect, int *w, int *h);
    ~Draw_on_screen() = default;

    void set_color(Color color, int alpha = 255);
    void set_default_font_color();

    void draw_rectangle(int x, int y, int width, int height, Color color, int alpha = 255, int radius = -1);
};


#endif //MEINCANVAS_DRAW_ON_SCREEN_HPP
