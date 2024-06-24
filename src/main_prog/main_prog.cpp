//
// Created by dell_nicolas on 31/05/24.
//

#include "main_prog.hpp"


// Le Saint Mutex protège les variables partagées entre les threads
namespace Quit_Mutex
{
    std::mutex mtx;
}

namespace Mein_canvas {


    void Main_prog::run()
    {
        // On lance les threads
        set_up_main_workers();

        // Exemple de chargement de vidéo
        m_video->load_video_with_id("video", "b.mp4", {"--no-xlib", "--no-audio"});

        bool clicked = false;
        m_button_control->create_button_by_id("button", 0, 0, 100, 100, [this, &clicked]() {
            if (clicked)
                return;

            // Buffer pour stocker le nom du thread
            char thread_name[16]; // La taille maximale du nom du thread est de 16 caractères sous Linux

            // Obtenir le nom du thread
            if(pthread_getname_np(pthread_self(), thread_name, sizeof(thread_name)) == 0) {
                std::cout << "Executed in thread: " << thread_name << std::endl;
            } else {
                std::cout << "Failed to get thread name" << std::endl;
            }
            m_video->edit_video_with_id("video", {0, 0, *m_window_width / 2, *m_window_height / 2});
            std::cout << m_video->get_number_of_video() << std::endl;
            m_video->load_video_with_id("video2", "e.mkv");
            std::cout << m_video->get_number_of_video() << std::endl;
            m_video->edit_video_with_id("video2", {*m_window_width / 2, *m_window_height / 2, *m_window_width / 2, *m_window_height / 2});

            clicked = true;
        });

        // Boucle principale, on limite le nombre d'itérations par seconde a 120
        limit_fps_of(m_quit, 120.0, [this]() {
            if (m_event.type == SDL_QUIT)
            {
                std::lock_guard<std::mutex> lock(Quit_Mutex::mtx);
                m_quit = true;
            } else {
                SDL_GetWindowSize(m_prog_window, m_window_width, m_window_height);
            }

            SDL_RenderClear(m_renderer);

            // On affiche les vidéos
            m_video->display_video_all_video();

            // Code de dessin ici
            display_on_screen();

            SDL_RenderPresent(m_renderer);
        });

        // On stoppe les vidéos
        m_video->stop_all_video();
    }


    void Main_prog::display_on_screen()
    {
        // Dessin du rectangle
        m_draw_on_window->set_color(Color(100, 100, 100));

        //
        //TODO: On dessuine la fenetre ici



        // Fin du dessin de la fenetre
        //
    }


    void Main_prog::set_up_main_workers()
    {
        // Thread Event qui gère les événements
        m_threads_workers->create_worker_by_id("event", [this]() {
            pthread_setname_np(pthread_self(), "prog-event");
            limit_fps_of(m_quit, 512.0, [this]() {
                m_event_control->poll_events();
            });
        }, true, false);

        // Thread Event qui recupère les événements
        m_threads_workers->create_worker_by_id("event2", [this]() {
            pthread_setname_np(pthread_self(), "prog-event2");
            limit_fps_of(m_quit, 512.0, [this]() {
                m_event_control->get_event(&m_event, m_quit);
            });
        }, true, false);

        // Thread Event update les boutons
        m_threads_workers->create_worker_by_id("refresh-buttons", [this]() {
            pthread_setname_np(pthread_self(), "prog-buttons");
            limit_fps_of(m_quit, 120.0, [this]() {
                m_button_control->check_all_buttons_clicked();
            });
        }, true, false);

        // Thread principal des workers, il lance les autres workers
        std::thread run_worker_thread([this]() {
            pthread_setname_np(pthread_self(), "prog-workers");
            limit_fps_of(m_quit, 512.0, [this]() {
                m_threads_workers->run_workers();
            });

            m_threads_workers->quit_all_threads();
        });
        run_worker_thread.detach();
    }


    Main_prog::Main_prog()
    {
        // Initialisation des variables
        m_window_width = &m_common_data.window_width;
        m_window_height = &m_common_data.window_height;
        m_prog_name = &m_common_data.prog_name;


        if (SDL_Init(SDL_INIT_VIDEO) != 0)      // SDL init_prog_var
            get_error("Erreur lors de l'initialisation de SDL : ", SDL_GetError(), -1);

        // Passe le format du nom de la fenetre de std::string a const char *
        m_prog_window = SDL_CreateWindow(m_prog_name->c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                         *m_window_width, *m_window_height,
                                         SDL_WINDOW_SHOWN);     // Creation de la fenetre
        if (!m_prog_window)
            get_error("Erreur lors de la création de la fenêtre : ", SDL_GetError(), 0);
        SDL_SetWindowResizable(m_prog_window, SDL_TRUE);
        SDL_SetWindowMinimumSize(m_prog_window, 400, 400);

        m_renderer = SDL_CreateRenderer(m_prog_window, -1, SDL_RENDERER_SOFTWARE);     // Creation du render
        if (!m_renderer) {
            SDL_DestroyWindow(m_prog_window);
            get_error("Erreur lors de la création du renderer : ", SDL_GetError(), 0);
        }

        // Initialisation des classes
        m_event_control = std::make_unique<Event_queue>();
        m_draw_on_window = std::make_unique<Draw_on_screen>(m_renderer, &m_rect, m_window_width, m_window_height);
        m_mouse_control = std::make_unique<Mouse>(&m_event);
        m_button_control = std::make_unique<Buttons>(&m_mouse_control, m_window_width, m_window_height);
        m_threads_workers = std::make_unique<ThreadsWorkers>();
        m_video = std::make_unique<Video>(m_renderer);


        m_quit = false;  // Init de la variable qui permet de quitter le programme, une fois a "true" la boucle while principale se coupe et le programme s'arrete
    }


    Main_prog::~Main_prog()
    {
        // On notifie le thread Event pour qu'il se termine
        m_event_control->notify_all();

        // On clear le render
        if (m_renderer)
        {
            SDL_RenderClear(m_renderer);
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }
        // On detruit la fenetre
        if (m_prog_window)
        {
            SDL_DestroyWindow(m_prog_window);
            m_prog_window = nullptr;
        }

        SDL_Quit();
    }


    void Main_prog::limit_fps_of(bool &quit, float fps, const std::function<void()> &function) {
        // Calcul du délai entre chaque frame en millisecondes
        auto delay = std::chrono::milliseconds(static_cast<long>(1000.0 / fps));

        // Boucle principale
        while (!quit && fps > 0.0) {
            //
            // Cette partie de code permet de calculer le nombre d'iteration par seconde
            // Et de limiter le nombre d'iteration par seconde a 120
            auto start = std::chrono::high_resolution_clock::now();


            // Exécution de la fonction
            function();


            // Enregistrement du moment de fin
            auto end = std::chrono::high_resolution_clock::now();

            // Calcul du temps écoulé
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            // Si le temps écoulé est inférieur au délai, on fait une pause pour atteindre le délai
            if (elapsed < delay) {
                std::this_thread::sleep_for(delay - elapsed);
            }
        }

        while (!quit && fps < 0.0) {
            function();
        }
    }

    void Main_prog::limit_fps_of(bool &quit, const std::function<void()> &function)
    {
        // On appelle la fonction limit_fps_of avec un nombre d'itérations par seconde illimité
        limit_fps_of(quit, -1.0, function);
    }


    void Main_prog::get_error(const std::string &error, const std::string &error_log, int code)
    {
        std::cerr << error << error_log << std::endl;   // Affiche l'erreur ainsi que le log SDL associe

        if (code != -1)     // Si l'erreur survient a l'initialisation de la sdl alors le code renvoyé est -1
            SDL_Quit();     // Il n'y a donc pas besoin de quitter la sdl car elle na pas put s'initialiser
                            // Si la sdl a put s'initialiser et qu'une erreur survient apres le code renvoye est 0
        exit(-1);
    }

}   // Mein_canvas
