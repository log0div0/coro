# Coro

## What's the problem?

Asynchronous IO is a general approach to create high load servers and process hundreds of thousands of sessions simultaneously. It gives us a huge performance, but makes our code ugly. For example, let's take a look at the code that reads some data from a socket:

```
socket.async_read_some(boost::asio::buffer(data, size),
  boost::bind(&MyClass::readHandler1, this,
    boost::asio::placeholders::error,
    boost::asio::placeholders::bytes_transferred));
    
// somewhere far from here ....

void MyClass::readHandler1(const boost::system::error_code& error, size_t bytesTransferred) {
  // ....
}
```

I see a lot of problems in this code:

- Tons of code! Such a simple thing has such a difficult realization. Moreover this code is split into 2 parts, which might be located far from each other.
- We have to create a single handle for each operation. It will be `readHandler1`, `readHandler2` .... `readHanderN` or something equivalent in our code. Of cource we can use lambdas from C++11, but it's not a good idea. Because in this case we have nested lambdas:

```
socket.async_read_some(boost::asio::buffer(data, size), [=](const boost::system::error_code& error, size_t bytesTransferred) {
  ....
  socket.async_read_some(boost::asio::buffer(data, size), [=](const boost::system::error_code& error, size_t bytesTransferred) {
    // and so on
  });
});
```

- Error codes. Yes, it's a problem, because we can't use an exception mechanism. With exceptions we can use RAII idiom to manage resources easily. With error codes you can forget about this feature. Moreover error codes are just numbers while in exceptions we can store any kind of information: text descriptions, backtraces, numbers of lines of code, timestamps, etc.
- When a handler is called, there is no stack at all (it will be changed after an asynchronous operation is initialized). If there is no stack - we need to hold our variables somewhere else. Moreover we have no idea from where handler has been called while debugging the program.

## How should it look like?

I like this code:

```
socket.readSome(&buffer);
```

Where method `readSome` should throw an exception if an error occurs.

## What's the solution?

The solution is well known, it's coroutines. The question is how much handy the realization could be. The main goal of this realization is the most clear code.

## Example

This is the code of an **asynchronous** tcp echo server (e.g. it can process multiple sessions simultaneously):

```
TcpServer server(endpoint);
server.run([](TcpSocket socket) {
	Buffer buffer;
	while (true) {
		buffer.pushBack(socket.readSome(&buffer));
		buffer.popFront(socket.write(buffer));
	}
});
```

A few words about `Buffer` class. It's a binary circular buffer. It has two parts: 'useful data' and 'free space'. You can move boundaries of the parts with `pushFront/Back` and `popFront/Back` methods.

## Example 2

You can use STL algorithms **asynchronously**:

```
auto newline = std::find(socket.iterator(buffer), socket.iterator(), '\n');
```
