/****************************************************************************
** Meta object code from reading C++ file 'wudpserverworker.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../wudpserverworker.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wudpserverworker.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSWUDPServerWorkerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSWUDPServerWorkerENDCLASS = QtMocHelpers::stringData(
    "WUDPServerWorker",
    "sigReceiveData",
    "",
    "byte",
    "showMsg",
    "slotBind",
    "port",
    "ip",
    "slotUnBind",
    "slotSendDatagram",
    "data",
    "QHostAddress",
    "host",
    "slotReadyRead"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSWUDPServerWorkerENDCLASS_t {
    uint offsetsAndSizes[28];
    char stringdata0[17];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[5];
    char stringdata4[8];
    char stringdata5[9];
    char stringdata6[5];
    char stringdata7[3];
    char stringdata8[11];
    char stringdata9[17];
    char stringdata10[5];
    char stringdata11[13];
    char stringdata12[5];
    char stringdata13[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSWUDPServerWorkerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSWUDPServerWorkerENDCLASS_t qt_meta_stringdata_CLASSWUDPServerWorkerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 16),  // "WUDPServerWorker"
        QT_MOC_LITERAL(17, 14),  // "sigReceiveData"
        QT_MOC_LITERAL(32, 0),  // ""
        QT_MOC_LITERAL(33, 4),  // "byte"
        QT_MOC_LITERAL(38, 7),  // "showMsg"
        QT_MOC_LITERAL(46, 8),  // "slotBind"
        QT_MOC_LITERAL(55, 4),  // "port"
        QT_MOC_LITERAL(60, 2),  // "ip"
        QT_MOC_LITERAL(63, 10),  // "slotUnBind"
        QT_MOC_LITERAL(74, 16),  // "slotSendDatagram"
        QT_MOC_LITERAL(91, 4),  // "data"
        QT_MOC_LITERAL(96, 12),  // "QHostAddress"
        QT_MOC_LITERAL(109, 4),  // "host"
        QT_MOC_LITERAL(114, 13)   // "slotReadyRead"
    },
    "WUDPServerWorker",
    "sigReceiveData",
    "",
    "byte",
    "showMsg",
    "slotBind",
    "port",
    "ip",
    "slotUnBind",
    "slotSendDatagram",
    "data",
    "QHostAddress",
    "host",
    "slotReadyRead"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSWUDPServerWorkerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   56,    2, 0x06,    1 /* Public */,
       4,    1,   59,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    1,   62,    2, 0x0a,    5 /* Public */,
       5,    2,   65,    2, 0x0a,    7 /* Public */,
       8,    0,   70,    2, 0x0a,   10 /* Public */,
       9,    3,   71,    2, 0x0a,   11 /* Public */,
      13,    0,   78,    2, 0x08,   15 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::UShort,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::UShort,    7,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray, 0x80000000 | 11, QMetaType::UShort,   10,   12,    6,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject WUDPServerWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSWUDPServerWorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSWUDPServerWorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSWUDPServerWorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<WUDPServerWorker, std::true_type>,
        // method 'sigReceiveData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>,
        // method 'showMsg'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slotBind'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint16, std::false_type>,
        // method 'slotBind'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint16, std::false_type>,
        // method 'slotUnBind'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotSendDatagram'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QHostAddress &, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint16, std::false_type>,
        // method 'slotReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void WUDPServerWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WUDPServerWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sigReceiveData((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 1: _t->showMsg((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->slotBind((*reinterpret_cast< std::add_pointer_t<quint16>>(_a[1]))); break;
        case 3: _t->slotBind((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint16>>(_a[2]))); break;
        case 4: _t->slotUnBind(); break;
        case 5: _t->slotSendDatagram((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QHostAddress>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<quint16>>(_a[3]))); break;
        case 6: _t->slotReadyRead(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WUDPServerWorker::*)(const QByteArray & );
            if (_t _q_method = &WUDPServerWorker::sigReceiveData; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (WUDPServerWorker::*)(QString );
            if (_t _q_method = &WUDPServerWorker::showMsg; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *WUDPServerWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WUDPServerWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSWUDPServerWorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WUDPServerWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void WUDPServerWorker::sigReceiveData(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WUDPServerWorker::showMsg(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
