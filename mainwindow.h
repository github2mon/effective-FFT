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

private:
    Ui::MainWindow *ui;

    QFile file;
    QFile out;

    void my_main();
    double my_sum(const QVector<double>, int);
    int rev(int, int);
    void bitReverseCopy(const QVector<double>, QVector<MyComplex>&, const int, const int);
    MyComplex get_omiga_m(int m);
    void iterativeFFT(QVector<MyComplex>&, const int, const int);
};

#endif // MAINWINDOW_H
