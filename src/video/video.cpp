//
// Created by dell_nicolas on 14/06/24.
//

#include "video.hpp"

Video::Video(SDL_Renderer *renderer) : m_renderer(renderer) {}


bool Video::load_video_with_id(const std::string &id, const std::string &path, SDL_Rect rect, std::vector<std::string> vec)
{
    // On crée un contexte pour la vidéo
    libvlc_instance_t *vlc_player;
    libvlc_media_t *m;

    // Créez un tableau de pointeurs const char* pour les arguments de VLC
    const char** vlc_argv;
    int vlc_argc;

    // Si le vecteur est vide, on crée un tableau avec les arguments par défaut
    if (vec.empty())
    {
        vlc_argv = new char const *[]{
            //"--no-audio", // Don't play audio.
            "--no-xlib", // Don't use Xlib.
        };
        vlc_argc = 1;
    // Si le vecteur n'est pas vide, on crée un tableau avec les arguments du vecteur
    } else {
        // Créez un nouveau tableau de pointeurs const char* de la taille du vecteur
        vlc_argv = new const char*[vec.size() + 1]; // +1 pour le nullptr à la fin

        // Remplissez le tableau avec les pointeurs vers les chaînes de caractères dans le vecteur
        for (std::size_t i = 0; i < vec.size(); ++i) {
            vlc_argv[i] = vec[i].c_str();
        }

        // Ajoutez un nullptr à la fin du tableau
        vlc_argv[vec.size()] = nullptr;
        vlc_argc = static_cast<int>(vec.size());
    }

    // On crée un contexte pour la vidéo
    std::unique_ptr<loaded_video> context = std::make_unique<loaded_video>();

    // On initialise le contexte avec les valeurs passées en paramètres
    // id pour l'identifiant de la vidéo
    context->id = std::make_unique<std::string>(id);
    // rect pour la position et la taille de la vidéo
    context->rect = std::make_unique<SDL_Rect>(rect);
    // mutex pour protéger les données de la vidéo
    context->mutex = std::unique_ptr<SDL_mutex, std::function<void(SDL_mutex *)>>(SDL_CreateMutex(), SDL_DestroyMutex);

    // Initialise libVLC.
    vlc_player = libvlc_new(vlc_argc, vlc_argv);
    if(nullptr == vlc_player) {
        printf("LibVLC initialization failure.\n");
    }

    // Créez un nouveau objet média
    m = libvlc_media_new_path(vlc_player, path.c_str());

    // On attache un événement à la vidéo pour savoir quand elle est prête
    libvlc_event_manager_t* em = libvlc_media_event_manager(m);
    libvlc_event_attach(em, libvlc_MediaParsedChanged, media_parsed_changed, nullptr);

    // On parse la vidéo
    libvlc_media_parse_with_options(m, libvlc_media_parse_local, -1);

    // On attend que la vidéo soit prête
    while(libvlc_media_get_parsed_status(m) != libvlc_media_parsed_status_done) {
        // Sleeping time
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>(5)));
    }

    // On récupère les informations de la vidéo
    libvlc_media_track_t** tracks = nullptr;
    unsigned int num_tracks = libvlc_media_tracks_get(m, &tracks);

    // On récupère la largeur et la hauteur de la vidéo
    unsigned int w = 0, h = 0;

    for (unsigned int i = 0; i < num_tracks; ++i) {
        if (tracks[i]->i_type == libvlc_track_video) {
            w = tracks[i]->video->i_width;
            h = tracks[i]->video->i_height;
            break;
        }
    }

    // On crée un lecteur pour la vidéo
    context->mp = std::unique_ptr<libvlc_media_player_t, std::function<void(libvlc_media_player_t *)>>(
            libvlc_media_player_new_from_media(m), libvlc_media_player_release
    );
    libvlc_media_tracks_release(tracks, num_tracks);
    libvlc_media_release(m);

    // On crée une texture pour la vidéo
    context->texture = std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture *)>>(
            SDL_CreateTexture(
                    m_renderer,
                    SDL_PIXELFORMAT_BGR565, SDL_TEXTUREACCESS_STREAMING,
                    static_cast<int>(w), static_cast<int>(h)
            ), SDL_DestroyTexture
    );

    // On attache la texture à la vidéo
    libvlc_video_set_callbacks(context->mp.get(), lock, unlock, display, context.get());
    libvlc_video_set_format(context->mp.get(), "RV16", w, h, w*2);
    libvlc_media_player_play(context->mp.get());

    // Changer le niveau du son à 100%
    libvlc_audio_set_volume(context->mp.get(), 100);
    // On désactive les logs de VLC
    libvlc_log_set(vlc_player, log_null, nullptr);

    // On libère la mémoire
    libvlc_release(vlc_player);


    {
        // On protège la liste des vidéos
        std::lock_guard<std::mutex> lock(Holy_Mutex::mtx);
        size_t size = m_loaded_videos.size();
        // On ajoute la vidéo à la liste
        m_loaded_videos.push_back(std::move(context));
        size_t new_size = m_loaded_videos.size();

        // Si la taille de la liste a changé, alors on a ajouté un bouton
        if (size != new_size) {
            return true;
        }
        return false;
    }
}


void Video::log_null([[maybe_unused]] void *data, [[maybe_unused]] int level, [[maybe_unused]] const libvlc_log_t *ctx, [[maybe_unused]] const char *fmt, [[maybe_unused]]va_list args)
{
    // Ne fait rien pour eviter le message libvlc parasite
}


void Video::media_parsed_changed(const libvlc_event_t* event, [[maybe_unused]] void* data)
{
    if (event->u.media_parsed_changed.new_status == libvlc_media_parsed_status_done) {
        // Il se passe rien, le parsing est fini
    }
}


void *Video::lock(void *data, void **p_pixels)
{
    // On récupère le contexte de la vidéo
    auto *c = (loaded_video *)data;

    // On lock le mutex
    int pitch;
    SDL_LockMutex(c->mutex.get());
    SDL_LockTexture(c->texture.get(), nullptr, p_pixels, &pitch);

    return nullptr; // Picture identifier, not needed here.
}

void Video::unlock(void *data, [[maybe_unused]] void *id, [[maybe_unused]] void *const *p_pixels)
{
    // On récupère le contexte de la vidéo
    auto *c = (loaded_video *)data;

    // Ici on peut aussi dessiner des trucs.
    // On peut edit les pixels de la vidéo

    /*uint16_t *pixels = (uint16_t *)*p_pixels;

    // We can also render stuff.
    int x, y;
    for(y = 10; y < 40; y++) {
        for(x = 10; x < 40; x++) {
            if(x < 13 || y < 13 || x > 36 || y > 36) {
                pixels[y * VIDEOWIDTH + x] = 0xffff;
            } else {
                // RV16 = 5+6+5 pixels per color, BGR.
                pixels[y * VIDEOWIDTH + x] = 0x02ff;
            }
        }
    }*/

    // On unlock la texture
    SDL_UnlockTexture(c->texture.get());
    SDL_UnlockMutex(c->mutex.get());
}

void Video::display(void *data, [[maybe_unused]] void *id)
{
    [[maybe_unused]] auto *c = (loaded_video *)data; // On fait rien ici mais c'est utilisé pour le callback
}

void Video::display_video_all_video()
{
    // On affiche toutes les vidéos
    for (auto &video : m_loaded_videos)
    {
        SDL_LockMutex(video->mutex.get());
        SDL_RenderCopy(m_renderer, video->texture.get(), nullptr, video->rect.get());
        SDL_UnlockMutex(video->mutex.get());
    }
}

void Video::stop_all_video()
{
    // On stoppe toutes les vidéos
    for (auto &video : m_loaded_videos)
    {
        libvlc_media_player_stop(video->mp.get());
    }
}

bool Video::edit_video_with_id(const std::string &id, SDL_Rect rect)
{
    // On édite la vidéo avec l'identifiant id
    for (auto &video : m_loaded_videos)
    {
        if (*video->id == id)
        {
            SDL_LockMutex(video->mutex.get());
            *video->rect = rect;
            SDL_UnlockMutex(video->mutex.get());

            return true;
        }
    }

    return false;
}

