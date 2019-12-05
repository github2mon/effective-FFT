#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QtCore/qmath.h>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QLineEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QValidator>
#include <QButtonGroup>
#include <QFont>
#include <QProcess>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include <QTableView>

#include <mycomplex.h>
#include <widgethelp.h>
#include <widgetdatabase.h>

#define _USE_MATH_DEFINES

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_openfile_clicked();

    void on_calculate_clicked();

    void on_savefile_clicked();

    void on_radioButton_toggled(int id, bool checked);

    void on_revert_clicked();

    void on_help_clicked();

    void on_database_clicked();

private:
    Ui::MainWindow *ui;

    WidgetHelp *uiHelp;
    widgetDatabase *uiDB;

    QFile file;
    QFile out;
    QButtonGroup *rbGroup;

    QString str_sex;
    double MDIST_AP, MDIST_ML;
    double RDIST_AP, RDIST_ML;
    double RANGE_AP, RANGE_ML;
    double AREA_SW;
    double MVELO_AP, MVELO_ML;
    double MFREQ_AP, MFREQ_ML;
    double pfml_50, pfml_95, pfap_50, pfap_95;
    double TDS;
    double s_speed, s_balance, s_total;

    int dataNum;
    QSqlDatabase db;
    QSqlQuery sql_query;

    void my_main();
    double my_sum(const QVector<double>, int);
    int rev(int, int);
    void bitReverseCopy(const QVector<double>, QVector<MyComplex>&, const int, const int);
    MyComplex get_omiga_m(int m);
    void iterativeFFT(QVector<MyComplex>&, const int, const int);

    void authentication();
    QString getWMIC(const QString &cmd);
    QString get_cpu_name();
    QString get_cpu_cores();
    QString get_cpu_processors();
    QString get_cpu_processorID();
    QString get_baseboard_serialnumber();
    QString get_bios_serialnumber();
    QString get_diskdriver_serialnumber();
//    QString my_encryption(QString);
};

#endif // MAINWINDOW_H
