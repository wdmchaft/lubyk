/****************************************************************************
** Meta object code from reading C++ file 'ListView.h'
**
** Created: Thu Oct 20 12:12:47 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/mimas/ListView.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ListView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_mimas__ListView[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      24,   16, 0x0a095001,
      36,   30, 0x87095103,

       0        // eod
};

static const char qt_meta_stringdata_mimas__ListView[] = {
    "mimas::ListView\0QString\0class\0float\0"
    "hue\0"
};

const QMetaObject mimas::ListView::staticMetaObject = {
    { &QListView::staticMetaObject, qt_meta_stringdata_mimas__ListView,
      qt_meta_data_mimas__ListView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &mimas::ListView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *mimas::ListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *mimas::ListView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_mimas__ListView))
        return static_cast<void*>(const_cast< ListView*>(this));
    if (!strcmp(_clname, "ThreadedLuaObject"))
        return static_cast< ThreadedLuaObject*>(const_cast< ListView*>(this));
    return QListView::qt_metacast(_clname);
}

int mimas::ListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = cssClass(); break;
        case 1: *reinterpret_cast< float*>(_v) = hue(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: setHue(*reinterpret_cast< float*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
