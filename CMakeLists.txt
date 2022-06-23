cmake_minimum_required(VERSION 3.22)
project(lab4)

set(CMAKE_CXX_STANDARD 98)

add_executable(lab4 Receiver.cpp Sender.cpp ProcessMessage.h)
