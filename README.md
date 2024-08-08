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
*   \[MySql\](https://dev.mysql.com/doc/) for data storage
*   \[Zlib\](https://zlib.net/) for data compression
*   \[Abseil\](https://abseil.io/) for additional C++ utility libraries
*   \[CMake\](https://cmake.org/) for build configuration

<h2>Future Plans of Server Architecture</h2>

### Overview

The future architecture for the server infrastructure of the MMORPG will consist of several distinct server types, each responsible for different aspects of the game. The main servers will include:

- **Login Server**
- **Game Server**
- **World Server**
- **Database Server**

### Server Components

<h3>Login Server</h3>

The Login Server will handle player registration, login, and authentication. It will be implemented in C#/.NET, leveraging the robust capabilities of the .NET framework for managing user sessions and secure authentication. This server will communicate with both the Database Server for retrieving and storing user credentials and the Game Server for initiating player sessions.

**Key Responsibilities:**
- Player registration and login
- User authentication and session management
- Communication with Database Server for user data
- Interaction with Game Server to create player sessions

<h3>Game Server</h3>

The Game Server will be responsible for managing the game state and interacting directly with clients. It will handle gameplay logic, player actions, and real-time communication. The Game Server will communicate with the World Server to update and synchronize the game world and with clients to relay game information and responses.

**Key Responsibilities:**
- Managing game state and player interactions
- Real-time communication with clients
- Synchronization with World Server for game world updates
- Handling gameplay logic and events

<h3> World Server</h3>

The World Server will focus on managing the game world, including the state of various in-game objects, environments, and player positions. It will be responsible for world simulation and will interact with the Game Server to ensure consistency and synchronization of the game world.

**Key Responsibilities:**
- Managing game world state and simulation
- Synchronization with Game Server
- Handling dynamic world updates and events

<h3>Database Server</h3>

The Database Server will be central to storing and managing all persistent data related to the game, including player information, game state, and world data. All other servers will communicate with the Database Server to retrieve or update data as needed.

**Key Responsibilities:**
- Storing player data, game state, and world information
- Providing data access and management services for other servers
- Ensuring data integrity and security

### Communication Flow

- **Login Server** communicates with the **Database Server** to authenticate users and with the **Game Server** to initiate player sessions.
- **Game Server** handles communication with clients and interacts with the **World Server** to manage the game world.
- **World Server** communicates with the **Game Server** to provide updates and synchronize the game world with world state such as world events, monsters behaviour and NPCs.
- All servers interact with the **Database Server** for data storage and retrieval.

### Future Enhancements

- **Scalability Improvements:** Explore horizontal scaling options for each server type to handle increased player load and data volume.
- **Redundancy and Failover:** Implement redundancy and failover mechanisms to ensure high availability and reliability of the server infrastructure.
- **Performance Optimization:** Continuously optimize server performance to handle real-time interactions and large-scale game worlds efficiently.
- **Security Enhancements:** Enhance security measures across all servers to protect against potential threats and ensure data integrity.



<h2>Currently in development</h2>

* MessageInterpreter class to separate handling different messages from client based on Wrapper::MessageType
