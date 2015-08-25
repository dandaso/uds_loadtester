# README

## uds_loadtest
 
    load test for unix domain socket

## Install

    yum install boost boost-devel

## Build

    make

## Usage

    -h [ --help ]            Display usage information (this message)
    -n [ --requests ] arg    Number of requests to perform
    -c [ --concurrency ] arg Number of multiple requests to make
    -s [ --sock ] arg        Path to unix domain socket
    -m [ --message ] arg     Send message to socket

## Run client

    ./uds_loadtest --sock=/tmp/netty.sock --requests=1000000 -c 4

## Result

    0%   10   20   30   40   50   60   70   80   90   100%
    |----|----|----|----|----|----|----|----|----|----|
    ***************************************************
    Concurrency Level:　4
    Num of requests: 1000000
    Time taken for tests: 9.89 sec
    Requests per second: 101112 req/sec  

