//
// Created by dell_nicolas on 31/05/24.
//

#include "mouse.hpp"

#include <iostream>
#include <SDL2/SDL.h>

/*
    Tout les bouton principaux de la souris sont pris ici

    get_click_statue renvoi true tant que le bouton est appuyé sinon false
        prend SDL_Event Event et le click demande
                                        : SDL_BUTTON_LEFT
                                        : SDL_BUTTON_MIDDLE
                                        : SDL_BUTTON_RIGHT

    les fonction button_pressed renvoient true uniquement au moment du click (une seul fois)
        prend SDL_Event Event

    les fonction Buttons released renvoient true uniquement au moment ou le click est relache (une seul fois)
        prend SDL_Event Event

    get_position prend deux pointer d'entier et les modifie avec la position de la souris
        SDL_Event Event, int *x, int *y

    mouse_wheel renvoie 1 ou -1 selon le sens dans lequel tourne la molette sinon 0 si elle tourne pas
        prend SDL_Event Event


*/

Mouse::Mouse(SDL_Event *event): _event(event)
{
    click_statue.fill(false);
}


bool Mouse::get_click_statue(int code)
{
    //std::cout << click_statue[0] << std::endl;
    switch (code)
    {
        case SDL_BUTTON_LEFT:
            left_button_pressed();
            left_button_released();
            return click_statue[0];

        case SDL_BUTTON_MIDDLE:
            middle_button_pressed();
            middle_button_released();
            return click_statue[1];

        case SDL_BUTTON_RIGHT:
            right_button_pressed();
            right_button_released();
            return click_statue[2];

        default:
            break;
    }

    return false;
}

void Mouse::update_position()
{
    SDL_GetMouseState(&_mouse_x, &_mouse_y);
}

void Mouse::return_position(int *x, int *y)
{
    update_position();

    *x = _mouse_x;
    *y = _mouse_y;
}

bool Mouse::left_button_pressed()
{
    bool is_pressed = false;
    if (SDL_MOUSEBUTTONDOWN == (*_event).type)
    {
        if (SDL_BUTTON_LEFT == (*_event).button.button)
        {
            is_pressed = true;
            click_statue[0] = true;
        }
    }

    return is_pressed;
}

bool Mouse::middle_button_pressed()
{
    bool is_pressed = false;
    if (SDL_MOUSEBUTTONDOWN == (*_event).type)
    {
        if (SDL_BUTTON_MIDDLE == (*_event).button.button)
        {
            is_pressed = true;
            click_statue[1] = true;
        }
    }

    return is_pressed;
}

bool Mouse::right_button_pressed()
{
    bool is_pressed = false;
    if (SDL_MOUSEBUTTONDOWN == (*_event).type)
    {
        if (SDL_BUTTON_RIGHT == (*_event).button.button)
        {
            is_pressed = true;
            click_statue[2] = true;
        }
    }

    return is_pressed;
}

bool Mouse::left_button_released()
{
    bool is_released = false;
    if (SDL_MOUSEBUTTONUP == (*_event).type)
    {
        if (SDL_BUTTON_LEFT == (*_event).button.button)
        {
            is_released = true;
            click_statue[0] = false;
        }
    }

    return is_released;
}

bool Mouse::middle_button_released()
{
    bool is_released = false;
    if (SDL_MOUSEBUTTONUP == (*_event).type)
    {
        if (SDL_BUTTON_MIDDLE == (*_event).button.button)
        {
            is_released = true;
            click_statue[1] = false;
        }
    }

    return is_released;
}

bool Mouse::right_button_released()
{
    bool is_released = false;
    if (SDL_MOUSEBUTTONUP == (*_event).type)
    {
        if (SDL_BUTTON_RIGHT == (*_event).button.button)
        {
            is_released = true;
            click_statue[2] = false;
        }
    }

    return is_released;
}

int Mouse::mouse_wheel()
{
    if (SDL_MOUSEWHEEL == (*_event).type)
        return (*_event).wheel.y;

    return 0;
}
