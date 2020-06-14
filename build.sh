#!/bin/bash

# compile
gcc -o multi_process_web_server multi_process_web_server.c -pthread
gcc -o multi_thread_web_server multi_thread_web_server.c -pthread
