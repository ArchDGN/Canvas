//
// Created by dell_nicolas on 31/05/24.
//

#ifndef MEINCANVAS_THREADS_WORKERS_HPP
#define MEINCANVAS_THREADS_WORKERS_HPP

#include <iostream>
#include <future>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <string>
#include <memory>
#include <map>
#include <algorithm>
#include <utility>
#include <pthread.h>

#include "../main_prog/data.hpp"

class ThreadsWorkers {
private:
    struct Workers {
        std::string id;
        bool working;
        bool can_be_run;
        bool self_destruct;
        std::function<void()> work;
    };

    unsigned int m_max_threads = (std::thread::hardware_concurrency() - 2) * 10; // -2 to keep some threads for the main program

    std::vector<std::unique_ptr<Workers>> m_workers;
    std::map<std::string, std::future<void>> m_futures_map;


private:
    void set_working_status();
    void remove_finished_threads();
    [[nodiscard]] bool can_worker_be_self_destruct(const std::string& id) const;


public:
    ThreadsWorkers() = default;

    void create_worker_by_id(const std::string& id, std::function<void()> work, bool can_be_run = true, bool self_destruct = true);
    void delete_worker_by_id(const std::string& id);
    void edit_worker_by_id(const std::string& id, bool can_be_run = true, bool self_destruct = true);
    void quit_all_threads();
    [[nodiscard]] unsigned int return_workers_size() const;
    [[nodiscard]] unsigned int numbers_of_running_workers() const;

    void run_workers();

    ~ThreadsWorkers() = default;
};


#endif //MEINCANVAS_THREADS_WORKERS_HPP
