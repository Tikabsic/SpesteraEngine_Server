<h1 align="center" id="title">SpesteraEngine GameServer</h1>

<p id="description">Server-side application for an MMORPG game fully written in C++ using the Boost library supporting async sessions for client TCP connections and async UDP messages using Google Protobuf and Zlib for serialization larger data structures.</p>

  
  
<h2>Features</h2>

Here're some of the project's best features:

*   Asynchronous handling of TCP client connections
*   Asynchronous UDP messaging
*   Google Protobuf for efficient serialization and deserialization of messages
*   Server-side movement handling
*   Session management for multiple players
*   Periodic server heartbeats to manage active connections and world state

<h2>Installation Steps:</h2>

<p>1. Clone the repository:</p>

```
   git clone https://github.com/Tikabsic/SpesteraEngine_Server.git    cd SpesteraEngine_Server
```

<p>2. Install dependencies:</p>

```
sudo apt-get install libboost-all-dev sudo apt-get install protobuf-compiler libprotobuf-dev sudo apt-get install zlib1g-dev
```

<p>3. Build the project using CMake:</p>

```
mkdir build cd build cmake .. make
```

  
  
<h2>Built with</h2>

Technologies used in the project:

*   \[Boost.Asio\](https://www.boost.org/doc/libs/1\_76\_0/doc/html/boost\_asio.html) for asynchronous I/O operations
*   \[Google Protobuf\](https://developers.google.com/protocol-buffers) for message serialization
*   \[Zlib\](https://zlib.net/) for data compression
*   \[Abseil\](https://abseil.io/) for additional C++ utility libraries
*   \[CMake\](https://cmake.org/) for build configuration

<h2>Currently in development</h2>

* MessageInterpreter class to separate handling different messages from client based on Wrapper::MessageType
