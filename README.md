# NDS-Raft-protocol-chat
```
<STOP> 
LEADER: 
turn off everything; 
won’t turn into a FOLLOWER as well
CANDIDATE: 
turn off candidateTimer
turn off sendVoteRequest(disconnect)
turn off processVoteRequest(disconnect)
Follower:
turn off electionTimer
turn off goVote
turn off processing HeartBeat

<START>
start as a follower
only stopped node turns to a FOLLOWER, other nodes stay the same

<DROP node_id>
add to QSet dropped
Q? dropping a leader and there are two leaders shifting between this node and the old leader, Correct? Even after retore connection with the old leader, there are still two leaders in the network, is that an expected bahavior?

<RESTORE node_id>
erase from QSet dropped

<GET_NODES>
participants[]
curleader if there is any// flush this variable when no leader is currently on duty
my state


<MSG >

<GET_CHAT>

```
