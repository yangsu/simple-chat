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
