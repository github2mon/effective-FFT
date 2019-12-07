/*
reverse bits
binary tree: 
                                        0,1,2,3,4,5,6,7
                          0,2,4,6                            1,3,5,7
                   0,4               2,6             1,5                 3,7
                0       4         2        6      1        5          3        7
					
input        :  0 ,  4 ,  2 ,  6 ,  1 ,  5 ,  3 ,  7
binary input : 000, 100, 010, 110, 001, 101, 011, 111
binary output: 000, 001, 010, 011, 100, 101, 110, 111
output       :  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7
*/
int MainWindow::rev(int input, int exponent)
{
    int result = 0;
    for(int i=0; i<exponent; i++)
    {
        result = (result << 1) | (input & 1);
        input >>= 1;
//        qDebug()<<result<<' '<<input;
    }
    return result;
}

void MainWindow::bitReverseCopy(const QVector<double> input, QVector<MyComplex> &fft, const int exponent, const int length)
{
    for(int i=0; i<length; i++)
        fft[rev(i, exponent)].setComplex(input[i], 0);
//        array[rev(i, exponent)] = input[i];
}

MyComplex MainWindow::get_omiga_m(int m)
{
    //欧拉公式：exp(ix) = cosx + isinx, x = 2*M_PI/m
    return MyComplex(qCos(2*M_PI/m), qSin(2*M_PI/m));
}

/*
        y1[k] ------------- + ----- y1[k] + omiga[k] * y2[k]
                         \ /
        omiga[k] ---      /
                    |    / \
        y2[k] ----- * ----- - ----- y2[k] - omiga[k] * y1[k]
*/
void MainWindow::iterativeFFT(QVector<MyComplex> &fft, const int exponent, const int length)
{
    for(int s=1; s<=exponent; s++)
    {
        int m = qPow(2, s);
        MyComplex omiga_m = get_omiga_m(m);
        for(int k=0; k<length; k+=m)
        {
            MyComplex omiga(1,0);
            for(int j=0; j<m/2; j++)
            {
                MyComplex t = omiga * fft[k+j+m/2];
                MyComplex u = fft[k+j];
                fft[k+j] = u + t;
                fft[k+j+m/2] = u - t;
                omiga = omiga * omiga_m;
            }
        }
    }
}

void MainWindow::my_main()
{
    QVector<int> time;
    QVector<double> x;
    QVector<double> x_centralize;
    QVector<double> y;
    QVector<double> y_centralize;
    QVector<double> f;
    QVector<MyComplex> fft_x;
    QVector<MyComplex> fft_y;
    QVector<double> psd_x;
    QVector<double> psd_y;
    QVector<double> sum_psd_area_x;
    QVector<double> sum_psd_area_y;

    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        QString str(line);
        QStringList list = str.split(' ');

        //数据输入
		//data input
        time.append(list[0].toInt());
        if(time.count()>1)
        {
            if(time[time.count()-1] == time[time.count()-2])
            {
                time.removeLast();
                continue;
            }
        }

        x.append(list[5].toDouble());
        x_centralize.append(0.0);
        y.append(list[6].toDouble());
        y_centralize.append(0.0);
        f.append(list[7].toDouble());
        fft_x.append(MyComplex(0.0, 0.0));
        fft_y.append(MyComplex(0.0, 0.0));
        psd_x.append(0.0);
        psd_y.append(0.0);
        sum_psd_area_x.append(0.0);
        sum_psd_area_y.append(0.0);
    }
    file.close();

    int iExponent = (int)(qLn(x.count())/qLn(2));
    int iLength = qPow(2, iExponent);

    //数据处理，总长度为2的n次方，向下取整
	//data processing for FFT, the length of data array should equal to 2^n
    int rm = x.count() - iLength;
    time.remove(0,rm);
    x.remove(0,rm);
    x_centralize.remove(0,rm);
    y.remove(0,rm);
    y_centralize.remove(0,rm);
    f.remove(0,rm);
    fft_x.remove(0,rm);
    fft_y.remove(0,rm);
    psd_x.remove(0,rm);
    psd_y.remove(0,rm);
    sum_psd_area_x.remove(0,rm);
    sum_psd_area_y.remove(0,rm);

    //中心化
	//data entralize
    double sum_x=0.0, sum_y=0.0;
    for(int i=0; i<iLength; i++)
    {
        sum_x += x[i];
        sum_y += y[i];
    }
    for(int i=0; i<iLength; i++)
    {
        x_centralize[i] = x[i] - sum_x/iLength;
        y_centralize[i] = y[i] - sum_y/iLength;
    }

    //计算FFT
	//calculate FFT
    bitReverseCopy(x_centralize, fft_x, iExponent, iLength);
    iterativeFFT(fft_x, iExponent, iLength);
    bitReverseCopy(y_centralize, fft_y, iExponent, iLength);
    iterativeFFT(fft_y, iExponent, iLength);

    //获得功率谱密度
	//calculate PSD(power spectrum density)
    for(int i=0; i<iLength; i++)
    {
        psd_x[i] = (qPow(fft_x[i].getReal(),2) + qPow(fft_x[i].getImage(),2))/iLength;
        psd_y[i] = (qPow(fft_y[i].getReal(),2) + qPow(fft_y[i].getImage(),2))/iLength;
    }

    //求功率谱面积
	//calculate Power Spectral Area
    for(int i=1; i<iLength/2; i++)
    {
        sum_psd_area_x[i] = sum_psd_area_x[i-1] + psd_x[i]/iLength;
        sum_psd_area_y[i] = sum_psd_area_y[i-1] + psd_y[i]/iLength;
    }
