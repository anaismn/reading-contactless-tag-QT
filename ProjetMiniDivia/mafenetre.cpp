#include "mafenetre.h"
#include "ui_mafenetre.h"
#include "ODALID.h"
#include <QtGui>
#include <QTimer>
#include <QString>
#define Auth_KeyA				true
#define Auth_KeyB				false

//Lste des clés A et B
unsigned char key_A[6] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 };
unsigned char key_B[6] = { 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5 };
unsigned char key_C[6] = { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5 };
unsigned char key_D[6] = { 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5 };
unsigned char key_ff[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
unsigned char  key_index;



MaFenetre::MaFenetre(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MaFenetre)
{
    ui->setupUi(this);
}

MaFenetre::~MaFenetre()
{
    delete ui;
}

ReaderName MonLecteur;

char psHost[] = "192.168.1.4";
char version[30];
uint8_t serial[4];
char stackReader[20];
uint16_t status = 0;
void MaFenetre::on_Connect_clicked(){
    uint8_t atq[2];
        uint8_t sak[1];
        uint8_t uid[12];
        uint16_t uid_len=12;
    int16_t status = 0;
    //MonLecteur.Type = ReaderTCP;
    //strcpy(MonLecteur.IPReader, pstHost);
    MonLecteur.Type = ReaderCDC;
    MonLecteur.device = 0;
    status = OpenCOM1(&MonLecteur);
    qDebug() << "OpenCOM1" << status;

    status = Version(&MonLecteur, version, serial, stackReader);
    ui->Affichage->setText(version);
    ui->Affichage->update();

    // Active le RF pour un temps indéfini
   RF_Power_Control(&MonLecteur,true,0);

    //Desactive le bouton de connexion
    ui->Connect->setEnabled(false);

    //Key load Key_ff A
    status=Mf_Classic_LoadKey(&MonLecteur,Auth_KeyA,key_A,0);
    if(status==0)
        qDebug()<<"Key Load ff A correctement effectue";

    //Key load Key_ff B
    status=Mf_Classic_LoadKey(&MonLecteur,Auth_KeyB,key_B,0);
    if(status==0)
        qDebug()<<"Key Load ff b correctement effectue";
/*
    //Key load NOM PREBOM
    status=Mf_Classic_LoadKey(&MonLecteur,Auth_KeyA,key_A,1);
    if(status==0)
        qDebug()<<"Key Load NOMpREN a correctement effectue";
    status=Mf_Classic_LoadKey(&MonLecteur,Auth_KeyB,key_B,1);
    if(status==0)
        qDebug()<<"Key Load NOMpREN b correctement effectue";

    //Key load credit
    status=Mf_Classic_LoadKey(&MonLecteur,Auth_KeyA,key_C,2);
    if(status==0)
        qDebug()<<"Key Load credit a correctement effectue";
    status=Mf_Classic_LoadKey(&MonLecteur,Auth_KeyB,key_D,2);
    if(status==0)
        qDebug()<<"Key Load Credit b correctement effectue";
*/
    //instanciation du timer
  // connect(timer,SIGNAL(timeout()),this ,SLOT(update_etat_carte()));
    // RF field ON
       RF_Power_Control(&MonLecteur, TRUE, 0);

       status = ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len);

    update_etat_carte();
}





void MaFenetre::update_etat_carte(){
    //pour la detection de la carte
   /* uint8_t atq[2];
    uint8_t sak[1];
    uint8_t uid[12];
    uint8_t uid_len=12;
    uint8_t key_index=0;*/

    //tableau pour le nom
    uint8_t  nom[16];
    QString chaine;
    chaine.append("bbb ");

    int sect =10;
    status = Mf_Classic_Authenticate(&MonLecteur, Auth_KeyA, true, 2, key_A, 0);
    qDebug() << "Authenticate " <<status ;
    status=Mf_Classic_Read_Block(&MonLecteur,true,sect,nom,Auth_KeyA,0);
    qDebug() << "Read Block" <<status ;
    for(int i=0;i<16 && nom[i]!='\0';i++)
        chaine.append((char)(nom[i]));

    qDebug() << chaine ;

    //ECRITURE NOM
    char DataIn[16];
    strncpy(DataIn,"testPrim",16);
    //strcpy(uint_8(DataIn),"testPrim");
    status=Mf_Classic_Write_Block(&MonLecteur,true,10,(uint8_t *)DataIn,Auth_KeyB,0);
    qDebug() << "Write Block" <<status ;
    //ECRITURE prenom
    char DataIn2[16];
    strncpy(DataIn2,"testPrenom",16);
    //strcpy(uint_8(DataIn),"testPrim");
    status=Mf_Classic_Write_Block(&MonLecteur,true,9,(uint8_t *)DataIn2,Auth_KeyB,0);
    qDebug() << "Write Block" <<status ;


    //Valeur lecture
    uint32_t value;
    status=Mf_Classic_Read_Value(&MonLecteur,true,14,&value,Auth_KeyA,1);
    qDebug() << "Write Block LECTURE VALEUR" <<status <<value;

    //DECREMENTATION
    int valeurDecre=1;
    status=Mf_Classic_Decrement_Value(&MonLecteur,true,14,valeurDecre,13,Auth_KeyA,1);
    qDebug() << "Decrement" <<status ;
    //Sauvegarde finale
    status=Mf_Classic_Restore_Value(&MonLecteur,true,13,14,Auth_KeyA,1);
    qDebug() << "Sauveagarde" <<status ;

    //INCREMENTATION
    int valeurIncre=1;
    status=Mf_Classic_Increment_Value(&MonLecteur,true,14,valeurIncre,13,Auth_KeyB,1);
    qDebug() << "Decrement" <<status ;
    //Sauvegarde finale
    status=Mf_Classic_Restore_Value(&MonLecteur,true,13,14,Auth_KeyB,1);
    qDebug() << "Sauveagarde" <<status ;




}

void MaFenetre::on_Saisie_clicked(){
    QString Text = ui->fenetre_saisi-> toPlainText();
    qDebug() << "Text : " << Text;
}

void MaFenetre::on_Quitter_clicked(){
     uint16_t status = 0;
     RF_Power_Control(&MonLecteur, FALSE, 0);
     status = LEDBuzzer(&MonLecteur, LED_OFF);
     status = CloseCOM1(&MonLecteur);
     qApp->quit();
}
