//
// Created by dell_nicolas on 31/05/24.
//

#ifndef MEINCANVAS_BUTTONS_HPP
#define MEINCANVAS_BUTTONS_HPP


#include <SDL2/SDL.h>
#include<string>
#include<vector>
#include<memory>
#include <iostream>
#include <functional>

#include "../mouse/mouse.hpp"

class Buttons
{
private:

    int *m_window_width;
    int *m_window_height;

    std::unique_ptr<Mouse> *m_mouse_control;

    struct Button {
        std::string id;
        int x;
        int y;
        int w;
        int h;
        std::function<void()> pointer_to_function;
    };

    std::vector<Button> m_button_list;


public:
    Buttons() = delete;
    explicit Buttons(std::unique_ptr<Mouse> *mouse, int *window_width, int *window_height);
    ~Buttons() = default;

    bool create_button_by_id(std::string id, int x, int y, int w, int h, std::function<void()> pointer_to_function);
    bool edit_button_by_id(const std::string& id, int x, int y, int w, int h, const std::function<void()>& pointer_to_function);
    bool delete_button_by_id(const std::string& id);
    Button* return_button_by_id(const std::string& id);
    [[nodiscard]] int return_button_list_size() const;

    bool is_button_clicked(Button *button);
    bool check_all_buttons_clicked();


};


#endif //MEINCANVAS_BUTTONS_HPP
