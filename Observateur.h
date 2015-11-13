#ifndef OBSERVATEUR_H
#define OBSERVATEUR_H

#include <string>
using namespace std;
class Observateur
{
public:
    virtual void updateName(string name)=0;
    virtual void updateFirstName(string firstName)=0;
    virtual void updateCredit(int credit)=0;
};

#endif // OBSERVATEUR_H
