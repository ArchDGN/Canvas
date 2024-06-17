//
// Created by dell_nicolas on 07/06/24.
//

#include "event.hpp"


void Event_queue::poll_events()
{
    SDL_Event event;
    // SDL_PollEvent renvoie 1 si un événement est disponible et le met dans 'event'
    if (SDL_PollEvent(&event)) {
        // Verrouille le mutex pour protéger l'accès aux données partagées
        std::unique_lock<std::mutex> lock(Holy_Mutex::mtx);
        // Met à jour le dernier événement et indique qu'un nouvel événement est disponible
        last_event = event;
        new_event_available = true;
        // Déverrouille le mutex
        lock.unlock();
        // Notifie tous les threads en attente qu'un nouvel événement est disponible
        cond_var.notify_all();
    }
}

void Event_queue::get_event(SDL_Event* event, bool &quit)
{
    // Verrouille le mutex pour protéger l'accès aux données partagées
    std::unique_lock<std::mutex> lock(Holy_Mutex::mtx);
    // Attend qu'un nouvel événement soit disponible ou que le programme soit en train de se terminer
    while (!new_event_available && !quit)
    {
        cond_var.wait(lock);
    }
    // Si le programme est en train de se terminer, sort de la fonction
    if (quit)
    {
        return;
    }
    // Indique qu'il n'y a plus de nouvel événement disponible
    new_event_available = false;
    // Récupère le dernier événement
    *event = last_event;
}

void Event_queue::notify_all() {
    // Verrouille le mutex pour protéger l'accès aux données partagées
    std::unique_lock<std::mutex> lock(Holy_Mutex::mtx);
    // Indique qu'un nouvel événement est disponible
    new_event_available = true;
    // Notifie tous les threads en attente qu'un nouvel événement est disponible
    cond_var.notify_all();
}



