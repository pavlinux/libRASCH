/****************************************************************************
** Meta object code from reading C++ file 'mdi_child.h'
**
** Created: Fr Jul 21 19:17:24 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdi_child.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdi_child.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_mdi_child[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_mdi_child[] = {
    "mdi_child\0\0document_was_modified()\0"
};

const QMetaObject mdi_child::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_mdi_child,
      qt_meta_data_mdi_child, 0 }
};

const QMetaObject *mdi_child::metaObject() const
{
    return &staticMetaObject;
}

void *mdi_child::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_mdi_child))
	return static_cast<void*>(const_cast<mdi_child*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int mdi_child::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: document_was_modified(); break;
        }
        _id -= 1;
    }
    return _id;
}
