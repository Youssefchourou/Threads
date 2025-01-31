#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define North 1
#define South 0

using namespace std;

// Globale Semaphoren und Mutex
sem_t Max_Norden, Max_South, Max_Bridge;
pthread_mutex_t Bridge;

void Drive() {
    cout << "Ich kann gerade die Brücke überqueren" << endl;
}

void* carthread(void* arg) {
    int direction = *((int*)arg);

    if (direction == North) {
        sem_wait(&Max_Norden);
    } else if (direction == South) {
        sem_wait(&Max_South);
    }

    sem_wait(&Max_Bridge);

    pthread_mutex_lock(&Bridge);
    cout << "Auto fährt auf die Brücke in Richtung " << (direction == North ? "Nord" : "Süd") << endl;
    pthread_mutex_unlock(&Bridge);

    Drive();

    pthread_mutex_lock(&Bridge);
    cout << "Auto verlässt die Brücke aus Richtung "
         << (direction == North ? "Nord" : "Süd") << endl;
                pthread_mutex_unlock(&Bridge);

    sem_post(&Max_Bridge);

    if (direction == North) {
        sem_post(&Max_Norden);
    } else if (direction == South) {
        sem_post(&Max_South);
    }

    return NULL;
}

int main() {
    // Initialisierung der Semaphoren und Mutex
    sem_init(&Max_Norden, 0, 4);
    sem_init(&Max_South, 0, 4);
    sem_init(&Max_Bridge, 0, 6);
    pthread_mutex_init(&Bridge, NULL);

    pthread_t cars[10];
    int directions[10] = {North, South, North, North, South, North, South, South, North, South};

    for (int i = 0; i < 10; i++) {
        pthread_create(&cars[i], NULL, carthread, &directions[i]);
        usleep(500000);  // Kleine Verzögerung für bessere Ausgabe
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(cars[i], NULL);
    }

    // Ressourcen freigeben
    sem_destroy(&Max_Norden);
    sem_destroy(&Max_South);
    sem_destroy(&Max_Bridge);
    pthread_mutex_destroy(&Bridge);

    return 0;
}
