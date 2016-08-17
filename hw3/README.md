# Chat Server
Implemented in C using only shared memory.

The only 4 functions used from the shared memory library are `shmget`,
`shmat`, `shmdt`, and `shmctl`.

## Usage
```sh
$ make
$ ./server  # On 1 process

# On another process
./client group_name client_name
Enter message:
```

The server must be up before any of the clients can be started.
All client names must be unique. A max of up to 10 users can exist
on the system at a time.

## Client commands
Broadcast a regular message:
```
Enter message: Hi everyone
```

Get a list of all other people within the same group:
```
Enter message: [display]
users in group: [c1, c2]
```

Send a private message to another user:
```
Enter message: [private other_user_name] private message content
```

Exit the client:
```
Enter message: [exit]
```
Or ctrl-c.

Exit the server: just ctrl-c


## Important stuff/limitations
1. There is a 1 second wait on everything. We sleep for 1 second every period of inactivity to prevent bombarding the CPU with conditional checks checking if we should kill the server/client. As a result, logging out will take ~1 second. You can exit both the client and server by hitting ctrl-c. Do not spam the ctrl-c for the client.
2. When displaying, due to some string handling issue I do not feel is worth fixing, when displaying the list of users in the same group, you will actually see garbage data following the displayed list of groups. This is due to a misplaced null terminator, but I am lazy to fix this. The displayed group is still correct.
3. You will see text server side as the sends text to broadcast. These are just debug messages irelevant to the client. I was too lazy to remove them, and felt it was OK for us to keep them.


