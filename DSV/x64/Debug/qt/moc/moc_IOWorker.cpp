/****************************************************************************
** Meta object code from reading C++ file 'IOWorker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../IOWorker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'IOWorker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_IOWorker_t {
    QByteArrayData data[14];
    char stringdata0[147];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IOWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_IOWorker_t qt_meta_stringdata_IOWorker = {
    {
QT_MOC_LITERAL(0, 0, 8), // "IOWorker"
QT_MOC_LITERAL(1, 9, 10), // "tileLoaded"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 8), // "QPixmap*"
QT_MOC_LITERAL(4, 30, 4), // "tile"
QT_MOC_LITERAL(5, 35, 5), // "tileX"
QT_MOC_LITERAL(6, 41, 5), // "tileY"
QT_MOC_LITERAL(7, 47, 8), // "tileSize"
QT_MOC_LITERAL(8, 56, 12), // "tileByteSize"
QT_MOC_LITERAL(9, 69, 9), // "tileLevel"
QT_MOC_LITERAL(10, 79, 12), // "ImageSource*"
QT_MOC_LITERAL(11, 92, 14), // "foregroundTile"
QT_MOC_LITERAL(12, 107, 16), // "foregroundPixmap"
QT_MOC_LITERAL(13, 124, 22) // "foregroundTileRendered"

    },
    "IOWorker\0tileLoaded\0\0QPixmap*\0tile\0"
    "tileX\0tileY\0tileSize\0tileByteSize\0"
    "tileLevel\0ImageSource*\0foregroundTile\0"
    "foregroundPixmap\0foregroundTileRendered"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IOWorker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    8,   34,    2, 0x06 /* Public */,
       1,    7,   51,    2, 0x26 /* Public | MethodCloned */,
       1,    6,   66,    2, 0x26 /* Public | MethodCloned */,
      13,    4,   79,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, 0x80000000 | 10, 0x80000000 | 3,    4,    5,    6,    7,    8,    9,   11,   12,
    QMetaType::Void, 0x80000000 | 3, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, 0x80000000 | 10,    4,    5,    6,    7,    8,    9,   11,
    QMetaType::Void, 0x80000000 | 3, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt,    4,    5,    6,    7,    8,    9,
    QMetaType::Void, 0x80000000 | 3, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt,    4,    5,    6,    9,

       0        // eod
};

void IOWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<IOWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->tileLoaded((*reinterpret_cast< QPixmap*(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3])),(*reinterpret_cast< uint(*)>(_a[4])),(*reinterpret_cast< uint(*)>(_a[5])),(*reinterpret_cast< uint(*)>(_a[6])),(*reinterpret_cast< ImageSource*(*)>(_a[7])),(*reinterpret_cast< QPixmap*(*)>(_a[8]))); break;
        case 1: _t->tileLoaded((*reinterpret_cast< QPixmap*(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3])),(*reinterpret_cast< uint(*)>(_a[4])),(*reinterpret_cast< uint(*)>(_a[5])),(*reinterpret_cast< uint(*)>(_a[6])),(*reinterpret_cast< ImageSource*(*)>(_a[7]))); break;
        case 2: _t->tileLoaded((*reinterpret_cast< QPixmap*(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3])),(*reinterpret_cast< uint(*)>(_a[4])),(*reinterpret_cast< uint(*)>(_a[5])),(*reinterpret_cast< uint(*)>(_a[6]))); break;
        case 3: _t->foregroundTileRendered((*reinterpret_cast< QPixmap*(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3])),(*reinterpret_cast< uint(*)>(_a[4]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (IOWorker::*)(QPixmap * , unsigned int , unsigned int , unsigned int , unsigned int , unsigned int , ImageSource * , QPixmap * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&IOWorker::tileLoaded)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (IOWorker::*)(QPixmap * , unsigned int , unsigned int , unsigned int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&IOWorker::foregroundTileRendered)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject IOWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_IOWorker.data,
    qt_meta_data_IOWorker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *IOWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IOWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_IOWorker.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int IOWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void IOWorker::tileLoaded(QPixmap * _t1, unsigned int _t2, unsigned int _t3, unsigned int _t4, unsigned int _t5, unsigned int _t6, ImageSource * _t7, QPixmap * _t8)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t8))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 3
void IOWorker::foregroundTileRendered(QPixmap * _t1, unsigned int _t2, unsigned int _t3, unsigned int _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
