#include "mycomplex.h"

MyComplex::MyComplex(double r, double i)
{
    real = r;
    image = i;
}

MyComplex::MyComplex(const MyComplex &c)
{
    real = c.real;
    image = c.image;
}

double MyComplex::getReal(void) const
{
    return real;
}

double MyComplex::getImage(void) const
{
    return image;
}

void MyComplex::setComplex(double r, double i)
{
    real = r;
    image = i;
}

//QDebug& operator<<(QDebug& out, const MyComplex& c)
//{
//    if(c.image>0)
//        out<<c.real<<'+'<<c.image<<'i';
//    if(c.image<0)
//        out<<c.real<<'-'<<c.image<<'i';
//    if(c.image==0)
//        out<<c.real;
//    return out;
//}

MyComplex operator+(const MyComplex& c1, const MyComplex& c2)
{
    MyComplex t;
    t.real = c1.real + c2.real;
    t.image = c1.image + c2.image;
    return t;
}

MyComplex operator-(const MyComplex& c1, const MyComplex& c2)
{
    MyComplex t;
    t.real = c1.real - c2.real;
    t.image = c1.image - c2.image;
    return t;
}

MyComplex operator*(const MyComplex& c1, const MyComplex& c2)
{
    MyComplex t;
    t.real = c1.real*c2.real - c1.image*c2.image;
    t.image = c1.image*c2.real + c1.real*c2.image;
    return t;
}
