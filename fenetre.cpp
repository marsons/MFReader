#pragma comment(lib, "ODALID.lib")
#include "fenetre.h"
#include "ui_fenetre.h"
#include <stdint.h>
#include <QMessageBox>
#include "ODALID.h"

/// Initialise la fenêtre de l'application
Fenetre::Fenetre(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Fenetre)
{
    ui->setupUi(this);
    lecteur = nullptr;
    disableAll();
    timer_carte = new QTimer();
}

/// Démarre le timer vérifiant si une carte est présente ou pas
void Fenetre::start_timer()
{
    timer_carte->setInterval(1000);
    timer_carte->start();
    connect(timer_carte, SIGNAL(timeout()), this, SLOT(poll_card()));
}

/// Vérifie s'il y a une carte présente
void Fenetre::poll_card()
{
    if (lecteur != nullptr)
    {
        if (lecteur->has_card()) // Il y avait une carte de détecté avant
        {
            if (! lecteur->pollCard())
            {
                razInfos();
                disableCardEdit();
            }
        }
        else // Il n'y avait pas de carte de détectée avant
        {
            if (lecteur->pollCard())
            {
                lecteur->readCard();
                enableCardEdit();
            }
        }
    }
}

void Fenetre::razInfos()
{
    updateName("");
    updateFirstName("");
    updateCredit(0);
}

/// Arrète le timer lorsque le lecteur est déconnecté
void Fenetre::stop_timer()
{
    timer_carte->stop();
}

/// Met à jour le champ nom
/// \brief name Le nouveau nom pour le champ
void Fenetre::updateName(string name)
{
    ui->nameValue->setText(QString::fromStdString(name));
}

/// Met à jour le champ prénom
/// \brief firstName Le nouveau prénom pour le champ
void Fenetre::updateFirstName(string firstName)
{
    ui->firstNameValue->setText(QString::fromStdString(firstName));
}

/// Met à jour le champ crédit
/// \brief credit Le nouveau crédit
void Fenetre::updateCredit(int credit)
{
    ui->creditValue->setText(QString::number(credit));
}

/// Termine la connexion avec le lecteur et ferme la fenêtre
Fenetre::~Fenetre()
{
    timer_carte->stop();
    delete timer_carte;
    delete ui;
    delete lecteur;
    lecteur = nullptr;
}

/// Recherche et connecte un lecteur de carte USB
void Fenetre::on_actionConnecter_le_lecteur_triggered()
{
    try
    {
        lecteur = new Lecteur();
        lecteur->subscribe(this);
        enablePollCard();
    }
    catch (Exceptions::ConnectionException ce)
    {
        QMessageBox::information(this, "Erreur", "Lecteur non trouvé ");
    }
}

/// Déconnecte le lecteur de carte
void Fenetre::on_actionD_connecter_le_lecteur_triggered()
{
    delete lecteur;
    lecteur = nullptr;
    disableAll();
}


void Fenetre::enableEnrolledEdit()
{

}

void Fenetre::disableEnrolledEdit()
{

}

void Fenetre::enableFormattedEdit()
{

}

void Fenetre::disableFormattedEdit()
{

}

/// Interdit l'édition des informations de carte
void Fenetre::disableCardEdit()
{
    ui->firstNameValue->setDisabled(true);
    ui->nameValue->setDisabled(true);
    ui->incrementButton->setDisabled(true);
    ui->decrementButton->setDisabled(true);
    ui->enrollButton->setDisabled(true);
    ui->formatButton->setDisabled(true);
}

/// Autorise l'édition des informations de carte
void Fenetre::enableCardEdit()
{
    ui->firstNameValue->setEnabled(true);
    ui->nameValue->setEnabled(true);
    ui->incrementButton->setEnabled(true);
    ui->decrementButton->setEnabled(true);
    ui->enrollButton->setEnabled(true);
    ui->formatButton->setEnabled(true);
}

/// Autorise la recherche d'une carte (à utiliser lorsqu'un lecteur est conecté)
void Fenetre::enablePollCard()
{
    ui->searchCardButton->setEnabled(true);
}

/// Interdit la recherche de carte
void Fenetre::disablePollCard()
{
    ui->searchCardButton->setDisabled(true);
}

/// Interdit toute action à l'exception de la connection de lecteur
void Fenetre::disableAll()
{
    disablePollCard();
    disableCardEdit();
}

/// Recherche une carte sans contact sur le lecteur
void Fenetre::on_searchCardButton_clicked()
{
    if (lecteur != nullptr)
        lecteur->pollCard();
    else
        QMessageBox::information(this, "Erreur", "Lecteur non connecté ");
}

/// Met à jour le nom dans la carte
void Fenetre::on_nameValue_editingFinished()
{
    if (lecteur != nullptr)
    {
        string name = ui->nameValue->text().toStdString();
        lecteur->writeName(name);
    }
    else
        throw "TODO Créer une boite de dialogue précisant qu'il n'y a pas de carte ou de lecteur";
}

/// Met à jour le prénom dans la carte
void Fenetre::on_firstNameValue_editingFinished()
{
    if (lecteur != nullptr)
    {
        string firstName = ui->firstNameValue->text().toStdString();
        lecteur->writeFirstName(firstName);
    }
    else
        throw "TODO Créer une boite de dialogue précisant qu'il n'y a pas de carte ou de lecteur";
}

/// Modifie la carte pour en faire une carte au format désiré
void Fenetre::on_enrollButton_clicked()
{
    if (lecteur != nullptr)
        lecteur->enroll();
    else
        throw "TODO Créer une boite de dialogue précisant qu'il n'y a pas de carte ou de lecteur";
}

/// Augmente le crédit dans la carte
void Fenetre::on_incrementButton_clicked()
{
    if (lecteur != nullptr)
        lecteur->incrementCredit();
    else
        throw "TODO Créer une boite de dialogue précisant qu'il n'y a pas de carte ou de lecteur";
}

/// Diminue le crédit dans la carte
void Fenetre::on_decrementButton_clicked()
{
    if (lecteur != nullptr)
        lecteur->decrementCredit();
    else
        throw "TODO Créer une boite de dialogue précisant qu'il n'y a pas de carte ou de lecteur";
}

/// Formate la carte pour la remettre à l'état initial
void Fenetre::on_formatButton_clicked()
{
    if (lecteur != nullptr)
        lecteur->format();
    else
        throw "TODO Créer une boite de dialogue précisant qu'il n'y a pas de carte ou de lecteur";
}
