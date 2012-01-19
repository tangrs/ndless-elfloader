# Notes for porting to Ndless 3.1

I've had a brief look at the new Ndless release. There have been some major changes to the code. However, most of the core loader should 'just work'. Most of the issues are in the 'decoration'.

I haven't tried compiling for the new SDK just yet so this is an approximation of what needs to be done.

1. Debugging console - won't work on the CX because it has no support for a color screen. Need to add compatibility
2. Hook code - Ndless 3.1 is apparently 'reboot proof'. I don't know how it's implemented yet but I believe some hooking code will be different. Addresses to hook for the program loader will be different on the new OS. This will be the most important one to do.
3. Benchmarking uses built in timers to function. We need to replace static IO addresses with the IO() macro.