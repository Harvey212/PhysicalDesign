#ifndef PARTITIONER_H
#define PARTITIONER_H

#include <fstream>
#include <vector>
#include <map>

#include<limits>

#include "cell.h"
#include "net.h"
using namespace std;

class Partitioner
{
public:
    // constructor and destructor
    Partitioner(fstream& inFile) :
        _cutSize(0), _bFactor(0),_netNum(0),_cellNum(0){
        _partSize[0] = 0;
        _partSize[1] = 0;

        //_temppartSize[0]=0;
        //_temppartSize[1]=0;

        parseInput(inFile);
    }
    ~Partitioner() 
    {
        clear();
    }

    // basic access methods
    int getPartSize(int part) const { return _partSize[part];}
    double getBFactor() const {return _bFactor;}
    //modify
    void incPartSize(int part){++_partSize[part];}
    void decPartSize(int part){--_partSize[part];}

    ////////////////////////////////////////////////////////////////////
    
    // modify method
    void parseInput(fstream& inFile);
    void partition();

    void calIni();

    void myinsert(Node *n1);
    void mydelete(Node *n2);






    // member functions about reporting
    void writeResult(fstream& outFile);
    void printSummary() const;
    //////////////////////////////////////////////////////////////

    //int gettempPartSize(int part) const { return _temppartSize[part];}

    //void inctempPartSize(int part){++_temppartSize[part];}
    //void dectempPartSize(int part){--_temppartSize[part];}

    void reset()
    {
        //_temppartSize[0] = _partSize[0];
        //_temppartSize[1] = _partSize[1];

        bmapA.clear();
        bmapB.clear();
        //ALargestVal=-1* numeric_limits<int>::infinity();
        //BLargestVal=-1* numeric_limits<int>::infinity();
    }
    /////////////////////////////////////////////////////////////


private:
    
    ////////////////////////////////////////////////
    vector<Net*>    _netArray;      // net array of the circuit
    vector<Cell*>   _cellArray;     // cell array of the circuit
    int _partSize[2];   // size (cell number) of partition A(0) and B(1)
    map<string, int>    _cellName2Id;   // mapping from cell name to id

    int _cutSize;       // cut size
    double  _bFactor;       // the balance factor to be met
    int _netNum;
    int _cellNum;

    //////////////////////////////////////////////
    map<int, Node*> bmapA;
    map<int, Node*> bmapB;

    //int ALargestVal;
    //int BLargestVal;

    //int _temppartSize[2];
    //////////////////////////////////////////////////

    // Clean up partitioner
    void clear();
};

#endif  // PARTITIONER_H
