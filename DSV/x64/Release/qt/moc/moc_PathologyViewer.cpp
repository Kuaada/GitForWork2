/****************************************************************************
** Meta object code from reading C++ file 'PathologyViewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../PathologyViewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PathologyViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PathologyViewer_t {
    QByteArrayData data[47];
    char stringdata0[574];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PathologyViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PathologyViewer_t qt_meta_stringdata_PathologyViewer = {
    {
QT_MOC_LITERAL(0, 0, 15), // "PathologyViewer"
QT_MOC_LITERAL(1, 16, 18), // "fieldOfViewChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 3), // "FOV"
QT_MOC_LITERAL(4, 40, 5), // "level"
QT_MOC_LITERAL(5, 46, 10), // "updateBBox"
QT_MOC_LITERAL(6, 57, 24), // "backgroundChannelChanged"
QT_MOC_LITERAL(7, 82, 9), // "channelNr"
QT_MOC_LITERAL(8, 92, 12), // "rightClicked"
QT_MOC_LITERAL(9, 105, 8), // "viewShow"
QT_MOC_LITERAL(10, 114, 3), // "pix"
QT_MOC_LITERAL(11, 118, 11), // "factorTrans"
QT_MOC_LITERAL(12, 130, 6), // "factor"
QT_MOC_LITERAL(13, 137, 8), // "initOver"
QT_MOC_LITERAL(14, 146, 8), // "mppTrans"
QT_MOC_LITERAL(15, 155, 3), // "num"
QT_MOC_LITERAL(16, 159, 10), // "fpsUpdated"
QT_MOC_LITERAL(17, 170, 3), // "fps"
QT_MOC_LITERAL(18, 174, 16), // "perimeterUpdated"
QT_MOC_LITERAL(19, 191, 9), // "perimeter"
QT_MOC_LITERAL(20, 201, 23), // "areaAndPerimeterUpdated"
QT_MOC_LITERAL(21, 225, 4), // "Area"
QT_MOC_LITERAL(22, 230, 6), // "moveTo"
QT_MOC_LITERAL(23, 237, 3), // "pos"
QT_MOC_LITERAL(24, 241, 20), // "onFieldOfViewChanged"
QT_MOC_LITERAL(25, 262, 24), // "onForegroundImageChanged"
QT_MOC_LITERAL(26, 287, 35), // "std::weak_ptr<MultiResolution..."
QT_MOC_LITERAL(27, 323, 7), // "for_img"
QT_MOC_LITERAL(28, 331, 5), // "scale"
QT_MOC_LITERAL(29, 337, 18), // "setFileWidgetState"
QT_MOC_LITERAL(30, 356, 5), // "state"
QT_MOC_LITERAL(31, 362, 17), // "setMiniMapVisible"
QT_MOC_LITERAL(32, 380, 15), // "setCoverageArea"
QT_MOC_LITERAL(33, 396, 11), // "setScaleBar"
QT_MOC_LITERAL(34, 408, 10), // "updateSnap"
QT_MOC_LITERAL(35, 419, 18), // "setLabelMapVisible"
QT_MOC_LITERAL(36, 438, 16), // "setDetailVisible"
QT_MOC_LITERAL(37, 455, 13), // "setChangedMpp"
QT_MOC_LITERAL(38, 469, 3), // "mpp"
QT_MOC_LITERAL(39, 473, 23), // "onActionDeleteSelection"
QT_MOC_LITERAL(40, 497, 11), // "scalingTime"
QT_MOC_LITERAL(41, 509, 1), // "x"
QT_MOC_LITERAL(42, 511, 12), // "zoomFinished"
QT_MOC_LITERAL(43, 524, 19), // "handleItemSelection"
QT_MOC_LITERAL(44, 544, 14), // "QGraphicsItem*"
QT_MOC_LITERAL(45, 559, 4), // "item"
QT_MOC_LITERAL(46, 564, 9) // "updateFPS"

    },
    "PathologyViewer\0fieldOfViewChanged\0\0"
    "FOV\0level\0updateBBox\0backgroundChannelChanged\0"
    "channelNr\0rightClicked\0viewShow\0pix\0"
    "factorTrans\0factor\0initOver\0mppTrans\0"
    "num\0fpsUpdated\0fps\0perimeterUpdated\0"
    "perimeter\0areaAndPerimeterUpdated\0"
    "Area\0moveTo\0pos\0onFieldOfViewChanged\0"
    "onForegroundImageChanged\0"
    "std::weak_ptr<MultiResolutionImage>\0"
    "for_img\0scale\0setFileWidgetState\0state\0"
    "setMiniMapVisible\0setCoverageArea\0"
    "setScaleBar\0updateSnap\0setLabelMapVisible\0"
    "setDetailVisible\0setChangedMpp\0mpp\0"
    "onActionDeleteSelection\0scalingTime\0"
    "x\0zoomFinished\0handleItemSelection\0"
    "QGraphicsItem*\0item\0updateFPS"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PathologyViewer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,  149,    2, 0x06 /* Public */,
       5,    1,  154,    2, 0x06 /* Public */,
       6,    1,  157,    2, 0x06 /* Public */,
       8,    0,  160,    2, 0x06 /* Public */,
       9,    1,  161,    2, 0x06 /* Public */,
      11,    1,  164,    2, 0x06 /* Public */,
      13,    0,  167,    2, 0x06 /* Public */,
      14,    1,  168,    2, 0x06 /* Public */,
      16,    1,  171,    2, 0x06 /* Public */,
      18,    1,  174,    2, 0x06 /* Public */,
      20,    2,  177,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      22,    1,  182,    2, 0x0a /* Public */,
      24,    2,  185,    2, 0x0a /* Public */,
      25,    2,  190,    2, 0x0a /* Public */,
      29,    1,  195,    2, 0x0a /* Public */,
      31,    1,  198,    2, 0x0a /* Public */,
      32,    1,  201,    2, 0x0a /* Public */,
      33,    1,  204,    2, 0x0a /* Public */,
      34,    0,  207,    2, 0x0a /* Public */,
      35,    0,  208,    2, 0x0a /* Public */,
      36,    0,  209,    2, 0x0a /* Public */,
      37,    1,  210,    2, 0x0a /* Public */,
      39,    0,  213,    2, 0x0a /* Public */,
      40,    1,  214,    2, 0x08 /* Private */,
      42,    0,  217,    2, 0x08 /* Private */,
      43,    1,  218,    2, 0x08 /* Private */,
      46,    0,  221,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QRectF, QMetaType::UInt,    3,    4,
    QMetaType::Void, QMetaType::QRectF,    3,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPixmap,   10,
    QMetaType::Void, QMetaType::Float,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,   15,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Float,   19,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   19,   21,

 // slots: parameters
    QMetaType::Void, QMetaType::QPointF,   23,
    QMetaType::Void, QMetaType::QRectF, QMetaType::UInt,    3,    4,
    QMetaType::Void, 0x80000000 | 26, QMetaType::Float,   27,   28,
    QMetaType::Void, QMetaType::Bool,   30,
    QMetaType::Void, QMetaType::Bool,   30,
    QMetaType::Void, QMetaType::Bool,   30,
    QMetaType::Void, QMetaType::Bool,   30,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,   38,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QReal,   41,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 44,   45,
    QMetaType::Void,

       0        // eod
};

void PathologyViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PathologyViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->fieldOfViewChanged((*reinterpret_cast< const QRectF(*)>(_a[1])),(*reinterpret_cast< const uint(*)>(_a[2]))); break;
        case 1: _t->updateBBox((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        case 2: _t->backgroundChannelChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->rightClicked(); break;
        case 4: _t->viewShow((*reinterpret_cast< const QPixmap(*)>(_a[1]))); break;
        case 5: _t->factorTrans((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 6: _t->initOver(); break;
        case 7: _t->mppTrans((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 8: _t->fpsUpdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->perimeterUpdated((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 10: _t->areaAndPerimeterUpdated((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 11: _t->moveTo((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 12: _t->onFieldOfViewChanged((*reinterpret_cast< const QRectF(*)>(_a[1])),(*reinterpret_cast< const uint(*)>(_a[2]))); break;
        case 13: _t->onForegroundImageChanged((*reinterpret_cast< std::weak_ptr<MultiResolutionImage>(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 14: _t->setFileWidgetState((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: _t->setMiniMapVisible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: _t->setCoverageArea((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: _t->setScaleBar((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->updateSnap(); break;
        case 19: _t->setLabelMapVisible(); break;
        case 20: _t->setDetailVisible(); break;
        case 21: _t->setChangedMpp((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 22: _t->onActionDeleteSelection(); break;
        case 23: _t->scalingTime((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 24: _t->zoomFinished(); break;
        case 25: _t->handleItemSelection((*reinterpret_cast< QGraphicsItem*(*)>(_a[1]))); break;
        case 26: _t->updateFPS(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PathologyViewer::*)(const QRectF & , const unsigned int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PathologyViewer::fieldOfViewChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PathologyViewer::*)(const QRectF & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PathologyViewer::updateBBox)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PathologyViewer::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PathologyViewer::backgroundChannelChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PathologyViewer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PathologyViewer::rightClicked)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (PathologyViewer::*)(const QPixmap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PathologyViewer::viewShow)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (PathologyViewer::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PathologyViewer::factorTrans)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (PathologyViewer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PathologyViewer::initOver)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (PathologyViewer::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PathologyViewer::mppTrans)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (PathologyViewer::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PathologyViewer::fpsUpdated)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (PathologyViewer::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PathologyViewer::perimeterUpdated)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (PathologyViewer::*)(float , float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PathologyViewer::areaAndPerimeterUpdated)) {
                *result = 10;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PathologyViewer::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsView::staticMetaObject>(),
    qt_meta_stringdata_PathologyViewer.data,
    qt_meta_data_PathologyViewer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PathologyViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PathologyViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PathologyViewer.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsView::qt_metacast(_clname);
}

int PathologyViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 27)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 27;
    }
    return _id;
}

// SIGNAL 0
void PathologyViewer::fieldOfViewChanged(const QRectF & _t1, const unsigned int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PathologyViewer::updateBBox(const QRectF & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PathologyViewer::backgroundChannelChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PathologyViewer::rightClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void PathologyViewer::viewShow(const QPixmap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void PathologyViewer::factorTrans(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void PathologyViewer::initOver()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void PathologyViewer::mppTrans(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void PathologyViewer::fpsUpdated(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void PathologyViewer::perimeterUpdated(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void PathologyViewer::areaAndPerimeterUpdated(float _t1, float _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
