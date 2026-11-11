# StudyBuddy

A Windows console program in C++ for finding and joining **study groups on a local network**. One person hosts a group; others on the same network discover it, ask where it meets and which courses it covers, see who's in it, and join.

It's built on **UDP sockets** with Winsock, and uses a **broadcast** message so clients can find hosts without knowing their addresses ahead of time.

## How to use it

When the program starts, it asks for your name and whether you want to **Host (1)** or **Join (2)**.

### Hosting a group

1. Enter your study location and the courses you're studying, one per line. Type `done` when finished.
2. The host listens on UDP port **31100** and answers requests from clients. You're automatically the first member of the group.
3. Each message it receives and each reply it sends is printed to the console.

### Joining a group

1. The client broadcasts a `Who?` message to the local network and waits for hosts to reply with their names.
2. Pick a group from the numbered list.
3. Use the menu to query that group:
   - **1. Where?** shows the study location
   - **2. What?** shows the courses
   - **3. Members?** shows the current members
   - **4. Join group** adds you to the group, then exits
   - **5. Exit**

## Message protocol

All messages are plain-text UDP datagrams (constants in `StudyBuddy.h`):

| Client sends | Host replies |
| --- | --- |
| `Who?` (broadcast) | `Name=<host name>` |
| `Where?` | `Loc=<location>` |
| `What?` | `Courses=<course1>\n<course2>…` |
| `Members?` | `Members=<member1>\n<member2>…` |
| `Join=<your name>` | `Join!` |

## How the code is organized

| File | Description |
| --- | --- |
| `StudyBuddy.cpp` | The program: `main` (host or join), `startHost` (the server loop), `findServers` (client discovery and menu) and `establishServerSocket` (binds the UDP socket). |
| `Utilities.cpp` | Networking helpers: `getServers` (broadcasts `Who?` and collects replies), `wait` (waits for incoming data with `select` and a timeout), and `GetBroadcastAddress` / `GetBroadcastAddressAlternate` (work out the subnet's broadcast address from the IP address and subnet mask). |
| `StudyBuddy.h` | Protocol strings, buffer sizes, the port number and the `ServerStruct` type. |
| `StudyBuddy.sln`, `StudyBuddy.vcxproj`, `StudyBuddy.vcxproj.filters` | Visual Studio solution and project files. |

## Building and running

Requires **Windows** and **Visual Studio** (it uses Winsock2 and the IP Helper API; the needed `Ws2_32.lib` and `iphlpapi.lib` are linked automatically with `#pragma comment`).

1. Open `StudyBuddy.sln` and build.
2. Run one copy as a host, and other copies (on the same machine or other machines on the same network) as clients.
3. Allow the program through Windows Firewall if asked, since it uses UDP port 31100.

## Notes and limitations

- The broadcast address is worked out from your network adapter automatically. If that fails, the program asks for your IP address and subnet mask. You can also set them in `getServers` in `Utilities.cpp`.
- The host runs until the program is closed, and members are kept only in memory.
- Broadcast discovery only finds hosts on the same local subnet.
