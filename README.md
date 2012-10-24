# simple-chat
## Yang Su

## Instructions
run the following commands
```bash
git clone git@github.com:yangsu/simple-chat.git
make all
```

this will produce a binary called main. run it with the following command
```bash
./main
```

Once you are in the program, there are many commands you can use:

- **s** : start server. can pass in an optional port number argument
- **c** : start client. must specify and ip address. you can also pass in an optional port number argument
- **cli** : get clients
- **connect** : connect to another client. must specify an id
- **p2p** : connect to another client using a p2p connection. must specify an id
- **disconnect** : disconnect from the current chat
- **y** : Accept request. must specify an id
- **n** : Reject request. must specify an id
- **h** : help
- **q** : quit

To run Client/Server chat follow the instructions below:

1. open up 3 or more instances of the `main` program
2. in 1 instance, type `s` to start server
3. in the other instances, type `c localhost`, which will create clients to connect to the server
4. in the clients, type `cli` to show a list of clients, which will be in the format of "User id"
5. in one of the clients, select a user from the clients list and type `connect id`, where id is the chosen user's id
6. in the terminal corresponding to the client being connected to, a message will appear prompting to accept or reject the connection
7. to reject, type `n`; to accept type `y id`, this is to allow for the client to choose which user's request to accept
8. the connection should be established after that, and simply type any message in either of the terminals to send messages back and forth
9. type `disconnect` to disconnect from the current chat and return to step 4
10. lastly, type `h` for help at any time and `q` to quit the program at any time

To run P2P/Client/Server, follow the instructions below:

1. open up 3 or more instances of the `main` program
2. in 1 instance, type `s` to start server
3. in the other instances, type `c localhost`, which will create clients to connect to the server
4. in the clients, type `cli` to show a list of clients, which will be in the format of "User id"
5. in one of the clients, select a user from the clients list and type `p2p id`, where id is the chosen user's id
6. in the terminal corresponding to the client being connected to, a message will appear prompting to accept or reject the connection
7. to reject, type `n`; to accept type `y id`, this is to allow for the client to choose which user's request to accept
8. the connection should be established after that, and simply type any message in either of the terminals to send messages back and forth
9. type `disconnect` to disconnect from the current chat and return to step 3
10. lastly, type `h` for help at any time and `q` to quit the program at any time

## Limitations
* Cannot chat with more than 1 user
* Only uses TCP connections
* Messages are limited in length so really long texts will have trouble getting transported across channels

## Analysis
In the client/server model, the server is the single point of failure, in that if the server gets overrun with messages or if it goes down completely, the users will have a very hard time reaching each other. However, the information required to run such a system is relatively simple. The clients only need to remember an unique identifier of some sort for whoever else they are talking to and that will be sufficient to allow the server to route the messages properly. Similarly, on the server side, aside from a simple mapping between the id of an user and his/her info and the first time a client registers with the system, all other forms of communication is rather simple and can be self contained. This make the logic on the server very easy to maintain, manage, and extend.

In contrast, while the hybrid model addresses the single point of failure problem that the client/server model has, it also introduces additional complexity. In this system, each client must act as both a client and a server in order to make connections with other users and accept incoming requests from other users. This requires additional storage requirements and data structures to keep all the relative information at each node that's connected to the mesh network. The server's responsibilities are greatly reduced as it simply provides the ip and port of each of the users.

## Documentation
The program has 3 abstraction layers, a input processing system, and a simple thread system to allow for simultaneous input processing and network communication

### Abstractions
#### Socket
This class wraps over the basic functionality of creating sockets, managing file descriptors, closing sockets, getting address information, and reading and writing data to sockets. This class also defines many of the constants used within socket communication, such as header format and types of messages that can be passed in between sockets. I have chose to use a custom header format to ensure flexibility and easy access of the relevant information without having to modify other parts of the code or creating additional sockets. In fact, regardless of the model, all the messages go through a single socket for each client or server
#### Server, Client
These two classes add additional functionality over the basic sockets, such as making connections to servers for client objects and accepting connections from clients for server objects. This abstracts away the code that handle all the error checking so things like monitoring non blocking sockets on when the data is read to be written or read can be done transparent from the consumer of the class
#### ChatClient, ChatServer
These two classes are the top level entities that the main program interacts with, and contain all the functionality of sending, receiving, processing, and displaying the actual messages, as well as all the states and functions for establishing and managing connections and issuing commands. They contain states and data structures that store things like the mapping from id to client information and what state the program is in so that only the correct and relevant commands can be issued

### Input Processing
This is done by separating commands from the text. Each command is defined with a trigger, a function pointer, and a description. this allows for very simple and flexible addition and removal of commands. The use of function pointers also means that once they are defined, the same code can issue all the commands within the same block rather than repeating the execution code over and over again. However, this created challenges for commands the need arguments to be passed in, such as specifying the address and port number of a server. I was able to resolve this by using variable argument lists.

### Thread System
The program uses a simple thread system using pthreads that spawns a program to monitor the network connections while leaving the main thread to process and wait on a user's input. this way, all the incoming or outgoing communication can be processed in the background and updated/displayed accordingly without interfering with user input