/****************************************************************************
** Meta object code from reading C++ file 'TileManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../TileManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TileManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TileManager_t {
    QByteArrayData data[21];
    char stringdata0[283];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TileManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TileManager_t qt_meta_stringdata_TileManager = {
    {
QT_MOC_LITERAL(0, 0, 11), // "TileManager"
QT_MOC_LITERAL(1, 12, 15), // "coverageUpdated"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 24), // "onForegroundTileRendered"
QT_MOC_LITERAL(4, 54, 8), // "QPixmap*"
QT_MOC_LITERAL(5, 63, 4), // "tile"
QT_MOC_LITERAL(6, 68, 5), // "tileX"
QT_MOC_LITERAL(7, 74, 5), // "tileY"
QT_MOC_LITERAL(8, 80, 9), // "tileLevel"
QT_MOC_LITERAL(9, 90, 12), // "onTileLoaded"
QT_MOC_LITERAL(10, 103, 8), // "tileSize"
QT_MOC_LITERAL(11, 112, 12), // "tileByteSize"
QT_MOC_LITERAL(12, 125, 12), // "ImageSource*"
QT_MOC_LITERAL(13, 138, 14), // "foregroundTile"
QT_MOC_LITERAL(14, 153, 16), // "foregroundPixmap"
QT_MOC_LITERAL(15, 170, 13), // "onTileRemoved"
QT_MOC_LITERAL(16, 184, 20), // "WSITileGraphicsItem*"
QT_MOC_LITERAL(17, 205, 26), // "onForegroundOpacityChanged"
QT_MOC_LITERAL(18, 232, 7), // "opacity"
QT_MOC_LITERAL(19, 240, 25), // "onRenderForegroundChanged"
QT_MOC_LITERAL(20, 266, 16) // "renderForeground"

    },
    "TileManager\0coverageUpdated\0\0"
    "onForegroundTileRendered\0QPixmap*\0"
    "tile\0tileX\0tileY\0tileLevel\0onTileLoaded\0"
    "tileSize\0tileByteSize\0ImageSource*\0"
    "foregroundTile\0foregroundPixmap\0"
    "onTileRemoved\0WSITileGraphicsItem*\0"
    "onForegroundOpacityChanged\0opacity\0"
    "onRenderForegroundChanged\0renderForeground"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TileManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    4,   45,    2, 0x0a /* Public */,
       9,    8,   54,    2, 0x0a /* Public */,
      15,    1,   71,    2, 0x0a /* Public */,
      17,    1,   74,    2, 0x0a /* Public */,
      19,    1,   77,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt,    5,    6,    7,    8,
    QMetaType::Void, 0x80000000 | 4, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, 0x80000000 | 12, 0x80000000 | 4,    5,    6,    7,   10,   11,    8,   13,   14,
    QMetaType::Void, 0x80000000 | 16,    5,
    QMetaType::Void, QMetaType::Float,   18,
    QMetaType::Void, QMetaType::Bool,   20,

       0        // eod
};

void TileManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TileManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->coverageUpdated(); break;
        case 1: _t->onForegroundTileRendered((*reinterpret_cast< QPixmap*(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3])),(*reinterpret_cast< uint(*)>(_a[4]))); break;
        case 2: _t->onTileLoaded((*reinterpret_cast< QPixmap*(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3])),(*reinterpret_cast< uint(*)>(_a[4])),(*reinterpret_cast< uint(*)>(_a[5])),(*reinterpret_cast< uint(*)>(_a[6])),(*reinterpret_cast< ImageSource*(*)>(_a[7])),(*reinterpret_cast< QPixmap*(*)>(_a[8]))); break;
        case 3: _t->onTileRemoved((*reinterpret_cast< WSITileGraphicsItem*(*)>(_a[1]))); break;
        case 4: _t->onForegroundOpacityChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 5: _t->onRenderForegroundChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TileManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TileManager::coverageUpdated)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TileManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_TileManager.data,
    qt_meta_data_TileManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TileManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TileManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TileManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TileManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void TileManager::coverageUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
