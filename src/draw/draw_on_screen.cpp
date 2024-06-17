//
// Created by dell_nicolas on 31/05/24.
//

#include "draw_on_screen.hpp"

Draw_on_screen::Draw_on_screen(SDL_Renderer *renderer, SDL_Rect *rect, int *w, int *h)
    : m_renderer(renderer), m_rect(rect), m_window_width(w), m_window_height(h)
{
    set_color(Color(255, 255, 255), 255);
}


void Draw_on_screen::set_color(Color color, int alpha)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, alpha);
}


void Draw_on_screen::draw_rectangle(int x, int y, int width, int height, Color color, int alpha, int radius)
{
    /*
    x : coord d'origine x
    y : coord d'origine y
    width : longueur
    height : largeur
    radius : épaisseur de la bordure, si radius == width ou radius == height alors le rectangle est plein
    Square_Color : couleur
    */

    set_color(color, alpha);

    if (radius == -1) {
        radius = std::min(width, height) / 2;
    } else if (radius > width / 2 || radius > height / 2) {
        radius = std::min({radius, width / 2, height / 2});
    }

    for (int i = 1; i <= radius; i++)
    {
        m_rect->x = x + i;
        m_rect->y = y + i;
        m_rect->w = width - 2 * i;
        m_rect->h = height - 2 * i;
        SDL_RenderDrawRect(m_renderer, m_rect);
    }

    set_default_font_color();
}


void Draw_on_screen::set_default_font_color()
{
    set_color(Color(255, 255, 255), 255);
}