# udp_server_cpp
UDP server to receive and store audio stream.

## Tests

### Test 0
It listens on port 1111 and response "Hello World\n"

### Test 1
Same as test0 but it prints what it gets

### Test 2
Also print received messages in a new line on a file

### Test 3
Check UDP packet arrive syncronously and non-blocking. Prints received messages in a file without newline. Ends and closes the file when getting SIGINT (Ctrl+C).

### Test 4
Saves UDP stream with defined headers in a wav file, syncronously and non-blocking.

### Compiling Example
```g++ -std=c++11 -o test0 test0_udp_responser.cpp -lboost_system -pthread```
