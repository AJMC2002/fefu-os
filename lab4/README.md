# Running the code

Get the PID from:

```bash
socat -d -d pty,raw,echo=0 pty,raw,echo=0 &
```

we'll name this `SOCAT_PID`.

Next, compile with cmake, run the simulator and then the main program.

```bash
cmake -S . -B build && cmake --build build/
./build/simulator/simulator
./build/main
```

Finally just Ctrl+C both programs and then `pkill SOCAT_PID`.
