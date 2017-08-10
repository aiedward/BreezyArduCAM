/*
BreezyArduCAM.h : class declarations for BreezyArduCAM libary

Copyright (C) Simon D. Levy 2017

This file is part of BreezyArduCAM.

BreezyArduCAM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BreezyArduCAM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with BreezyArduCAM.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BreezyArduCAM_H
#define BreezyArduCAM_H

#include <Arduino.h>

#if defined (__AVR__)
#define regtype volatile uint8_t
#define regsize uint8_t
#endif

#if defined(__SAM3X8E__)
#define regtype volatile uint32_t
#define regsize uint32_t
#endif


// a structure for sensor register initialization values
struct sensor_reg {
    uint16_t reg;
    uint16_t val;
};

/**
 * An abstract class for the ArduCAM Mini 2MP.  
 */
class ArduCAM_Mini_2MP
{
    friend class FrameGrabber;

    public:

        /**
         * Constructs an ArduCAM_Mini_2MP object.
         * @param cs pin for Chip Select signal
         */
        ArduCAM_Mini_2MP(int cs);

        /**
         * Begins capture in QVGA (raw) mode.
         * @param scaledown logarithm of 2 by which to scale down image
         * @param grayscale flag for grayscale conversion
         */
        void beginQvga(uint8_t scaledown=0, bool grayscale=false);

        /**
         * Begins 160x120 JPEG capture.
         */
        void beginJpeg160x120(void);

        /**
         * Begins 176x144 JPEG capture.
         */
        void beginJpeg176x144(void);
        /**
         * Begins 320x240 JPEG capture.
         */
        void beginJpeg320x240(void);

        /**
         * Begins 352x288 JPEG capture.
         */
        void beginJpeg352x288(void);

        /**
         * Begins 640x480 JPEG capture.
         */
        void beginJpeg640x480(void);

        /**
         * Begins 800x600 JPEG capture.
         */
        void beginJpeg800x600(void);

        /**
         * Begins 1024x768 JPEG capture.
         */
        void beginJpeg1024x768(void);

        /**
         * Begins 1200x1024 JPEG capture.
         */
        void beginJpeg1280x1024(void);

        /**
         * Begins 1600x1200 JPEG capture.
         */
        void beginJpeg1600x1200(void);

        /**
         * Runs in Arduino loop() function to capture bytes.
         * Calls gotStartRequest(), gotStopRequest(), and sendByte() methods
         * of implementing class.
         */
        void capture(void);

    protected:

        /**
         * Override this method in your implementing class.
         * @return true when you're ready to begin capture, false otherwise
         */
        virtual bool gotStartRequest(void) = 0;

        /**
         * Override this method in your implementing class.
         * @return true when you're ready to stop capture, false otherwise
         */
        virtual bool gotStopRequest(void) = 0;

        /**
         * Override this method in your implementing class.
         * @param b byte to send to host or other consumer
         */
         virtual void sendByte(uint8_t b) = 0;

    private:

        void csHigh(void);
        void csLow(void);

        void grabJpegFrame(uint32_t length);
        void grabQvgaFrame(uint32_t length);

        void flush_fifo(void);
        void start_capture(void);
        void clear_fifo_flag(void);
        uint8_t read_fifo(void);

        uint8_t read_reg(uint8_t addr);
        void write_reg(uint8_t addr, uint8_t data);	

        uint32_t read_fifo_length(void);
        void set_fifo_burst(void);

        void set_bit(uint8_t addr, uint8_t bit);
        void clear_bit(uint8_t addr, uint8_t bit);
        uint8_t get_bit(uint8_t addr, uint8_t bit);

        uint8_t bus_write(int address, int value);
        uint8_t bus_read(int address);	

        int wrSensorRegs8_8(const struct sensor_reg*);
        byte wrSensorReg8_8(int regID, int regDat);
        byte rdSensorReg8_8(uint8_t regID, uint8_t* regDat);

        void begin();

        void beginJpeg(const struct sensor_reg reglist[]);

        regtype *P_CS;
        regsize B_CS;
        byte sensor_addr;

        uint8_t scaledown;
        bool grayscale;
        bool usingJpeg;
        bool capturing;
        bool starting;
};

/**
 * A class for communicating with the ArduCAM Mini 2MP over a serial connection.  
 */
class Serial_ArduCAM_Mini_2MP : public ArduCAM_Mini_2MP {

    public:

        /**
         * Constructs a Serial_ArduCAM_Mini_2MP object.
         * @param cs pin for Chip Select signal
         */
        Serial_ArduCAM_Mini_2MP(uint8_t cs) : ArduCAM_Mini_2MP(cs) { }

    protected:

        /**
         * Implements the gotStartRequest() method by checking for a nonzero byte from the host computer.
         */
        virtual bool gotStartRequest(void) override 
        {
            return (Serial.available() && Serial.read());
        }

        /**
         * Implements the gotStopRequest() method by checking for a zero byte from the host computer.
         */
        virtual bool gotStopRequest(void) override 
        {
            return (Serial.available() && !Serial.read());
        }

        /**
         * Implements the sencByte() method by sending the byte to the host computer.
         * @param b the byte to send
         */
        virtual void sendByte(uint8_t b) override 
        {
            Serial.write(b);
        }
};

#include "ov2640_regs.h"

#endif