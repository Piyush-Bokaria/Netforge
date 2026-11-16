\# NetForge – Multi-Client TCP Networking System



NetForge is a Linux-based networking application built using C++ and POSIX sockets. It implements a multi-client TCP server that handles simultaneous client connections using multithreading.



\## Features



\- TCP client-server communication using POSIX sockets.

\- Multiple simultaneous client connections using C++ threads.

\- Independent client request handling.

\- Message framing and partial data transmission handling.

\- Graceful connection termination and error handling.

\- TCP packet inspection using Wireshark/TShark.



\## Tech Stack



\- \*\*Language:\*\* C++17

\- \*\*Platform:\*\* Linux (WSL Ubuntu)

\- \*\*Networking:\*\* TCP/IP, POSIX Sockets

\- \*\*Concurrency:\*\* C++ Threads, Mutex

\- \*\*Build Tool:\*\* Make

\- \*\*Packet Analysis:\*\* Wireshark / TShark



\## Project Structure



```text

NetForge/

├── src/

│   ├── server.cpp

│   └── client.cpp

├── include/

├── Makefile

└── README.md

```



\## Setup \& Execution



Install dependencies:



```bash

sudo apt update

sudo apt install build-essential git tshark -y

```



Clone the repository:



```bash

git clone <repository-url>

cd NetForge

```



Build the project:



```bash

make

```



Start the server in one terminal:



```bash

./server

```



Connect clients from separate terminals:



```bash

./client

```



Multiple clients can connect and exchange messages with the server simultaneously.



\## Packet Inspection



Capture TCP traffic on the loopback interface:



```bash

sudo tshark -i lo -f "tcp port 8080" -w netforge.pcapng

```



Analyze captured packets:



```bash

tshark -r netforge.pcapng

```



\## Learning Outcomes



Through this project, the following concepts can be explored:

\- Linux socket programming using C++.

\- TCP/IP communication fundamentals.

\- Client-server architecture.

\- Concurrent programming and thread synchronization.

\- Reliable data transmission and TCP stream framing.

\- Linux networking utilities.

\- Network packet analysis using Wireshark.



\## Future Enhancements



\- Implement UDP client-server communication.

\- Introduce a custom application-layer protocol.

\- Implement a thread pool for efficient concurrency.

\- Add client authentication and TLS encryption.

\- Implement heartbeat-based connection monitoring.

\- Add client-to-client broadcast messaging.

\- Benchmark network latency and throughput.



\## License



This project is developed for educational purposes and learning Linux network programming.

