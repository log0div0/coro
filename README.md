Coro
====

Asynchronous IO is a general approach to create high load servers and handle hundreds of thousands sessions simultaneously. It gives us a huge performance, but make our code ugly. For example, let's consider the code that reads some data from a socket:

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

- Tons of code! Tons of code do the simpliest thing possible. Moreover this code is separated into 2 parts, which can be located far from each other.
- You have to create a single handle for practically each operation. It will be `readHandler1`, `readHandler2` .... `readHanderN` or something like this in your code. Of cource you can use lambdas from C++11, but it's not a good idea. Because is this case we will have nested lambdas:

```
socket.async_read_some(boost::asio::buffer(data, size), [=](const boost::system::error_code& error, size_t bytesTransferred) {
  ....
  socket.async_read_some(boost::asio::buffer(data, size), [=](const boost::system::error_code& error, size_t bytesTransferred) {
    // and so on
  });
});
```

- Go on - error codes. Yes, it's a problem, because we can not using exception mechanism. With exceptions we can use RAII idiom to manage resources easily. With error codes you can forget about this future. Moreover 
