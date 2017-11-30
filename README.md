#Random Sequence Generator
---
A **True Random Sequence Generator** using noise from external hardware as a nondeterministic data source.

The output of the program is a raw binary file of given size which can be useful for many cryptographic purposes.

###Hardware needed
---
####Arduino
---
<img src="https://www.arduino.cc/en/uploads/Trademark/ArduinoCommunityLogo.png" width="200">


Arduino is used for sampling analog values from the output of White Noise Generator, converting them and sending via UART to our machine.


The sketch and connection diagram is stored in `/arduino/analog_read` folder.

####Avalanche White Noise Generator
---

"Avalanche noise is the noise produced when a junction diode is operated at the onset of avalanche breakdown."
We use this noise to get true random values by amplifying and sampling it.


PCB design (in near future) and schematics can be found in in `/arduino/schematics`folder.

####Soundcard (any device detectable by Linux)
---
<img src="http://alien.slackbook.org/blog/wp-content/uploads/2010/08/alsalogo.gif" width="100">

The application uses `Advanced Linux Sound Architecture (ALSA)` driver (which is a part of Linux kernel) to get access to the default soundcard of the system.  Program collects audio samples (PCM) of the noise on unplugged input line and converts them to numbers (The more noisy soundcard is the better).

##Compile and run the source code
---

Once you have cloned the repository to your local machine, follow the directions below:
If you haven't installed cmake yet open your ` terminal ` and :
```
$ sudo apt-get install cmake
```
Then, `` cd `` into main directory of the project ( this one including  `` CMakeLists.txt `` file,  ``/random-sequence-generator-master`` directory probably) and run `cmake` to generate ` Makefile ` by typing

```
$ cmake
```
After that compile the program with:

```
$ make
```

Program can be executed by calling:


```
$ ./rand-seq-gen -l  <seq_length>
```
where ` <seq_length> ` is the size of output binary file specified in bytes.
If any error occured, the problem will be described in terminal.

For help/info run:
```
$ ./rand-seq-gen -h
```

######*Software has been tested on Ubuntu 17.10 x64*
