/****************************************************************************
** Meta object code from reading C++ file 'screenworker.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../screenworker.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'screenworker.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSscreenWorkerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSscreenWorkerENDCLASS = QtMocHelpers::stringData(
    "screenWorker",
    "showMsg",
    "",
    "write2Kafka",
    "topic",
    "strJson",
    "strKey",
    "init",
    "sendData",
    "content",
    "qstr2Hex",
    "instr",
    "slotConnectToControl",
    "slotReadyRead",
    "slotUpdateScreenData",
    "ip",
    "port",
    "sbbh",
    "m_openState"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSscreenWorkerENDCLASS_t {
    uint offsetsAndSizes[38];
    char stringdata0[13];
    char stringdata1[8];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[6];
    char stringdata5[8];
    char stringdata6[7];
    char stringdata7[5];
    char stringdata8[9];
    char stringdata9[8];
    char stringdata10[9];
    char stringdata11[6];
    char stringdata12[21];
    char stringdata13[14];
    char stringdata14[21];
    char stringdata15[3];
    char stringdata16[5];
    char stringdata17[5];
    char stringdata18[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSscreenWorkerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSscreenWorkerENDCLASS_t qt_meta_stringdata_CLASSscreenWorkerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 12),  // "screenWorker"
        QT_MOC_LITERAL(13, 7),  // "showMsg"
        QT_MOC_LITERAL(21, 0),  // ""
        QT_MOC_LITERAL(22, 11),  // "write2Kafka"
        QT_MOC_LITERAL(34, 5),  // "topic"
        QT_MOC_LITERAL(40, 7),  // "strJson"
        QT_MOC_LITERAL(48, 6),  // "strKey"
        QT_MOC_LITERAL(55, 4),  // "init"
        QT_MOC_LITERAL(60, 8),  // "sendData"
        QT_MOC_LITERAL(69, 7),  // "content"
        QT_MOC_LITERAL(77, 8),  // "qstr2Hex"
        QT_MOC_LITERAL(86, 5),  // "instr"
        QT_MOC_LITERAL(92, 20),  // "slotConnectToControl"
        QT_MOC_LITERAL(113, 13),  // "slotReadyRead"
        QT_MOC_LITERAL(127, 20),  // "slotUpdateScreenData"
        QT_MOC_LITERAL(148, 2),  // "ip"
        QT_MOC_LITERAL(151, 4),  // "port"
        QT_MOC_LITERAL(156, 4),  // "sbbh"
        QT_MOC_LITERAL(161, 11)   // "m_openState"
    },
    "screenWorker",
    "showMsg",
    "",
    "write2Kafka",
    "topic",
    "strJson",
    "strKey",
    "init",
    "sendData",
    "content",
    "qstr2Hex",
    "instr",
    "slotConnectToControl",
    "slotReadyRead",
    "slotUpdateScreenData",
    "ip",
    "port",
    "sbbh",
    "m_openState"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSscreenWorkerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   62,    2, 0x06,    1 /* Public */,
       3,    3,   65,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    0,   72,    2, 0x0a,    7 /* Public */,
       8,    1,   73,    2, 0x0a,    8 /* Public */,
      10,    1,   76,    2, 0x0a,   10 /* Public */,
      12,    0,   79,    2, 0x0a,   12 /* Public */,
      13,    0,   80,    2, 0x0a,   13 /* Public */,
      14,    4,   81,    2, 0x0a,   14 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    4,    5,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::QString, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::QString,   15,   16,   17,   18,

       0        // eod
};

Q_CONSTINIT const QMetaObject screenWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSscreenWorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSscreenWorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSscreenWorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<screenWorker, std::true_type>,
        // method 'showMsg'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'write2Kafka'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'init'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'qstr2Hex'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slotConnectToControl'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotUpdateScreenData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>
    >,
    nullptr
} };

void screenWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<screenWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->showMsg((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->write2Kafka((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 2: _t->init(); break;
        case 3: _t->sendData((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: { QString _r = _t->qstr2Hex((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 5: _t->slotConnectToControl(); break;
        case 6: _t->slotReadyRead(); break;
        case 7: _t->slotUpdateScreenData((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (screenWorker::*)(QString );
            if (_t _q_method = &screenWorker::showMsg; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (screenWorker::*)(QString , QString , QString );
            if (_t _q_method = &screenWorker::write2Kafka; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *screenWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *screenWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSscreenWorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int screenWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void screenWorker::showMsg(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void screenWorker::write2Kafka(QString _t1, QString _t2, QString _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
