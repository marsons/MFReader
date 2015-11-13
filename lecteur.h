#ifndef LECTEUR_H
#define LECTEUR_H
#include <stdint.h>
#include "ODALID.h"
#include "MfErrNo.h"
#include "Observateur.h"
#include <string>
#include <set>

using namespace std;
class Lecteur
{
public:
    Lecteur();
    void subscribe(Observateur* obs);
    void loadKeys(uint8_t keyA[6], uint8_t keyB[6], char block);
    void connectionOK();
    void pollCard();

    string readName();
    string readFirstname();
    int readCredit();

    void writeName(const string& name);
    void writeFirstName(const string& firstName);
    void incrementCredit();
    void decrementCredit();

    void enroll();
    void format();

    ~Lecteur();
private:
    typedef enum { AUCUNE_CARTE, FORMATEE, ENROLLEE, INCONNU } t_carte;
    t_carte carte = AUCUNE_CARTE;
    const int B_CREDIT=14;
    const int B_BACKUP=13;
    const int B_NAME=10;
    const int B_FIRST_NAME=9;
    const int S_ID=2;
    const int S_CREDIT=3;
    unsigned char keyA_ID[6] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 };
    unsigned char keyB_ID[6] = { 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5 };
    unsigned char keyA_Credit[6] = { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5 };
    unsigned char keyB_Credit[6] = { 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5 };
    unsigned char keyA[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    unsigned char keyB[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    void updateInfos();
    void checkTag(BYTE atq[2]);
    set<Observateur*> abonnes;
    ReaderName* reader;
};

namespace Exceptions
{
  class ConnectionException
  {
  };

  class LoadKeyException
  {
  };

  class NotAMifareClassicException
  {
  };

  class DeconnectionException
  {
  };

  class WriteException
  {
  };

  class ReadException
  {
  };

  class UpdateAccessBlockException
  {
  };
}

#endif // LECTEUR_H
