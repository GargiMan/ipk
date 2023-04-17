#!/bin/bash

HOST=localhost
PORT=2023

printf "UDP - LF: "
printf "\x00\x08(+ 1 2)\n" | netcat -u -W 1 "$HOST" "$PORT" | grep -cqzP $'\x01\x01\x1B'"Could not parse the message" && echo "OK" || echo "FAIL"

printf "UDP - no LF: "
printf "\x00\x0F(+ 1 2 (+ 1 2))" | netcat -u -W 1 "$HOST" "$PORT" | grep -cqzP $'\x01\x00\x01'"6" && echo "OK" || echo "FAIL"