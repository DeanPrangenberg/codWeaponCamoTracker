/****************************************************************************
** Meta object code from reading C++ file 'WeaponEditor.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../WeaponEditor.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WeaponEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.1. It"
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
struct qt_meta_stringdata_CLASSWeaponEditorENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSWeaponEditorENDCLASS = QtMocHelpers::stringData(
    "WeaponEditor",
    "updateWeaponClass",
    "",
    "loadWeaponData",
    "ensureFileExists",
    "createWeaponTile",
    "weaponName",
    "index",
    "genJsonWeaponData",
    "liveUpdateWeaponData",
    "key",
    "value",
    "updateStatus"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSWeaponEditorENDCLASS_t {
    uint offsetsAndSizes[26];
    char stringdata0[13];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[15];
    char stringdata4[17];
    char stringdata5[17];
    char stringdata6[11];
    char stringdata7[6];
    char stringdata8[18];
    char stringdata9[21];
    char stringdata10[4];
    char stringdata11[6];
    char stringdata12[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSWeaponEditorENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSWeaponEditorENDCLASS_t qt_meta_stringdata_CLASSWeaponEditorENDCLASS = {
    {
        QT_MOC_LITERAL(0, 12),  // "WeaponEditor"
        QT_MOC_LITERAL(13, 17),  // "updateWeaponClass"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 14),  // "loadWeaponData"
        QT_MOC_LITERAL(47, 16),  // "ensureFileExists"
        QT_MOC_LITERAL(64, 16),  // "createWeaponTile"
        QT_MOC_LITERAL(81, 10),  // "weaponName"
        QT_MOC_LITERAL(92, 5),  // "index"
        QT_MOC_LITERAL(98, 17),  // "genJsonWeaponData"
        QT_MOC_LITERAL(116, 20),  // "liveUpdateWeaponData"
        QT_MOC_LITERAL(137, 3),  // "key"
        QT_MOC_LITERAL(141, 5),  // "value"
        QT_MOC_LITERAL(147, 12)   // "updateStatus"
    },
    "WeaponEditor",
    "updateWeaponClass",
    "",
    "loadWeaponData",
    "ensureFileExists",
    "createWeaponTile",
    "weaponName",
    "index",
    "genJsonWeaponData",
    "liveUpdateWeaponData",
    "key",
    "value",
    "updateStatus"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSWeaponEditorENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x08,    1 /* Private */,
       3,    0,   57,    2, 0x08,    2 /* Private */,
       4,    0,   58,    2, 0x08,    3 /* Private */,
       5,    2,   59,    2, 0x08,    4 /* Private */,
       8,    0,   64,    2, 0x08,    7 /* Private */,
       9,    3,   65,    2, 0x08,    8 /* Private */,
      12,    0,   72,    2, 0x08,   12 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    6,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QVariant,    6,   10,   11,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject WeaponEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSWeaponEditorENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSWeaponEditorENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSWeaponEditorENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<WeaponEditor, std::true_type>,
        // method 'updateWeaponClass'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'loadWeaponData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ensureFileExists'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'createWeaponTile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'genJsonWeaponData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'liveUpdateWeaponData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariant &, std::false_type>,
        // method 'updateStatus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void WeaponEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WeaponEditor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWeaponClass(); break;
        case 1: _t->loadWeaponData(); break;
        case 2: _t->ensureFileExists(); break;
        case 3: _t->createWeaponTile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->genJsonWeaponData(); break;
        case 5: _t->liveUpdateWeaponData((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[3]))); break;
        case 6: _t->updateStatus(); break;
        default: ;
        }
    }
}

const QMetaObject *WeaponEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WeaponEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSWeaponEditorENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int WeaponEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
