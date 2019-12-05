#ifndef MYCOMPLEX_H
#define MYCOMPLEX_H

#include <iostream>
#include <QDebug>
using namespace std;

class MyComplex
{
private:
    double real;
    double image;

public:
    MyComplex(double=0.0, double=0.0);
    MyComplex(const MyComplex &c);
    double getReal(void) const;
    double getImage(void) const;
    void setComplex(double r, double i);

//    friend QDebug& operator<<(QDebug& out, const complex& c);
    friend MyComplex operator+(const MyComplex& c1, const MyComplex& c2);
    friend MyComplex operator-(const MyComplex& c1, const MyComplex& c2);
    friend MyComplex operator*(const MyComplex& c1, const MyComplex& c2);
};

#endif // MYCOMPLEX_H
