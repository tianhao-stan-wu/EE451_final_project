#include <iostream>
#include <fstream>
#include "GeneticAlgorithm.h"
#include <time.h>


// this is the main function that runs
// we parallelize population process and calDiversity in Population.cpp
void GeneticAlgorithm::run(int numThreads) {

    struct timespec start, stop; 
    double time;

    std::cout << "Running with " << numThreads << " threads." << std::endl;

    // start the timer to measure the execution time
    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) { perror( "clock gettime" );}

    Population p = createBasePopulation();
    p.process(numThreads);
    generations.push_back(p);

    double fitness = 0.001;
    int generation = 0;

    while(generation < params.generations){
        //p.printChromos();
        generation++;
        p = p.tournament_selection(generation);
        p.crossover_selection(params.crossoverPercent);
        p.mutation(params.mutationPercent);
        p.process(numThreads);

        p.calcDiversity(numThreads);

        generations.push_back(p);
     }

    // stop the timer
    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror( "clock gettime" );}     
    time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;

    std::cout << "Execution time: " << time << " s." << std::endl;

    generations.back().printBestCandidate();

    //resultToFile();
    resultAsJson();
}

Population GeneticAlgorithm::createBasePopulation() {
    Population base(params.populationSize);
    return base;
}

bool GeneticAlgorithm::keepGoing() {
    return false;
}

void GeneticAlgorithm::resultAsJson() {
    json results;
    json min;
    json max;
    json average;
    json diversity;

    int generation = 0;
    for (auto p : generations){
        min.push_back(json::object({{"x", generation},{"y", p.minFitness}}));
        max.push_back(json::object({{"x", generation},{"y", p.maxFitness}}));
        average.push_back(json::object({{"x", generation},{"y", p.averageFitness}}));
        diversity.push_back(json::object({{"x", generation},{"y", p.diversity*100}}));
        generation++;
        //results.push_back(p.toJson());
    }
    results["min"] =  min;
    results["max"] =  max;
    results["average"] =  average;
    results["diversity"] =  diversity;

    std::ofstream myfile ("C:\\Users\\alican\\ClionProjects\\GeneticAlgorithm\\viewer\\results.json");
    if (myfile.is_open())
    {
        myfile << "data = " << results;
        myfile.close();
    }

}

void GeneticAlgorithm::resultToFile() {
    std::ofstream myfile ("C:\\Users\\alican\\ClionProjects\\GeneticAlgorithm\\viewer\\results.tsv");
    if (myfile.is_open())
    {
        myfile << "Generation" << "\t" << "max fitness" << "\t" << "min fitness" << "\t" << "average fitness" << "\n";
        for (auto p : generations){
            myfile << p.minFitness << "\t" << p.maxFitness << "\t" << p.averageFitness << "\n";
        };
        myfile.close();
    }
}












