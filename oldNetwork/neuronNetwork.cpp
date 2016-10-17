//
//  neuronNetwork.cpp
//  PinaPL
//

#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>

#include "neuronNetwork.hpp"
#include "../simpleNeuron/neuron.hpp"

#define MAX_DIFFERENCE_OUTPUT 0.001
#define MAX_DIFFERENCE_ERROR 0


using namespace std;

// Initialization

NeuronNetwork::NeuronNetwork(string givenName, string givenDate, unsigned long nbin, unsigned long nbout, unsigned long ntot, double learning) {
    name = givenName;
    date = givenDate;
    inputCount = nbin;
    outputCount = nbout;
    neuronsCount = ntot;
    learningFactor = learning;

    input = vector<double>(nbin);
    output = vector<double>(ntot);

    relation = vector<vector<bool>>(ntot);
    for (unsigned long i = 0; i < ntot; i++) {
        vector<bool> v = vector<bool>(ntot);
        relation[i] = v;
    }

    weight = vector<vector<double>>(ntot);
    for(unsigned long i = 0; i < ntot; i++){
        vector<double> v(ntot);
        weight[i] = v;
    }

    inputNeurons = vector<unsigned long>(nbin);
    outputNeurons = vector<unsigned long>(nbout);
    neurons = vector<Neuron>(ntot);
}

void NeuronNetwork::reset() {
    input = vector<double>(inputCount);
    output = vector<double>(neuronsCount);
    for (unsigned long i = 0; i < neuronsCount; i++) {
        neurons[i].reset();
        output[i] = 0;
    }
    for(unsigned long i = 0; i < inputCount; i++) {
        neurons[inputNeurons[i]].setBalancedWeight();
        input[i] = 0;
    }
}

string NeuronNetwork::description() {
    stringstream str;
    str << "Name: " << name << endl;
    str << "Date: " << date << endl;
    str << "Number of neurons: " << neuronsCount << endl;
    str << "  Neurons: {" << endl;
    for (unsigned long i = 0; i < neuronsCount; i++) {
        str << "    #" << i << " { " << neurons[i].description() << endl;
        str << "        Connected to: {";
        for (unsigned long j = 0; j < neuronsCount; j++) {
            if (relation[i][j])
                str << "  " << j;
        }
        str << "  }" << endl;
        str << "    }" << endl;
    }
    str <<  "}" << endl;
    
    str << "Number of input neurons: " << inputCount << endl;
    str << "  Input neurons: {";
    for (unsigned long i = 0; i < inputCount; i++) {
        str << "  " << inputNeurons[i] << "(" << input[i] << ")";
    }
    str << "  }" <<  endl;
    str << "Number of output neurons: " << outputCount << endl;
    str << "  Output neurons: {";
    for (unsigned long i = 0; i < outputCount; i++) {
        str << "  " << outputNeurons[i] << "(" << output[outputNeurons[i]] << ")";
    }
    str << "  }" <<  endl;

    return str.str();
}

void NeuronNetwork::setRelation(vector<vector<bool> > tab) {
    relation = tab;
}

void NeuronNetwork::setWeight(vector<vector<double> > tab) {
    weight = tab;
}

void NeuronNetwork::setInputNeurons(vector<unsigned long> tab) {
    inputNeurons = tab;
}

void NeuronNetwork::setOutputNeurons(vector<unsigned long> tab) {
    outputNeurons = tab;
}

void NeuronNetwork::setNeurons(vector<Neuron> tab) {
    neurons = tab;
}

// Computing

void NeuronNetwork::setInput(vector<double> data) {
    reset();
    for (unsigned long i = 0; i < inputCount; i++) {
        input[i] = data[i];
    }
}

void NeuronNetwork::calculate() {
    // Initialization
    vector<double> outputAfter(neuronsCount);
    for (unsigned long i = 0; i < neuronsCount; i++) {
        outputAfter[i] = neurons[i].getOutput();
    }

    // Main loop
    do {
        for (unsigned long i = 0; i < neuronsCount; i++) {
            output[i] = outputAfter[i];
        }
        plugInputIntoNeuron();
        calculeNeurons();
        for (unsigned long i = 0; i < neuronsCount; i++) {
            outputAfter[i] = neurons[i].getOutput();
        }
    } while(leastSquareError(output, outputAfter, neuronsCount) >= MAX_DIFFERENCE_OUTPUT);

    // Result
    for (unsigned long i = 0; i < neuronsCount; i++) {
        output[i] = outputAfter[i];
    }
}

void NeuronNetwork::plugInputIntoNeuron() {
    unsigned long length;
    unsigned long k;
    for (unsigned long j = 0; j < inputCount; j++) {
        vector<double> x(1);
        x[0] = input[j];
        neurons[inputNeurons[j]].setInput(x);
    }
    for (unsigned long j = 0; j < neuronsCount; j++) {
        k = 0;
        length = neurons[j].getInputCount();
        vector<double> x(length);
        for (unsigned long i = 0; i < neuronsCount; i++) {
            if (relation[i][j]) {
                x[k] = output[i];
                k++;
            }
        }
        if (k == length) {
            neurons[j].setInput(x);
        }
    }
}

void NeuronNetwork::calculeNeurons() {
    for (unsigned long i = 0; i < neuronsCount; i++) {
        neurons[i].calculateOutput();
    }
}

string NeuronNetwork::getName() {
    return name;
}

string NeuronNetwork::getDate() {
    return date;
}

double NeuronNetwork::getLearningFactor() {
    return learningFactor;
}

unsigned long NeuronNetwork::getInputCount() {
    return inputCount;
}

unsigned long NeuronNetwork::getOutputCount() {
    return outputCount;
}

unsigned long NeuronNetwork::getNeuronsCount() {
    return neuronsCount;
}

vector<Neuron> NeuronNetwork::getNeurons() {
    return neurons;
}

vector<unsigned long> NeuronNetwork::getInputNeurons() {
    return inputNeurons;
}

vector<unsigned long> NeuronNetwork::getOutputNeurons() {
    return outputNeurons;
}

vector<vector<bool>> NeuronNetwork::getRelation() {
    return relation;
}

vector<double> NeuronNetwork::getInput() {
    return input;
}

vector<double> NeuronNetwork::getOutput() {
    vector<double> result(outputCount);
    for (unsigned long i = 0; i < outputCount; i++) {
        result[i] = output[outputNeurons[i]];
    }
    return result;
}

double NeuronNetwork::leastSquareError(vector<double> x, vector<double> y, unsigned long n) {
    double result = 0;
    for (unsigned long i = 0; i < n; i++) {
        double d = x[i] - y[i];
        result += d*d;
    }
    return result;
}

vector<double> NeuronNetwork::computeGradient(vector<double> expectedOutput){
    vector<double> error(neuronsCount, 0);
    vector<double> gradient(neuronsCount, 0);
    vector<double> gradientBefore(neuronsCount, 0);
     
    do {
        for (unsigned long i = 0; i < neuronsCount; i++) {
            gradientBefore[i] = gradient[i];
            error[i] = 0;
            for(unsigned j = 0; j < neuronsCount; j++) {
                if(relation[i][j]){
                    error[i] += weight[i][j] * gradient[j];
                }
            }
        }
        for (unsigned long i = 0; i < outputCount; i++) {
            error[outputNeurons[i]] = expectedOutput[i] - output[outputNeurons[i]];
        }
        for (unsigned long i = 0; i < neuronsCount; i++) {
            gradient[i] = error[i] * neurons[i].getActivationDerivative();
        }

    } while(leastSquareError(gradient, gradientBefore, neuronsCount) > MAX_DIFFERENCE_ERROR);

    for(int i = 0; i < inputCount; i++){
        gradient[inputNeurons[i]] = 0;
    }

    return gradient;
}

vector<vector<double>> NeuronNetwork::computeWeight(vector<double> gradient){

    vector<vector<double> > result(neuronsCount);

    for(unsigned long i = 0; i < neuronsCount; i++){
        vector<double> v(neuronsCount);
        result[i] = v;
        for (unsigned long j = 0; j < neuronsCount; j++) {
            result[i][j] = 0;
            if (relation[i][j]) {
                result[i][j] = gradient[j]*output[i];
            }
        }
    }

    return result;
}

void NeuronNetwork::applyWeight(vector<vector<double>> difference, vector<double> gradient) {
    for(unsigned long i = 0; i < neuronsCount; i++){
        neurons[i].setBiais(neurons[i].getBiais() + gradient[i]);
        unsigned long k = 0;
        vector<double> v(neurons[i].getInputCount());
        for (unsigned long j = 0; j < neuronsCount; j++) {
            weight[i][j] += learningFactor * difference[i][j];
            if (relation[j][i]){
                v[k] = weight[j][i];
                k++;
            }
        }
        if(k != 0){
            neurons[i].setWeight(v);
        }
    }
}

void NeuronNetwork::onlineLearn(vector<vector<double>> dataInput, vector<vector<double>> dataOutput, unsigned long dataCount){
    for (unsigned long i = 0; i < dataCount; i++){
        reset();
        setInput(dataInput[i]);
        calculate();
        vector<double> gradient = computeGradient(dataOutput[i]);
        vector<vector<double>> difference = computeWeight(gradient);
        applyWeight(difference, gradient);
    }
}


void NeuronNetwork::batchLearn(vector<vector<double>> dataInput, vector<vector<double>> dataOutput, unsigned long dataCount){
    vector<vector<double>> difference(neuronsCount);
    for(unsigned long i = 0; i < neuronsCount; i++){
        vector<double> v(neuronsCount, 0);
        difference[i] = v;
    }
    vector<double> gradients(neuronsCount, 0);
    for (unsigned long i = 0; i < dataCount; i++){
        cout << i << endl;
        reset();
        setInput(dataInput[i]);
        calculate();
        vector<double> grad = computeGradient(dataOutput[i]);
        vector<vector<double> > diff = computeWeight(grad);
        for(unsigned long j = 0; j < neuronsCount; j++){
            for(unsigned long k = 0; k < neuronsCount; k++){
                difference[i][j] += diff[i][j];
            }
            gradients[i] += grad[i];
        }
    }
    applyWeight(difference, gradients);
}