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
    }
    catch (Exceptions::ConnectionException ce)
    {
        throw QMessageBox::information(this, "Erreur", "Lecteur non trouvé ");
    }
    lecteur->subscribe(this);
}

/// Déconnecte le lecteur de carte
void Fenetre::on_actionD_connecter_le_lecteur_triggered()
{
    delete lecteur;
    lecteur = nullptr;
}

/// Recherche une carte sans contact sur le lecteur
void Fenetre::on_searchCardButton_clicked()
{
    if (lecteur != nullptr)
        lecteur->pollCard();
    else
        throw "TODO Créer une boite de dialogue";
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
