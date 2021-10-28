# Description
An old project for performing morsing with a LED.
Made for raspberry.

# Dependencies
* WiringPi
* cmake
* gcc

# Sender
The sender uses a LED with a dropping resistor and sends morse code visually. The GPIO ping can be defined during the call.
```
/3_send_threaded/mcsend -h
Usage: mcsend [OPTION] [STRING]
Send STRING (if given) or stdin as morse.
	valid options are:
		-h	display this help and exit
		-s WPM	 speed in words per minute (1 WpM = 1200ms per dit) (Default = 8 WpM)
		-p PIN	GPIO pin to use (Default = 1 (GPIO18))
```

# Receiver
The receiver uses a light-dependent resistor together with a voltage divider for decoding a visual morse signal.

## Important
The code understands a voltage of 0V as HIGH and a voltage of 3.3V as LOW.
* LOW = No Light
* HIGH = Signal
Make sure that your voltage divider fullfils these requirements.

The trigger level (the border between HIGH and LOW) can be adjusted with a parameter.
```
/4_receive_threaded/mcrecv -h
Usage: mcrecv [OPTION]
Receive and decode morse code. Signals are received by a MCP3008 ADC.
The program waits for the start of the transmission and stops receiving after 49 dits of silence.
	valid options are:
		-h	display this help and exit
		-t TRIGGER	 ADC trigger level (above = 0) (below = 1) (default = 600) (range: 0 - 1023)
```

# Building
1. `cd 3_send_threaded` or `cd 4_receive_threaded`
2. cmake .
3. make


# License
GPLv3
Copyright Hedius 2021 git@hedius.eu

