/****************************************************************************
** Meta object code from reading C++ file 'main.hh'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "main.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetSocket[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_NetSocket[] = {
    "NetSocket\0"
};

void NetSocket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData NetSocket::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetSocket::staticMetaObject = {
    { &QUdpSocket::staticMetaObject, qt_meta_stringdata_NetSocket,
      qt_meta_data_NetSocket, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetSocket::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetSocket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetSocket))
        return static_cast<void*>(const_cast< NetSocket*>(this));
    return QUdpSocket::qt_metacast(_clname);
}

int NetSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QUdpSocket::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ChatDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      25,   11,   11,   11, 0x05,
      41,   11,   11,   11, 0x05,
      56,   11,   11,   11, 0x05,
      69,   11,   11,   11, 0x05,
      90,   11,   11,   11, 0x05,
     100,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     111,   11,   11,   11, 0x0a,
     130,   11,   11,   11, 0x0a,
     147,   11,   11,   11, 0x0a,
     164,   11,   11,   11, 0x0a,
     182,   11,   11,   11, 0x0a,
     201,   11,   11,   11, 0x0a,
     217,   11,   11,   11, 0x0a,
     231,   11,   11,   11, 0x0a,
     256,   11,   11,   11, 0x0a,
     265,   11,   11,   11, 0x0a,
     284,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ChatDialog[] = {
    "ChatDialog\0\0higherTerm()\0getThreeVotes()\0"
    "getHeartBeat()\0getVoteReq()\0"
    "getVote(QVariantMap)\0getStop()\0"
    "getStart()\0gotReturnPressed()\0"
    "readPendDgrams()\0timeoutHandler()\0"
    "followerHandler()\0candidateHandler()\0"
    "leaderHandler()\0stopHandler()\0"
    "processVote(QVariantMap)\0goVote()\0"
    "processHeartBeat()\0sendHeartBeat()\0"
};

void ChatDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChatDialog *_t = static_cast<ChatDialog *>(_o);
        switch (_id) {
        case 0: _t->higherTerm(); break;
        case 1: _t->getThreeVotes(); break;
        case 2: _t->getHeartBeat(); break;
        case 3: _t->getVoteReq(); break;
        case 4: _t->getVote((*reinterpret_cast< QVariantMap(*)>(_a[1]))); break;
        case 5: _t->getStop(); break;
        case 6: _t->getStart(); break;
        case 7: _t->gotReturnPressed(); break;
        case 8: _t->readPendDgrams(); break;
        case 9: _t->timeoutHandler(); break;
        case 10: _t->followerHandler(); break;
        case 11: _t->candidateHandler(); break;
        case 12: _t->leaderHandler(); break;
        case 13: _t->stopHandler(); break;
        case 14: _t->processVote((*reinterpret_cast< QVariantMap(*)>(_a[1]))); break;
        case 15: _t->goVote(); break;
        case 16: _t->processHeartBeat(); break;
        case 17: _t->sendHeartBeat(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ChatDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChatDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ChatDialog,
      qt_meta_data_ChatDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChatDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChatDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChatDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChatDialog))
        return static_cast<void*>(const_cast< ChatDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ChatDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void ChatDialog::higherTerm()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ChatDialog::getThreeVotes()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void ChatDialog::getHeartBeat()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void ChatDialog::getVoteReq()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void ChatDialog::getVote(QVariantMap _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ChatDialog::getStop()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void ChatDialog::getStart()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}
QT_END_MOC_NAMESPACE
