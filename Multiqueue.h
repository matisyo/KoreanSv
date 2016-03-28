
#ifndef MULTIQUEUE_H
#define MULTIQUEUE_H

#include <pthread.h>
#include <list>
#include "Mensaje.h"

using namespace std;

class Multiqueue
{
    list<Mensaje>    m_queue;
    pthread_mutex_t  m_mutex;
    pthread_cond_t   m_condv;

  public:
    Multiqueue() {
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_condv, NULL);
    }
    ~Multiqueue() {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_condv);
    }
    void add(Mensaje item) {
        pthread_mutex_lock(&m_mutex);
        m_queue.push_back(item);
        pthread_cond_signal(&m_condv);
        pthread_mutex_unlock(&m_mutex);
    }
    Mensaje remove() {
        pthread_mutex_lock(&m_mutex);
        while (m_queue.size() == 0) {
            pthread_cond_wait(&m_condv, &m_mutex);
        }
        Mensaje item = m_queue.front();
        m_queue.pop_front();
        pthread_mutex_unlock(&m_mutex);
        return item;
    }
    int size() {
        pthread_mutex_lock(&m_mutex);
        int size = m_queue.size();
        pthread_mutex_unlock(&m_mutex);
        return size;
    }
};

#endif //MULTIQUEUE_H
