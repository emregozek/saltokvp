# saltokvp
## Description
This project developed as a part of technical interview process for C++ Software Development position in Clay.

As a part of project set of applications and libraries together with their tests and documentation are implemented.

### Supported commands (both for kvp_server and kvp_client applications)
```
SET <key> <value>
GET <key>
DELETE <key>
```

## Build/run instructions

### Build and Run kvp_server

```
./scripts/build_and_run_kvp_server.sh
```

### Build and Run kvp_client

```
./scripts/build_and_run_kvp_client.sh
```

### Build and Run Tests

```
./scripts/build_and_run_tests.sh
```

### Generate Doxygen Documentation

```
./scripts/generate_doc.sh
```

### Watch backing storage (persistent storage file)

```
./scripts/watch_kvp_storage.sh
```

## Project Structure

- Following components are implemented
    - Application
        1) kvp_server
        2) kvp_client
    - Library
        1) kvp_app
        2) kvp_store
        3) salto_ipc
    - Tests
        1) src (unit tests for components located in src folder)
        2) lib  (unit tests for components located in lib folder)

## Applications
### kvp_server application
- Supports following commands:
    1) GET
    2) SET
    3) DELETE
- Supports 2 different input methods:
    1) IPC messages (from kvp_client app)
    2) stdin

### kvp_client application
- Supports following commands:
    1) GET
    2) SET
    3) DELETE
- Supports 2 different input methods:
    2) stdin
- Sends json encoded messages to kvp_server by using boost::interprocess::ipc::message_queue

## Libraries
### kvp_app
- Provides a generic infrastructure for implementing server/client applications
    - Event registration
    - Event loop support
    - ipc infrastructure (through salto_ipc library)
### kvp_store
- Library for storing key/value pairs with backing storage support as it is described in the project description
### salto_ipc
- IPC infrasture
- Links to 3rd party libraries
    - boost::interprocess
    - nholmann::json
### Tests
- GoogleTest framework is utilized to implement unit tests.