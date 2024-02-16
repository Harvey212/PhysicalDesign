#ifndef BSTAR_H
#define BSTAR_H


class Node
{
    public:
        Node(int idd): L(nullptr),R(nullptr),P(nullptr),id(idd){

        }

        ~Node(){}

        /////////////////////////////////
        Node* getP()
        {
            return P;
        }
        Node* getL()
        {
            return L;
        }
        Node* getR()
        {
            return R;
        }
        int getid()
        {
            return id;
        }
        ////////////////////////////////////////////////////
        void setP(Node* pp)
        {
            P = pp;
        }
        void setL(Node* ll)
        {
            L = ll;
        }
        void setR(Node* rr)
        {
            R = rr;
        }
        void setid(int ii)
        {
            id = ii;
        }
        //////////////////////////////////////////////////////////

        //M1:swap
        static void swap(Node *left, Node *right)
        {
            /////////////////////////////////
            bool neighbor = false;

            if((left->getP() == right) || (right->getP() == left))
            {
                neighbor = true;
            }



            if(neighbor)
            {

                Node *pap=nullptr;
                Node *child=nullptr;

                if(left->getP()==right)
                {
                    pap=right;
                    child=left;
                }else
                {
                    pap=left;
                    child=right;
                }
                ////////////////////////////////
                
                if(child->getL()!=nullptr)
                {
                    child->getL()->setP(pap);
                }

                if(child->getR()!=nullptr)
                {
                    child->getR()->setP(pap);
                }
                /////////////////////////////////////

                if(pap->getP()!=nullptr)
                {
                    if(pap->getP()->getL()==pap)
                    {
                        pap->getP()->setL(child);
                    }else
                    {
                        pap->getP()->setR(child);
                    }
                }

                ///////////////////////////////////////////
                Node *oldpapP = pap->getP();
                Node *oldpapR = pap->getR();
                Node *oldpapL = pap->getL();


                if(pap->getL()==child)
                {
                    pap->getR()->setP(child);
                    //////////////////

                    pap->setP(child);
                    pap->setL(child->getL());
                    pap->setR(child->getR());
                    ////////////////////////////

                    child->setP(oldpapP);

                    child->setL(pap);
                    child->setR(oldpapR);
                }else
                {
                    pap->getL()->setP(child);
                    ////////////////////////////

                    pap->setP(child);
                    pap->setL(child->getL());
                    pap->setR(child->getR());
                    /////////////////////////////

                    child->setP(oldpapP);
                    child->setR(pap);
                    child->setL(oldpapL);
                    
                }


            }else
            {
                //////////////////////////////////////////////////
                if(left->getP()!=nullptr)
                {
               
                    if(left->getP()->getL()==left)
                    {
                        left->getP()->setL(right);
                    }else
                    {
                        left->getP()->setR(right);
                    }
                }

                if((left->getL()!=nullptr))
                {
                    left->getL()->setP(right);
                }
            
                if((left->getR()!=nullptr))
                {
                    left->getR()->setP(right);
                }
                //////////////////////////////////////////////////
                if(right->getP()!=nullptr)
                {
                
                    if(right->getP()->getL()==right)
                    {
                        right->getP()->setL(left);
                    }else
                    {
                        right->getP()->setR(left);
                    }
                }

                if((right->getL()!=nullptr))
                {
                    right->getL()->setP(left);
                }
            
                if((right->getR()!=nullptr))
                {
                    right->getR()->setP(left);
                }
                /////////////////////////////////////////////////////////////
                Node *oldRP=nullptr;
                Node *oldRL=nullptr;
                Node *oldRR=nullptr;

                oldRP=right->getP();
                oldRL=right->getL();
                oldRR=right->getR();

                right->setP(left->getP());
                right->setR(left->getR());
                right->setL(left->getL());

                left->setP(oldRP);
                left->setR(oldRR);
                left->setL(oldRL);
            }


        }
        /////////////////////////////////////////////////////
        void remove()
        {
            Node* temp=nullptr;
            int mark=0;
            //0:left   1:right

            if(L!=nullptr)
            {
                temp=L;
                mark=0;
            }else
            {
                temp=R;
                mark=1;
            }

            if(P!=nullptr)
            {
                if(P->getL()==this)
                {
                    P->getP()->setL(temp);
                }else
                {
                    P->getP()->setR(temp);
                }
            }

            if(temp!=nullptr)
            {
                temp->setP(P);
            }

            if(mark==0)
            {
                P=nullptr;
                L=nullptr;

                /////////////////////
                //R=nullptr;
            }else
            {
                P=nullptr;
                R=nullptr;

                //////////////////
                //L=nullptr;
            }

        }


    private:

        int id;
        Node *L;
        Node *R;
        Node *P;
        
};


























#endif