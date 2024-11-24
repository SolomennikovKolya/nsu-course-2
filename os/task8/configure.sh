#!/bin/bash

g++ main.cpp -o main.out
sudo chown testuser:testuser main.out
sudo chmod u+s main.out
