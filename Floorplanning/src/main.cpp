#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "floorplan.h"
using namespace std;

int main(int argc, char** argv)
{
    fstream input_blk, input_net, output;
    double alpha=0.0;

    if (argc == 5) {
        alpha = stod(argv[1]);
        input_blk.open(argv[2], ios::in);
        input_net.open(argv[3], ios::in);
        output.open(argv[4], ios::out);
        if (!input_blk) {
            cerr << "Cannot open the input file \"" << argv[2]
                 << "\". The program will be terminated..." << endl;
            exit(1);
        }
        if (!input_net) {
            cerr << "Cannot open the input file \"" << argv[3]
                 << "\". The program will be terminated..." << endl;
            exit(1);
        }
        if (!output) {
            cerr << "Cannot open the output file \"" << argv[4]
                 << "\". The program will be terminated..." << endl;
            exit(1);
        }
    }
    else {
        cerr << "Usage: ./Floorplanner <alpha> <input block file> " <<
                "<input net file> <output file>" << endl;
        exit(1);
    }

    


    auto start = chrono::high_resolution_clock::now();
    Floorplan* fp = new Floorplan(alpha, input_blk, input_net);
    fp->plan();
    auto stop = chrono::high_resolution_clock::now();
    chrono::duration<double> exe_time = stop - start;
    fp->myprint(output, exe_time.count());


    return 0;
}
