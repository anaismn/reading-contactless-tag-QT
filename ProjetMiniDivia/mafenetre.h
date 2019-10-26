#ifndef MAFENETRE_H
#define MAFENETRE_H

#include <QWidget>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MaFenetre; }
QT_END_NAMESPACE

class MaFenetre : public QWidget
{
    Q_OBJECT

public:
    MaFenetre(QWidget *parent = nullptr);
    ~MaFenetre();

private slots:
    void on_Connect_clicked();
    void on_Saisie_clicked();
    void on_Quitter_clicked();
    void update_etat_carte();

private:
    Ui::MaFenetre *ui;
    QTimer timer;
};
#endif // MAFENETRE_H
