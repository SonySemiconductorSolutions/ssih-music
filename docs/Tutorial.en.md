# Sound Signal Processing Library for Spresense (2022-11-25 Edition)

The Sound Signal Processing Library for Spresense (called ssprocLib) is a library for easy instrument development in [Spresense](https://developer.sony.com/develop/spresense/).
It provides an API based on the MIDI standard that defines performance data for electronic instruments, and a software module that easily handles Spresense audio input and output.

You need sound source data to try out ssprocLib.
Download the sound source data [here](https://github.com/SonySemiconductorSolutions/ssih-music/releases/latest/download/assets.zip).

## What's in the tutorial

* Setting up a development environment
* Hardware setup
* Installing instrument software
* Sample instrument software
* Prior knowledge of electronic instruments
* Instrument software modules
* Make sound using SDSink
* Use the Src module to handle complex inputs
* Use SFZSink to control how a sound source plays

## Setting up the development environment

Instrument software is developed in the Arduino IDE.
Follow these steps to install the Arduino IDE, the Spresense Arduino board package, and the Sound Signal Processing Library for Spresense.

1. Using the instructions in the [Spresense Arduino Library Getting Started Guide](https://developer.sony.com/develop/spresense/docs/arduino_set_up_en.html), install the Arduino IDE and the Spresense Arduino board package.
2. Install the Sound Signal Processing Library for Spresense in the Arduino IDE.
    1. Download the Source code (zip) from the [Release Page](https://github.com/SonySemiconductorSolutions/ssih-music/releases/).
    2. Open the Arduino IDE and click "Sketch" > "Include Library" > "Install .ZIP Library".
    3. When the dialog opens, select ssih-music.zip, which you downloaded above, and click "Open".

## Hardware setup

ssprocLib provides examples of instrument software with Spresense, including instruments played with buttons and instruments played with microphones.
The hardware required to run the sample and its connection instructions are listed below.

* Hardware to use
    * [Spresense Mainboard](https://developer.sony.com/develop/spresense/specifications)
    * [Spresense Expansion Board](https://developer.sony.com/develop/spresense/specifications)
    * [Mic&LCD KIT for SPRESENSE](https://github.com/autolab-fujiwaratakayoshi/MIC-LCD_kit_for_SPRESENSE)
        * 4 microphones
        * Microphone substrate
        * LCD substrate with four switches
    * microSD card
        * It uses a microSDHC card formatted with FAT 32. It is recommended to format the card with the [SD Memory Card Formatter](https://www.sdcard.org/downloads/formatter/) when using it for the first time.
    * Microphone (connected by mini plug (3.5 mm))
        * The microphone comes with the Mic & LCD KIT for SPRESENSE.
    * Speakers (connected by mini plugs (3.5 mm))
        * Connect to the Headphone Connector on the Spresense expansion board.
    * USB cable (micro-B)
* Connecting Hardware
    1. Connect the Spresense main board to the Spresense expansion board.
    2. Connect the Mic & LCD KIT for SPRESENSE microphone board and the LCD board with four switches to the Spresense expansion board.
    3. Connect the microphone to the MJA jack on the microphone board.
    4. Connect the speakers to the Headphone Connector on the Spresense expansion board.
        * ![Picture After Connection](/docs/UNION.png)

## Install instrument software

After connecting the hardware, write the sound source files and instrument software.
Follow these steps to write a SimpleHorn that sounds the same pitch as your voice.

1. Write sound source files to a microSD card.
    1. Insert a microSD card into your PC.
    2. Download [assets.zip](https://github.com/SonySemiconductorSolutions/ssih-music/releases/latest/download/assets.zip) from the [Release Page](https://github.com/SonySemiconductorSolutions/ssih-music/releases/) and copy the extracted files to a microSD card.
    3. Insert the microSD card into the microSDHC slot on the Spresense expansion board.
2. Write a common sketch.
    1. Open the Arduino IDE and click "File" > "Sample Sketch" > "Sound Signal Processing Library for Spresense" > "YuruHorn_SubCore1".
    2. Click "Tools" > "Board" > "Spresense Board" > "Spresense".
    3. Click "Tools" > "Core" > "SubCore1".
    4. In "Tools" > "Serial Port", click on the COM port where Spresense is connected.
    5. Click "Sketch" > "Write to microcomputer board".
    6. Wait until it says, "Writing to board complete".
        * If it fails, try again from "Installing the Spresense Bootloader" in the [Spresense Arduino Library Getting Started Guide](https://developer.sony.com/develop/spresense/docs/arduino_set_up_en.html).
3. Write DSP binaries to SD card.
    1. Open the Arduino IDE and click "File" > "Sample Sketch" > "Audio" > "dsp_installer" > "src_installer".
    2. Click "Tools" > "Board" > "Spresense Board" > "Spresense".
    3. Choose "Tools" > "Core" > "MainCore".
    4. In "Tools" > "Serial Port", click on the COM port where Spresense is connected.
    5. Click "Sketch" > "Write to microcomputer board".
    6. Wait until it says, "Writing to board complete".
        * If it fails, try again from "Installing the Spresense Bootloader" in the [Spresense Arduino Library Getting Started Guide](https://developer.sony.com/develop/spresense/docs/arduino_set_up_en.html).
    7. Click "Tools" > "Serial Monitor" to start the serial monitor.
    8. When asked, "Select where to install SRC?" to install it on the SD card, type in "1" and click "Send".
    9. When "Finished" appears, it's over.
4. Write instrument software.
    1. Open the Arduino IDE and click "File" > "Sample Sketch" > "Sound Signal Processing Library for Spresense" > "SimpleHorn".
    2. Click "Tools" > "Board" > "Spresense Board" > "Spresense".
    3. Click "Tools" > "Core" > "MainCore".
    4. In "Tools" > "Serial Port", click on the COM port where Spresense is connected.
    5. Click "Sketch" > "Write to microcomputer board".
    6. Wait until it says, "Writing to board complete".
        * If it fails, try again from "Installing the Spresense Bootloader" in the [Spresense Arduino Library Getting Started Guide](https://developer.sony.com/develop/spresense/docs/arduino_set_up_en.html).
5. Play SimpleHorn.
    * When you hum into the microphone, the instrument sounds according to the pitch of your voice.

## Sample instrument software

ssprocLib provides several instrument software samples.

* a musical instrument played with buttons
    * ButtonDrum
        * This instrument makes a corresponding sound when a button is pressed. Use button 4 ~ 6 on the LCD board.
            * ![LCD KIT Board Image](/docs/Sample_LCD.png)
            * Button Behavior
                * Button 4: Play Sound Source 1
                * Button 5: Play Sound Source 2
                * Button 6: Switch the sound source
                * Button 7: Don't use
    * OneKeySynth
        * It is a musical instrument that plays according to the score file when you press the button. Use button 4 ~ 5 on the LCD board.
            * Button Behavior
                * Button 4: Play the sound source according to the score file
                * Button 5: Same as Button 4
                * Button 6: Don't use
                * Button 7: Don't use
* a musical instrument played with a microphone
    * SimpleHorn
        * It is an instrument that produces a sound as high as the humming of the performer. It uses a microphone and a microphone board.
    * YuruHorn
        * It is an instrument that produces a sound as high as the humming of the performer. It uses a microphone and a microphone board.
        * You can control the playback of sound source files using the [SFZ Format](http://sfzformat.com/legacy/).
        * For example, looped playback allows you to play longer than an instrument file.

## Prior knowledge of electronic instruments

Electronic instruments exchange performance data as specified in the [MIDI standard](https://midi.org/).
The performance data is called a MIDI message.

Typical MIDI messages include *Note On* and *Note Off*.
Note On is a "play note sound" MIDI message and Note Off is a "stop note sound" MIDI message.
Each MIDI message has parameters called note number and velocity.

Note numbers are 0 ~ 127 numbers that indicate the type of sound.
On a musical instrument with a scale, the note number represents the pitch of the note.
The lowest note is 0 (27.5 Hz) and the highest is 127 (12543.9 Hz).
For example, the note number for C (261.6 Hz) in the middle of the piano is 60.
On instruments without scales, the note number represents the type of sound (Bass drums, snare drums, cymbals, etc.).

Velocity is a value that represents the intensity of a sound and takes a value of 0 ~ 127.

![Relationship Between Scale and Note Numbers](/docs/NoteNumber.png)

## Instrument software modules

Instruments in ssprocLib consist of two types of modules: Source (abbreviated as Src), which receives sensor inputs such as buttons and microphones and transmits performance data, and Sink, which receives performance data and produces sound.
You can put any number of filters between Src and Sink that will process your performance data as needed.

![Instrument Software Module](/docs/DataFlow-framework.png)

ssprocLib provides several pre-implemented modules.

* Implemented Src modules:
    * `YuruhornSrc`: Analyzes the sound input to the microphone and sends performance data that produces the same pitch.
    * `ScoreSrc`: Sends performance data based on a score file on an SD card.
* Implemented Sink modules:
    * `SDSink`: Play an instrument file according to the instrument table set by the user.
    * `SFZSink`: Play an instrument file according to the instrument definition file (SFZ file).
        * Supports more advanced performance by specifying playback conditions and controls. For more information, see "Use SFZSink to control how a sound source plays" below.
* Implemented Filter module:
    * `OctaveShift`: Increases or decreases the note number of the performance data in octaves.
    * `OneKeySynthesizerFilter`: Replaces the note number in the performance data with the note number in the music file.

The Src and Sink modules provide the following functions:
These functions exchange performance data.

| Functions              | Features                                                                                                    |
| ---                    | ---                                                                                                         |
| `begin` function       | This function prepares the instrument for playing. Run it from the Arduino `setup`.                         |
| `update`               | Play an instrument by calling this function periodically. You can do this from the Ardiuno `loop` function. |
| `sendNoteOn` function  | Send note on to start playing sound.                                                                        |
| `sendNoteOff` function | Sends a note-off to stop sound playback.                                                                    |

The sendNoteOn and sendNoteOff functions take the following arguments:

| Arguments  | Description    | Range in which settings can be made |
| ---        | ---            | ---                                 |
| `note`     | Note number    | 0~127                               |
| `velocity` | Velocity       | 0~127                               |
| `channel`  | Channel number | 0 ~ 15                              |

## Make sound using SDSink

Take a look at the sample sketch ButtonDrum to see how it sounds.

![Module Configuration for ButtonDrum](/docs/DataFlow-ButtonDrum.png)

1. Register the sound source file in the `SDSink`.
    * Place the sound source files on the SD card.
    * Make a correspondence table between note numbers and sound source files and register them on SDSink.
    * Sample sketches
        ```ButtonDrum.ino
        // 1. Define a correspondence table between note numbers and instrument files
        const SDSink::Item table[12] = {
            {60, "SawLpf/60_C4.wav"},
            {61, "SawLpf/61_C#4.wav"},
            {62, "SawLpf/62_D4.wav"},
            {63, "SawLpf/63_D#4.wav"},
            {64, "SawLpf/64_E4.wav"},
            {65, "SawLpf/65_F4.wav"},
            {66, "SawLpf/66_F#4.wav"},
            {67, "SawLpf/67_G4.wav"},
            {68, "SawLpf/68_G#4.wav"},
            {69, "SawLpf/69_A4.wav"},
            {70, "SawLpf/70_A#4.wav"},
            {71, "SawLpf/71_B4.wav"}
        };
        
        // 2. Register a correspondence table with SDSink
        SDSink inst(table, 12);
        ```
    * In this example, note number 60 is associated with "SawLpf/60_C4.wav", and note number 62 is associated with "SawLpf/62_D4.wav", for a total of 12 element tables.
2. Run the sample instrument ButtonDrum.
    * Run the `begin` function with the `setup` function.
    * Run the `update` function with the `loop` function.
    * Write a process in the `loop` that executes the `sendNoteOn` function when the button is pressed and the `sendNoteOff` function when it is released.
    * Sample sketches
        ```ButtonDrum.ino
        void setup() {
            inst.begin();
        }
        
        int selector = 0;
        int note = 60 + (selector * 2);

        void loop() {
            //Button4
            int button4_input = digitalRead(PIN_D04);
            if (button4_input != button4) {
                if (button4_input == LOW) {
                    //When the button is pressed, select the sound to be played and play it
                    note = 60 + (selector * 2);
                    inst.sendNoteOn(note, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
                } else {
                    //When the button is released, stop the ringing sound
                    inst.sendNoteOff(note, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
                }
                button4 = button4_input;
            }
            //Button5 (omitted)
            //Button6 (omitted)
            
            inst.update();
        }
        ```

## Use the Src module to handle complex inputs

Take a look at the sample sketch SimpleHorn to see how to use the Src module.

![SimpleHorn Module Configuration](/docs/DataFlow-SimpleHorn.png)

The ButtonDrum was an instrument that was played by button input, so I could create an instrument by writing a simple sketch.
But how do you make an instrument that plays with voice input?
Let's use ssprocLib, which provides a module called `YuruhornSrc` that converts pitch into performance data.

1. Connect `YuruhornSrc` and `SDSink`.
    * Sample sketches
        ```SimpleHorn.ino
        // 1. Define a correspondence table between note numbers and instrument files
        const SDSink::Item table[25] = {
            {48, "SawLpf/48_C3.wav"}, {49, "SawLpf/49_C#3.wav"},
            {50, "SawLpf/50_D3.wav"}, {51, "SawLpf/51_D#3.wav"},
            {52, "SawLpf/52_E3.wav"},
            {53, "SawLpf/53_F3.wav"}, {54, "SawLpf/54_F#3.wav"},
            {55, "SawLpf/55_G3.wav"}, {56, "SawLpf/56_G#3.wav"},
            {57, "SawLpf/57_A3.wav"}, {58, "SawLpf/58_A#3.wav"},
            {59, "SawLpf/59_B3.wav"},
            {60, "SawLpf/60_C4.wav"}, {61, "SawLpf/61_C#4.wav"},
            {62, "SawLpf/62_D4.wav"}, {63, "SawLpf/63_D#4.wav"},
            {64, "SawLpf/64_E4.wav"},
            {65, "SawLpf/65_F4.wav"}, {66, "SawLpf/66_F#4.wav"},
            {67, "SawLpf/67_G4.wav"}, {68, "SawLpf/68_G#4.wav"},
            {69, "SawLpf/69_A4.wav"}, {70, "SawLpf/70_A#4.wav"},
            {71, "SawLpf/71_B4.wav"},
            {72, "SawLpf/72_C5.wav"}
        };

        // 2. Register a correspondence table with SDSink
        SDSink sink(table, 25);

        // 3. Register SDSink as next module for YuruhornSrc
        YuruhornSrc inst(sink);
        ```
2. Run the sample instrument SimpleHorn.
    * Run the `begin` function of `YuruhornSrc` with the `setup` function.
    * Run the `update` function of `YuruhornSrc` with the `loop` function.
    * Sample sketches
        ```SimpleHorn.ino
        void setup() {
            inst.begin();
        }
        
        void loop() {
            inst.update();
        }
        ```

`YuruhornSrc` converts the input voice into performance data, and based on that performance data, `SDSink` plays the sound.
In general, you need to write a large program to analyze speech, but ssprocLib can handle speech with this much programming.

An instrument using the `SDSink` will play back the sound source file as it is, so if there is silence in the sound source file, the sound of the instrument will be interrupted.
To prevent this, you can fix it by setting up a loop point with an SFZ file and a `SFZSink`.
For more information about `SFZSink`, see Using SFZSink to control how a sound source plays.

## Use SFZSink to control how a sound source plays

Let's look at a sample sketch called YuruHorn that deals with SFZ files.
The YuruHorn is an instrument that, like the SimpleHorn, is played by voice input, but instead of using the `SDSink`, you use the `SFZSink`.

![YuruHorn module configuration](/docs/DataFlow-YuruHorn.png)

`SFZSink` is a Sink module that plays a sound source according to an SFZ file.
The SFZ file is a text file that describes the correspondence between the note number and the sound source file, along with additional information about the sound source file, such as its valid range and playback method.
Let's take a look at how to write SFZ files simply.

1. Describe the correspondence between note numbers and sound source files.
    * Describe the correspondence table using the keywords `<region>`, `key` and `sample`.
    * Sample (excerpt)
        ```SawLpf.sfz
        <region> key=48 sample=SawLpf/48_C3.wav
        <region> key=49 sample=SawLpf/49_C#3.wav
        <region> key=50 sample=SawLpf/50_D3.wav
        ```
        | Keywords                         | Classification | Description                                                           |
        | ---                              | ---            | ---                                                                   |
        | `<region>`                       | Header         | Define a single sound. It is one element of the correspondence table. |
        | `key`={Note number or note name} | Opcode         | The note number corresponding to the sound.                           |
        | `sample`={file name}             | Opcode         | Sound source file name corresponding to the sound.                    |
    * In this example, note number 48 corresponds to "SawLpf/48_C3.wav" and note number 50 corresponds to "SawLpf/50_D3.wav".
2. Describe how to play back sound source files.
    * Specify the keywords `loop_mode=loop_continuous`, `loop_start` and `loop_end` to loop the sound source file.
    * Sample (excerpt)
        ```SawLpf.sfz
        <region> key=48 sample=SawLpf/48_C3.wav  loop_mode=loop_continuous loop_start=48000 loop_end=143994
        <region> key=49 sample=SawLpf/49_C#3.wav loop_mode=loop_continuous loop_start=48127 loop_end=144036
        <region> key=50 sample=SawLpf/50_D3.wav  loop_mode=loop_continuous loop_start=47938 loop_end=144137
        ```
        | Keywords                     | Classification | Description                                                                 |
        | ---                          | ---            | ---                                                                         |
        | `offset`={audio samples}     | Opcode         | Specifies the beginning of the valid range of the instrument file.          |
        | `end`={audio samples}        | Opcode         | The end of the valid range of the instrument file.                          |
        | `loop_start`={audio samples} | Opcode         | Start point of the loop point.                                              |
        | `loop_end`={audio samples}   | Opcode         | The end point of the loop point.                                            |
        | `loop_mode=no_loop`          | Opcode         | Does not loop. This is the initial value of `loop_mode`.                    |
        | `loop_mode=one_shot`         | Opcode         | Play one shot. Play and stop instructions are not accepted during playback. |
        | `loop_mode=loop_continuous`  | Opcode         | Enable looping playback. Repeatedly plays from `loop_start` to `loop_end`.  |
    * If you do not specify an `offset` and an `end`, the valid range is the entire instrument file.
    * If you do not specify `loop_start` and `loop_end`, the loop point is the entire instrument file.
    * The audio samples that you specify for `end` and `loop_end` are included in the playback. For example, if you specify `offset=100 end=100`, 1 audio sample is played.
    * Some of the other Opcodes (parameters) listed above are also supported. To learn more about SFZ, see [SFZ Format](https://sfzformat.com/).
3. Run the sample instrument YuruHorn.
    * Sample sketches
        ```YuruHorn.ino
        SFZSink sink("yuruhorn.sfz");   //name a file for SFZSink instead of a corresponding table for SDSink
        OctaveShift filter(sink);       //Register SFZSink as the next module of OctaveShift
        YuruhornSrc inst(filter);       //Register OctaveShift as the next module in YuruhornSrc

        void setup() {
            inst.begin();
        }

        void loop() {
            inst.update();
        }
        ```

Now you have an instrument that can play your instrument files better.

## License

The Software is provided under Apache License 2.0 or GNU Lesser General Public License v 2.1 or later.
See LICENSE for more information.

## Copyright

Copyright 2022 Sony Semiconductor Solutions Corporation
