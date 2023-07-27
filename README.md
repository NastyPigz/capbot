To run:

make sure to install libssl-dev, libopus0, libopus-dev, libboost-all-dev (specifically boost/asio and boost/beast)

Make sure to set option DPP_CORO=ON in CMakeLists.txt when compiling the DPP library,

and use the latest CMakeLists.txt in this project to ensure that coroutines are running smoothly.

Then
```sh
mkdir build
cd build
cmake ..
make -j(proc)

./capbot
``````