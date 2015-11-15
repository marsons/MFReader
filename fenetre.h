#ifndef FENETRE_H
#define FENETRE_H

#include <QMainWindow>
#include <QTimer>
#include "lecteur.h"
#include "Observateur.h"

namespace Ui {
class Fenetre;
}

class Fenetre : public QMainWindow, Observateur
{
    Q_OBJECT

public:
    explicit Fenetre(QWidget *parent = 0);
    void updateName(string name);
    void updateFirstName(string firstName);
    void updateCredit(int credit);
    ~Fenetre();

private slots:
    void on_actionConnecter_le_lecteur_triggered();

    void on_actionD_connecter_le_lecteur_triggered();

    void on_nameValue_editingFinished();

    void on_firstNameValue_editingFinished();

    void on_enrollButton_clicked();

    void on_incrementButton_clicked();

    void on_decrementButton_clicked();

    void on_formatButton_clicked();

    void poll_card();

private:
    Ui::Fenetre *ui;
    Lecteur* lecteur;
    QTimer* timer_carte=nullptr;

    void disableAll();
    void start_timer();
    void stop_timer();
    void enableFormattedEdit();
    void disableFormattedEdit();
    void enableCardEdit();
    void disableCardEdit();
    void razInfos();
};

#endif // FENETRE_H
