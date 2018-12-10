#include "main.hh"
#include <iostream>


ChatDialog::ChatDialog()
{
    setWindowTitle("P2Papp");

    // Read-only text box where we display messages from everyone.
    // This widget expands both horizontally and vertically.
    textview = new QTextEdit(this);
    textview->setReadOnly(true);
    // Small text-entry box the user can enter messages.
    // This widget normally expands only horizontally, leaving extra vertical space for the textview widget.
    // You might change this into a read/write QTextEdit, so that the user can easily enter multi-line messages.
    textline = new QLineEdit(this);

    // Lay out the widgets to appear in the main window.
    // For Qt widget and layout concepts see:  http://doc.qt.nokia.com/4.7-snapshot/widgets-and-layouts.html
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(textview);
    layout->addWidget(textline);
    setLayout(layout);

    qDebug()<<"#Debug ChatDialog(): initilize statemachine";
    follower = new QState();
    candidate = new QState();
    leader = new QState();
    stopped = new QState();

    qDebug()<<"#Debug ChatDialog(): initilize states";

    timtoutTimer = new QTimer(this);
    connect(timtoutTimer, SIGNAL(timeout()), this, SLOT(timeoutHandler()));

    electTimer = new QTimer(this);
    candidateTimer = new QTimer(this);
    heartTimer = new QTimer();

    follower->addTransition(electTimer, SIGNAL(timeout()), candidate);
    //follower->addTransition(this, SIGNAL(getHeartBeat()), follower);
    candidate->addTransition(candidateTimer, SIGNAL(timeout()), candidate);
    candidate->addTransition(this, SIGNAL(getHeartBeat()), follower);
    candidate->addTransition(this, SIGNAL(higherTerm()), follower);
    candidate->addTransition(this, SIGNAL(getVoteReq()), follower);
    candidate->addTransition(this, SIGNAL(getThreeVotes()), leader);
    leader->addTransition(this, SIGNAL(higherTerm()),follower);
    //stop command
    follower->addTransition(this, SIGNAL(getStop()), stopped);
    candidate->addTransition(this, SIGNAL(getStop()), stopped);
    leader->addTransition(this, SIGNAL(getStop()), stopped);
    //start command
    stopped->addTransition(this, SIGNAL(getStart()), follower);



    qDebug()<<"#Debug ChatDialog(): initilize transition";
    rolemachine.addState(follower);
    rolemachine.addState(candidate);
    rolemachine.addState(leader);
    rolemachine.addState(stopped);

    follower->assignProperty(&rolemachine, "state", "follower");
    candidate->assignProperty(&rolemachine, "state", "candidate");
    leader->assignProperty(&rolemachine, "state", "leader");
    stopped->assignProperty(&rolemachine, "state", "stopped");

    rolemachine.setInitialState(follower);
    rolemachine.start();

    qDebug()<<"#Debug ChatDialog(): finish initilize statemachine";

    //bind mySocket

    mySocket = new NetSocket();
    if (!mySocket->bind())
        exit(1);

    for (int i = mySocket->getminport(); i <= mySocket->getmaxport(); i++) {
        if (i == mySocket->getmyport()) {
            continue;
        }
        participants.push_back(i);
    }



    connect(follower, SIGNAL(entered()), this, SLOT(followerHandler()));
    connect(candidate, SIGNAL(entered()), this, SLOT(candidateHandler()));
    connect(leader, SIGNAL(entered()), this, SLOT(leaderHandler()));
    connect(stopped, SIGNAL(entered()),this, SLOT(stopHandler()));



    m_messageStatus = new QMap<QString, quint32>;

    SeqNo = 0;
    connect(textline, SIGNAL(returnPressed()), this, SLOT(gotReturnPressed()));
    //listen to anything received on the port
    connect(mySocket, SIGNAL(readyRead()), this, SLOT(readPendDgrams()));

    qDebug()<<"#Debug ChatDialog(): initilize finish";

}

void ChatDialog::followerHandler() {
        qDebug() << "#Debug: Become a Follower " << mySocket->getmyport();
        textview->append("#Debug: Become a Follower " + QString::number(mySocket->getmyport()));
        int r = rand() % (8000 - 5000) + 5000;
        electTimer->start(r);
        //Follower: rev RequestVote
        connect(this, SIGNAL(getVoteReq()), this, SLOT(goVote()));

        //Follower: rev heartbeat
        connect(this, SIGNAL(getHeartBeat()), this, SLOT(processHeartBeat()));
}

void ChatDialog::goVote(){
        qDebug()<<"#Debug: Start voting process "  << mySocket->getmyport();
        textview->append("#Debug: Start voting process " + QString::number(mySocket->getmyport()));
        qDebug()<< "#" << mySocket->getmyport() << " Status: received a RequestVote RPC" << " #";
        electTimer->start();
        QVariantMap map;
        map.insert("Vote", 1);
        if(revTerm < curterm){
            map.insert("VoteFor", false);// candidate->follower
        }
        else{
            map.insert("VoteFor", true);
            map.insert("From", mySocket->getmyport());
            curterm = revTerm;
            qDebug() << "#Debug: Vote!";
        }
        QByteArray body;
        QDataStream out(&body, QIODevice::WriteOnly);
        out << map;
        mySocket->writeDatagram(body, QHostAddress::LocalHost, toBeVoted);
}

void ChatDialog::processHeartBeat(){
    qDebug()<<"#Debug: receive a heartbeat " << mySocket->getmyport();
    textview->append("#Debug: receive a heartbeat " + QString::number(mySocket->getmyport()));
    electTimer->start();
    //check term, if less than cur, reply false
    //if term higher, reply true
    if(revTerm >= curterm){
        curterm = revTerm;
        //###send an ack
        curleader = poleader;
        qDebug()<< "#" << mySocket->getmyport() << " Status: Current leader is: " << curleader << " #";
        textview->append(" Status: Current leader is: " + QString::number(curleader));

    }
    else{
        //reply false to leader
        qDebug()<<"#" << mySocket->getmyport() << "Status: Receive smaller term from a FAKE Leader #";
    }

}

void ChatDialog::candidateHandler(){
    curleader = 0;
    qDebug()<<"#Debug: Become a Candidate " << mySocket->getmyport();
    textview->append("#Debug: SBecome a Candidate " + QString::number(mySocket->getmyport()));
    candidateTimer->start(500);
	voter.clear();
	numofvotes = 1;
	sendVoteReq();
	
    connect(this, SIGNAL(getVote(QVariantMap)), this, SLOT(processVote(QVariantMap)));
	
}

void ChatDialog::sendVoteReq() {
        //check a set of port who has voted me, if not send a request again
        qDebug()<< "#Debug: Send out RequestVote RPC "  << mySocket->getmyport();
        textview->append("#Debug: Send out RequestVote RPC " + QString::number(mySocket->getmyport()));

        QVariantMap votereq;
        votereq.insert("RequestVote", 1);
        votereq.insert("From", mySocket->getmyport());
        votereq.insert("term", curterm);

        QByteArray data;
        QDataStream stream(&data, QIODevice::ReadWrite);
        stream << votereq;
        if(participants.empty())
            qDebug()<<"#Debug: sendVoteReq  no participants";
        for (int i = 0; i < participants.size(); i++) {

        if(!voter.contains(participants[i]))
                qDebug()<< "#Debug: sendVoteReq() to " << participants[i];
                mySocket->writeDatagram(data, QHostAddress::LocalHost, participants[i]);
        }
}

void ChatDialog::processVote(QVariantMap voteMsg){
    qDebug() <<"#Debug: Processing Votes " << mySocket->getmyport();
    textview->append("#Debug: Processing Votes " + QString::number(mySocket->getmyport()));

	//see whether true or false
	if(voteMsg["VoteFor"] == false){
		emit higherTerm();	
	}
	else{
        voter.insert(voteMsg["From"].toInt());
		numofvotes++;
		if(numofvotes ==3){
			emit getThreeVotes();		
		}
	}
	
}


void ChatDialog::leaderHandler(){
        curleader = mySocket->getmyport();
        qDebug()<<"#Debug: Become a leader now! " << mySocket->getmyport();
        textview->append("#Debug: Become a *** LEADER *** " + QString::number(mySocket->getmyport()));

        sendHeartBeat();

        heartTimer->start(5000);
        connect(heartTimer, SIGNAL(timeout()),this, SLOT(sendHeartBeat()));
        //leader->follower
        //connect(higher term)
	
}

void ChatDialog::sendHeartBeat(){
        qDebug()<<"#Debug: Send a HeartBeat "<< mySocket->getmyport();
        textview->append("#Debug: Send a HeartBeat " + QString::number(mySocket->getmyport()));
        qDebug() <<"#Debug: participants number: " << participants.size();
	//restart timer
        heartTimer->start(5000);
        qDebug()<<"#Debug: heartTimer started";
	//send
        QVariantMap map;
        map.insert("AppendEntries", 1);
        qDebug()<<"#Debug: insert AppendEntries";
        map.insert("term", curterm);
        qDebug()<<"#Debug: insert curterm";
        map.insert("From", mySocket->getmyport());
        //later to add more for append logs

        qDebug()<<"#Debug: create heart map";
        QByteArray body;
        QDataStream out(&body, QIODevice::WriteOnly);
        out << map;
        qDebug()<<"#Debug: ready to send heart beat";
        //if(participants.empty())
//            qDebug()<< "#Debug: sendHeartBeat() participants is empty";
        for(int i = 0; i< 4; i++ ){
            qDebug()<<"#Debug: sendHeartBeat() " << mySocket->getmyport();
            if(participants[i] != mySocket->getmyport()){
                qDebug() << "< Send HeartBeat to " << participants[i] <<  " >";
                textview->append("#Debug: Send a HeartBeat to " + QString::number(participants[i]));

                mySocket->writeDatagram(body, body.size(), QHostAddress::LocalHost, participants[i]);
            }

        }
	
}


void ChatDialog::stopHandler(){
    //No Leader Activites
    heartTimer->stop();
    //No Follower Activites (Leader could receive a higher term and become a follower eventually)
    electTimer->stop();
    disconnect(this, SIGNAL(getVoteReq()), this, SLOT(goVote()));
    disconnect(this, SIGNAL(getHeartBeat()), this, SLOT(processHeartBeat()));
    //No Candidate Activities
    candidateTimer->stop();
    disconnect(this, SIGNAL(getVote(QVariantMap)), this, SLOT(processVote(QVariantMap)));

    textview->append("$Cresponse:Current activities got stopped" +QString::number(mySocket->getmyport()));
    /*
    if(rolemachine.configuration().contains(leader)){
        qDebug()<<"#Debug: leader got stopped";
        textview->append(QString::number(mySocket->getmyport()) + "$Cresponse: leader got stopped");
        heartTimer->stop();

    }
    else if(rolemachine.configuration().contains(candidate)){
        qDebug()<<"#Debug: candidate got stopped";
        textview->append(QString::number(mySocket->getmyport()) + "$Cresponse: candidate got stopped");
        candidateTimer->stop();
        disconnect(this, SIGNAL(getVote(QVariantMap)), this, SLOT(processVote(QVariantMap)));
    }
    else if(rolemachine.configuration().contains(follower)){
        qDebug()<<"#Debug: follower got stopped";
        textview->append(QString::number(mySocket->getmyport()) + "$Cresponse: follower got stopped");
        electTimer->stop();
    }
    */
}

void ChatDialog::createMessageMap(QVariantMap * map, QString text) {
    map->insert("ChatText", text);
    map->insert("Origin", QString::number(mySocket->getmyport()));
    map->insert("SeqNo",  SeqNo);
}

QByteArray ChatDialog::serialize(QString message_text) {

    QVariantMap msgMap;

    createMessageMap(&msgMap, message_text);
    SeqNo += 1;

    if(messages_list.contains(QString::number(mySocket->getmyport()))) {
        messages_list[QString::number(mySocket->getmyport())].insert(msgMap.value("SeqNo").toUInt(), msgMap);
    }
    else {
        QMap<quint32, QVariantMap> qvariantmap;
        messages_list.insert(QString::number(mySocket->getmyport()), qvariantmap);
        messages_list[QString::number(mySocket->getmyport())].insert(msgMap.value("SeqNo").toUInt(), msgMap);
    }

    QByteArray msgBarr;
    QDataStream stream(&msgBarr,QIODevice::ReadWrite);
    stream << msgMap;

    return msgBarr;
}

QByteArray ChatDialog::serializeStatus() {
    QMap<QString, QMap<QString, quint32> > statusMap;
    statusMap.insert("Want", localWants);

    QByteArray datagram;
    QDataStream stream(&datagram,QIODevice::ReadWrite);
    stream << statusMap;

    return datagram;
}

void ChatDialog::sendDgram(QByteArray datagram) {

    if (mySocket->getmyport() == mySocket->getmaxport()) {
        neighbor = mySocket->getmyport() - 1;

    } else if (mySocket->getmyport() == mySocket->getminport()) {
        neighbor = mySocket->getmyport() + 1;

    } else {
        srand(time(NULL));
        (rand() % 2 == 0) ?  neighbor = mySocket->getmyport() + 1: neighbor = mySocket->getmyport() - 1;
    }

    mySocket->writeDatagram(datagram, datagram.size(), QHostAddress("127.0.0.1"), neighbor);
    int r = rand() % (301 - 150) + 150;
    timtoutTimer->start(r);
}

void ChatDialog::sendStatus(QByteArray datagram)
{
    mySocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress("127.0.0.1"), remotePort);
    int r = rand() % (301 - 150) + 150;
    timtoutTimer->start(r);
}


void ChatDialog::rumorMongering(QVariantMap messageMap){

    QByteArray rumorBytes;
    QDataStream stream(&rumorBytes,QIODevice::ReadWrite);

    stream << messageMap;
    sendDgram(rumorBytes);

}

void ChatDialog::readPendDgrams()
{
    while (mySocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(mySocket->pendingDatagramSize());
        QHostAddress senderAddress;
        quint16 senderPort;

        mySocket->readDatagram(datagram.data(), datagram.size(), &senderAddress,  &senderPort);
        remotePort = senderPort;
        processIncomingDatagram(datagram);
    }
}

void ChatDialog::processIncomingDatagram(QByteArray incomingBytes)
{

    QVariantMap messageMap;
    QDataStream serializer(&incomingBytes, QIODevice::ReadOnly);
    serializer >> messageMap;
    if (serializer.status() != QDataStream::Ok) {
        return;
    }

//    QMap<QString, QMap<QString, quint32> > statusMap;
//    QDataStream stream(&incomingBytes, QIODevice::ReadOnly);
//    stream >> statusMap;
    int sender = messageMap["From"].toInt();

    if(dropped.find(sender)==dropped.end()){
        if(messageMap.contains("AppendEntries")){
            //deal with term compare and leaderId here
            revTerm = messageMap["term"].toInt();
            if(revTerm < curterm){
                emit higherTerm();
            }
            poleader = messageMap["From"].toInt();
            //if Candidate->follower, if Follower, process heartbeat reply ack
            emit getHeartBeat();
        }
        else if(messageMap.contains("RequestVote")){
            //receive a voting request
            qDebug()<<"#Debug: Receive a Voting Request";
            toBeVoted = messageMap["From"].toInt();
            emit getVoteReq();

        }
        else if(messageMap.contains("VoteFor")){
            //receive a vote from follower
            qDebug()<<"#Debug: Receive a Vote from Follower";
            emit getVote(messageMap);
        }
    }



//    if (messageMap.contains("Want")) {
//        if (statusMap.isEmpty()) {
//            return;
//        }

//        processStatus(statusMap);
//    } else if(messageMap.contains("ChatText")){
//         processMessage(messageMap);
//    } else {
//        return;
//    }
}

//void ChatDialog::processMessage(QVariantMap messageMap){

//    quint32 origin = messageMap.value("Origin").toUInt();
//    quint32 seqNo = messageMap.value("SeqNo").toUInt();

//    if(mySocket->getmyport() != origin) {
//        if(localWants.contains(QString::number(origin))) {\
//            if (seqNo == localWants.value(QString::number(origin))) {
//                 addMlist(messageMap, origin, seqNo);
//                 localWants[QString::number(origin)] = seqNo + 1;
//            }
//        } else {
//            localWants.insert(QString::number(origin), seqNo+1);
//            addMlist(messageMap, origin, seqNo);
//        }
//    } else {

//        if(localWants.contains(QString::number(origin))) {
//            localWants[QString::number(origin)] = seqNo + 1;
//        } else {
//            localWants.insert(QString::number(origin), seqNo+1);
//        }
//    }

//    timtoutTimer->stop();
//    sendStatus(serializeStatus());
//}


//void ChatDialog::addMlist(QVariantMap messageMap, quint32 origin, quint32 seqNo){

//    if(messages_list.contains(QString::number(origin))) {
//        messages_list[QString::number(origin)].insert(seqNo, messageMap);
//    }
//    else {
//        QMap<quint32, QMap<QString, QVariant> > qMap;
//        messages_list.insert(QString::number(origin), qMap);
//        messages_list[QString::number(origin)].insert(seqNo, messageMap);
//    }

//    textview->append(QString::number(origin) + ": " + messageMap.value("ChatText").toString());
//    rumorMongering(messageMap);
//    last_message = messageMap;

//}


//void ChatDialog::processStatus(QMap<QString, QMap<QString, quint32> > receivedStatusMap)
//{

//    QMap<QString, QVariant> rumorMapToSend;
//    QMap<QString, quint32> remwant = receivedStatusMap["Want"];

//    enum Status { SYNCED = 1, AHEAD = 2 , BEHIND = 3 };
//    Status status =  SYNCED;

//    QMap<QString, quint32>::const_iterator localIter = localWants.constBegin();

//    while (localIter != localWants.constEnd()){
//        if(!remwant.contains(localIter.key())){
//            status = AHEAD;
//            rumorMapToSend = messages_list[localIter.key()][quint32(0)];
//        } else if(remwant[localIter.key()] < localWants[localIter.key()]) {
//            status = AHEAD;
//            rumorMapToSend = messages_list[localIter.key()][remwant[localIter.key()]];
//        }
//        else if(remwant[localIter.key()] > localWants[localIter.key()]){
//            status = BEHIND;
//        }
//        ++localIter;
//    }

//    QMap<QString, quint32>::const_iterator remoteIter = remwant.constBegin();
//    while (remoteIter != remwant.constEnd()){
//        if(!localWants.contains(remoteIter.key())) {
//            status = BEHIND;
//        }
//        ++remoteIter;
//    }
//    timtoutTimer->stop();

//    QByteArray rumorByteArray;
//    QDataStream * stream = new QDataStream(&rumorByteArray, QIODevice::ReadWrite);
//    (*stream) << rumorMapToSend;
//    delete stream;

//    switch(status) {
//        case AHEAD:
//            mySocket->writeDatagram(rumorByteArray, QHostAddress::LocalHost, remotePort);
//            break;
//        case BEHIND:
//            sendStatus(serializeStatus());
//            break;
//        case SYNCED:
//            if(qrand() > .5*RAND_MAX) {
//                rumorMongering(last_message);
//            }
//            break;
//    }
//}


void ChatDialog::gotReturnPressed()
{
    // Just echo the string locally.
    qDebug() << "INFO: Entered gotReturnPressed(). Message Sending: " << textline->text();
    textview->append(QString::number(mySocket->getmyport()) + ": " + textline->text());

    QString input = textline->text();
    QByteArray message = serialize(input);

    if(input=="<STOP>"){
        emit getStop();
    }
    if(input == "<START>"){
        emit getStart();
    }

    if(input.startsWith("<DROP")){

        int toBeDropped = input.mid(6,5).toInt();
        dropped.insert(toBeDropped);
        textview->append(QString::number(mySocket->getmyport()) +"$Cresponse: Current dropped nodes --");
        QSet<int>::const_iterator i = dropped.constBegin();
        while(i!= dropped.constEnd()){
            textview->append(QString::number(*i));
            ++i;
        }

    }

    if(input.startsWith("<RESTORE")){
        int toBeRestored = input.mid(9,5).toInt();
        dropped.erase(dropped.find(toBeRestored));
        textview->append(QString::number(mySocket->getmyport()) +"$Cresponse: Current dropped nodes --");
        QSet<int>::const_iterator i = dropped.constBegin();
        while(i!= dropped.constEnd()){
            textview->append(QString::number(*i));
            ++i;
        }
    }

    if(input=="<GET_NODES>"){
        textview->append(QString::number(mySocket->getmyport()) +"$Cresponse: Current nodes info --");

        QVector<int>::const_iterator i = participants.constBegin();
        while(i!=participants.constEnd()){
            textview->append(QString::number(*i));
            ++i;
        }
        if(curleader!=0){
            textview->append("$Cresponse: Current leader is "+QString::number(curleader));
        }
        textview->append("$Cresponse: My current state is "+ rolemachine.property("state").toString());

    }


    if(localWants.contains(QString::number(mySocket->getmyport()))) {
        localWants[QString::number(mySocket->getmyport())] += 1;
    }
    else {
        localWants.insert(QString::number(mySocket->getmyport()), 1);
    }

    sendDgram(message);
    textline->clear();
}

void ChatDialog::timeoutHandler() {
    qDebug() << "Timeout.";
    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream << last_message;
    mySocket->writeDatagram(data, data.size(), QHostAddress("127.0.0.1"), neighbor);

    int r = rand() % (301 - 150) + 150;
    timtoutTimer->start(r);
}



//constructing NetSocket Class
NetSocket::NetSocket()
{
    // Pick a range of four UDP ports to try to allocate by default, computed based on my Unix user ID.
    // This makes it trivial for up to four P2Papp instances per user to find each other on the same host, barring UDP port conflicts with other applications (which are quite possible).
    // We use the range from 32768 to 49151 for this purpose.
    myPortMin = 32768 + (getuid() % 4096)*4;
    myPortMax = myPortMin + 4;
}

bool NetSocket::bind()
{
    // Try to bind to each of the range myPortMin..myPortMax in turn.
    for (int p = myPortMin; p <= myPortMax; p++) {
        if (QUdpSocket::bind(p)) {
            qDebug() << "INFO: bound to UDP port " << p;
            //store myPort number
            myPort = p;
            qDebug()<<"#Debug: My port# is: " << myPort;
           return true;
        }
    }
    qDebug() << "ERROR: No ports avaialble in the default range " << myPortMin << "-" << myPortMax << " available";
    return false;
}

int NetSocket::getmaxport() {
    return myPortMax;
}

int NetSocket::getminport() {
    return myPortMin;
}

int NetSocket::getmyport() {
    return myPort;
}

//main function
int main(int argc, char **argv)
{
    // Initialize Qt toolkit
    QApplication app(argc,argv);

    // Create an initial chat dialog window
    ChatDialog dialog;
    dialog.show();

    // Enter the Qt main loop; everything else is event driven
    return app.exec();

}
