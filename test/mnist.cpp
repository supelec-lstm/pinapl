#include <iostream>
#include <map>
#include <vector>
#include <random>
#include <string.h>
#include <math.h>

#include "mnist.hpp"
#include "../perceptron/neuronNetwork.hpp"
#include "../perceptron/mathFunctions.hpp"
#include "../idxParser/idxParser.hpp"

#define LOG
#define FILE_NAME "mnist.cpp\t\t"
#include "../log.hpp"

#define BATCH

using namespace std;

Mnist::Mnist(){

    // Données à modifier

    #ifdef BATCH
    nbreData = 5000; // nombre de données à importer de la base d'apprentissage
    nbreLearn = 10; // nombre de batch learnings avec les données ci-dessus
    nbreTest = 1000; // nombre de données à importer de la base de test
    batchSize = 50; // taille des batchs
    #else
    nbreData = 6000;
    nbreLearn = 5;
    nbreTest = 1000;
    #endif

    learningRate = 0.3;
    function = SIGMOID;

    nbreLayout = 2;
    nbreNeuron = new int[nbreLayout];
    nbreNeuron[0] = 10;
    nbreNeuron[1] = 10;

     // Données à ne pas modifier

    nbreInput = 784;

    nbreTotalNeuron = 0;
    for(int i = 0; i < nbreLayout; i++){
        nbreTotalNeuron += nbreNeuron[i];
    }

    PRINT_LOG("Importation des entrées")
    inputData = inputConverter("./test/MNIST/train-images-idx3-ubyte.gz", nbreData);
    inputTest = inputConverter("./test/MNIST/t10k-images-idx3-ubyte.gz", nbreTest);


    PRINT_LOG("Importation des sorties")
    outputData = outputConverter("./test/MNIST/train-labels-idx1-ubyte.gz", nbreData);
    outputTest = outputConverter("./test/MNIST/t10k-labels-idx1-ubyte.gz", nbreTest);

    PRINT_LOG("Création du réseau")
    network = new NeuronNetwork(nbreInput, 10, nbreTotalNeuron, learningRate);

    PRINT_LOG("Création de la matrice de relation")
    setRelation();

    PRINT_LOG("Création des poids")
    setWeight();

    PRINT_LOG("Création des fonctions")
    setFunctions();

    PRINT_LOG("Initialisation du réseau")
    network->init();
}

void Mnist::learn(){
    #ifdef NBATCH
    PRINT_LOG("Apprentissage stochastique")
    network->stochasticLearning(inputData, nbreData, outputData, nbreLearn);
    #endif
    #ifdef BATCH
    PRINT_LOG("Apprentissage par batch")
    network->batchLearning(inputData, nbreData, outputData, batchSize, nbreLearn);
    #endif
}

void Mnist::test(){
    PRINT_BEGIN_FUNCTION("Tests")
    PRINT_LOG("Attendu - Obtenu")
    int n = 0;
    for(int i = 0; i < nbreTest; i++){
        network->reset();
        network->setInput(inputTest[i]);
        network->calculate();
        /*for(int j = 0; j < 10; j++){
          cout << outputTest[i][j] << " - " << network->getOutput()[j] << endl;
        }*/
        int a = maximum(network->getOutput());
        int b = maximum(outputTest[i]);
        //PRINT_LOG(to_string(b) + " - " +  to_string(a))
        if(a == b){
            n++;
        }
    }
    cout << (n*100.0/nbreTest) << endl;
}

double** Mnist::inputConverter(string path, int nbre){
    PRINT_BEGIN_FUNCTION("Conversion des entrées")
    IdxParser parser;
    vector<vector<int> > data = parser.importMNISTImages(path);

    double** input = new double*[nbre];
    for(int i = 0; i < nbre; i++){
        input[i] = new double[nbreInput];
        for(int j = 0; j < nbreInput; j++){
            double singleData = ((double)data[i][j])/255;
            input[i][j] = singleData;
        }
    }
    PRINT_END_FUNCTION()
    return input;
}

double** Mnist::outputConverter(string path, int nbre){
    PRINT_BEGIN_FUNCTION("Conversion des sorties")
    IdxParser parser;
    vector<int> data = parser.importMNISTLabels(path);

    double** output = new double*[nbre];
    for(int i = 0; i < nbre; i++){
        output[i] = new double[10];
        for(int j = 0; j < 10; j++){
            output[i][j] = 0;
        }
        output[i][data[i]] = 1;
    }
    PRINT_END_FUNCTION()
    return output;
}

void Mnist::setRelation(){
    vector<vector<bool> > relation(nbreTotalNeuron);
    for(int i = 0; i < nbreTotalNeuron; i++){
        vector<bool> v(nbreInput + nbreTotalNeuron, false);
        relation[i] = v;
    }
    for(int i = 0; i < nbreNeuron[0]; i++){
        for(int j = 0; j < nbreInput; j++){
            relation[i][j] = true;
        }
    }
    int n1 = nbreInput;
    int n2 = nbreNeuron[0];
    for(int k = 1; k < nbreLayout; k++){
        for(int i = 0; i < nbreNeuron[k]; i++){
            for(int j = 0; j < nbreNeuron[k-1]; j++){
                relation[n2+i][n1+j] = true;
            }
        }
        n1 += nbreNeuron[k-1];
        n2 += nbreNeuron[k];
    }

    // We create dot code
    #ifdef GRAPH
      std::cout << "digraph MNIST {" << std::endl;
      for(int j = 0; j < nbreInput; j++ ){
        std::cout << "input"<< j << " [shape=point];" << std::endl;
      }

      for(int i = 0; i < nbreTotalNeuron; i++){
        for(int j = 0; j < nbreInput; j++ ){
          if(relation[i][j] == true){
            std::cout << "input"<< j << " -> neuron" << i << ";" << std::endl;
          }
        }
        for(int j = 0;j < nbreTotalNeuron; j++){
          if(relation[i][j+nbreInput] == true){
            std::cout << "neuron" << j << " -> neuron"<< i << ";"<< std::endl;
          }
        }
      }
    std::cout << "}" << std::endl;
    #endif

    network->setRelation(relation);
}

void Mnist::setWeight(){
    vector<vector<double> > weight(nbreTotalNeuron);
    for(int i = 0; i < nbreTotalNeuron; i++){
        vector<double> v(nbreInput + nbreTotalNeuron, 0);
        weight[i] = v;
    }
    for(int i = 0; i < nbreTotalNeuron; i++){
        for(int j = 0; j < nbreInput ; j++){
            weight[i][j] = randomizer(-0.1, 0.1);
        }
        for(int j = nbreInput; j < nbreInput + nbreTotalNeuron; j++){
            weight[i][j] = randomizer(-0.1, 0.1);
        }
    }
    network->setWeight(weight);
}

void Mnist::setFunctions(){
    vector<activationFunctionType> functions(nbreTotalNeuron, function);
    network->setFunctions(functions);
}

int Mnist::maximum(double* tab){
    int res = 0;
    for(int i = 1; i < 10; i++){
        if(tab[i] > tab[res]){
            res = i;
        }
    }
    return res;
}
