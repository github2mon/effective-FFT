#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    authentication();

    QFont font_9;
    QFont font_10;
    font_9.setPixelSize(9);
    font_10.setPixelSize(10);
    ui->calculate->setFont(font_9);
    ui->revert->setFont(font_9);
    ui->company_name->setFont(font_10);

    uiHelp = new WidgetHelp();
    uiHelp->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    uiHelp->hide();
    uiHelp->setWindowModality(Qt::ApplicationModal);


    ui->label_result->setAlignment(Qt::AlignVCenter);

    rbGroup = new QButtonGroup(this);
    rbGroup->addButton(ui->radioButton_male,0);
    rbGroup->addButton(ui->radioButton_female,1);
    connect(rbGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(on_radioButton_toggled(int,bool)));
    str_sex = "MALE";

    ui->openfile->setIconSize(QSize(25,25));
    ui->savefile->setIconSize(QSize(25,25));
    ui->help->setIconSize(QSize(25,25));
    ui->database->setIconSize(QSize(25,25));

    QRegExp regx_num("[a-zA-Z0-9_]+$");
    QRegExp regx_height("[1-9]\\d{0,2}\\.\\d");
    QRegExp regx_weight("[1-9]\\d{0,2}\\.\\d");
    QValidator *validator_num = new QRegExpValidator(regx_num, ui->lineEdit_num);
    QValidator *validator_height = new QRegExpValidator(regx_height, ui->lineEdit_height);
    QValidator *validator_weight = new QRegExpValidator(regx_weight, ui->lineEdit_height);

    ui->lineEdit_num->setValidator(validator_num);
    ui->lineEdit_height->setValidator(validator_height);
    ui->lineEdit_weight->setValidator(validator_weight);

    QIntValidator *validator_age = new QIntValidator;
    validator_age->setRange(0,150);
    ui->lineEdit_age->setValidator(validator_age);

    ui->openfile->setEnabled(true);
    ui->calculate->setEnabled(false);
    ui->savefile->setEnabled(false);

    /*********************SQL*********************/
    //添加数据库驱动
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        db = QSqlDatabase::database("qt_sql_default_connection");
    else
        db = QSqlDatabase::addDatabase("QSQLITE");
    //设置数据库路径
    db.setDatabaseName(qApp->applicationDirPath().append("/data.db"));
    if (!db.open())
    {
        qDebug() << "无法建立数据库连接." << db.lastError();
    }
    else
    {
        qDebug()<<"open db!";
        QSqlQuery sql_query;
        sql_query = QSqlQuery (db);//建立数据库连接

        //新建数据库查询表
        sql_query.exec("create table tableData("
                       "Number varchar(20),"
                       "Sex varchar(10),"
                       "Height varchar(10),"
                       "Weight varchar(10),"
                       "Age varchar(10),"
                       "MDIST_AP varchar(10),"
                       "MDIST_ML varchar(10),"
                       "RDIST_AP varchar(10),"
                       "RDIST_ML varchar(10),"
                       "RANGE_AP varchar(10),"
                       "RANGE_ML varchar(10),"
                       "MVELO_AP varchar(10),"
                       "MVELO_ML varchar(10),"
                       "AREA_SW varchar(10),"
                       "MFREQ_AP varchar(10),"
                       "MFREQ_ML varchar(10),"
                       "pfap_50 varchar(10),"
                       "pfml_50 varchar(10),"
                       "pfap_95 varchar(10),"
                       "pfml_95 varchar(10),"
                       "TDS varchar(10),"
                       "s_speed varchar(10),"
                       "s_balance varchar(10),"
                       "s_total varchar(10))");
        db.close();
    }

    uiDB = new widgetDatabase();
    uiDB->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    uiDB->hide();
    uiDB->setWindowModality(Qt::ApplicationModal);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::authentication()
{
    QString pwd;
    pwd.append(get_baseboard_serialnumber())
            .append(get_bios_serialnumber())
            .append(get_cpu_cores())
            .append(get_cpu_processorID())
            .append(get_cpu_processors())
            .append(get_diskdriver_serialnumber());
    qDebug()<<"pwd_hw: "<<pwd;
    QString serialnumber_md5;
    QString serialnumber_final;
    QByteArray ba_pwd;
    QCryptographicHash md_hw(QCryptographicHash::Md5);
    ba_pwd.append(pwd);
    md_hw.addData(ba_pwd);
    serialnumber_md5.append(md_hw.result().toHex());
    qDebug()<<"serialnumber_md5: "<<serialnumber_md5;
    QString pwd_1007;
    pwd_1007.append(serialnumber_md5).append("Monlove1007");
    QByteArray ba_pwd_1007;
    QCryptographicHash md_1007(QCryptographicHash::Md5);
    ba_pwd_1007.append(pwd_1007);
    md_1007.addData(ba_pwd_1007);
    serialnumber_final.append(md_1007.result().toHex());
    qDebug()<<"serialnumber_final"<<serialnumber_final;

    QString strSerialnumberPath(qApp->applicationDirPath());
    strSerialnumberPath.append("/serialnumber/serialnumber");
    qDebug()<<strSerialnumberPath;

    QFile file_auth(strSerialnumberPath);
    bool is_auth_ok = false;
    if(! file_auth.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::critical(this, "ERROR", "Authentication File Error", QMessageBox::Ok);
        if(ret == QMessageBox::Ok)
            exit(0);
    }
    else
    {
//        qDebug()<<"authentication file open";
        while(!file_auth.atEnd())
        {
            QByteArray line = file_auth.readLine();
            QString str(line);
            str.toUtf8();
            str.replace("\r", "");
            str.replace("\n", "");
            str.replace(" ", "");
            qDebug()<<str;
            if(str == serialnumber_final)
            {
                is_auth_ok = true;
                break;
            }
        }
    }
    if(!is_auth_ok)
    {
        QMessageBox::StandardButton ret_md5;
        ret_md5 = QMessageBox::information(this, "Authentication", QString("No Licence!\n%1").arg(serialnumber_md5), QMessageBox::Ok);
        if(ret_md5 == QMessageBox::Ok)
            exit(0);
    }
}

//QString MainWindow::my_encryption(QString fw_version)
//{

//}

QString MainWindow::getWMIC(const QString &cmd)
{
    //获取cpu名称：wmic cpu get Name
    //获取cpu核心数：wmic cpu get NumberOfCores
    //获取cpu线程数：wmic cpu get NumberOfLogicalProcessors
    //查询cpu序列号：wmic cpu get processorid
    //查询主板序列号：wmic baseboard get serialnumber
    //查询BIOS序列号：wmic bios get serialnumber
    //查看硬盘：wmic diskdrive get serialnumber
    QProcess p;
    p.start(cmd);
    p.waitForFinished();
    QString result = QString::fromLocal8Bit(p.readAllStandardOutput());
    QStringList list = cmd.split(" ");
    result = result.remove(list.last(), Qt::CaseInsensitive);
    result = result.replace("\r", "");
    result = result.replace("\n", "");
    result = result.replace(" ", "");
    result = result.simplified();
    return result;
}

QString MainWindow::get_baseboard_serialnumber()
{
    return getWMIC("wmic baseboard get serialnumber");
}

QString MainWindow::get_bios_serialnumber()
{
    return getWMIC("wmic bios get serialnumber");
}

QString MainWindow::get_cpu_cores()
{
    return getWMIC("wmic cpu get NumberOfCores");
}

QString MainWindow::get_cpu_name()
{
    return getWMIC("wmic cpu get Name");
}

QString MainWindow::get_cpu_processorID()
{
    return getWMIC("wmic cpu get processorid");
}

QString MainWindow::get_cpu_processors()
{
    return getWMIC("wmic cpu get NumberOfLogicalProcessors");
}

QString MainWindow::get_diskdriver_serialnumber()
{
    return getWMIC("wmic diskdrive get serialnumber");
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
    bitReverseCopy(x_centralize, fft_x, iExponent, iLength);
    iterativeFFT(fft_x, iExponent, iLength);
    bitReverseCopy(y_centralize, fft_y, iExponent, iLength);
    iterativeFFT(fft_y, iExponent, iLength);

    //获得功率谱密度
    for(int i=0; i<iLength; i++)
    {
        psd_x[i] = (qPow(fft_x[i].getReal(),2) + qPow(fft_x[i].getImage(),2))/iLength;
        psd_y[i] = (qPow(fft_y[i].getReal(),2) + qPow(fft_y[i].getImage(),2))/iLength;
    }

    //求功率谱面积
    for(int i=1; i<iLength/2; i++)
    {
        sum_psd_area_x[i] = sum_psd_area_x[i-1] + psd_x[i]/iLength;
        sum_psd_area_y[i] = sum_psd_area_y[i-1] + psd_y[i]/iLength;
    }

    //取50%和95%
    int psd_x_50=0, psd_y_50=0, psd_x_95=0, psd_y_95=0;
//    double ref_50=0.0, ref_95=0.0;
    for(int i=0; i<iLength/2; i++)
    {
        if(sum_psd_area_x[i]/sum_psd_area_x[iLength/2-1] < 0.5)
            psd_x_50 = i;
        if(sum_psd_area_x[i]/sum_psd_area_x[iLength/2-1] < 0.95)
            psd_x_95 = i;
        if(sum_psd_area_y[i]/sum_psd_area_y[iLength/2-1] < 0.5)
            psd_y_50 = i;
        if(sum_psd_area_y[i]/sum_psd_area_y[iLength/2-1] < 0.95)
            psd_y_95 = i;
    }
    pfml_50=0.0; pfml_95=0.0; pfap_50=0.0; pfap_95=0.0;
    /*
    a b c
    d e f=d+1
    e = f-(c-b)/(c-a)
    */
    pfml_50 = (psd_x_50+1
            - ((sum_psd_area_x[psd_x_50+1] - sum_psd_area_x[iLength/2-1]*0.5)
                 / (sum_psd_area_x[psd_x_50+1] - sum_psd_area_x[psd_x_50])))
            * 100 / iLength;
    pfml_95 = (psd_x_95+1
            - ((sum_psd_area_x[psd_x_95+1] - sum_psd_area_x[iLength/2-1]*0.95)
                 / (sum_psd_area_x[psd_x_95+1] - sum_psd_area_x[psd_x_95])))
            * 100 / iLength;
    pfap_50 = (psd_y_50+1
            - ((sum_psd_area_y[psd_y_50+1] - sum_psd_area_y[iLength/2-1]*0.5)
                 / (sum_psd_area_y[psd_y_50+1] - sum_psd_area_y[psd_y_50])))
            * 100 / iLength;
    pfap_95 = (psd_y_95+1
            - ((sum_psd_area_y[psd_y_95+1] - sum_psd_area_y[iLength/2-1]*0.95)
                 / (sum_psd_area_y[psd_y_95+1] - sum_psd_area_y[psd_y_95])))
            * 100 / iLength;
//    qDebug()<<pfap_50<< ' '<<pfap_95<<pfml_50<< ' '<<pfml_95;

    //other parameter
    /****************************************************
     * AP: Y轴方向，坐标与平均坐标的差，即y_centralize
     * ML: X轴方向，坐标与平均坐标的差，即x_centralize
     * MDIST_AP: AP绝对值的平均值
     * MDIST_ML: ML绝对值的平均值
     * RDIST_AP: AP均方根 = sqrt(Zigma(AP^2)/iLength)
     * RDIST_ML: ML均方根
     * RANGE_AP: AP相邻两列数据最大差值
     * RANGE_ML: ML相邻两列数据最大差值
     * MVELO_AP: AP平均速率
     * MVELO_ML: ML平均速率
     * AREA_SW: Zigma|AP[n+1]*ML[n] - AP[n]*ML[n+1]| / 2T
     * MFREQ_AP: MVELO_AP/MDIST_AP/4/sqrt(2)
     * MFREQ_ML: MVELO_ML/MDIST_ML/4/sqrt(2)
    ****************************************************/
    //将x_centralize修改为x，y同理
    MDIST_AP=0.0; MDIST_ML=0.0;
    double sum_mdist_ap=abs(y/*_centralize*/[0]), sum_mdist_ml=abs(x/*_centralize*/[0]);
    RDIST_AP=0.0; RDIST_ML=0.0;
    double sum_mdist_ap_2=qPow(y/*_centralize*/[0],2), sum_mdist_ml_2=qPow(x/*_centralize*/[0],2);
    RANGE_AP=0.0; RANGE_ML=0.0;
    double sum_velocity_x=0.0, sum_velocity_y=0.0;
    AREA_SW = 0.0;
    double sum_area = 0.0;
    MVELO_AP=0.0; MVELO_ML=0.0;
    MFREQ_AP=0.0; MFREQ_ML=0.0;
    double temp_x_min = x[0];
    double temp_x_max = x[0];
    double temp_y_min = y[0];
    double temp_y_max = y[0];
    for(int i=1; i<iLength; i++)
    {
        sum_mdist_ap += abs(y/*_centralize*/[i]);
        sum_mdist_ml += abs(x/*_centralize*/[i]);
        sum_mdist_ap_2 += qPow(y/*_centralize*/[i],2);
        sum_mdist_ml_2 += qPow(x/*_centralize*/[i],2);

        double x_distance = abs(x/*_centralize*/[i] - x/*_centralize*/[i-1]);
        double y_distance = abs(y/*_centralize*/[i] - y/*_centralize*/[i-1]);
        sum_velocity_x += x_distance * 1000 / (time[i] - time[i-1]);
        sum_velocity_y += y_distance * 1000/ (time[i] - time[i-1]);

//        if(RANGE_AP < y_distance)
//            RANGE_AP = y_distance;
//        if(RANGE_ML < x_distance)
//            RANGE_ML = x_distance;

        if(x[i] > temp_x_max)
            temp_x_max = x[i];
        if(x[i] < temp_x_min)
            temp_x_min = x[i];
        if(y[i] > temp_y_max)
            temp_y_max = y[i];
        if(y[i] < temp_y_min)
            temp_y_min = y[i];

        sum_area += abs(y/*_centralize*/[i]*x/*_centralize*/[i-1] - y/*_centralize*/[i-1]*x/*_centralize*/[i]) *1000;
    }


    MDIST_AP = sum_mdist_ap / iLength;
    MDIST_ML = sum_mdist_ml / iLength;
    RDIST_AP = sqrt(sum_mdist_ap_2 / iLength);
    RDIST_ML = sqrt(sum_mdist_ml_2 / iLength);
    RANGE_AP = temp_y_max - temp_y_min;
    RANGE_ML = temp_x_max - temp_x_min;
    MVELO_AP = sum_velocity_y / (iLength-1);
    MVELO_ML = sum_velocity_x / (iLength-1);
    AREA_SW  = sum_area / (time[iLength-1] - time[0]) / 2;
    MFREQ_AP = MVELO_AP / MDIST_AP / 4 / sqrt(2);
    MFREQ_ML = MVELO_ML / MDIST_ML / 4 / sqrt(2);

    TDS = 0.0;
    s_speed = 0.0; s_balance = 0.0; s_total = 0.0;
    double force_max = f[0], force_min = f[0];
    double t90_force_max = 0.0, t25_force_max = 0.0;
    int force_max_index = 0, force_min_index = 0;
    double y_min = y[0];
    int y_min_index = 0;
    for(int i=1; i<iLength; i++)
    {
        TDS += sqrt(qPow(x[i]-x[i-1], 2) + qPow(y[i]-y[i-1], 2));
        if(f[i] > force_max)
        {
            force_max = f[i];
            force_max_index = i;
        }
        if(y[i] < y_min)
        {
            y_min = y[i];
            y_min_index = i;
        }
    }
    if((force_max_index < 300) || (time[iLength-1] < time[y_min_index] + 3000))
        return;
    else
        force_min = f[force_max_index-300];//need modify
    for(int i=force_max_index-300; i<force_max_index; i++)
    {
        if(f[i] < force_min)
        {
            force_min = f[i];
            force_min_index = i;
        }
    }
    double f_90 = force_min + (force_max - force_min)*0.9;
    double f_25 = force_min + (force_max - force_min)*0.25;
    for(int i=force_max_index; i>force_min_index; i--)
    {
        if(f[i] <= f_90)
        {
            t90_force_max = time[i] + (f_90-f[i]) / (f[i+1]-f[i]) * (time[i+1]-time[i]);
            break;
        }
    }
    for(int i=force_max_index; i>force_min_index; i--)
    {
        if(f[i] <= f_25)
        {
            t25_force_max = time[i] + (f_25-f[i]) / (f[i+1]-f[i]) * (time[i+1]-time[i]);
            break;
        }
    }
    double weight = 60.0;
//    bool ok;
//    weight = QInputDialog::getDouble(this, "Input Weight", "Please input weight(default = 60)", 60.0, 0, 500, 1, &ok);
    if(ui->lineEdit_weight->text().isEmpty())
        weight = 60.0;
    else
        weight = ui->lineEdit_weight->text().toDouble();
    qDebug()<<"weight = "<<weight;
    s_speed = 0.65 * force_max / (t90_force_max - t25_force_max) / weight;

    for(int i=y_min_index; i<iLength-1; i++)
    {
        if(time[i] < time[y_min_index] + 3000)
            s_balance += sqrt(qPow(x[i+1]-x[i], 2) + qPow(y[i+1]-y[i], 2));
        else
            break;
    }
    s_total = s_speed * s_balance;
}

double MainWindow::my_sum(const QVector<double> input, int length)
{
    double sum = 0.0;
    for(int i=0; i<length; i++)
        sum += input[i];
    return sum;
}

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

void MainWindow::on_openfile_clicked()
{
    QString filename;
    filename = QFileDialog::getOpenFileName(this, tr("Get File"), "");
    //读数据
    file.setFileName(filename);
    if(! file.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->path->setText(file.errorString());
//        ui->statusBar->showMessage(file.errorString());
    else
    {
        ui->path->setText(filename);
//        ui->statusBar->showMessage(QString("%1 open successful!").arg(filename));
        ui->openfile->setEnabled(false);
        ui->calculate->setEnabled(true);
        ui->savefile->setEnabled(false);
    }
}

void MainWindow::on_calculate_clicked()
{
    my_main();
//    ui->statusBar->showMessage(tr("calculate finished"));
    ui->openfile->setEnabled(false);
    ui->calculate->setEnabled(false);
    ui->savefile->setEnabled(true);
    ui->label_result->setText(QString("MDIST_AP %1\n"
                                      "MDIST_ML %2\n"
                                      "RDIST_AP %3\n"
                                      "RDIST_ML %4\n"
                                      "RANGE_AP %5\n"
                                      "RANGE_ML %6\n"
                                      "MVELO_AP %7\n"
                                      "MVELO_ML %8\n"
                                      "AREA_SW  %9\n"
                                      "MFREQ_AP %10\n"
                                      "MFREQ_ML %11\n"
                                      "pfap_50  %12\n"
                                      "pfml_50  %13\n"
                                      "pfap_95  %14\n"
                                      "pfml_95  %15\n"
                                      "TDS      %16\n"
                                      "speed score   %17\n"
                                      "balance score %18\n"
                                      "total score   %19\n")
                              .arg(MDIST_AP).arg(MDIST_ML).arg(RDIST_AP).arg(RDIST_ML)
                              .arg(RANGE_AP).arg(RANGE_ML).arg(MVELO_AP).arg(MVELO_ML)
                              .arg(AREA_SW).arg(MFREQ_AP).arg(MFREQ_ML).arg(pfap_50)
                              .arg(pfml_50).arg(pfap_95).arg(pfml_95).arg(TDS)
                              .arg(s_speed).arg(s_balance).arg(s_total));
    ui->label_result->setAlignment(Qt::AlignVCenter);

    db.open();
    sql_query = QSqlQuery(db);
    sql_query.exec ("select * from tableSearch");

    sql_query.prepare("insert into tableData values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
    sql_query.bindValue(0, QString("%1").arg(ui->lineEdit_num->text()));
    sql_query.bindValue(1, QString("%1").arg(str_sex));
    sql_query.bindValue(2, QString("%1").arg(ui->lineEdit_height->text()));
    sql_query.bindValue(3, QString("%1").arg(ui->lineEdit_weight->text()));
    sql_query.bindValue(4, QString("%1").arg(ui->lineEdit_age->text()));
    sql_query.bindValue(5, QString("%1").arg(MDIST_AP));
    sql_query.bindValue(6, QString("%1").arg(MDIST_ML));
    sql_query.bindValue(7, QString("%1").arg(RDIST_AP));
    sql_query.bindValue(8, QString("%1").arg(RDIST_ML));
    sql_query.bindValue(9, QString("%1").arg(RANGE_AP));
    sql_query.bindValue(10, QString("%1").arg(RANGE_ML));
    sql_query.bindValue(11, QString("%1").arg(MVELO_AP));
    sql_query.bindValue(12, QString("%1").arg(MVELO_ML));
    sql_query.bindValue(13, QString("%1").arg(AREA_SW));
    sql_query.bindValue(14, QString("%1").arg(MFREQ_AP));
    sql_query.bindValue(15, QString("%1").arg(MFREQ_ML));
    sql_query.bindValue(16, QString("%1").arg(pfap_50));
    sql_query.bindValue(17, QString("%1").arg(pfml_50));
    sql_query.bindValue(18, QString("%1").arg(pfap_95));
    sql_query.bindValue(19, QString("%1").arg(pfml_95));
    sql_query.bindValue(20, QString("%1").arg(TDS));
    sql_query.bindValue(21, QString("%1").arg(s_speed));
    sql_query.bindValue(22, QString("%1").arg(s_balance));
    sql_query.bindValue(23, QString("%1").arg(s_total));
    sql_query.exec();
    db.close();
    qDebug()<<"db write";
}

void MainWindow::on_savefile_clicked()
{
    QString outname = QFileDialog::getSaveFileName(this, tr("Save File"), "", "Files (*.txt)");

    out.setFileName(outname);
    if(! out.open(QIODevice::WriteOnly | QIODevice::Text))
        ui->path->setText(out.errorString());
//        ui->statusBar->showMessage(file.errorString());
    else
    {
        out.write(QString("Number %1\n").arg(ui->lineEdit_num->text()).toUtf8());
        out.write(QString("Sex %1\n").arg(str_sex).toUtf8());
        out.write(QString("Height %1\n").arg(ui->lineEdit_height->text()).toUtf8());
        out.write(QString("Weight %1\n").arg(ui->lineEdit_weight->text()).toUtf8());
        out.write(QString("Age %1\n\n").arg(ui->lineEdit_age->text()).toUtf8());
        out.write(QString("MDIST_AP %1\n").arg(MDIST_AP).toUtf8());
        out.write(QString("MDIST_ML %1\n").arg(MDIST_ML).toUtf8());
        out.write(QString("RDIST_AP %1\n").arg(RDIST_AP).toUtf8());
        out.write(QString("RDIST_ML %1\n").arg(RDIST_ML).toUtf8());
        out.write(QString("RANGE_AP %1\n").arg(RANGE_AP).toUtf8());
        out.write(QString("RANGE_ML %1\n").arg(RANGE_ML).toUtf8());
        out.write(QString("MVELO_AP %1\n").arg(MVELO_AP).toUtf8());
        out.write(QString("MVELO_ML %1\n").arg(MVELO_ML).toUtf8());
        out.write(QString("AREA_SW %1\n").arg(AREA_SW).toUtf8());
        out.write(QString("MFREQ_AP %1\n").arg(MFREQ_AP).toUtf8());
        out.write(QString("MFREQ_ML %1\n").arg(MFREQ_ML).toUtf8());
        out.write(QString("pfap_50 %1\n").arg(pfap_50).toUtf8());
        out.write(QString("pfml_50 %1\n").arg(pfml_50).toUtf8());
        out.write(QString("pfap_95 %1\n").arg(pfap_95).toUtf8());
        out.write(QString("pfml_95 %1\n").arg(pfml_95).toUtf8());
        out.write(QString("TDS %1\n").arg(TDS).toUtf8());
        out.write(QString("speed score %1\n").arg(s_speed).toUtf8());
        out.write(QString("balance score %1\n").arg(s_balance).toUtf8());
        out.write(QString("total score %1\n").arg(s_total).toUtf8());
        out.close();

//        ui->statusBar->showMessage(QString("parameter save successful!"));
        ui->openfile->setEnabled(true);
        ui->calculate->setEnabled(false);
        ui->savefile->setEnabled(false);
    }
}

void MainWindow::on_radioButton_toggled(int id, bool checked)
{
    if(rbGroup->checkedId() == 0)
        str_sex = "MALE";
    else if(rbGroup->checkedId() == 1)
        str_sex = "FEMALE";
    else
        str_sex = "NULL";
}

void MainWindow::on_revert_clicked()
{
    ui->openfile->setEnabled(true);
    ui->calculate->setEnabled(false);
    ui->savefile->setEnabled(false);
    ui->path->setText("No file name specified");
    ui->lineEdit_num->clear();
    ui->lineEdit_height->clear();
    ui->lineEdit_weight->clear();
    ui->lineEdit_age->clear();
    ui->label_result->setText(QString("MDIST_AP 0\n"
                                      "MDIST_ML 0\n"
                                      "RDIST_AP 0\n"
                                      "RDIST_ML 0\n"
                                      "RANGE_AP 0\n"
                                      "RANGE_ML 0\n"
                                      "MVELO_AP 0\n"
                                      "MVELO_ML 0\n"
                                      "AREA_SW  0\n"
                                      "MFREQ_AP 0\n"
                                      "MFREQ_ML 0\n"
                                      "pfap_50  0\n"
                                      "pfml_50  0\n"
                                      "pfap_95  0\n"
                                      "pfml_95  0\n"
                                      "TDS      0\n"
                                      "speed score   0\n"
                                      "balance score 0\n"
                                      "total score   0\n"));
    ui->label_result->setAlignment(Qt::AlignVCenter);
    if(file.isOpen())
        file.close();
}

void MainWindow::on_help_clicked()
{
    uiHelp->show();
    uiHelp->setWindowModality(Qt::ApplicationModal);
}

void MainWindow::on_database_clicked()
{
    uiDB->dbDisplay();
    uiDB->lineEditorClear();

    uiDB->show();
    uiDB->setWindowModality(Qt::ApplicationModal);
}
