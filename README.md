# Radio Controller
A simple project that uses HC-12 100mW multi-channel wireless transceiver. The idea is to transmit control signals to the UAV board and receive data signals from the UAV.
Due to Arduino IDE specifics files from core must be copied/overwriten into controller directory and recipent directory.
Controller directory contains the transmitter source code.
The controller project may be uploaded onto Arduino Pro Mini board.

Recipent directory contains the receiver source code.
Recipent project may be uploaded onto DFRobot Beetle Board. This board is chosen 
for the reason of size and weight, it's one of the smallest arduino boards out there in the market at the present.


# Requirements
Arduino Pro Mini, DFRobot Beetle Board, Arduino IDE

# License
Copyright 2021 Arturs Kalnins

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.