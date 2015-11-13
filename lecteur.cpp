#include "lecteur.h"
#include <iostream>
#include <sstream>
#include <iterator>
#include <string.h>
#include <algorithm>
#include <Windows.h>


using namespace std;

/// Connecte un lecteur de carte USB
Lecteur::Lecteur()
{
    reader = new ReaderName();
    if (MI_OK != OpenCOM1(reader))
    {
        delete reader;
        throw Exceptions::ConnectionException();
    }

    connectionOK();

    if (MI_OK != RF_Power_Control(reader, true, 0))
    {
        delete reader;
        throw Exceptions::ConnectionException();
    }

    try
    {
        loadKeys(keyA, keyB, 0);
        loadKeys(keyA_ID, keyB_ID, 1);
        loadKeys(keyA_Credit, keyB_Credit, 2);
    }
    catch (Exceptions::LoadKeyException lke)
    {
        delete reader;
        throw Exceptions::ConnectionException();
    }
}

/// Réalise un signal lumineux pour confirmer la connexion du lecteur
void Lecteur::connectionOK()
{
    LEDBuzzer(reader, LED_GREEN_ON);
    Sleep(100);
    LEDBuzzer(reader, LED_YELLOW_ON|LED_GREEN_ON);
    Sleep(100);
    LEDBuzzer(reader, BUZZER_ON|LED_RED_ON|LED_YELLOW_ON|LED_GREEN_ON);
    Sleep(10);
    LEDBuzzer(reader, BUZZER_OFF|LED_RED_ON|LED_YELLOW_ON|LED_GREEN_ON);
}

/// Charge un couple de clés en mémoire
void Lecteur::loadKeys(uint8_t keyA[6], uint8_t keyB[6], char block)
{
    if (MI_OK != Mf_Classic_LoadKey(reader, Auth_KeyA, keyA, block))
        throw Exceptions::LoadKeyException();

    if (MI_OK != Mf_Classic_LoadKey(reader, Auth_KeyB, keyB, block))
        throw Exceptions::LoadKeyException();
}

/// Recherche une carte sur le lecteur et lit ses informations
void Lecteur::pollCard()
{
    BYTE atq[2];
    BYTE sak[1];
    BYTE uid[12];
    uint16_t uid_len = 12;

    if (MI_OK == ISO14443_3_A_PollCard(reader, atq, sak, uid, &uid_len))
    {
        updateCardType();
        checkTag(atq);
        updateInfos();
    }
}

void Lecteur::updateCardType()
{
    try
        checkTag();
    catch (Exceptions::NotAMifareClassicException namce)
        carte = INCONNU;
}

void Lecteur::updateInfos()
{
    string name = readName();
    string firstName = readFirstname();
    int credit = readCredit();
    for (auto i=abonnes.begin() ; i!=abonnes.end() ; ++i)
    {
        (*i)->updateName(name);
        (*i)->updateFirstName(firstName);
        (*i)->updateCredit(credit);
    }
}

/// Vérifie si la carte est une carte Mifare Classic
void Lecteur::checkTag(BYTE atq[2])
{
    if ((atq[1] != 0x00) || ((atq[0] != 0x02) && (atq[0] != 0x04) && (atq[0] != 0x18)))
        throw Exceptions::NotAMifareClassicException();
}

/// Ajoute un observateur au lecteur (pour mettre à jour les données lues par le lecteur)
void Lecteur::subscribe(Observateur* obs)
{
    abonnes.insert(obs);
}

/// Récupère le nom présent dans la carte
string Lecteur::readName()
{
    unsigned char buffer[16];
    if (MI_OK == Mf_Classic_Read_Block(reader, true, B_NAME, buffer, Auth_KeyA, 1))
    {
        cout << "Read ok" << endl;
        stringstream ss;
        bool found_null = false;
        for(int i=0 ; i<16 && ! found_null; i++)
        {
            if (buffer[i] == '\0')
                found_null = true;
            else
                ss << buffer[i];
        }
        string s = ss.str();
        return s;
    }
    throw Exceptions::ReadException();
}

/// Récupère le prénom présent dans la carte
string Lecteur::readFirstname()
{
    unsigned char buffer[16];
    if (MI_OK == Mf_Classic_Read_Block(reader, true, B_FIRST_NAME, buffer, Auth_KeyA, 1))
    {
        cout << "Read ok" << endl;
        stringstream ss;
        bool found_null = false;
        for(int i=0 ; i<16 && ! found_null; i++)
        {
            if (buffer[i] == '\0')
                found_null = true;
            else
                ss << buffer[i];
        }
        string s = ss.str();
        return s;
    }
    throw Exceptions::ReadException();
}

/// Récupère le crédit présent dans la carte
int Lecteur::readCredit()
{
    uint32_t val;
    if (MI_OK == Mf_Classic_Read_Value(reader, true, B_CREDIT, &val, Auth_KeyA, 2))
        return val;

    // throw ReadException();
}

/// Modifie le nom dans la carte
void Lecteur::writeName(const string& name)
{
    uint8_t buffer[16];
    const int borne_max = min<int>(16, name.size());
    for (int i=0 ; i<borne_max ; i++)
        buffer[i] = name[i];

    if (borne_max < 16)
        buffer[borne_max] = '\0';

    if (MI_OK != Mf_Classic_Write_Block(reader, true, B_NAME, buffer, Auth_KeyB, 1))
        throw Exceptions::WriteException();
}

/// Modifie le prénom dans la carte
void Lecteur::writeFirstName(const string& firstName)
{
    uint8_t buffer[16];
    const int borne_max = min<int>(16, firstName.size());
    for (int i=0 ; i<borne_max ; i++)
        buffer[i] = firstName[i];

    if (borne_max < 16)
        buffer[borne_max] = '\0';

    if (MI_OK != Mf_Classic_Write_Block(reader, true, B_FIRST_NAME, buffer, Auth_KeyB, 1))
        throw Exceptions::WriteException();
}

/// Incrémente le crédit dans la carte
void Lecteur::incrementCredit()
{
    if (MI_OK != Mf_Classic_Increment_Value(reader, true, B_CREDIT, 1, B_BACKUP, Auth_KeyB, 2))
        throw Exceptions::WriteException();
}

/// Décrémente le crédit dans la carte
void Lecteur::decrementCredit()
{
    if (MI_OK != Mf_Classic_Decrement_Value(reader, true, B_CREDIT, 1, B_BACKUP, Auth_KeyB, 2))
        throw Exceptions::WriteException();
}

/// Modifie les données dans la carte pour qu'elle soit utilisable par l'application
void Lecteur::enroll()
{
    if (MI_OK != Mf_Classic_UpdadeAccessBlock(reader, true, S_CREDIT, 0, keyA_Credit, keyB_Credit,
                                              ACC_BLOCK_READWRITE, ACC_BLOCK_VALUE, ACC_BLOCK_VALUE, ACC_AUTH_NORMAL,
                                              Auth_KeyA))
        throw Exceptions::UpdateAccessBlockException();

    if (MI_OK != Mf_Classic_UpdadeAccessBlock(reader, true, S_ID, 0, keyA_ID, keyB_ID,
                                              ACC_BLOCK_READWRITE, ACC_BLOCK_READWRITE, ACC_BLOCK_READWRITE, ACC_AUTH_NORMAL,
                                              Auth_KeyA))
        throw Exceptions::UpdateAccessBlockException();

    uint8_t buffer[48];
    string chaine = "App identité";
    fill(buffer, buffer+48, '\0');
    for (unsigned int i=0 ; i<chaine.size() ; ++i)
        buffer[32+i] = chaine[i];

    if (MI_OK != Mf_Classic_Write_Sector(reader, true, S_ID, buffer, Auth_KeyA, 0))
        throw Exceptions::WriteException();

    chaine = "App compteur";
    fill(buffer, buffer+4, '\0');
    fill(buffer+4, buffer+8, !'\0');
    fill(buffer+8, buffer+12, '\0');
    buffer[12] = B_CREDIT;
    buffer[13] = !B_CREDIT;
    buffer[14] = B_CREDIT;
    buffer[15] = !B_CREDIT;


    fill(buffer+16, buffer+20, '\0');
    fill(buffer+20, buffer+24, !'\0');
    fill(buffer+24, buffer+28, '\0');
    buffer[28] = B_BACKUP;
    buffer[29] = !B_BACKUP;
    buffer[30] = B_BACKUP;
    buffer[31] = !B_BACKUP;

    if (MI_OK == Mf_Classic_Write_Sector(reader, true, S_ID, buffer, Auth_KeyA, 0))
        throw Exceptions::WriteException();
}

/// Remet la carte au format initial
void Lecteur::format()
{
    if (MI_OK != Mf_Classic_UpdadeAccessBlock(reader, true, S_CREDIT, 2, keyA, keyB,
                                              ACC_BLOCK_TRANSPORT, ACC_BLOCK_TRANSPORT, ACC_BLOCK_TRANSPORT, ACC_AUTH_TRANSPORT,
                                              Auth_KeyB))
        throw Exceptions::UpdateAccessBlockException();

    if (MI_OK != Mf_Classic_UpdadeAccessBlock(reader, true, S_ID, 1, keyA, keyB,
                                              ACC_BLOCK_TRANSPORT, ACC_BLOCK_TRANSPORT, ACC_BLOCK_TRANSPORT, ACC_AUTH_TRANSPORT,
                                              Auth_KeyB))
        throw Exceptions::UpdateAccessBlockException();

    throw "TODO : RAZ toutes les informations";
}

/// Ferme la connexion avec le lecteur
Lecteur::~Lecteur()
{
    if (MI_OK != ISO14443_3_A_Halt(reader))
    {
        delete reader;
        throw Exceptions::DeconnectionException();
    }
    if (MI_OK != RF_Power_Control(reader, false, 0))
    {
        delete reader;
        throw Exceptions::DeconnectionException();
    }
    if (MI_OK != CloseCOM1(reader))
    {
        delete reader;
        throw Exceptions::DeconnectionException();
    }
    delete reader;
}
