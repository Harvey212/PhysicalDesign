#ifndef CELL_H
#define CELL_H

#include <vector>
#include <set>

using namespace std;

class Node
{
//    friend class Cell;

public:
    // Constructor and destructor
    Node(const int& id) :
        _id(id), _prev(NULL), _next(NULL) { }
    ~Node() { }

    // Basic access methods
    int getId() const       { return _id; }
    Node* getPrev() const   { return _prev; }
    Node* getNext() const   { return _next; }

    // Set functions
    void setId(const int& id) { _id = id; }
    void setPrev(Node* prev)  { _prev = prev; }
    void setNext(Node* next)  { _next = next; }

    void remove()
    {
        if (_prev) 
        {
            _prev->setNext(_next);
        }
        if (_next) 
        {
            _next->setPrev(_prev);
        }
        _prev=NULL;
        _next=NULL;
    }

private:
    int         _id;    // id of the node (indicating the cell)
    Node*       _prev;  // pointer to the previous node
    Node*       _next;  // pointer to the next node
};

class Cell
{
public:
    // Constructor and destructor //,_temppart(part)
    Cell(string& name ,int part, int id) :
        _gain(0), _lock(false),_pinNum(0),_name(name),_part(part), _id(id) {
        _node = new Node(id);
    }
    ~Cell() { }

    // Basic access methods
    int getGain() const     { return _gain; }
    int getPinNum() const   { return _pinNum; }
    int getPart() const    { return _part; }
    bool getLock() const    { return _lock; }
    string getName() const  { return _name; }
    vector<int> getNetList() const  { return _netList; }

    Node* getNode() const {return _node;}
    ////////////////////////////////////////////////////////////

    // Modify methods
    void move()         { _part = 1- _part;}
    void lock()         { _lock = true; }
    void unlock()       { _lock = false; }
    void incGain()      { ++_gain; }
    void decGain()      { --_gain; }
    void incPinNum()    { ++_pinNum; }
    void decPinNum()    { --_pinNum; }
    void addNet(const int netId) { _netList.push_back(netId); }
    ///////////////////////////////////////////////////////////////////
//    void fakemove()
//    {
//        _temppart=1-_temppart;
//    }

//    int gettempPart() const    { return _temppart; }

    void reset()
    {
        unlock();
        _gain=0;
        //_temppart=_part;
    }
    ////////////////////////////////////////////////////////////////

public :
    int _gain;      // gain of the cell
    bool _lock;      // whether the cell is locked
    int _pinNum;    // number of pins the cell are connected to 
    string _name;      // name of the cell
    int _part;      // partition the cell belongs to (0-A, 1-B)
    int _id;
    Node* _node;

    vector<int>     _netList;   // list of nets the cell is connected to
    ////////////////////////////////////////////////////
//    int _temppart;
//    Node*           _node;      // node used to link the cells together
};

#endif  // CELL_H
