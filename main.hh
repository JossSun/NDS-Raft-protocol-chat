#ifndef P2PAPP_MAIN_HH
#define P2PAPP_MAIN_HH

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QUdpSocket>
#include <unistd.h>
#include <QDataStream>
#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>
#include <QHostInfo>
#include <QtGlobal>
#include <QTimer>
#include <ctime>
#include <cstdlib>
#include <QVector>
#include <QStateMachine>
#include <QState>



class NetSocket : public QUdpSocket
{
    Q_OBJECT

    public:
        NetSocket();

        // Bind this socket to a P2Papp-specific default port.
        bool bind();

        int getminport();
        int getmaxport();
        int getmyport();

    private:
        int myPortMin, myPortMax, myPort;

};

class ChatDialog : public QDialog
{
    Q_OBJECT
    signals:
        void higherTerm();
        void getThreeVotes();
        void getHeartBeat();
        void getVoteReq();
        void getVote(QVariantMap);
        void getStop();
        void getStart();

    public:
        ChatDialog();
        void sendDgram(QByteArray);
        NetSocket *mySocket;
        int SeqNo;
        int remotePort; //port i receive from
        int neighbor; //port i send to
        QMap<QString, quint32> localWants;
        QVariantMap last_message;
        QMap<QString, QMap<quint32, QVariantMap> > messages_list;
        QTimer * timtoutTimer;
        QTimer * electTimer;
        QTimer * candidateTimer;
        QTimer * heartTimer;
        QTimer * antientropyTimer;
        //QTimer * voteReqTimeout;

        QStateMachine rolemachine;
        QState *follower;
        QState *candidate;
        QState *leader;
        QState *stopped;
        bool isStop = false;

	
        QVariantMap voteMsg;

        enum Role {FOLLOWER, CANDIDATE, LEADER};
        Role role = FOLLOWER;
        int curleader = 0;
        int poleader = 0;
        int curterm = 0;
        int revTerm = 0;
        int votedFor = 0;
        int toBeVoted = 0;
        bool voted = false;
        QVector<int> participants;
        int numofvotes = 0;
        int majority = 3;
        QSet<int> voter;
        QSet<int> dropped;

        void sendVoteReq();
        void voteReqTimeoutHandler();




    public slots:
        void gotReturnPressed();
        void readPendDgrams();
        void timeoutHandler();
        //state Handler
        void followerHandler();
        void candidateHandler();
        void leaderHandler();
        //comand handler
        void stopHandler();
        //void startHandler();

        void processVote(QVariantMap);

        void goVote();
        void processHeartBeat();

        void sendHeartBeat();
        //void stoppedHandler();


    private:
        QTextEdit *textview;
        QLineEdit *textline;
        QMap<QString, quint32>* m_messageStatus;
        void processIncomingDatagram(QByteArray incomingBytes);
        void processStatus(QMap<QString, QMap<QString, quint32> > wants);
        void processMessage(QVariantMap wants);
        void sendStatus(QByteArray);
        void rumorMongering(QVariantMap messageMap);
        void addMlist(QVariantMap messageMap, quint32 origin, quint32 seqNo);
        QByteArray serialize(QString);
        QByteArray serializeStatus();
        void createMessageMap(QVariantMap * map, QString text);

};

#endif // P2PAPP_MAIN_HH
