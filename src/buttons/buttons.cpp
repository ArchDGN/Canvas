//
// Created by dell_nicolas on 31/05/24.
//

#include "buttons.hpp"
#include "../main_prog/data.hpp"

#include <algorithm>
#include <utility>


Buttons::Buttons(std::unique_ptr<Mouse> *mouse, int *window_width, int *window_height)
{
    m_window_width = window_width;
    m_window_height = window_height;

    m_mouse_control = mouse;
}

bool Buttons::create_button_by_id(std::string id, int x, int y, int w, int h, std::function<void()> pointer_to_function)
{
    // Creation des boutons avec les parametres suivants donnés par l'utilisateur
    Button button;
    button.id = std::move(id);
    button.x = x;
    button.y = y;
    button.w = w;
    button.h = h;
    button.pointer_to_function = std::move(pointer_to_function);

    {
        std::lock_guard<std::mutex> lock(Holy_Mutex::mtx);
        size_t size = m_button_list.size();
        m_button_list.push_back(button);
        size_t new_size = m_button_list.size();

        // Si la taille de la liste a changé, alors on a ajouté un bouton
        if (size != new_size) {
            return true;
        }

        return false;
    }
}

bool Buttons::edit_button_by_id(const std::string& id, int x, int y, int w, int h, const std::function<void()>& pointer_to_function)
{
    // Modification des boutons avec les parametres suivants donnés par l'utilisateur
    // On parcourt la liste des boutons et on modifie celui qui a le meme id que celui donné par l'utilisateur
    std::lock_guard<std::mutex> lock(Holy_Mutex::mtx);
    for (auto &i : m_button_list)
    {
        if (i.id == id)
        {
            i.x = x;
            i.y = y;
            i.w = w;
            i.h = h;
            i.pointer_to_function = pointer_to_function;

            return true;
        }
    }

    return false;

}

bool Buttons::delete_button_by_id(const std::string& id)
{
    // Suppression des boutons avec les parametres suivants donnés par l'utilisateur
    std::lock_guard<std::mutex> lock(Holy_Mutex::mtx);
    size_t size = m_button_list.size();
    m_button_list.erase(std::remove_if(m_button_list.begin(), m_button_list.end(), [id](Button &i){return i.id == id;}), m_button_list.end());
    size_t new_size = m_button_list.size();

    // Si la taille de la liste a changé, alors on a supprimé un bouton
    if (size != new_size)
    {
        return true;
    }

    return false;
}

Buttons::Button* Buttons::return_button_by_id(const std::string& id)
{
    // Retourne le bouton avec l'id donné par l'utilisateur
    // On parcourt la liste des boutons et on retourne celui qui a le meme id que celui donné par l'utilisateur
    std::lock_guard<std::mutex> lock(Holy_Mutex::mtx);
    for (auto &i : m_button_list)
    {
        if (i.id == id)
        {
            return &i;
        }
    }
    return nullptr;
}

bool Buttons::is_button_clicked(Button *button)
{
    // On vérifie si le bouton a été cliqué
    // On récupère la position de la souris
    int x, y;
    (*m_mouse_control)->return_position(&x, &y);

    // Si la position de la souris est dans le rectangle du bouton, alors on appelle la fonction associée au bouton
    if (x > button->x && x < button->x + button->w && y > button->y && y < button->y + button->h)
    {
        button->pointer_to_function();
        return true;
    }

    return false;
}

bool Buttons::check_all_buttons_clicked()
{
    // On vérifie si le ckick de la souris a été pressé
    if (!(*m_mouse_control)->left_button_pressed())
    {
        return false;
    }
    // On vérifie si un bouton a été cliqué
    std::lock_guard<std::mutex> lock(Holy_Mutex::mtx);
    for (auto &i : m_button_list)
    {
        if (is_button_clicked(&i))
        {
            return true;
        }
    }

    return false;
}


int Buttons::return_button_list_size() const {
    // On retourne la taille de la liste des boutons
    std::lock_guard<std::mutex> lock(Holy_Mutex::mtx);
    return int(m_button_list.size());
}