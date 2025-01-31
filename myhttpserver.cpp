#include <QJsonDocument>
#include <QTextCodec>
#include <QSettings>
#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QJsonArray>
#include <QSerialPortInfo>
#include <windows.h>

#include "myhttpserver.h"
#include "lightcontroll.h"
#include "lightCmdList.h"

#define INIFILE_SAST "/SdkClient.ini"
#define INIFILE_LIGHT "/Controllers.ini"

MyHttpServer* g_pMainWnd = nullptr;

MyHttpServer::MyHttpServer(QObject *parent)
    : QObject(parent)
    //, m_controllCount(0)
    , m_sendType(TCP)
{
    g_pMainWnd  = this;
}

MyHttpServer::~MyHttpServer()
{
    if(m_httpServer != nullptr) delete m_httpServer;
    m_httpServer = nullptr;
    emit showMsg("http服务已关闭");
    //TSDK_DeInit();
}

void MyHttpServer::create(QHostAddress address, int port)
{
    // 初始化音响数据
    initSast();

    // 初始化http服务
    m_address = address;
    m_port = port;
    m_httpServer = new QHttpServer(this);

    //处理雾灯 post 请求
    m_httpServer->route("/light", QHttpServerRequest::Method::Post,
                         [this](const QHttpServerRequest &request) {
    /*  旧
        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(request.body());
        QJsonObject json = jsonDoc.object();

        //qDebug() << request.body();
        //emit showMsg(request.body());
        //解析json数据 回复json数据
        return parseLightJson(json);*/

        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(request.body());
        QJsonObject json = jsonDoc.object();
        //showMsg(request.body());
        QJsonObject backJson;
        backJson.insert("code", 0);
        backJson.insert("msg", "成功");
        //如果必要参数不存在，直接退出
        if(json.find("TermIp") == json.end()) {
            backJson.find("code").value() = 1;
            backJson.find("msg").value() = "ip字段不存在";
            return backJson;
        }

        if(json.find("Version") != json.end()){
            if(json["Version"].isDouble()){
                if(json["Version"].toInt() == 2){
                    return parseLightJson2(json);
                }else{
                    return parseLightJson(json);
                }
            }else{
                backJson.find("code").value() = 1;
                backJson.find("msg").value() = "Version 错误";
                return backJson;
            }

        }else{
            return parseLightJson(json);
        }

        //qDebug() << request.body();
        //emit showMsg(request.body());
    });

    // 处理雾灯 post 请求 支持最新版雾灯协议 3.2
    m_httpServer->route("/lightVer3", QHttpServerRequest::Method::Post,
                        [this](const QHttpServerRequest &request) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(request.body());
        QJsonObject json = jsonDoc.object();
        showMsg(request.body());

        return parseLightJson3(json);

    });


   //处理屏幕 post 请求
    m_httpServer->route("/screen/changeDef", QHttpServerRequest::Method::Post,
                         [this](const QHttpServerRequest &request) {

        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(request.body());
        QJsonObject json = jsonDoc.object();

        //qDebug() << request.body();

        //showMsg(request.body());
        //解析json数据 回复json数据
        //jsonBack.insert("code", "0");
        return parseScreenJson(json);
    });

    //处理屏幕 post 请求
     m_httpServer->route("/screen/setScreenState", QHttpServerRequest::Method::Post,
                          [this](const QHttpServerRequest &request) {

        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(request.body());
        QJsonObject json = jsonDoc.object();
        QJsonObject jsonBack;

        //qDebug() << request.body();

        //showMsg("车道改变状态接收json： " + request.body());

        //解析json数据 回复json数据
        if(json.find("state") != json.end() && json.find("state")->isString()
                && json.find("TermIp") != json.end() && json.find("TermIp")->isString()){
            if(json.find("state")->toString() != "0" && json.find("state")->toString() != "1"){
                qDebug() << json.find("state")->toString();
                jsonBack.insert("code", "1");
                jsonBack.insert("msg", "state，值类错误");
                return jsonBack;
            }else{
                emit sigalUpdateScreenState(json.find("TermIp")->toString(), json.find("state")->toString());
            }
        }else{
            jsonBack.insert("code", "1");
            jsonBack.insert("msg", "TermIp或者state不存在，或者值类型错误");
            return jsonBack;
        }
        jsonBack.insert("code", "0");
        return jsonBack;
     });

    //post 应急车道开关
    m_httpServer->route("/setRoadState", QHttpServerRequest::Method::Post,
                         [this](const QHttpServerRequest &request) {

        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(request.body());
        QJsonObject json = jsonDoc.object();
        QJsonObject jsonBack;

        //qDebug() << request.body();

        //showMsg("车道改变状态接收json： " + request.body());

        //解析json数据 回复json数据
        if(json.find("state") != json.end() && json.find("state")->isString())
            emit sigalUpdateRoadState(json.find("state")->toString());
        else{
            jsonBack.insert("code", "1");
            jsonBack.insert("msg", "state不存在，或者值类型错误");
            return jsonBack;
        }
        jsonBack.insert("code", "0");
        return jsonBack;
    });

    //处理音响 post 请求
    m_httpServer->route("/sast", QHttpServerRequest::Method::Post,
                         [this](const QHttpServerRequest &request) {

        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(request.body());
        QJsonObject json = jsonDoc.object();

        //qDebug() << request.body();

        showMsg(request.body());

        //解析json数据 回复json数据
        return parseSastJson(json);
    });

    // forTest
    m_httpServer->route("/radar/data", QHttpServerRequest::Method::Post,
                         [this](const QHttpServerRequest &request) {

        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(request.body());
        QJsonObject json = jsonDoc.object();

        //qDebug() << request.body();

        showMsg(request.body());

        //解析json数据 回复json数据
        return parseSastJson(json);
    });

    m_httpServer->listen(address, m_port);
    emit showMsg(QString("http服务已开启，监听地址：%1  监听端口：%2").arg(m_address.toString()).arg(m_port));
}

bool MyHttpServer::ipAddrIsOK(const QString &ip)
{
    if (ip.isEmpty())
    {
        return false;
    }

    QStringList list = ip.split('.');
    if (list.size() != 4)
    {
        return false;
    }

    for (const auto& num : list)
    {
        bool ok = false;
        int temp = num.toInt(&ok);
        if (!ok || temp < 0 || temp > 255)
        {
            return false;
        }
    }

    return true;
}

void MyHttpServer::updateLightControllList(QList<lightcontroll *> * lightControllList)
{
    m_lightControllList = lightControllList;
}

QJsonObject MyHttpServer::parseLightJson(QJsonObject& json)
{
    QString errorMsg;
    QJsonObject backJson;
    backJson.insert("code", 0);
    backJson.insert("msg", "成功");
    /*   加入设备编号
        value

        直接写入kafka
    */

    // 获取版本号
    int version;
    if(json.find("Version") == json.end()) {
        version = 1;
    }else{
        version = json["Version"].toInt();
        if(version < 1) version = 1;
    }

    //如果必要参数不存在，直接退出
    if(json.find("TermIp") == json.end()
        || json.find("TermId") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数";
        return backJson;
    }

    //不需要修改
    if(json.find("Content") == json.end()
        && json.find("Luminance") == json.end()
        && json.find("Flicker") == json.end()
        && json.find("FontColor") == json.end()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "没有需要修改的值";
        return backJson;
    }

    //获取控制器 ip
    QString TermIp = json["TermIp"].toString(); // 控制器 ip
    if(!ipAddrIsOK(TermIp)){    // 判断ip是否合法
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "ip地址不合法";
        return backJson;
    }

    //获取控制器
    lightcontroll* controller = getControllerFromIp(TermIp);
    /*if(m_sendType == TCP){*/
    if(controller == nullptr){                       // 判断该 ip 控制器是否在线
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "控制器[" + TermIp + "]不在线";
        return backJson;
    }
    /*}*/

    QJsonArray TermId = json["TermId"].toArray();       // 雾灯编号
    //int sbbh = json["sbbh"].toInt();                  // 控制器编号
    int Luminance;              // 亮度值
    //QString Content ;         // 需要显示的文字列表
    QJsonArray Content ;        // 需要显示的文字列表
    QJsonArray Flicker;         // 闪烁值列表
    QStringList sendDataList;   // 需要发送的命令列表
    QString cmdStr;             // 发送的命令                *********
    int TermIdSize = 0;         // 雾灯编号数量              *********

    backJson.insert("code", 0);

    // 获取 TermId
    if(json.find("TermId") != json.end()){
        if(!json["TermId"].isArray()){
            backJson.find("code").value() = 1;
            backJson.find("msg").value() = "TermId 内容不是array";
            return backJson;
        }
        TermId = json["TermId"].toArray();
        TermIdSize = TermId.size();

    }else{
        backJson.insert("TermId", "字段不存在");
    }

    //需要更改显示内容
    if(json.find("Content") != json.end()){
        cmdStr = DISPLAY_FONT;                        // 需要发送的命令
        if(!json["Content"].isArray()){
            backJson.find("code").value() = 1;
            backJson.find("msg").value() = "content 内容不是array";
            return backJson;
        }
        Content = json["Content"].toArray();          // 文字内容
        int contentSize = Content.size();             // 文字数量
        //qDebug() << "Content = " << Content;
        if(contentSize == 0){                         //关灯
            cmdStr.replace("%2", "A0 F0");

            if(TermIdSize == 0){                    //全局灯进行控制
                sendDataList.insert(0, cmdStr.replace("%1","FF").toUpper());
            }else{                                    //挨个控制
                for(int i = 0; i<TermIdSize; i++){
                    sendDataList.append(QString(cmdStr).arg(TermId.at(i).toInt(), 2, 16, QLatin1Char('0')).toUpper());
                }
            }
        }else if(contentSize == 1){                  //显示一个字
            cmdStr = DISPLAY_FONT;
            cmdStr.replace("%1", "%3");
            cmdStr.replace("%2", "%1");

            QString signalFont = Content.at(0).toString().replace(" ","").trimmed().replace(" ","");
            int fontSize = signalFont.size();

            if(fontSize == 1){
                cmdStr = QString(cmdStr).arg(qstr2Hex(signalFont));
            }else if(fontSize == 4 && signalFont.toUpper().contains("A0")){
                cmdStr = QString(cmdStr).arg(signalFont.insert(2, " "));
            }else if(fontSize == 0){
                cmdStr = QString(cmdStr).arg("A0 F0");
            }else{
                backJson.find("code").value() = 1;
                backJson.find("msg").value() = "content 内容有误";
                return backJson;
            }

            cmdStr.replace("%3", "%1");

            if(TermIdSize == 0){                    //全局灯进行控制
                sendDataList.append(cmdStr.replace("%1", "FF").toUpper());

            }else{                                   //挨个控制

                for(int i = 0; i<TermIdSize; i++){
                    sendDataList.append(QString(cmdStr).arg(TermId.at(i).toInt(), 2, 16, QLatin1Char('0')).toUpper());
                }
            }
        }else if(contentSize >1){                    //显示多个字
            QString signalFont;
            int fontSize;

            if(TermIdSize == contentSize){
                for(int i = 0; i<TermIdSize; i++){
                    int Id = TermId.at(i).toInt();
                    cmdStr = DISPLAY_FONT;
                    //sendDataList.append(QString(cmdStr).arg(TermId.at(i).toInt(), 2, 16, QLatin1Char('0')));

                    signalFont = Content.at(i).toString().replace(" ","").trimmed();
                    fontSize = signalFont.size();

                    if(fontSize == 1){
                        sendDataList.append(QString(cmdStr)
                                                .arg(Id, 2, 16, QLatin1Char('0'))
                                                .arg(qstr2Hex(signalFont)).toUpper());
                    }else if(fontSize == 4 && signalFont.toUpper().contains("A0")){
                        sendDataList.append(QString(cmdStr)
                                                .arg(Id, 2, 16, QLatin1Char('0'))
                                                .arg(signalFont.insert(2, " ")).toUpper());
                    }else if(fontSize == 0){
                        sendDataList.append(QString(cmdStr)
                                                .arg(Id, 2, 16, QLatin1Char('0'))
                                                .arg("A0 F0"));
                    }else{
                        backJson.find("code").value() = 1;
                        backJson.find("msg").value() = "content 内容有误";
                        return backJson;
                    }
                }
            }else{
                backJson.insert("errorContent", "Content数量与TermId数量不匹配");
            }

        }
    }else{
        backJson.insert("Content", "字段不存在");
    }

//    if(json.find("Content") != json.end()){
//        Content = json["Content"].toString();
//        if(Content.size() == 0){        //关灯
//            if(TermId.size() == 0){     //全局灯进行控制
//                sendDataList.insert(0, QString("ff 55 ff a0 f0 aa"));
//            }else{                      //挨个控制
//                for(int i = 0; i<TermId.size(); i++){
//                    sendDataList.append(QString("ff 55 %1 a0 f0 aa")
//                                            .arg(TermId.at(i).toInt(), 2, 16, QLatin1Char('0')));
//                }
//            }
//        }else if(Content.size() == 1){  //显示一个字
//            if(TermId.size() == 0){     //全局灯进行控制
//                sendDataList.append(QString("ff 55 ff %1 aa").arg(qstr2Hex(Content.at(0))));
//            }else{                      //挨个控制
//                for(int i = 0; i<TermId.size(); i++){
//                    sendDataList.append(QString("ff 55 %1 %2 aa")
//                                            .arg(TermId.at(i).toInt(), 2, 16, QLatin1Char('0'))
//                                            .arg(qstr2Hex(Content.at(0))));
//                }
//            }
//        }else if(Content.size() >1){    //显示多个字
//            if(Content.size() == TermId.size()){    //正常显示
//                for(int i = 0; i<TermId.size(); i++){
//                    sendDataList.append(QString("ff 55 %1 %2 aa")
//                                            .arg(TermId.at(i).toInt(), 2, 16, QLatin1Char('0'))
//                                            .arg(qstr2Hex(Content.at(i))));
//                }
//            }else{
//              backJson.find("code").value() = 1;
//              backJson.find("msg").value() = "content 内容有误";
//              return backJson;
//            }
//        }
//    }

    //需要更改亮度
    if(json.find("Luminance") != json.end()){
        Luminance = json["Luminance"].toInt();  // 文字亮度
        if(Luminance < 1 || Luminance > 100) {
            Luminance = 60;         //如果亮度不合法设置为 60
            backJson.find("code").value() = 1;
            backJson.find("msg").value() = errorMsg += "亮度值不合法";
        }
        sendDataList.insert(0, QString("ff 66 ff %1 00 aa").arg(Luminance, 2, 16, QLatin1Char('0')));
    }

    //需要更改闪烁
    if(json.find("Flicker") != json.end()){
        Flicker = json["Flicker"].toArray();
        if(Flicker.size() == 0){    //关闭闪烁
            sendDataList.insert(0, QString("ff 77 ff 10 00 aa"));
            emit signalUpdateFF88(QString("ff 77 ff 10 00 aa"));
        }else if(Flicker.size() == 2){    //如果参数数量正确
            int sec1, sec2;
            sec1 = Flicker.at(0).toInt();
            sec2 = Flicker.at(1).toInt();

            //数据合法
            if(sec1 >= 1000 && sec1 <= 5000 && sec2 >= 1000 && sec2 <= 5000){
                sendDataList.insert(0, QString("ff 77 ff %1 %2 aa")
                                           .arg(QString::asprintf("%.0f", sec1*0.44).toInt()/100,2,16,QLatin1Char('0'))
                                           .arg(QString::asprintf("%.0f", sec2*1.52).toInt()/100,2,16,QLatin1Char('0')));
                emit signalUpdateFF88(QString("ff 77 ff %1 %2 aa")
                                          .arg(QString::asprintf("%.0f", sec1*0.44).toInt()/100,2,16,QLatin1Char('0'))
                                          .arg(QString::asprintf("%.0f", sec2*1.52).toInt()/100,2,16,QLatin1Char('0')));
            }else if(sec1 == 0 || sec2 == 0){
                sendDataList.insert(0, QString("ff 77 ff 10 00 aa"));
                emit signalUpdateFF88(QString("ff 77 ff 10 00 aa"));
            }
            else{  //数据不合法
                backJson.find("code").value() = 1;
                backJson.find("msg").value() = errorMsg += "闪烁值不合法，需要大于1000-5000";
            }
        }else{
            backJson.find("code").value() = 1;
            backJson.find("msg").value() = errorMsg += "闪烁值数量错误";
        }
    }

    if(m_sendType == TCP){
        QString name = json["name"].toString();
        QString sbbh = json["sbbh"].toString();
        emit controller->sigSendDatagram(sendDataList, sbbh, name, version, 9);
    }else{
        //controller->sendDataUART(m_serialPort, sendDataList);
    }

    return backJson;
}

QJsonObject MyHttpServer::parseLightJson2(QJsonObject &json)
{
    QString errorMsg;
    QJsonObject backJson;
    backJson.insert("code", 0);
    backJson.insert("msg", "成功");

    //获取控制器 ip
    QString TermIp = json["TermIp"].toString(); // 控制器 ip
    if(!ipAddrIsOK(TermIp)){    // 判断ip是否合法
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "ip地址不合法";
        return backJson;
    }

    //获取控制器
    lightcontroll* controller = getControllerFromIp(TermIp);

    if(controller == nullptr){                       // 判断该 ip 控制器是否在线
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "控制器[" + TermIp + "]不在线";
        return backJson;
    }

    // 以下是需要更改的内容
    int version;      // 雾灯版本号
    int fontColor = 9;
    QJsonArray TermId;
    int Luminance;              // 亮度值
    QJsonArray Content ;        // 需要显示的文字列表
    QJsonArray Flicker;         // 闪烁值列表
    QStringList sendDataList;   // 需要发送的命令列表
    QString cmdStr;             // 发送的命令
    int TermIdSize = 0;         // 雾灯编号数量

    // 获取 TermId
    if(json.find("TermId") != json.end()){
        TermId = json["TermId"].toArray();
        TermIdSize = TermId.size();

    }else{
        backJson.insert("TermId", "字段不存在");
    }

    // 获取雾灯版本号
    if(json.find("Version") != json.end()){
        version = json["Version"].toInt();
        if(version < 0) version = 1;
    } else {
        version = 1;
    }

    //需要更改显示内容
    if(json.find("Content") != json.end()){
        cmdStr = DISPLAY_FONT;                        // 需要发送的命令
        Content = json["Content"].toArray();          // 文字内容
        int contentSize = Content.size();             // 文字数量
        //qDebug() << "Content = " << Content;
        if(contentSize == 0){                         //关灯
            cmdStr.replace("%2", "A0 F0");

            if(TermIdSize == 0){                    //全局灯进行控制
                sendDataList.insert(0, cmdStr.replace("%1","FF").toUpper());
            }else{                                    //挨个控制
                for(int i = 0; i<TermIdSize; i++){
                    sendDataList.append(QString(cmdStr).arg(TermId.at(i).toInt(), 2, 16, QLatin1Char('0')).toUpper());
                }
            }
        }else if(contentSize == 1){                  //显示一个字
            cmdStr = DISPLAY_FONT;
            cmdStr.replace("%1", "%3");
            cmdStr.replace("%2", "%1");

            QString signalFont = Content.at(0).toString().replace(" ","").trimmed().replace(" ","");
            int fontSize = signalFont.size();

            if(fontSize == 1){
                cmdStr = QString(cmdStr).arg(qstr2Hex(signalFont));
            }else if(fontSize == 4 && signalFont.toUpper().contains("A0")){
                cmdStr = QString(cmdStr).arg(signalFont.insert(2, " "));
            }else if(fontSize == 0){
                cmdStr = QString(cmdStr).arg("A0 F0");
            }else{
                backJson.find("code").value() = 1;
                backJson.find("msg").value() = "content 内容有误";
                return backJson;
            }

            cmdStr.replace("%3", "%1");

            if(TermIdSize == 0){                    //全局灯进行控制
                sendDataList.append(cmdStr.replace("%1", "FF").toUpper());

            }else{                                   //挨个控制
                for(int i = 0; i<TermIdSize; i++){
                    sendDataList.append(QString(cmdStr).arg(TermId.at(i).toInt(), 2, 16, QLatin1Char('0')).toUpper());
                }
            }
        }else if(contentSize >1){                    //显示多个字
            QString signalFont;
            int fontSize;

            if(TermIdSize == contentSize){
                for(int i = 0; i<TermIdSize; i++){
                    int Id = TermId.at(i).toInt();
                    cmdStr = DISPLAY_FONT;
                    //sendDataList.append(QString(cmdStr).arg(TermId.at(i).toInt(), 2, 16, QLatin1Char('0')));

                    signalFont = Content.at(i).toString().replace(" ","").trimmed();
                    fontSize = signalFont.size();

                    if(fontSize == 1){
                        sendDataList.append(QString(cmdStr)
                                                .arg(Id, 2, 16, QLatin1Char('0'))
                                                .arg(qstr2Hex(signalFont)).toUpper());
                    }else if(fontSize == 4 && signalFont.toUpper().contains("A0")){
                        sendDataList.append(QString(cmdStr)
                                                .arg(Id, 2, 16, QLatin1Char('0'))
                                                .arg(signalFont.insert(2, " ")).toUpper());
                    }else if(fontSize == 0){
                        sendDataList.append(QString(cmdStr)
                                                .arg(Id, 2, 16, QLatin1Char('0'))
                                                .arg("A0 F0"));
                    }else{
                        backJson.find("code").value() = 1;
                        backJson.find("msg").value() = "content 内容有误";
                        return backJson;
                    }
                }
            }else{
                backJson.insert("errorContent", "Content数量与TermId数量不匹配");
            }

        }
    }else{
        backJson.insert("Content", "字段不存在");
    }

    //需要更改亮度
    if(json.find("Luminance") != json.end()){
        cmdStr = CMD_LUMINANCE;
        Luminance = json["Luminance"].toInt();   // 文字亮度
        if(Luminance < MINI_LUMINANCE || Luminance > MAX_LUMINANCE) {
            Luminance = MAX_LUMINANCE/2;         // 如果亮度不合法设置为 总亮度的一半
        }
        sendDataList.insert(0, QString(cmdStr).arg(Luminance, 2, 16, QLatin1Char('0')).toUpper());
    }else{
        backJson.insert("Luminance", "字段不存在");
    }

    //需要更改闪烁
    if(json.find("Flicker") != json.end()){
        cmdStr = CMD_FLICKER;
        Flicker = json["Flicker"].toArray();
        int flickerSize = Flicker.size();
        int sec1 = 0, sec2 = 0;
        //qDebug() << "flickerSize = " << flickerSize;
        if(flickerSize == 0){    //关闭闪烁
            sec1 = 16;
            sec2 = 0;
            sendDataList.insert(0, cmdStr.arg(sec1,2,16,QLatin1Char('0'))
                                       .arg(sec2,2,16,QLatin1Char('0')).toUpper());
            emit signalUpdateFF88(sendDataList.at(0));
        } else if(flickerSize == 2){    //如果参数数量正确
            sec1 = Flicker.at(0).toInt();
            sec2 = Flicker.at(1).toInt();

            //数据合法
            if(sec1 >= 250 && sec1 <= 2000 && sec2 >= 250 && sec2 <= 2000){
                sec1 = sec1/20;
                sec2 = sec2/20;
                sendDataList.insert(0, cmdStr.arg(sec1,2,16,QLatin1Char('0'))
                                           .arg(sec2,2,16,QLatin1Char('0')).toUpper());
                emit signalUpdateFF88(sendDataList.at(0));
            }else{
                backJson.insert("errorFlicker", "数据非法，值范围：250 ~ 2000");
            }
        }else {
            backJson.insert("errorFlicker", "数值数量错误，只接受【亮、灭】两个数值");
        }

    }else{
        backJson.insert("Flicker", "字段不存在");
    }

    //需要更改颜色
    if(json.find("FontColor") != json.end()){

        fontColor = json["FontColor"].toInt();
        cmdStr = CHANGE_LIGHT_COLOR;        // "FF 40 FF %1 02 03 04 %5 AA"  SET_LIGHT_DEFAULT_EFFECT CHANGE_LIGHT_COLOR

        if(fontColor > 0 && fontColor <10){
            cmdStr = QString(cmdStr).arg(fontColor,2,16,QLatin1Char('0'));
        }

        cmdStr.replace("%2", "00");
        sendDataList.insert(0, cmdStr.toUpper());
    }else{
        backJson.insert("FontColor", "字段不存在");
    }

//    if(m_sendType == TCP){
//        QString name = json["name"].toString();
//        QString sbbh = json["sbbh"].toString();
//        emit controller->sigSendDatagram(sendDataList, sbbh, name, version, 9);
//    }else{
//        //controller->sendDataUART(m_serialPort, sendDataList);
//    }
    if(!json["name"].isString() || !json["sbbh"].isString()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "name或sbbh 内容不是string";
        return backJson;
    }

    QString name = json["name"].toString();
    QString sbbh = json["sbbh"].toString();
    emit controller->sigSendDatagram(sendDataList, sbbh, name, version, fontColor);


    return backJson;
}

QJsonObject MyHttpServer::parseLightJson3(QJsonObject &json)
{
    QJsonObject backJson;
    backJson.insert("code", 0);
    backJson.insert("msg", "成功");


    //如果必要参数不存在，或者不合理，直接返回
    if(missingParameter(json, backJson)){
        return backJson;
    }

    QString ip;                                     // 控制器 ip
    int port;                                       // 控制器 port
    QString TermIp = json["TermIp"].toString();     // 获取控制器 ip:port
    QString name = json["name"].toString();         // 获取 name
    QString DeviceId = json["DeviceId"].toString(); // 获取 DeviceId
    int Broadcast = json["Broadcast"].toInt();      // 获取 Broadcast
    QJsonArray TermId = json["TermId"].toArray();   // 获取雾灯编号 TermId
    int TermIdSize = TermId.size();                 // 雾灯编号数量
    int fontColor = -1;                             // 文字颜色
    int Luminance = -1;                             // 亮度值
    int version = json["Version"].toInt();          // 获取版本号
    QJsonArray Content = json["Content"].toArray(); // 文字内容 array
    QString ContentStr;                             // 实际发送内容的字符串
    int contentSize = Content.size();               // 文字内容数量
    QJsonArray Flicker;                             // 闪烁值列表
    QString FlickerList = "no";                     // 闪烁值字符串
    QStringList sendDataList;                       // 需要发送的命令列表
    QString cmdStr;                                 // 发送的命令

    lightcontroll* controller = nullptr;

    ip = TermIp.split(":").at(0);
    port = TermIp.split(":").at(1).toInt();

    if(!ipAddrIsOK(ip)){    // 判断ip是否合法
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "ip地址不合法";
        return backJson;
    }
    //获取控制器
    // lightcontroll* controller = getControllerFromIp(TermIp);
    controller = getControllerFromIpPort(ip, port);

    // 判断该 ip 控制器是否在线
    if(!controller){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "控制器[" + TermIp + "]不在线";
        return backJson;
    }

    if(Broadcast != 1 && Broadcast != 0){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "Broadcast 值错误 1 | 0";
        return backJson;
    }

    if(version != 1 && version != 2){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "version有误，只支持1/2";
        return backJson;
    }

    // content
    foreach(QJsonValue value, Content){     // 获取实际发送内容的字符串
        ContentStr += value.toString();
    }

    // 需要更改内容
    if(Broadcast){              // 广播模式
        cmdStr = QString(DISPLAY_FONT).replace("%1", "FF");     // 需要发送的命令
        if(contentSize == 0){           // 关灯
            cmdStr.replace("%2", "A0 F0");
        }else if(contentSize == 1){     // 开灯
            cmdStr.replace("%2", qstr2Hex(Content.at(0).toString()));
        }else{
            backJson.find("code").value() = 1;
            backJson.find("msg").value() = "广播模式下 content 数量只能为 0 或者 1";
            return backJson;
        }
        sendDataList.insert(0, cmdStr.replace("%1","FF").toUpper());
    }else{                      // 多文字模式
        if(TermIdSize != contentSize){
            backJson.find("code").value() = 1;
            backJson.find("msg").value() = "多文字模式下 TermId 数量只能跟 content 数量相等";
            return backJson;
        }

        for(int i=0; i<TermIdSize; i++){
            cmdStr = DISPLAY_FONT;
            int Id = TermId.at(i).toInt();
            QString word = Content.at(i).toString().replace(" ","").trimmed();

            sendDataList.append(QString(cmdStr)
                                    .arg(Id, 2, 16, QLatin1Char('0'))
                                    .arg(qstr2Hex(word)).toUpper());
        }
    }


    //需要更改亮度
    if(json.find("Luminance") != json.end()){
        if(!json["Luminance"].isDouble()){              // 亮度类型不合法
            backJson.find("code").value() = 1;
            backJson.find("msg").value() = "Luminance 类型不为int";
            return backJson;
        }
        cmdStr = CMD_LUMINANCE;
        Luminance = json["Luminance"].toInt();          // 文字亮度
        if(Luminance < 1 || Luminance > 100) {          // 亮度值不合法
            backJson.find("code").value() = 1;
            backJson.find("msg").value() = "亮度值不合法";
            return backJson;
        }
        sendDataList.insert(0, QString(cmdStr).arg(Luminance, 2, 16, QLatin1Char('0')).toUpper());
    }else{
        backJson.insert("Luminance", "字段不存在");
    }

    //需要更改闪烁
    if(json.find("Flicker") != json.end()){
        cmdStr = CMD_FLICKER;
        Flicker = json["Flicker"].toArray();
        int flickerSize = Flicker.size();
        int sec1 = 0, sec2 = 0;
        //qDebug() << "flickerSize = " << flickerSize;
        if(flickerSize == 0){    //关闭闪烁
            FlickerList = "0,0";
            sendDataList.insert(0, cmdStr.arg(16,2,16,QLatin1Char('0'))
                                       .arg(0,2,16,QLatin1Char('0')).toUpper());
            emit signalUpdateFF88(sendDataList.at(0));
        } else if(flickerSize == 2){        //如果参数数量正确
            sec1 = Flicker.at(0).toInt();
            sec2 = Flicker.at(1).toInt();

            if(sec1 == 0 || sec2 == 0){     // 关闭闪烁
                FlickerList = "0,0";
                sendDataList.insert(0, cmdStr.arg(16,2,16,QLatin1Char('0'))
                                           .arg(0,2,16,QLatin1Char('0')).toUpper());
            }else if(version == 1){         // v1 闪烁
                if(sec1 < 1000 || sec1 > 5000 || sec2 < 1000 || sec2 > 5000){
                    backJson.find("code").value() = 1;
                    backJson.find("msg").value() = "version1 闪烁值不合法 1000~5000";
                    return backJson;
                }
                FlickerList = QString("%1,%2").arg(sec1).arg(sec2);
                sendDataList.insert(0, QString("ff 77 ff %1 %2 aa")
                                           .arg(QString::asprintf("%.0f", sec1*0.44).toInt()/100,2,16,QLatin1Char('0'))
                                           .arg(QString::asprintf("%.0f", sec2*1.52).toInt()/100,2,16,QLatin1Char('0')));
            }else if(version == 2){         // v2 闪烁
                if(sec1 < 250 || sec1 > 2000 || sec2 < 250 || sec2 > 2000){
                    backJson.find("code").value() = 1;
                    backJson.find("msg").value() = "version2 闪烁值不合法 250~2000";
                    return backJson;
                }
                FlickerList = QString("%1,%2").arg(sec1).arg(sec2);
                sendDataList.insert(0, cmdStr.arg(sec1/20,2,16,QLatin1Char('0'))
                                           .arg(sec2/20,2,16,QLatin1Char('0')).toUpper());
            }
            emit signalUpdateFF88(sendDataList.at(0));
        }else {
            backJson.find("code").value() = 1;
            backJson.find("msg").value() = "Flicker 闪烁值数量不对 只接受0/2个数值";
            return backJson;
        }
    }else{
        backJson.insert("Flicker", "字段不存在");
    }

    if(version == 2){
        //需要更改颜色
        if(json.find("FontColor") != json.end()){
            if(!json["FontColor"].isDouble()){              // 亮度类型不合法
                backJson.find("code").value() = 1;
                backJson.find("msg").value() = "FontColor 类型不为int";
                return backJson;
            }

            cmdStr = CHANGE_LIGHT_COLOR;        // "FF 40 FF %1 02 03 04 %5 AA"  SET_LIGHT_DEFAULT_EFFECT CHANGE_LIGHT_COLOR
            fontColor = json["FontColor"].toInt();

            if(fontColor < 1 || fontColor > 9) {          // 亮度值不合法
                backJson.find("code").value() = 1;
                backJson.find("msg").value() = "FontColor值不合法";
                return backJson;
            }

            cmdStr = QString(cmdStr).arg(fontColor,2,16,QLatin1Char('0'));
            cmdStr.replace("%2", "00");

            sendDataList.insert(0, cmdStr.toUpper());
        }else{
            backJson.insert("FontColor", "字段不存在");
        }
    }

    emit controller->sigSendDatagram3(sendDataList, DeviceId, name, version, fontColor, Luminance, FlickerList);
    //showMsg("***************" + name);
    /*QString tmp = "\n";
    int i=1;
    foreach(QString str, sendDataList){
        tmp += QString("[%1]  ").arg(str);
        if(((i++) % 3) == 0) tmp += "\n";
    }
    tmp.removeLast();
    showMsg(tmp);*/
    return backJson;
}

bool MyHttpServer::missingParameter(QJsonObject &json, QJsonObject &backJson)
{
    if(json.find("TermIp") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 TermIp ";
        return true;
    }else if(!json.find("TermIp")->isString()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "TermIp 数据类型错误 应该为 string";
        return true;
    }else if(json["TermIp"].toString().split(":").size() != 2){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "ip地址不合法,格式范例：192.168.1.186:8886";
        return true;
    }

    if(json.find("TermId") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 TermId ";
        return true;
    }else if(!json.find("TermId")->isArray()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "TermId 数据类型错误  应该为 int 类型的 array";
        return true;
    }else {
        QJsonArray TermId = json["TermId"].toArray();
        int TermIdSize = TermId.size();

        if(TermIdSize < 1){
            backJson.find("code").value() = 1;
            backJson.find("msg").value() = "TermId 数量不能小于1";
            return true;
        }

        foreach(QJsonValue value, TermId){
            if(!value.isDouble()){
                backJson.find("code").value() = 1;
                backJson.find("msg").value() = "TermId 内容不是 int";
                return true;
            }
        }
    }

    if(json.find("Content") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 Content ";
        return true;
    }else if(!json.find("Content")->isArray()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "Content 数据类型错误 应该为 string 类型的 array";
        return true;
    }else{
        QJsonArray Content = json["Content"].toArray();
        foreach(QJsonValue value, Content){
            if(!value.isString()){
                backJson.find("code").value() = 1;
                backJson.find("msg").value() = "content Array 里面的部分数据类型不是 string";
                return true;
            }else if(value.toString().size() != 1){
                backJson.find("code").value() = 1;
                backJson.find("msg").value() = "content Array 里面的数据内容不是 单个文字";
                return true;
            }
        }
    }

    if(json.find("Version") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 Version ";
        return true;
    }else if(!json.find("Version")->isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "Version 数据类型错误 应该为 int";
        return true;
    }

    if(json.find("DeviceId") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 DeviceId ";
        return true;
    }else if(!json.find("DeviceId")->isString()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "DeviceId 数据类型错误 应该为 string";
        return true;
    }

    if(json.find("Broadcast") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 Broadcast ";
        return true;
    }else if(!json.find("Broadcast")->isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "Broadcast 数据类型错误 应该为 int";
        return true;
    }

    if(json.find("name") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 name ";
        return true;
    }else if(!json.find("name")->isString()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "name 数据类型错误 应该为 string";
        return true;
    }

    return false;
}

lightcontroll *MyHttpServer::getControllerFromIp(QString ip)
{
    int m_controllCount = 0;
    if(m_lightControllList)
        m_controllCount = m_lightControllList->size();

    for(int i=0; i<m_controllCount; i++){
        if(m_lightControllList->at(i)->getControllIp() == ip){
            return m_lightControllList->at(i);
        }
    }
    return nullptr;
}

lightcontroll *MyHttpServer::getControllerFromIpPort(QString ip, int Port)
{
    int m_controllCount = 0;
    if(m_lightControllList){
        m_controllCount = m_lightControllList->size();
    }

    for(int i=0; i<m_controllCount; i++){
        if(m_lightControllList->at(i)->getControllIp() == ip && m_lightControllList->at(i)->getControllPort() == Port){
            return m_lightControllList->at(i);
        }
    }
    return nullptr;
}

QString MyHttpServer::qstr2Hex(QString instr)
{
    if(instr.isEmpty()) return instr;

    QTextCodec *codec = QTextCodec::codecForName("GB2312");  //set char code
    QByteArray encodedString = codec->fromUnicode(instr);//convert to unicode
    instr = encodedString.toHex(QChar::Space); //hex add space example:FFAC21 convert to FF AC 21
    return instr;
}

void MyHttpServer::setSendingPara()
{
    int m_controllCount = m_lightControllList->size();
    for(int i=0; i<m_controllCount; i++){
        emit m_lightControllList->at(i)->siglSetIntervalAndCount(m_sendingInterval, m_sendingCount);
    }
}

void MyHttpServer::writeSendingInterval()
{
    QString iniPath = QCoreApplication::applicationDirPath() + INIFILE_LIGHT;
    if(!QFileInfo::exists(iniPath)){
        QMessageBox::information(nullptr, "提示", "配置文件读取失败");
        return;
    }

    QSettings settings(QCoreApplication::applicationDirPath() + INIFILE_LIGHT, QSettings::IniFormat);
    settings.setValue("Controllers/SendingInterval", m_sendingInterval);
}

void MyHttpServer::writeSendingCount()
{
    QString iniPath = QCoreApplication::applicationDirPath() + INIFILE_LIGHT;
    if(!QFileInfo::exists(iniPath)){
        QMessageBox::information(nullptr, "提示", "配置文件读取失败");
        return;
    }

    QSettings settings(QCoreApplication::applicationDirPath() + INIFILE_LIGHT, QSettings::IniFormat);
    settings.setValue("Controllers/SendingCount", m_sendingCount);
}

QJsonObject MyHttpServer::parseScreenJson(QJsonObject &json)
{
    QJsonObject backJson;
    backJson.insert("code", 0);
    backJson.insert("msg", "成功");

    //如果必要参数不存在，直接退出
    if(json.find("state") == json.end()
            || json.find("TermIp") == json.end()
            || json.find("upStr") == json.end()
            || json.find("downStr") == json.end()
            || json.find("upColor") == json.end()
            || json.find("downColor") == json.end()
            || json.find("sbbh") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数";
        return backJson;
    }else if(!json["state"].isString()
             || !json["TermIp"].isString()
             || !json["upStr"].isString()
             || !json["downStr"].isString()
             || !json["upColor"].isString()
             || !json["downColor"].isString()
             || !json["sbbh"].isString()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "有参数类型不对";
        return backJson;
    }

    // 0 关闭状态 1 打开状态 2 即将关闭状态 3 修改车牌显示时下排文字
    QString state = json["state"].toString();
    if(state != "0" && state != "1" && state != "2" && state != "3"){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "state值有误";
        return backJson;
    }

    QString upStr = json["upStr"].toString();
    if(upStr.isEmpty()) upStr = "-";
    QString downStr = json["downStr"].toString();
    if(downStr.isEmpty()) downStr = "-";
    QString upColor = json["upColor"].toString();
    QString downColor = json["downColor"].toString();
    QString sbbh = json["sbbh"].toString();
    Q_UNUSED(sbbh);
    QString TermIp = json["TermIp"].toString();

    emit signalUpDateScreenDefauleContent(state, upStr, downStr, upColor, downColor, TermIp);

    return backJson;
}

void MyHttpServer::initSast()
{
    memset(m_TermList, 0, sizeof(m_TermList));
    memset(m_TermState, 0, sizeof(m_TermState));
    memset(m_GroupInfoList, 0, sizeof(m_GroupInfoList));
    m_nTermCnt = 0;

    //初始化分组信息
    for(int i = 0; i<MAX_BROAD_GROUP; i++){
        m_GroupInfoList[i].groupNum = i+1;      // 分组号
        m_GroupInfoList[i].playTimes = 0;       // 播放次数
        m_GroupInfoList[i].playState = closed;  // 播放状态
        m_GroupInfoList[i].mp3IsOpen = false;   // MP3是否打开
        m_GroupInfoList[i].curFileIndex = 0;    // 播放文件下标
        m_GroupInfoList[i].DevExist = false;    // 是否有设备存在
    }

    int nResult = TSDK_Init(OnTzlSdkCallback, TRUE, 0x12345678);
    QString strText;
    if(nResult == CERR_SUCCESS)
        strText = "SDK 初始化成功！";
    else
        strText = "SDK 初始化失败！错误代码: " + QString::number(nResult);
    emit showMsg(strText);

    LoadTermList();
    StoreTermList();
}

static LPCSTR GetTermType(enSdkDevType eTermType)
{
    switch(eTermType)
    {
    case TSDK_DEV_TYPE_TALK:
        return "[对讲广播终端]";
    case TSDK_DEV_TYPE_BROAD:
        return "[广播终端]";
    case TSDK_DEV_TYPE_MP3:
        return "[网络拾音器]";
    default:
        return "[未知类型终端]";
    }  
}

int MyHttpServer::OnTzlSdkCallback(enSdkCbType eCbType, LPVOID pParam, DWORD dwSize, int usr_data)
{
    Q_UNUSED(usr_data);
    Q_UNUSED(dwSize);
    switch(eCbType)
        {
            case CB_Event_TermRegister:     // 终端请求注册通知，返回-1代表注册失败，返回0代表注册成功
            {
                assert(dwSize == sizeof(TSdkEventTermRegister));
                TSdkEventTermRegister * pEventTermRegister = (TSdkEventTermRegister*)pParam;
                LPBYTE p = pEventTermRegister->TermMac;
                QString strText;
                strText = QString("===> %1 请求注册：终端ID:%2 , MAC地址:%3-%4-%5-%6-%7-%8 , IP地址:%9 , 名称:%10")
                        .arg((QString(GetTermType(pEventTermRegister->eTermType)))
                        , QString::number(pEventTermRegister->dwTermID, 16)
                        , QString("%1").arg(p[0], 2, 16, QLatin1Char('0')).toUpper()
                        , QString("%1").arg(p[1], 2, 16, QLatin1Char('0')).toUpper()
                        , QString("%1").arg(p[2], 2, 16, QLatin1Char('0')).toUpper()
                        , QString("%1").arg(p[3], 2, 16, QLatin1Char('0')).toUpper()
                        , QString("%1").arg(p[4], 2, 16, QLatin1Char('0')).toUpper()
                        , QString("%1").arg(p[5], 2, 16, QLatin1Char('0')).toUpper()
                        , QString::fromLocal8Bit(pEventTermRegister->TermIp, 16)
                        , QString(pEventTermRegister->TermName));
                emit g_pMainWnd->showMsg(strText);

                int i;
                for(i=0; i<g_pMainWnd->m_nTermCnt; i++)
                {
                    if(g_pMainWnd->m_TermList[i].dwTermID == pEventTermRegister->dwTermID)
                        break;
                }

                if(i < g_pMainWnd->m_nTermCnt)
                {
                    // 更新
                    g_pMainWnd->m_TermList[i] = *pEventTermRegister;

                    g_pMainWnd->StoreTermList();
                    g_pMainWnd->UpdateTermListUI();
                }
                else
                {
                    // 注册
                    if(g_pMainWnd->m_nTermCnt >= MAX_TERM_COUNT)
                        return -1; // 返回 0 代表注册成功，否则代表注册失败 !!!

                    g_pMainWnd->m_TermList[i] = *pEventTermRegister;
                    g_pMainWnd->m_TermState[i].eState = TSDK_DEV_STATE_OFFLINE;
                    g_pMainWnd->m_nTermCnt++;

                    g_pMainWnd->StoreTermList();
                    g_pMainWnd->UpdateTermListUI();
                }

                return CERR_SUCCESS; // 返回 0 代表注册成功，否则代表注册失败 !!!
            }

            case CB_Event_TermConnect:      // 终端请求连接通知，返回-1代表不允许连接，返回0代表允许连接
            {
                assert(dwSize == sizeof(TSdkEventTermConnect));
                TSdkEventTermRegister * pEventTermConnect = (TSdkEventTermRegister*)pParam;
                LPBYTE p = pEventTermConnect->TermMac;
                QString strText;
                strText = QString("===> %1 请求连接：终端ID:%2 , MAC地址:%3-%4-%5-%6-%7-%8 , IP地址:%9 , 名称:%10")
                        .arg(QString(GetTermType(pEventTermConnect->eTermType))
                        , QString::number(pEventTermConnect->dwTermID, 16)
                        , QString("%1").arg(p[0], 2, 16, QLatin1Char('0')).toUpper()
                        , QString("%1").arg(p[1], 2, 16, QLatin1Char('0')).toUpper()
                        , QString("%1").arg(p[2], 2, 16, QLatin1Char('0')).toUpper()
                        , QString("%1").arg(p[3], 2, 16, QLatin1Char('0')).toUpper()
                        , QString("%1").arg(p[4], 2, 16, QLatin1Char('0')).toUpper()
                        , QString("%1").arg(p[5], 2, 16, QLatin1Char('0')).toUpper()
                        , QString::fromLocal8Bit(pEventTermConnect->TermIp, 16)
                        , QString(pEventTermConnect->TermName));
                emit g_pMainWnd->showMsg(strText);

                int i;
                for(i=0; i<g_pMainWnd->m_nTermCnt; i++)
                {
                    if( (g_pMainWnd->m_TermList[i].dwTermID == pEventTermConnect->dwTermID) &&
                        memcmp(g_pMainWnd->m_TermList[i].TermMac, pEventTermConnect->TermMac, 6) == 0)
                    {
                        break;
                    }
                }

                if(i < g_pMainWnd->m_nTermCnt)
                {
                    // 已注册过
                    BOOL bChangeIp = strcmp(g_pMainWnd->m_TermList[i].TermIp, pEventTermConnect->TermIp);
                    if(bChangeIp)
                        strcpy_s(g_pMainWnd->m_TermList[i].TermIp, sizeof(pEventTermConnect->TermIp), pEventTermConnect->TermIp);

                    BOOL bChangeName = strcmp(g_pMainWnd->m_TermList[i].TermName, pEventTermConnect->TermName);
                    if(bChangeName)
                        strcpy_s(g_pMainWnd->m_TermList[i].TermName, sizeof(pEventTermConnect->TermName), pEventTermConnect->TermName);

                    g_pMainWnd->m_TermList[i].eTermType = pEventTermConnect->eTermType;
                    g_pMainWnd->m_TermState[i].eState = TSDK_DEV_STATE_ONLINE;

                    if(bChangeIp || bChangeName)
                        g_pMainWnd->StoreTermList();

                    g_pMainWnd->UpdateTermListUI();

                    g_pMainWnd->updateTermVolume(pEventTermConnect->TermIp);
                    return CERR_SUCCESS; // 返回 0 代表允许连接，否则代表不允许 !!!
                }
                else
                {
                    // 未注册
                    return -1; // 返回 0 代表允许连接，否则代表不允许 !!!
                }
            }

            case CB_Event_TermCnnLost:      // 终端断开连接通知
            {
                assert(dwSize == sizeof(TSdkEventTermCnnLost));
                TSdkEventTermCnnLost * pEventTermCnnLost = (TSdkEventTermCnnLost*)pParam;
                QString strText;
                strText = QString("xxx 终端断开连接：终端ID:%1").arg(QString::number(pEventTermCnnLost->dwTermID, 16));
                emit g_pMainWnd->showMsg(strText);

                for(int i=0; i<g_pMainWnd->m_nTermCnt; i++)
                {
                    if(g_pMainWnd->m_TermList[i].dwTermID == pEventTermCnnLost->dwTermID)
                    {
                        g_pMainWnd->m_TermState[i].eState = TSDK_DEV_STATE_OFFLINE;
                        break;
                    }
                }

                g_pMainWnd->UpdateTermListUI();

                break;
            }

            case CB_Asw_OpenTermAudVid:     // 应答打开终端音频视频
            {
                assert(dwSize == sizeof(TSdkAswOpenTermAudVid));
                TSdkAswOpenTermAudVid * pAswOpenTermAudVid = (TSdkAswOpenTermAudVid*)pParam;
                QString strText;
                if(pAswOpenTermAudVid->nResult == CERR_SUCCESS)
                    strText = QString("打开终端音频成功！终端ID:%1 ").arg(QString::number(pAswOpenTermAudVid->dwTermID, 16));
                else
                    strText = QString("打开终端音频失败！终端ID:%1 ，错误代码:%2")
                            .arg(QString::number(pAswOpenTermAudVid->dwTermID, 16))
                            .arg(pAswOpenTermAudVid->nResult);
                emit g_pMainWnd->showMsg(strText);

                break;
            }

            case CB_Asw_CloseTermAudVid:    // 应答关闭终端音频视频
            {
                assert(dwSize == sizeof(TSdkAswCloseTermAudVid));
                TSdkAswCloseTermAudVid * pAswCloseTermAudVid = (TSdkAswCloseTermAudVid*)pParam;
                QString strText;
                if(pAswCloseTermAudVid->nResult == CERR_SUCCESS)
                    strText = QString("关闭终端音频成功！终端ID:%1 ").arg(QString::number(pAswCloseTermAudVid->dwTermID, 16));
                else
                    strText = QString("关闭终端音频失败！终端ID:%1 ，错误代码:%2")
                            .arg(QString::number(pAswCloseTermAudVid->dwTermID, 16))
                            .arg(pAswCloseTermAudVid->nResult);

                break;
            }

            case CB_Asw_OpenTermTalk:       // 应答打开终端对讲
            {
                assert(dwSize == sizeof(TSdkAswOpenTermTalk));
                TSdkAswOpenTermTalk * pAswOpenTermTalk = (TSdkAswOpenTermTalk*)pParam;
                QString strText;
                if(pAswOpenTermTalk->nResult == CERR_SUCCESS)
                    strText = QString("打开终端对讲成功！终端ID:%1 ").arg(QString::number(pAswOpenTermTalk->dwTermID, 16));
                else
                    strText = QString("打开终端对讲失败！终端ID:%1 ，错误代码:%2")
                            .arg(QString::number(pAswOpenTermTalk->dwTermID, 16))
                            .arg(pAswOpenTermTalk->nResult);
                emit g_pMainWnd->showMsg(strText);

                break;
            }

            case CB_Asw_CloseTermTalk:      // 应答关闭终端对讲
            {
                assert(dwSize == sizeof(TSdkAswCloseTermTalk));
                TSdkAswCloseTermTalk * pAswCloseTermTalk = (TSdkAswCloseTermTalk*)pParam;
                QString strText;
                if(pAswCloseTermTalk->nResult == CERR_SUCCESS)
                    strText = QString("关闭终端对讲成功！终端ID:%1 ").arg(QString::number(pAswCloseTermTalk->dwTermID, 16));
                else
                    strText = QString("关闭终端对讲失败！终端ID:%1 ，错误代码:%2")
                            .arg(QString::number(pAswCloseTermTalk->dwTermID, 16))
                            .arg(pAswCloseTermTalk->nResult);
                emit g_pMainWnd->showMsg(strText);

                break;
            }

            case CB_Asw_GetAudLevel:        // 应答获取终端音量级别
            {
                assert(dwSize == sizeof(TSdkAswGetAudLevel));
                TSdkAswGetAudLevel * pAswGetAudLevel = (TSdkAswGetAudLevel*)pParam;

                for (int i = 0; i < g_pMainWnd->m_nTermCnt; ++i) {
                    if(g_pMainWnd->m_TermList[i].dwTermID == pAswGetAudLevel->dwTermID){
                        g_pMainWnd->m_GroupInfoList[i].volume = pAswGetAudLevel->nAmpValue;
                    }
                }
                break;
            }

            case CB_Asw_SetAudLevel:        // 应答设置终端音量级别
            {
                assert(dwSize == sizeof(TSdkAswSetAudLevel));
                TSdkAswSetAudLevel * pAswSetAudLevel = (TSdkAswSetAudLevel*)pParam;
                QString strText;
                if(pAswSetAudLevel->nResult == CERR_SUCCESS)
                {
                    strText = QString("设置终端音量成功！终端ID:%1 ").arg(QString::number(pAswSetAudLevel->dwTermID, 16));

                    if(pAswSetAudLevel->bIsEx)
                    {
                        //g_pMainWnd->m_nAmpLevel = pAswSetAudLevel->eAmpLevel;
                        //((CComboBox*)g_pMainWnd->GetDlgItem(IDC_CMB_AMP))->SetCurSel(g_pMainWnd->m_nAmpLevel);
                    }
                    else
                    {
                        //g_pMainWnd->m_AmpSlider.SetPos(pAswSetAudLevel->nAmpValue);
                        QString strText;
                        strText = QString("当前:%1").arg(pAswSetAudLevel->nAmpValue);
                        //g_pMainWnd->GetDlgItem(IDC_STATIC_CUR)->SetWindowText(strText);
                    }
                }
                else
                {
                    strText = QString("设置终端音量失败！终端ID:%1 ，错误代码:%2")
                            .arg(QString::number(pAswSetAudLevel->dwTermID, 16))
                            .arg(pAswSetAudLevel->nResult);
                }
                emit g_pMainWnd->showMsg(strText);

                break;
            }
/*
            case CB_Asw_DismissTerm:
            {
                assert(dwSize == sizeof(TSdkAswDismissTerm));
                TSdkAswDismissTerm * pAswDismissTerm = (TSdkAswDismissTerm*)pParam;
                QString strText;
                if(pAswDismissTerm->nResult == CERR_SUCCESS)
                {
                    g_pMainWnd->m_stcTips.SetWindowText("呼叫提示：");
                    strText = QString("解除呼叫成功！终端ID:%X ", pAswDismissTerm->dwTermID);
                }
                else
                {
                    strText = QString("解除呼叫失败！终端ID:%X ，错误代码:%d", pAswDismissTerm->dwTermID, pAswDismissTerm->nResult);
                }
                g_pMainWnd->showMsg(1, strText);

                break;
            }

            case CB_Asw_GetDbAlmCfg:
            {
                assert(dwSize == sizeof(TSdkAswGetDbAlmCfg));
                TSdkAswGetDbAlmCfg * pAswGetDbAlmCfg = (TSdkAswGetDbAlmCfg*)pParam;
                g_pMainWnd->m_bDbAlarm = pAswGetDbAlmCfg->Cfg.bDbAlmEnable;
                ((CButton*)g_pMainWnd->GetDlgItem(IDC_CHK_DB_ALARM))->SetCheck(pAswGetDbAlmCfg->Cfg.bDbAlmEnable);
                g_pMainWnd->m_DbValSpin.SetPos(pAswGetDbAlmCfg->Cfg.nDbAlmVal);
                break;
            }

            case CB_Asw_SetDbAlmCfg:
            {
                assert(dwSize == sizeof(TSdkAswSetDbAlmCfg));
                TSdkAswSetDbAlmCfg * pAswSetDbAlmCfg = (TSdkAswSetDbAlmCfg*)pParam;
                QString strText;
                if(pAswSetDbAlmCfg->nResult == CERR_SUCCESS)
                    strText = QString("修改终端配置成功！终端ID:%X ", pAswSetDbAlmCfg->dwTermID);
                else
                    strText = QString("修改终端配置失败！终端ID:%X ，错误代码:%d", pAswSetDbAlmCfg->dwTermID, pAswSetDbAlmCfg->nResult);
                g_pMainWnd->showMsg(1, strText);

                break;
            }

            case CB_Post_TermSos:
            {
                assert(dwSize == sizeof(TSdkPostTermSos));
                TSdkPostTermSos * pPostTermSos = (TSdkPostTermSos*)pParam;
                QString strText;
                if(pPostTermSos->nSosType == 1)
                    strText = QString("终端紧急呼叫 !!!!!! \n终端ID:%X", pPostTermSos->dwTermID);
                else
                    strText = QString("终端咨询呼叫 !!!!!! \n终端ID:%X", pPostTermSos->dwTermID);
                g_pMainWnd->showMsg(1, strText);

                g_pMainWnd->m_stcTips.SetWindowText(strText);

                for(int i=0; i<g_pMainWnd->m_nTermCnt; i++)
                {
                    if(g_pMainWnd->m_TermList[i].dwTermID == pPostTermSos->dwTermID)
                    {
                        g_pMainWnd->m_cbxTermList.SetCurSel(i);
                        break;
                    }
                }

                break;
            }

            case CB_Post_TermState:
            {
                assert(dwSize == sizeof(TSdkPostTermState));
                TSdkPostTermState * pPostTermState = (TSdkPostTermState*)pParam;

                for(int i=0; i<g_pMainWnd->m_nTermCnt; i++)
                {
                    if(g_pMainWnd->m_TermList[i].dwTermID == pPostTermState->dwTermID)
                    {
                        g_pMainWnd->m_TermState[i].eState = pPostTermState->eTermState;
                        g_pMainWnd->m_TermState[i].AlmInState = pPostTermState->AlmInState;
                        g_pMainWnd->m_TermState[i].AlmOutState = pPostTermState->AlmOutState;
                        break;
                    }
                }

                QString strText;
                strText = QString("终端ID:%X 终端状态:%s , IO报警 == 输入1:%s , 输入2:%s , 输出1:%s , 输出2:%s", pPostTermState->dwTermID, GetStateText(pPostTermState->eTermState),
                    AMSG(pPostTermState->AlmInState & 0x1), AMSG((pPostTermState->AlmInState >> 1) & 0x1),
                    AMSG(pPostTermState->AlmOutState & 0x1), AMSG((pPostTermState->AlmOutState >> 1) & 0x1));
                g_pMainWnd->showMsg(1, strText);


                int nSel = g_pMainWnd->m_cbxTermList.GetCurSel();
                if(nSel >= 0 && g_pMainWnd->m_TermList[nSel].dwTermID == pPostTermState->dwTermID)
                {
                    if(pPostTermState->AlmOutState & 0x1)
                        g_pMainWnd->GetDlgItem(IDC_BTN_OPEN_ALM_OUT_1)->SetWindowText("关闭报警输出1");
                    else
                        g_pMainWnd->GetDlgItem(IDC_BTN_OPEN_ALM_OUT_1)->SetWindowText("打开报警输出1");

                    if((pPostTermState->AlmOutState >> 1) & 0x1)
                        g_pMainWnd->GetDlgItem(IDC_BTN_OPEN_ALM_OUT_2)->SetWindowText("关闭报警输出2");
                    else
                        g_pMainWnd->GetDlgItem(IDC_BTN_OPEN_ALM_OUT_2)->SetWindowText("打开报警输出2");
                }

                break;
            }

            case CB_Post_485PipeData:
            {
                assert(dwSize == sizeof(TSdkPost485PipeData));
                TSdkPost485PipeData * pPost485PipeData = (TSdkPost485PipeData*)pParam;
                QString strText;
                strText = QString("===> 收到485数据：终端ID:%X , 长度:%d , 内容:%s",
                    pPost485PipeData->dwTermID, pPost485PipeData->nFrmLen, pPost485PipeData->FrmData);
                g_pMainWnd->showMsg(1, strText);


                g_pMainWnd->m_str485PipeData += (LPCSTR)pPost485PipeData->FrmData;
                g_pMainWnd->m_str485PipeData += "\r\n";
                g_pMainWnd->m_edt485PipeData.SetWindowText(g_pMainWnd->m_str485PipeData);
                break;
            }
*/
            case CB_Post_Mp3PlayFinish:     // 推送MP3文件播放结束通知
            {
                assert(dwSize == sizeof(TSdkPostMp3PlayFinish));
                TSdkPostMp3PlayFinish * pPostMp3PlayFinish = (TSdkPostMp3PlayFinish*)pParam;
                QString strText;
                GroupInfo* groupInfo = &(g_pMainWnd->m_GroupInfoList[pPostMp3PlayFinish->nBroadNum - 1]);
                groupInfo->playState = closed;

                if (pPostMp3PlayFinish->nResult == CERR_SUCCESS) {
                    strText = QString("分组%1 : MP3文件播放：正常结束。。").arg(groupInfo->groupNum);

                    g_pMainWnd->groupBroadMp3(groupInfo);
                }
                else
                    strText = QString("分组%1 : MP3文件播放：播放出错，错误代码:%2")
                            .arg(groupInfo->groupNum)
                            .arg(pPostMp3PlayFinish->nResult);
                break;
            }
/*
            case CB_Post_AlmNotify:
            {
                assert(dwSize == sizeof(TSdkPostAlmNotify));
                TSdkPostAlmNotify * pPostAlmNotify = (TSdkPostAlmNotify*)pParam;
                QString strText;
                if((pPostAlmNotify->eAlmType & SDK_ALMTYPE_DB_ALARM) == SDK_ALMTYPE_DB_ALARM)
                    strText = QString("终端喧哗报警 !!!    终端ID:%X", pPostAlmNotify->dwTermID);
                else
                    strText = QString("终端报警类型: 0x%X  !!!    终端ID:%X", pPostAlmNotify->eAlmType);
                g_pMainWnd->showMsg(1, strText);

                break;
            }

            case CB_Data_TermAudio:
            {
                assert(dwSize == sizeof(TSdkDataTermAudio));
                TSdkDataTermAudio * pDataTermAudio = (TSdkDataTermAudio*)pParam;
    //			TRACE("== 终端音频 ID:%X ：大小 %d 字节\n", pDataTermAudio->dwTermID,pDataTermAudio->nDataSize);
                TSDK_Player_AudVidAdd(1, TSDK_AUDIO, pDataTermAudio->pAudData, pDataTermAudio->nDataSize);
                break;
            }

            case CB_Data_TermVideo:
            {
                assert(dwSize == sizeof(TSdkDataTermVideo));
                TSdkDataTermVideo * pDataTermVideo = (TSdkDataTermVideo*)pParam;
                if(pDataTermVideo->bIsKeyFrm)
                    TRACE("== 终端I帧视频 ID:%X ：大小 %d 字节\n", pDataTermVideo->dwTermID,pDataTermVideo->nFrmSize);
    //			else
    //				TRACE("== 终端P帧视频 ID:%X ：大小 %d 字节\n", pDataTermVideo->dwTermID,pDataTermVideo->nFrmSize);

                TSDK_Player_AudVidAdd(1, TSDK_VIDEO, pDataTermVideo->pVidFrm, pDataTermVideo->nFrmSize);
                break;
            }

            case CB_Data_PcMicAudio:
            {
                assert(dwSize == sizeof(TSdkDataPcMicAudio));
                TSdkDataPcMicAudio * pDataPcAudio = (TSdkDataPcMicAudio*)pParam;
    //			TRACE("== 电脑麦克风音频：大小 %d 字节\n", pDataPcAudio->nDataSize);

                if(g_pMainWnd->m_nTalkTermIdx != -1)
                {
                    TSDK_Send_TalkAudio(g_pMainWnd->m_TermList[g_pMainWnd->m_nTalkTermIdx].dwTermID,
                        pDataPcAudio->pPcmData, pDataPcAudio->nDataSize);
                }

                if(g_pMainWnd->m_bIsBroadingVoice)
                {
                    TSDK_BroadPcmVoice(g_MyGroupNum, pDataPcAudio->pPcmData, pDataPcAudio->nDataSize);


                    // 如果有多个分组
                    //TSDK_BroadPcmVoice(xx, pDataPcAudio->pPcmData, pDataPcAudio->nDataSize);
                    //TSDK_BroadPcmVoice(xxx, pDataPcAudio->pPcmData, pDataPcAudio->nDataSize);

                }
                break;
            }

            case CB_Data_BypassAudio:
            {
                assert(dwSize == sizeof(TSdkDataBypassAudio));
                TSdkDataBypassAudio * pDataBypassAudio = (TSdkDataBypassAudio*)pParam;
    //			TRACE("== 旁路音频 ID:%X ：大小 %d 字节\n", pDataBypassAudio->dwTermID,pDataBypassAudio->nDataSize);
                TSDK_Player_PcmAdd(1, pDataBypassAudio->pPcmData, pDataBypassAudio->nDataSize);
                break;
            }

            case CB_Data_TermMp3L:
            {
                assert(dwSize == sizeof(TSdkDataTermMp3L));
                TSdkDataTermMp3L * pDataTermMp3L = (TSdkDataTermMp3L*)pParam;
    //			TRACE("== MP3音频 ID:%X ：大小 %d 字节\n", pDataTermMp3L->dwTermID,pDataTermMp3L->nDataSize);
                if(g_pMp3File1)
                    fwrite(pDataTermMp3L->pMp3Data, 1, pDataTermMp3L->nDataSize, g_pMp3File1);
                break;
            }

            case CB_Data_TermMp3R:
            {
                assert(dwSize == sizeof(TSdkDataTermMp3R));
                TSdkDataTermMp3R * pDataTermMp3R = (TSdkDataTermMp3R*)pParam;
    //			TRACE("== MP3音频 ID:%X ：大小 %d 字节\n", pDataTermMp3R->dwTermID,pDataTermMp3R->nDataSize);
                if(g_pMp3File2)
                    fwrite(pDataTermMp3R->pMp3Data, 1, pDataTermMp3R->nDataSize, g_pMp3File2);
                break;
            }
*/
            default:
            {
                //assert(FALSE);
                break;
            }
        }

    return CERR_SUCCESS;
}

void MyHttpServer::LoadTermList()
{
    QString strIniFile = QCoreApplication::applicationDirPath() + INIFILE_SAST;;
    QSettings settings(strIniFile, QSettings::IniFormat);

    if(!QFile::exists(strIniFile)){
        QMessageBox::information(nullptr, "提示", "配置文件读取失败");
        return;
    }

    m_nTermCnt = settings.value("TermRegister/TermCnt").toInt();
    for(int i=0; i<m_nTermCnt; i++)
    {
        bool ok;
        m_TermList[i].dwTermID = settings.value(QString("TermRegister/t%1_TermID").arg(i, 4, 10, QLatin1Char('0')), 0).toString().toInt(&ok, 16);

        int nMac1 = settings.value(QString("TermRegister/t%1_TermMac1").arg(i, 4, 10, QLatin1Char('0')), 0).toString().toInt(&ok, 16);
        int nMac2 = settings.value(QString("TermRegister/t%1_TermMac2").arg(i, 4, 10, QLatin1Char('0')), 0).toString().toInt(&ok, 16);
        memcpy(m_TermList[i].TermMac, &nMac1, 3);
        memcpy(m_TermList[i].TermMac+3, &nMac2, 3);

        QByteArray byteA = settings.value(QString("TermRegister/t%1_TermIp").arg(i, 4, 10, QLatin1Char('0')), 0).toString().toLatin1();
        for(int j = 0; j< byteA.size(); j++){
            m_TermList[i].TermIp[j] = byteA[j];
        }

        byteA = settings.value(QString("TermRegister/t%1_TermName").arg(i, 4, 10, QLatin1Char('0')), 0).toString().toLatin1();
        for(int j = 0; j< byteA.size(); j++){
            m_TermList[i].TermName[j] = byteA[j];
        }

        //如果设备未分组， 进行分组
        if(TSDK_GetTermGroupNum(m_TermList[i].dwTermID) == 0){
            for(int j=0; j <MAX_BROAD_GROUP; j++){
                if(!m_GroupInfoList[j].DevExist){
                    //给当前音响设置分组
                    TSdkGroupTermList GroupTermList_xxx;
                    memset(&GroupTermList_xxx, 0, sizeof(GroupTermList_xxx));
                    GroupTermList_xxx.TermList[0] = m_TermList[i].dwTermID;
                    int nResult = TSDK_SetupGroup(m_GroupInfoList[j].groupNum, &GroupTermList_xxx);
                    if (nResult != CERR_SUCCESS)
                    {
                        QString strText;
                        strText = QString("创建%1号分区失败！错误代码:%2").arg(m_GroupInfoList[i].groupNum).arg(nResult);
                        QMessageBox::information(nullptr, "错误", strText);
                        return;
                    }
                    m_GroupInfoList[j].DevExist = true;
                    break;
                }
            }
        }
    }
}

void MyHttpServer::UpdateTermListUI()
{
    emit sigleUpdateTermListUi();
}

void MyHttpServer::StoreTermList()
{
    QString strIniFile = QCoreApplication::applicationDirPath() + INIFILE_SAST;
    QSettings settings(strIniFile, QSettings::IniFormat);

    if(!QFile::exists(strIniFile)){
        QMessageBox::information(nullptr, "提示", "配置文件读取失败");
        return;
    }

    for(int i=0; i<m_nTermCnt; i++)
    {
        settings.setValue(QString("TermRegister/t%1_TermID")
                          .arg(i, 4, 10, QLatin1Char('0')), QString::number(m_TermList[i].dwTermID, 16).toUpper().insert(0, "0x"));

        LPBYTE p = m_TermList[i].TermMac;
        int nMac1 = p[0] | (p[1] << 8) | (p[2] << 16);
        int nMac2 = p[3] | (p[4] << 8) | (p[5] << 16);
        settings.setValue(QString("TermRegister/t%1_TermMac1")
                          .arg(i, 4, 10, QLatin1Char('0')), QString::number(nMac1, 16).toUpper().insert(0, "0x"));
        settings.setValue(QString("TermRegister/t%1_TermMac2")
                          .arg(i, 4, 10, QLatin1Char('0')), QString::number(nMac2, 16).toUpper().insert(0, "0x"));

        settings.setValue(QString("TermRegister/t%1_TermIp")
                          .arg(i, 4, 10, QLatin1Char('0')), QString(m_TermList[i].TermIp));
        settings.setValue(QString("TermRegister/t%1_TermName")
                          .arg(i, 4, 10, QLatin1Char('0')), QString(m_TermList[i].TermName));
    }
    settings.setValue(QString("TermRegister/TermCnt"), QString::number(m_nTermCnt));
}

QJsonObject MyHttpServer::parseSastJson(QJsonObject &json)
{
    QJsonObject backJson;
    int volume = -1;
    //QString errorMsg;
    backJson.insert("code", 0);
    backJson.insert("msg", "success");

    QString sbbh;   //设备编号
   //如果设备编号存在，保存下来
   if(json.find("sbbh") != json.end()) {
       sbbh = json.value("sbbh").toString();
   }

    //判断是否为有效数据
    if(json.find("TermIp") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "missing necessary para";
        return backJson;
    }

    QString TermIp = json.value("TermIp").toString();
    if(!ipAddrIsOK(TermIp)){    // 判断ip是否合法
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "illegality ipaddr";
        return backJson;
    }
    setSbbh(TermIp, sbbh);
    volume = getVolume(TermIp);

    if(!sastIsOnline(TermIp)){    // 判断该 ip 设备是否在线
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = TermIp + " offline";
        return backJson;
    }

    if(json.find("Volume") != json.end()){      //需要设置音量
        volume = json.value("Volume").toInt();
        setVolume(TermIp, volume);
        setTermVolume(TermIp, volume);
    }

    QJsonArray FileNameList;    //文件列表
    if(json.find("FileNames") != json.end()){    //文件是否存在
         FileNameList = json.value("FileNames").toArray();

         if(FileNameList.size() < 1) {
             backJson.find("code").value() = 1;
             backJson.find("msg").value() = "文件名不存在";
             return backJson;
         }else{
             for(int i=0; i<FileNameList.size(); i++){
                 if(!mp3Exist(FileNameList.at(i).toString())){
                     backJson.find("code").value() = 1;
                     backJson.find("msg").value() = "文件-[" + FileNameList.at(i).toString() + "]-不存在";
                     return backJson;
                 }
             }
         }
    }

    int PlayTimes;              //播放次数
    if(json.find("PlayTimes") != json.end()){  //播放次数是否存在
        PlayTimes = json.value("PlayTimes").toInt();
        if(PlayTimes < -1){
            backJson.find("code").value() = 1;
            backJson.find("msg").value() = "播放次数不存在，默认播放一次";
        }
    }else{
        PlayTimes = 1;
    }

    setGroupInfo(TermIp, FileNameList, PlayTimes);

    // 音响效果，发送到kafka
    QJsonObject kafkaJson;
    QString tmp;

    for(int i=0; i<FileNameList.size(); i++){
        tmp += FileNameList.at(i).toString();
    }

    kafkaJson.insert("deviceBh", sbbh);
    kafkaJson.insert("deviceState", "1");
    kafkaJson.insert("checkDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    kafkaJson.insert("insertDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    kafkaJson.insert("text", tmp);
    kafkaJson.insert("state", PlayTimes == 0 ? "0" : "1");
    kafkaJson.insert("value", tmp);
    kafkaJson.insert("color", "");
    kafkaJson.insert("volume", volume);
    emit signalWrite2Kafka("nh-deviceState", QJsonDocument(kafkaJson).toJson(), "sast");

    return backJson;
}

void MyHttpServer::setGroupInfo(QString TermIp, QJsonArray &fileNameList, int playTimes)
{
    int GroupNum = 0;
    QString exeDir = QCoreApplication::applicationDirPath() + "/";
    //QString strMp3FilePath;
    //QString strText;

    for (int i = 0; i < m_nTermCnt; i++)	//查询当前设备所在分组
    {
        if (m_TermList[i].TermIp == TermIp) {
            GroupNum = TSDK_GetTermGroupNum(m_TermList[i].dwTermID)-1;
        }
    }

    m_GroupInfoList[GroupNum].fileNameList.clear();
    for (int i = 0; i < fileNameList.size(); i++) {	//初始化播放列表
        m_GroupInfoList[GroupNum].fileNameList.append(exeDir + "recourse/" + fileNameList.at(i).toString());
    }

    if (m_GroupInfoList[GroupNum].mp3IsOpen) {
        if (!CloseBroadMp3(m_GroupInfoList[GroupNum].groupNum)) return;
    }
    m_GroupInfoList[GroupNum].mp3IsOpen = false;
    m_GroupInfoList[GroupNum].playState = closed;
    m_GroupInfoList[GroupNum].playTimes = playTimes;	//初始化播放次数
    m_GroupInfoList[GroupNum].curFileIndex = m_GroupInfoList[GroupNum].fileNameList.size();
    groupBroadMp3(&m_GroupInfoList[GroupNum]);
}

void MyHttpServer::setTermVolume(QString TermIp, int Volume)
{
    TSdkAudioLevelEx Level;
    Level.eCapLevel = SDK_ACAP_LEVEL_1;
    Level.nAmpValue = Volume;
    Level.eCapTalkLevel = SDK_ACAP_TALK_LEVEL_1;

    QString strText;
    int nResult = -1;
    for (int i = 0; i < m_nTermCnt; i++)	//查询当前设备    dwTermID
    {
        if (m_TermList[i].TermIp == TermIp) {
            nResult = TSDK_Req_SetAudLevelEx(m_TermList[i].dwTermID, &Level);
        }
    }

    if(nResult != CERR_SUCCESS)
    {
        if(nResult == CERR_DATA_NOT_SYNC)
            strText = QString("设置音量级别失败！错误信息: 数据未同步，请先获取再进行设置");
        else if(nResult == CERR_INVALID_BIOS_VERSION)
            strText = QString("设置音量级别失败！错误信息: 分机BIOS版本不正确，请先升级");
        else
            strText = QString("设置音量级别失败！错误代码:%1").arg(nResult);
        //QMessageBox::information(nullptr, "错误", strText);
        emit showMsg(strText);
    }else{
        //QMessageBox::information(nullptr, "提示", "音量设置成功");
        strText = QString("音量设置成功, 当前音量：%1").arg(Volume);
        emit showMsg(strText);
    }
}

void MyHttpServer::groupBroadMp3(GroupInfo *groupInfo)
{

    QString strText;
    int nResult;

    switch (groupInfo->playState)
    {
    case playing:	//正在播放中
        return;
        break;
    case pause:		//暂停中

        nResult = TSDK_PauseMp3Broad(groupInfo->groupNum, false);
        if (nResult != CERR_SUCCESS)
        {
            strText = QString("暂停MP3广播失败！错误代码:%1").arg(nResult);
            //QMessageBox::information(nullptr, "错误", strText);
            emit showMsg(strText);
            return;
        }
        groupInfo->playState = playing;

        break;
    case closed:	//关闭中
        if (!groupInfo->mp3IsOpen) {
            if (!StartBroadMp3(groupInfo->groupNum)) return;
            groupInfo->mp3IsOpen = true;
        }

        if (groupInfo->curFileIndex < groupInfo->fileNameList.size()) {     //一轮未播完，继续播放
            if (!BroadMp3File(groupInfo->groupNum, groupInfo->fileNameList[groupInfo->curFileIndex])) return;
            groupInfo->curFileIndex++;
            groupInfo->playState = playing;
            return;
        }
        else {		//一轮播完，查看播放次数
            groupInfo->curFileIndex = 0;

            if (groupInfo->playTimes == -1) {		//无限播放
                if (!BroadMp3File(groupInfo->groupNum, groupInfo->fileNameList[groupInfo->curFileIndex])) return;
                groupInfo->curFileIndex++;
                groupInfo->playState = playing;
                return;
            }
            else if (groupInfo->playTimes == 0) {	//停止播放
                QJsonObject kafkaJson;
                kafkaJson.insert("deviceBh", groupInfo->sbbh);
                kafkaJson.insert("deviceState", "1");
                kafkaJson.insert("checkDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                kafkaJson.insert("insertDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                kafkaJson.insert("text", "");
                kafkaJson.insert("state", "0");
                kafkaJson.insert("value", "");
                kafkaJson.insert("color", "");
                kafkaJson.insert("volume", groupInfo->volume);
                emit signalWrite2Kafka("nh-deviceState", QJsonDocument(kafkaJson).toJson(), "sast");

                if (groupInfo->mp3IsOpen) {
                    if (!CloseBroadMp3(groupInfo->groupNum)) return;
                    groupInfo->mp3IsOpen = false;
                    return;
                }
            }
            else if (groupInfo->playTimes > 0) {	//播放次数未完，继续播放
                if (!BroadMp3File(groupInfo->groupNum, groupInfo->fileNameList[groupInfo->curFileIndex])) return;
                groupInfo->playTimes--;
                groupInfo->curFileIndex++;
                groupInfo->playState = playing;
                return;
            }
        }

        break;
    default:
        break;
    }
}

bool MyHttpServer::StartBroadMp3(int groupNum)
{
    int nResult = TSDK_StartBroadMp3(groupNum);
    if (nResult != CERR_SUCCESS)
    {
        QString strText;
        strText = QString("开启广播MP3失败！错误代码:%1").arg(nResult);
        //QMessageBox::information(nullptr, "错误", strText);
        emit showMsg(strText);
        return false;
    }
    return true;
}

bool MyHttpServer::CloseBroadMp3(int groupNum)
{
    int nResult = TSDK_StopBroadMp3(groupNum);
    if (nResult != CERR_SUCCESS)
    {
        QString strText;
        strText = QString("关闭广播MP3失败！错误代码:%1").arg(nResult);
        //QMessageBox::information(nullptr, "错误", strText);
        emit showMsg(strText);
        return false;
    }
    return true;
}

bool MyHttpServer::BroadMp3File(int groupNum, QString fileNamePath)
{
    QString strText;
    int nResult = TSDK_BroadMp3File(groupNum, fileNamePath.toLocal8Bit());
    if (nResult != CERR_SUCCESS)
    {
        strText = QString("广播MP3文件失败！错误代码:%1").arg(nResult);
        //QMessageBox::information(nullptr, "错误", strText);
        emit showMsg(strText);
        return false;
    }
    return true;
}

bool MyHttpServer::sastIsOnline(const QString &ip)
{
    for(int i=0; i<m_nTermCnt; i++){
        if(m_TermList[i].TermIp == ip){
            if(m_TermState[i].eState == TSDK_DEV_STATE_ONLINE){
                return true;
            }
        }
    }
    return false;
}

bool MyHttpServer::mp3Exist(QString mp3)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/" + "recourse/" + mp3;
    qDebug() << filePath;
    if(QFile(filePath).exists()) return true;
    return false;
}

bool MyHttpServer::updateTermVolume(QString TermIp)
{
    for (int i = 0; i < m_nTermCnt; i++)	//查询当前设备    dwTermID
    {
        if (m_TermList[i].TermIp == TermIp) {
            int res = TSDK_Req_GetAudLevel(m_TermList[i].dwTermID);
            if(res == CERR_SUCCESS){
                qDebug() << "获取设备音量成功: " << res;
                return true;
            }else{
                qDebug() << "获取设备音量失败: " << res;
            }
        }
    }
    //QThread::msleep(50);
    return false;
}

int MyHttpServer::getVolume(QString TermIp)
{
    for (int i = 0; i < m_nTermCnt; i++)	//查询当前设备    dwTermID
    {
        if (m_TermList[i].TermIp == TermIp) {
            return m_GroupInfoList[i].volume;
        }
    }
    return -1;
}

bool MyHttpServer::setVolume(QString TermIp, int volume)
{
    for (int i = 0; i < m_nTermCnt; i++)	//查询当前设备    dwTermID
    {
        if (m_TermList[i].TermIp == TermIp) {
            m_GroupInfoList[i].volume = volume;
            return true;
        }
    }
    return false;
}

bool MyHttpServer::setSbbh(QString TermIp, QString sbbh)
{
    for (int i = 0; i < m_nTermCnt; i++)	//查询当前设备    dwTermID
    {
        if (m_TermList[i].TermIp == TermIp) {
            m_GroupInfoList[i].sbbh = sbbh;
            return true;
        }
    }
    return false;
}
