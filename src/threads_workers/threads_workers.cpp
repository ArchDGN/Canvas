//
// Created by dell_nicolas on 31/05/24.
//

#include "threads_workers.hpp"


void ThreadsWorkers::create_worker_by_id(const std::string &id, std::function<void()> work, bool can_be_run, bool self_destruct)
{
    // On verrouille l'accès à m_workers
    std::lock_guard<std::mutex> lock(Holy_Mutex::mtx);
    // On ajoute un nouveau worker avec son id, son travail, et s'il peut être exécuté
    m_workers.push_back(std::move(std::make_unique<Workers>(Workers{id, false, can_be_run, self_destruct, std::move(work)})));
}


void ThreadsWorkers::run_workers()
{
    // ON update le status de chaque worker (s'il travaille ou non)
    set_working_status();
    remove_finished_threads();

    // On lance les threads
    for (auto &worker: m_workers)
    {
        // Si le nombre de threads en cours d'exécution est supérieur ou égal au nombre maximal de threads, on arrête
        if (numbers_of_running_workers() >= m_max_threads)
            break;

        // Si le worker ne peut pas être exécuté ou s'il est déjà en train de travailler, on passe au suivant
        if (!worker->can_be_run || worker->working)
            continue;

        // On lance le thread
        worker->working = true;
        m_futures_map[worker->id] = std::async(std::launch::async, worker->work);
    }

}


void ThreadsWorkers::quit_all_threads()
{
    // On empêche les workers de pouvoir redémarrer et on les marque pour destruction
    // On ne les supprime pas tout de suite pour ne pas interférer avec les threads en cours d'exécution
    for (auto &worker : m_workers)
    {
        worker->can_be_run = false;
        worker->self_destruct = true;
    }

}


void ThreadsWorkers::remove_finished_threads() // Cette fonction supprime les threads terminés mais pas les m_workers associés
{
    std::vector<std::string> keys_to_remove;
    {
        std::lock_guard<std::mutex> lock(Holy_Mutex::mtx); // Verrouiller l'accès à m_futures_map
        for (auto &thread: m_futures_map)
        {
            // Si le thread est terminé, on le marque pour suppression
            if (thread.second.valid() && thread.second.wait_for(std::chrono::milliseconds(static_cast<long>(0))) == std::future_status::ready)
            {
                keys_to_remove.push_back(thread.first);
            }
        }
    }

    // On supprime les threads terminés
    for (const auto &key : keys_to_remove) {
        {
            // On verrouille l'accès à m_futures_map
            std::lock_guard<std::mutex> lock(Holy_Mutex::mtx); // Verrouiller l'accès à m_futures_map
            m_futures_map.erase(key);
        }

        // Si le worker peut être détruit, on le supprime
        if (can_worker_be_self_destruct(key)) {
            delete_worker_by_id(key);
        }
    }
}


void ThreadsWorkers::set_working_status()
{
    // On vérifie si les threads sont terminés
    for (auto &worker : m_workers)
    {
        // Si le worker n'est pas dans m_futures_map, on passe au suivant
        if (m_futures_map.find(worker->id) == m_futures_map.end())
            continue;

        // Si le worker est en train de travailler et que le thread est terminé, on le marque comme non travaillant
        auto &future = m_futures_map.find(worker->id)->second;
        if (future.valid() && future.wait_for(std::chrono::milliseconds(static_cast<long>(0))) == std::future_status::ready)
        {
            //std::cout << m_futures_map.size() << " and Thread " << worker->id << " finished" << std::endl;
            worker->working = false;

            // Si le worker peut être détruit, on empêche son redémarrage
            if(worker->self_destruct)
                worker->can_be_run = false;
        }
    }
}

void ThreadsWorkers::delete_worker_by_id(const std::string &id)
{
    // On verrouille l'accès à m_workers et on supprime le worker avec l'id correspondant
    std::lock_guard<std::mutex> lock(Holy_Mutex::mtx);
    m_workers.erase(std::remove_if(m_workers.begin(), m_workers.end(), [id](const std::unique_ptr<Workers>& worker){return worker->id == id;}), m_workers.end());
}

unsigned int ThreadsWorkers::return_workers_size() const
{
    // On verrouille l'accès à m_workers et on retourne la taille de m_workers
    std::lock_guard<std::mutex> lock(Holy_Mutex::mtx);
    return m_workers.size();
}

unsigned int ThreadsWorkers::numbers_of_running_workers() const
{
    // On verrouille l'accès à m_workers et on retourne le nombre de workers en train de travailler
    std::lock_guard<std::mutex> lock(Holy_Mutex::mtx);
    return std::count_if(m_workers.begin(), m_workers.end(), [](const std::unique_ptr<Workers>& worker){return worker->working;});
}

bool ThreadsWorkers::can_worker_be_self_destruct(const std::string& id) const
{
    // On verrouille l'accès à m_workers et on retourne si le worker avec l'id correspondant peut être détruita
    std::lock_guard<std::mutex> lock(Holy_Mutex::mtx);
    auto it = std::find_if(m_workers.begin(), m_workers.end(), [&id](const std::unique_ptr<Workers>& worker){return worker->id == id;});
    return it != m_workers.end() && (*it)->self_destruct;
}

void ThreadsWorkers::edit_worker_by_id(const std::string &id, bool can_be_run, bool self_destruct)
{
    // On verrouille l'accès à m_workers et on modifie les paramètres du worker avec l'id correspondant
    std::lock_guard<std::mutex> lock(Holy_Mutex::mtx);
    auto it = std::find_if(m_workers.begin(), m_workers.end(), [&id](const std::unique_ptr<Workers>& worker){return worker->id == id;});
    if (it != m_workers.end()) {
        (*it)->can_be_run = can_be_run;
        (*it)->self_destruct = self_destruct;
    } else {
        std::cout << "Worker " << id << " not found" << std::endl;
    }
}



