#include <iostream>
#include <list>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <cmath>
#include <vector>
#include <time.h>


#include "algorithm/GeneticAlgorithm.h"

using namespace std;

// benchmark sequences for the 2d HP model
// 0 = hydrophil, "white"
// 1 = hydrophob, "black"
// data source: Ron Unger, John Moult: Genetic Algorithms for Protein Folding Simulations,
//         Journal of Molecular Biology, Vol. 231, No. 1, May 1993

std::string SEQ20 = "10100110100101100101";
std::string SEQ24 = "110010010010010010010011";
std::string SEQ25 = "0010011000011000011000011";
std::string SEQ36 = "000110011000001111111001100001100100";
std::string SEQ48 = "001001100110000011111111110000001100110010011111";
std::string SEQ50 = "11010101011110100010001000010001000101111010101011";
std::string SEQ150 = "110101010111101000100110101010111101000100010000100010001011110101010111101010101111010001000100001000100010111101010101101000010001000101111010101011";
int TIMEOUT = 72;  // seconds
std::string SEQ156 = "000000000001001111111111100111111111110011111111111001111111111100111111111110011111111111001111111111100111111111110011111111111001111111111100100000000000";
std::string SEQ300 = "110101010111101000100110101010111101000100010000100010001011110101010111101010101111010001000100001000100010111101010101101000010001000101111010101011110101010111101000100110101010111101000100010000100010001011110101010111101010101111010001000100001000100010111101010101101000010001000101111010101011";


GeneticAlgorithmParams GeneticAlgorithm::params = {
        SEQ300,  // sequence
        100,     // populationSize
        300,     // generations
        0.10,   // elitePercent
        0.15,   // crossoverPercent
        0.15    // mutationPercent
};

std::ostream& operator<<(std::ostream& ostr, const std::vector<std::string>& list) {
    for (auto &i : list) {
        ostr << " " << i;
    }
    return ostr;
}


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("#threads not given\n");
        return -1;
    }

    int NUM_THREADS = atoi(argv[1]);

    GeneticAlgorithm geneticAlgorithm;
    geneticAlgorithm.run(NUM_THREADS);

    return 0;
}

