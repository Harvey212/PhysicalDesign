#ifndef NET_H
#define NET_H

#include <vector>
#include <set>

using namespace std;

class Net
{
public:
    // constructor and destructor
    Net(int netid) :
        _netid(netid) {
        _partCount[0] = 0; 
        _partCount[1] = 0;

        //_temppartCount[0]=0;
        //_temppartCount[1]=0;
        
        //unlockcells={};
        //tempunlockcells={};

    }
    ~Net()  { }

    //////////////////////////////////////////////////
    // basic access methods
    int getPartCount(int part) const { return _partCount[part]; }
    int getcellnum() {
        return _cellList.size();
        //return  unlockcells.size();
        }

    vector<int> getCellList()  const { return _cellList;}

    //////////////////////////////////////////////////////////
    // modify methods
    void incPartCount(int part)     { ++_partCount[part]; }
    void decPartCount(int part)     { --_partCount[part]; }
    void addCell(const int cellId){
         _cellList.push_back(cellId);
        //unlockcells.insert(cellId);
         }
    ///////////////////////////////////////////////////////

    //int gettempPartCount(int part) const { return _temppartCount[part];}

    //void inctempPartCount(int part) { ++_temppartCount[part];}
    //void dectempPartCount(int part) { --_temppartCount[part];}
    //void reset()
    //{
        //_temppartCount[0] = _partCount[0];
        //_temppartCount[1] = _partCount[1];
    //    tempunlockcells = unlockcells;
    //}

    //set<int> gettempunlock(){return tempunlockcells;}

    //void deleteunlock(int who){
    //    tempunlockcells.erase(who);
    //}
    //////////////////////////////////////////////////


public:
    //int _temppartCount[2];
    //set<int> unlockcells;
    //set<int> tempunlockcells;
    /////////////////////////////////////////////
    int _partCount[2];  // Cell number in partition A(0) and B(1)
    int _netid;          // Name of the net
    vector<int> _cellList;      // List of cells the net is connected to
};

#endif  // NET_H
