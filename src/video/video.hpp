//
// Created by dell_nicolas on 14/06/24.
//

#ifndef CANVAS_VIDEO_HPP
#define CANVAS_VIDEO_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mutex.h>

#include <vlc/vlc.h>

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <functional>
#include <iostream>

#include "../main_prog/data.hpp"

class Video {
private:
    struct loaded_video {
        std::unique_ptr<std::string> id;
        std::unique_ptr<std::string> path;

        std::unique_ptr<libvlc_media_player_t, std::function<void(libvlc_media_player_t *)>> mp;

        std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture *)>> texture;
        std::unique_ptr<SDL_mutex, std::function<void(SDL_mutex *)>> mutex;
        std::unique_ptr<SDL_Rect> dst_rect;
        std::unique_ptr<SDL_Rect> src_rect;
    };

    std::vector<std::unique_ptr<loaded_video>> m_loaded_videos;
    SDL_Renderer *m_renderer;

private:
    static void *lock(void *data, void **p_pixels);
    static void unlock(void *data, void *id, void *const *p_pixels);
    static void display(void *data, void *id);

    static void log_null( void *data, int level, const libvlc_log_t *ctx, const char *fmt, va_list args);
    static void media_parsed_changed(const libvlc_event_t* event, void* data);

public:
    explicit Video(SDL_Renderer *renderer);
    ~Video() = default;

    bool load_video_with_id(const std::string &id, const std::string &path, SDL_Rect rect, std::vector<std::string> vec = {});
    bool load_video_with_id(const std::string &id, const std::string &path, std::vector<std::string> vec = {});
    void display_video_all_video();
    void stop_all_video();
    bool edit_video_with_id(const std::string &id, SDL_Rect rect);
    bool delete_video_with_id(const std::string &id);
    uint32_t get_number_of_video();

};


#endif //CANVAS_VIDEO_HPP
