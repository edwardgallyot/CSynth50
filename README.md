# CSynth50
## Video Demo

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/3ijF-oWja_o/0.jpg)](https://www.youtube.com/watch?v=3ijF-oWja_o)

## Description

CSynth50 is my final project for HarvardX's CS50. I started this project as a way of teaching myself
to program oscillators and envelopes as well as parse MIDI data into C.

### Features

Currently CSynth50 has these functions:

* Four Oscillators: Digital Saw, Analog Saw, Digital Square and Analog Square
* An Volume Envelope
* A Global Volume Control

I would love to add either an LFO section and/or a filter section to make this synth more powerful. If 
anyone is interested in helping to add these features please get in touch.

### Goals of this Project

* To help people like myself get into DSP and program Audio Apps through practical experimentation
* To create a somewhat working and useable audio app even if the program conventions aren't always the greatest.
* To help people who are coming from beginner into intermediate with the C language experiment in a field of interest.
* To aim for compatability on Linux and MAC OS

### Why C?

Fundamentally, I'm using C here for performance. I have tried to build similar apps in python and 
there is no way of effectively working with anything realtime.

I chose to use C and the librarys I have added in the credits section because it is the language 
that is first taught on the course. I found that C keeps things really simple and the syntax is 
relatively simple across the board once you know it. It is also the native language of the PortMedia
APIs which made integrating my knowledge of C with Third Party Librarys even easier.

If you want to contribute to this project and you know the C language or more than I do about DSP then
please feel free to get in touch with me.

I'm also open to porting this project into either rust or C++ if there is anyone who can enlighten 
me on how this would make the development easier. 

## Usage

### Dependencies

This project depends on PortMidi, PortAudio and GTK3. You should be able to install these by following the
links in the Credits section.

### Linux 

Open up a terminal of your choice and run these commands.     
      
`git clone https://github.com/edwardgallyot/CSynth50.git`  
` cd CSynth50 `    
` make `    
`./main`     


Once the repo is cloned on your system the make command should be enough on a Linux System. In the Mac OS Catalina
branch there is some slightly changed source code and a different makefile reflected portaudios dependencies
on Mac OS.

## Known Issues

### Jump Discontinuitys

There seems to be some jump discontinuitys when changing frequencys. You can probably see in the oscillator
function that I have tried to ommit these jump discontinuitys by fading down and ramping back up to switch
frequencys over a period of about 1ms or more accurately half the buffer size.

If anyone has a better way of adressing jump discontinuitys I'd be open to any suggestions.

### The sin() function

Use of the sin() function is heavy on the cpu and unbounded in a realtime call back. I know this isn't ideal
but my research on algorithms was inconclusive.

### Glitching on Mac OS 

The GUI is currently very glitchy on catalina. I think this is something to do with GTK3 being installed via 
brew on my laptop.

#### Global Variables

I know the use of global variables is not ideal and at some point I know that it would be a good idea to pass pointers
around as opposed to using globals.

## Credits

### The PortAudio Library

The Port Audio Library and its documentation has made this final project possible
and (more importantly) portable. There is a branch that I have created to support Mac OS Catalina
on my laptop and the library transfered over from Linux with ease.

You can find the PortAudio Library along with it's great docs [here](http://www.portaudio.com/)

### The PortMidi Library

The portmidi library I believe is part of portmedia as is portaudio and has examples in on their
git hub and although it takes some time to get comfortable with the way the port time callback works, 
I have some projects that I can share with anyone who needs a simpler way of testing portmidi.

The PortMedia API information can be found [here](http://portmedia.sourceforge.net/)

### The GTK Library

The GTK Library was very quick and easy to implement with Glade as I could use the GTK builder
to link functions in C and CSS styles to the functionality of widgets. The reason I chose GTK was
because it can be implemented in C without moving over to C++. This project makes use of GTK3 and 
should be portable to any system that supports it.

More Info on GTK and Glade can be found [here](https://www.gtk.org/)

### Great Online Resources

Although it is impossible to credit a million stack overflow posts here are a couple of great 
online resources that deserve huge credit.      

[Here is a great resource for learning glade](https://prognotes.net/gtk-glade-c-programming/)    

[Here is a great resource for the basics of my oscillator functions](https://www.youtube.com/watch?v=tgamhuQnOkM)     



