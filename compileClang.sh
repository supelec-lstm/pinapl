#!/bin/sh
# clang++ compilation script

# object files generation
# PERCEPTRON
clang++ -std=c++11 -c perceptron/mathFunctions.cpp -o mathFunctions.o
clang++ -std=c++11 -c perceptron/neuronNetwork.cpp -o neuronNetwork.o

# PARSER
clang++ -std=c++11 -c idxParser/idxParser.cpp -o idxParser.o 

# TESTS
clang++ -std=c++11 -c test/test.cpp -o test.o
clang++ -std=c++11 -c test/mnist.cpp -o mnist.o
clang++ -std=c++11 -c test/xor.cpp -o xor.o

# linking and building
# -lz option is necessary to interact with .gz files (IDX parser)
# -Weverything option shows all warnings
# -std=c++11 option forces c++11 compatibility
clang++ -std=c++11 -lz -o testBuild main.cpp mathFunctions.o neuronNetwork.o idxParser.o test.o mnist.o xor.o
# cleaning object files
rm *.o
