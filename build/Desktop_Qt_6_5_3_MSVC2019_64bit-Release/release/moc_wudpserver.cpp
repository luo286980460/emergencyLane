/****************************************************************************
** Meta object code from reading C++ file 'wudpserver.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../wudpserver.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wudpserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSWUDPServerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSWUDPServerENDCLASS = QtMocHelpers::stringData(
    "WUDPServer",
    "sigBind",
    "",
    "port",
    "ip",
    "sigUnBind",
    "sigSendDatagram",
    "data",
    "QHostAddress",
    "host",
    "showMsg",
    "slotReceiveUdpData"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSWUDPServerENDCLASS_t {
    uint offsetsAndSizes[24];
    char stringdata0[11];
    char stringdata1[8];
    char stringdata2[1];
    char stringdata3[5];
    char stringdata4[3];
    char stringdata5[10];
    char stringdata6[16];
    char stringdata7[5];
    char stringdata8[13];
    char stringdata9[5];
    char stringdata10[8];
    char stringdata11[19];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSWUDPServerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSWUDPServerENDCLASS_t qt_meta_stringdata_CLASSWUDPServerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "WUDPServer"
        QT_MOC_LITERAL(11, 7),  // "sigBind"
        QT_MOC_LITERAL(19, 0),  // ""
        QT_MOC_LITERAL(20, 4),  // "port"
        QT_MOC_LITERAL(25, 2),  // "ip"
        QT_MOC_LITERAL(28, 9),  // "sigUnBind"
        QT_MOC_LITERAL(38, 15),  // "sigSendDatagram"
        QT_MOC_LITERAL(54, 4),  // "data"
        QT_MOC_LITERAL(59, 12),  // "QHostAddress"
        QT_MOC_LITERAL(72, 4),  // "host"
        QT_MOC_LITERAL(77, 7),  // "showMsg"
        QT_MOC_LITERAL(85, 18)   // "slotReceiveUdpData"
    },
    "WUDPServer",
    "sigBind",
    "",
    "port",
    "ip",
    "sigUnBind",
    "sigSendDatagram",
    "data",
    "QHostAddress",
    "host",
    "showMsg",
    "slotReceiveUdpData"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSWUDPServerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x06,    1 /* Public */,
       1,    2,   53,    2, 0x06,    3 /* Public */,
       5,    0,   58,    2, 0x06,    6 /* Public */,
       6,    3,   59,    2, 0x06,    7 /* Public */,
      10,    1,   66,    2, 0x06,   11 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    1,   69,    2, 0x08,   13 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::UShort,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::UShort,    4,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray, 0x80000000 | 8, QMetaType::UShort,    7,    9,    3,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QByteArray,    2,

       0        // eod
};

Q_CONSTINIT const QMetaObject WUDPServer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSWUDPServerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSWUDPServerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSWUDPServerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<WUDPServer, std::true_type>,
        // method 'sigBind'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint16, std::false_type>,
        // method 'sigBind'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint16, std::false_type>,
        // method 'sigUnBind'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sigSendDatagram'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QHostAddress &, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint16, std::false_type>,
        // method 'showMsg'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slotReceiveUdpData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>
    >,
    nullptr
} };

void WUDPServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WUDPServer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sigBind((*reinterpret_cast< std::add_pointer_t<quint16>>(_a[1]))); break;
        case 1: _t->sigBind((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint16>>(_a[2]))); break;
        case 2: _t->sigUnBind(); break;
        case 3: _t->sigSendDatagram((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QHostAddress>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<quint16>>(_a[3]))); break;
        case 4: _t->showMsg((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->slotReceiveUdpData((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WUDPServer::*)(quint16 );
            if (_t _q_method = &WUDPServer::sigBind; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (WUDPServer::*)(QString , quint16 );
            if (_t _q_method = &WUDPServer::sigBind; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (WUDPServer::*)();
            if (_t _q_method = &WUDPServer::sigUnBind; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (WUDPServer::*)(const QByteArray & , const QHostAddress & , quint16 );
            if (_t _q_method = &WUDPServer::sigSendDatagram; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (WUDPServer::*)(QString );
            if (_t _q_method = &WUDPServer::showMsg; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject *WUDPServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WUDPServer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSWUDPServerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WUDPServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void WUDPServer::sigBind(quint16 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WUDPServer::sigBind(QString _t1, quint16 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WUDPServer::sigUnBind()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void WUDPServer::sigSendDatagram(const QByteArray & _t1, const QHostAddress & _t2, quint16 _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void WUDPServer::showMsg(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
