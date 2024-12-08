#include <iostream>
#include <functional>
#include "Population.h"
#include <pthread.h>
#include <iterator>
#include <vector>
#include <cstdlib>
#include <algorithm>


Population Population::selection(int generation) {

    std::default_random_engine generator;

    float ex = (float) (0.5 + (generation / 300.0));
    std::exponential_distribution<double> distribution(ex);

    std::vector<Chromosome> new_chromosomes;

    for(int i = 0; i < chromosomes.size();){
        double number = distribution(generator);
        if (number<1.0){
            i++;
            int position = int(chromosomes.size()*number);
            new_chromosomes.push_back(chromosomes.at(position));
        }
    }

    return Population(new_chromosomes);
}


Population Population::tournament_selection(int generation) {

    std::vector<Chromosome> new_chromosomes;


    //Fisher-Yates shuffle
    int t_rounds = (int) chromosomes.size();

    long left = std::distance(chromosomes.begin(), chromosomes.end());
    auto current = chromosomes.begin();

    while (t_rounds) {
        auto rI1 = current;
        auto rI2 = current;
        std::advance(rI1, rand() % left);
        std::advance(rI2, rand() % left);

        Chromosome &ch1 = (*(rI1));
        Chromosome &ch2 = (*(rI2));

        Chromosome winner = (ch1.getFitness() > ch2.getFitness()) ? ch1 : ch2;
        new_chromosomes.push_back(std::move(winner));

        --t_rounds;
    }

    return Population(new_chromosomes);

 }



void Population::crossover_selection(double crossover_rate) {

    // 0.25 der chromosome.
    //Fisher-Yates shuffle
    int crossover_count = (int) (chromosomes.size() * crossover_rate );

    long left = std::distance(chromosomes.begin(), chromosomes.end());
    auto current = chromosomes.begin();

    while (crossover_count) {
        auto r = current;
        std::advance(r, rand()%left);
        //std::swap(*current, *r);
        Chromosome& ch1 = (*(current));
        Chromosome& ch2 = (*(r));

        if(ch1.id != ch2.id){
            ch1.crossover(ch2);
            --crossover_count;
        }
        ++current;
        --left;
    }

}


void Population::mutation(double mutation_rate) {
    /*
     *   Wahl der Mutationen innerhalb der Population
     *
     */
    double mutation_count = std::ceil((chromosomes.size() * mutation_rate ));
    //double mutation_count = chromosomes.size();

    long size = chromosomes.size();
    auto current = chromosomes.begin();

    while (mutation_count--) {
        auto r = current;
        std::advance(r, rand()%size);
        (*(r)).mutate();
    }


}

void Population::printChromos() {


    for (auto chromo : chromosomes){
        std::cout << chromo.id << ": ";
        chromo.printTurns();
    }
    std::cout << chromosomes.size()<< std::endl;
}

// parallel processing for p.process() using pthreads
// Mutex for protecting shared variable totalFitness
pthread_mutex_t fitnessMutex;

struct Process_Data {
    std::vector<Chromosome>* chromosomes; // Pointer to the chromosomes vector
    size_t start;                         // Start index of this thread's workload
    size_t end;                           // End index of this thread's workload
    double totalFitness;                // Accumulated fitness for this thread
};

// define thread function here
void* processChromosomes(void* arg) {
    Process_Data* data = (Process_Data*)arg;
    double localFitness = 0;

    // Process chromosomes in this thread's range
    for (size_t i = data->start; i < data->end; ++i) {
        Chromosome& chromo = (*data->chromosomes)[i];
        chromo.process();
        localFitness += chromo.getFitness();
    }

    // Safely add the local fitness to the shared totalFitness
    pthread_mutex_lock(&fitnessMutex);
    data->totalFitness += localFitness;
    pthread_mutex_unlock(&fitnessMutex);

    return nullptr;
}


// parallelized code for process a population
// totalFitness is a shared variable, we implement data parallelism across each individual in a population
void Population::process(int numThreads) {

    double totalFitness = 0;
    pthread_t threads[numThreads];
    Process_Data threadData[numThreads];

    size_t numChromosomes = chromosomes.size();
    size_t chunkSize = numChromosomes / numThreads;

    pthread_mutex_init(&fitnessMutex, nullptr);

    // Create threads
    for (size_t i = 0; i < numThreads; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i == numThreads - 1) ? numChromosomes : start + chunkSize;

        threadData[i] = {&chromosomes, start, end, totalFitness};
        pthread_create(&threads[i], nullptr, processChromosomes, &threadData[i]);
    }

    // Join threads
    for (size_t i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    pthread_mutex_destroy(&fitnessMutex);

    // Sort chromosomes (serial step)
    std::sort(chromosomes.rbegin(), chromosomes.rend());

    // Calculate min, max, and average fitness
    minFitness = chromosomes.back().getFitness();
    maxFitness = chromosomes.front().getFitness();
    averageFitness = totalFitness / chromosomes.size();
}

void Population::printBestCandidate() {
    std::sort(chromosomes.rbegin(), chromosomes.rend());
    chromosomes.front().printInfo();
}

json Population::toJson() {
    json j;
    j["minFitness"] = minFitness;
    j["maxFitness"] = maxFitness;
    j["averageFitness"] = averageFitness;
    return j;
}


// parallelize diversity calculation
// totalSum is the shared variable here.

pthread_mutex_t mutex_div;

// Struct for passing data to threads
struct Div_Data {
    const std::vector<Chromosome>* chromosomes;
    size_t orientationCount;
    int startIdx;
    int endIdx;
    uint64_t totalSum;
};

void* calcPartialDiversity(void* arg) {
    Div_Data* data = static_cast<Div_Data*>(arg);
    const auto& chromosomes = *data->chromosomes;

    uint64_t localSum = 0;

    for (int i = data->startIdx; i < data->endIdx; ++i) {
        for (int j = i + 1; j < chromosomes.size(); ++j) {
            const Chromosome& c1 = chromosomes[i];
            const Chromosome& c2 = chromosomes[j];
            for (size_t pos = 0; pos < data->orientationCount; ++pos) {
                if (c1.turnList[pos] != c2.turnList[pos]) {
                    ++localSum;
                }
            }
        }
    }

    // Lock the mutex to safely update the shared sum
    pthread_mutex_lock(&mutex_div);
    data->totalSum += localSum;
    pthread_mutex_unlock(&mutex_div);

    return nullptr;
}

void Population::calcDiversity(int numThreads) {
    size_t orientationCount = chromosomes.front().turnList.size();
    int populationSize = static_cast<int>(chromosomes.size());

    int chunkSize = (populationSize + numThreads - 1) / numThreads;

    pthread_t threads[numThreads];
    Div_Data threadData[numThreads];
    uint64_t totalSum = 0;

    // Initialize the mutex
    pthread_mutex_init(&mutex_div, nullptr);

    // Create threads
    for (int t = 0; t < numThreads; ++t) {
        threadData[t].chromosomes = &chromosomes;
        threadData[t].orientationCount = orientationCount;
        threadData[t].startIdx = t * chunkSize;
        threadData[t].endIdx = std::min((t + 1) * chunkSize, populationSize);
        threadData[t].totalSum = totalSum;

        pthread_create(&threads[t], nullptr, calcPartialDiversity, &threadData[t]);
    }

    // Join threads and accumulate results
    for (int t = 0; t < numThreads; ++t) {
        pthread_join(threads[t], nullptr);
    }

    // Clean up
    pthread_mutex_destroy(&mutex_div);

    uint64_t numComparisons = static_cast<uint64_t>((populationSize * (populationSize - 1)) / 2);
    uint64_t total = numComparisons * orientationCount;

    diversity = static_cast<float>(static_cast<double>(totalSum) / total);
    diversity *= 100.0f;
    diversity = std::roundf(diversity);
}

