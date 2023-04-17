#!/bin/bash

HOST=localhost
PORT=2023

printf "TCP - LF: "
printf "HELLO\nSOLVE (+ 1 2)\nBYE\n" | netcat "$HOST" "$PORT" | grep -cqzP "HELLO\nRESULT 3\nBYE\n" && echo "OK" || echo "FAIL"

printf "TCP - no LF: "
printf "HELLO  SOLVE (+ 1 2)  BYE  " | netcat "$HOST" "$PORT" | grep -cqzP "BYE\n" && echo "OK" || echo "FAIL"

printf "TCP - LF + no LF: "
printf "HELLO\nSOLVE (+ 1 2)  BYE  " | netcat "$HOST" "$PORT" | grep -cqzP "HELLO\nBYE\n" && echo "OK" || echo "FAIL"
