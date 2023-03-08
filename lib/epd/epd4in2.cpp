/**
 *  @filename   :   epd4in2.cpp
 *  @brief      :   Implements for Dual-color e-paper library
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 10 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include "epd4in2.h"
#include "epdif.h" // for PIN definitions

Epd::~Epd() {
};

Epd::Epd() {
};

void Epd::Init(void) {
    /* this calls the peripheral hardware interface, see epdif */
    IfInit();   /* EPD hardware init start */
    Reset();
    SendCommand(POWER_SETTING);
    SendData(0x03);                  // VDS_EN, VDG_EN
    SendData(0x00);                  // VCOM_HV, VGHL_LV[1], VGHL_LV[0]
    SendData(0x2b);                  // VDH
    SendData(0x2b);                  // VDL

    SendCommand(BOOSTER_SOFT_START);
    SendData(0x17);
    SendData(0x17);
    SendData(0x17);                  //07 0f 17 1f 27 2F 37 2f
    SendCommand(POWER_ON);
    WaitUntilIdle();
    SendCommand(PANEL_SETTING);
    SendData(0xbf);    // KW-BF   KWR-AF  BWROTP 0f

    SendCommand(PLL_CONTROL);
    SendData(0x3c);        // 3A 100HZ   29 150Hz 39 200HZ  31 171HZ

    SendCommand(RESOLUTION_SETTING);
    SendData(0x01);
    SendData(0x90); //128
    SendData(0x01); //
    SendData(0x2c);

    SendCommand(VCM_DC_SETTING);
    SendData(0x12);

    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendData(0x97); // 97white border 77black border    VBDF 17|D7 VBDW 97 VBDB 57    VBDF F7 VBDW 77 VBDB 37  VBDR B7

    SetLut();
    /* EPD hardware init end */
}

void Epd::InitPartial(void)
{
    /* this calls the peripheral hardware interface, see epdif */
    // in the Raspberry Pi example it only calls this once, and then inits the rest repeatedly
    IfInit();
    /* EPD hardware init start */
    Reset();

    SendCommand(POWER_SETTING);
    SendData(0x03);
    SendData(0x00);
    SendData(0x2b);
    SendData(0x2b);

    SendCommand(BOOSTER_SOFT_START);
    SendData(0x17);		//A
    SendData(0x17);		//B
    SendData(0x17);		//C

    SendCommand(POWER_ON);
    WaitUntilIdle();

    SendCommand(PANEL_SETTING); // panel setting
    SendData(0xbf); // KW-BF   KWR-AF	BWROTP 0f	BWOTP 1f

    SendCommand(PLL_CONTROL);
    SendData(0x3C); // 3A 100HZ   29 150Hz 39 200HZ	31 171HZ

    SendCommand(RESOLUTION_SETTING);
    SendData(0x01);
    SendData(0x90);
    SendData(0x01);
    SendData(0x2c);

    SendCommand(VCM_DC_SETTING);
    SendData(0x12);

    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendData(0x07); // 97white border 77black border		VBDF 17|D7 VBDW 97 VBDB 57		VBDF F7 VBDW 77 VBDB 37  VBDR B7

    SetPartialLut();
}

/**
 *  @brief: basic function for sending commands
 */
void Epd::SendCommand(unsigned char command) {
    DigitalWrite(DC_PIN, LOW);
    SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void Epd::SendData(unsigned char data) {
    DigitalWrite(DC_PIN, HIGH);
    SpiTransfer(data);
}

/**
 *  @brief: Wait until the BUSY_PIN goes HIGH
 */
void Epd::WaitUntilIdle(void) {
    while(DigitalRead(BUSY_PIN) == 0) {      //0: busy, 1: idle
        // Wait
    }
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void Epd::Reset(void) {
    DigitalWrite(RST_PIN, LOW);
    DelayMs(2);
    DigitalWrite(RST_PIN, HIGH);
    DelayMs(20);
    DigitalWrite(RST_PIN, LOW);
    DelayMs(2);
    DigitalWrite(RST_PIN, HIGH);
    DelayMs(20);
    DigitalWrite(RST_PIN, LOW);
    DelayMs(2);
    DigitalWrite(RST_PIN, HIGH);
    DelayMs(20);
}

/**
 *
 */
void Epd::SetPartialWindow(const unsigned char* frame_buffer, int x, int y, int w, int l) {
  SetPartialWindow(frame_buffer, x, y, w, l, false);
}

/**
 *
 */
void Epd::FillPartialWindowBlack(int x, int y, int w, int l) {
  SetPartialWindow(NULL, x, y, w, l, false);
}

/**
 *
 */
void Epd::FillPartialWindowWhite(int x, int y, int w, int l) {
  SetPartialWindow(NULL, x, y, w, l, true);
}

 
/**
 *  @brief: transmit partial data to the SRAM
 */
void Epd::SetPartialWindow(const unsigned char* buffer_black, int x, int y, int w, int l, bool erase) {
    SendCommand(PARTIAL_IN);
    SendCommand(PARTIAL_WINDOW);
    SendData(x >> 8);
    SendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    SendData(((x & 0xf8) + w  - 1) >> 8);
    SendData(((x & 0xf8) + w  - 1) | 0x07);
    SendData(y >> 8);
    SendData(y & 0xff);
    SendData((y + l - 1) >> 8);
    SendData((y + l - 1) & 0xff);
    SendData(0x28); // took this from RPi. was 0x01

    DelayMs(2);


    SendCommand(DATA_START_TRANSMISSION_1);
    for(int i = 0; i < w  / 8 * l; i++) {
      SendData(0xFF);
    }
 
    SendCommand(DATA_START_TRANSMISSION_2);
    if (buffer_black != NULL) {
      for(int i = 0; i < w  / 8 * l; i++) {
        SendData(buffer_black[i]);
      }
    } else {
      for(int i = 0; i < w  / 8 * l; i++) {
        if (!erase)
          SendData(0x00);
        else
          SendData(0xFF);
      }
    }

    DelayMs(2);
    SendCommand(PARTIAL_OUT);
}

/**
 *  @brief: set the look-up table for "Full" draws
 */
void Epd::SetLut(void) {
    unsigned int count;
    SendCommand(LUT_FOR_VCOM);
    for(count = 0; count < 18; count++) {
        SendData(lut_vcom[count]);
    }
    SendCommand(LUT_WHITE_TO_WHITE);
    for(count = 0; count < 18; count++) {
        SendData(lut_ww[count]);
    }
    SendCommand(LUT_BLACK_TO_WHITE);
    for(count = 0; count < 18; count++) {
        SendData(lut_bw[count]);
    }
    SendCommand(LUT_WHITE_TO_BLACK);
    for(count = 0; count < 18; count++) {
        SendData(lut_bb[count]);
    }
    SendCommand(LUT_BLACK_TO_BLACK);
    for(count = 0; count < 18; count++) {
        SendData(lut_wb[count]);
    }
}

/**
 *  @brief: set the look-up table for partial drawing
 */
void Epd::SetPartialLut(void) {
    unsigned int count;
    SendCommand(LUT_FOR_VCOM);
    for(count = 0; count < 6; count++) {
        SendData(partial_lut_vcom[count]);
    }
    SendCommand(LUT_WHITE_TO_WHITE);
    for(count = 0; count < 6; count++) {
        SendData(partial_lut_ww[count]);
    }
    SendCommand(LUT_BLACK_TO_WHITE);
    for(count = 0; count < 6; count++) {
        SendData(partial_lut_bw[count]);
    }
    SendCommand(LUT_WHITE_TO_BLACK);
    for(count = 0; count < 6; count++) {
        SendData(partial_lut_bb[count]);
    }
    SendCommand(LUT_BLACK_TO_BLACK);
    for(count = 0; count < 6; count++) {
        SendData(partial_lut_wb[count]);
    }
}

/**
 * @brief: This displays the frame data from SRAM, with a full on-off cycle.
 */
void Epd::DisplayFrame(void) {
    SetLut();
    SendCommand(DISPLAY_REFRESH);
    DelayMs(100);
    WaitUntilIdle();
}

/**
 * @brief: Draw frame_buffer, then displays the frame with full on-off cycle.
 */
void Epd::DisplayFullScreen(const unsigned char* frame_buffer) {
    SendCommand(RESOLUTION_SETTING);
    SendData(width >> 8);
    SendData(width & 0xff);
    SendData(height >> 8);
    SendData(height & 0xff);

    SendCommand(VCM_DC_SETTING);
    SendData(0x12);

    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendCommand(0x97);    //VBDF 17|D7 VBDW 97  VBDB 57  VBDF F7  VBDW 77  VBDB 37  VBDR B7

    if (frame_buffer != NULL) {
        SendCommand(DATA_START_TRANSMISSION_1);
        for(unsigned int i = 0; i < width / 8 * height; i++) {
            SendData(0xFF);      // bit set: white, bit reset: black
        }
        DelayMs(2);
        SendCommand(DATA_START_TRANSMISSION_2);
        for(unsigned int i = 0; i < width / 8 * height; i++) {
            SendData(pgm_read_byte(&frame_buffer[i]));
        }
        DelayMs(2);
    }

    SendCommand(DISPLAY_REFRESH);
    DelayMs(100);
    WaitUntilIdle();
}

/**
 * @brief: This displays the frame from SRAM immediately, without a full on-off cycle.
 * WARNING, using this too many times may cause burn in.
*/
void Epd::QuickDisplayFrame(void) {
  SetPartialLut();
  SendCommand(DISPLAY_REFRESH);
  DelayMs(10);
  // maybe get rid of this..
  WaitUntilIdle();
}

/**
 * @brief: clear the frame data from the SRAM, this won't refresh the display
 */
void Epd::ClearFrame(void) {
    // TODO. figure out what this resolution setting is for.. why it happens here instead of init etc.
    SendCommand(RESOLUTION_SETTING);
    SendData(width >> 8);
    SendData(width & 0xff);
    SendData(height >> 8);
    SendData(height & 0xff);

    SendCommand(DATA_START_TRANSMISSION_1);
    DelayMs(2);
    for(unsigned int i = 0; i < width / 8 * height; i++) {
        SendData(0xFF);
    }
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_2);
    DelayMs(2);
    for(unsigned int i = 0; i < width / 8 * height; i++) {
        SendData(0xFF);
    }
    DelayMs(2);
}

/**
 * @brief: After this command is transmitted, the chip would enter the deep-sleep mode to save power.
 *         The deep sleep mode would return to standby by hardware reset. The only one parameter is a
 *         check code, the command would be executed if check code = 0xA5.
 *         You can use Epd::Reset() to awaken and use Epd::Init() to initialize.
 */
void Epd::Sleep() {
    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendData(0x17);                       //border floating
    SendCommand(VCM_DC_SETTING);          //VCOM to 0V
    SendCommand(PANEL_SETTING);
    DelayMs(100);

    SendCommand(POWER_SETTING);           //VG&VS to 0V fast
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);
    DelayMs(100);

    SendCommand(POWER_OFF);          //power off
    WaitUntilIdle();
    SendCommand(DEEP_SLEEP);         //deep sleep
    SendData(0xA5);
}

const unsigned char lut_vcom[] =
{
  0x00, 0x08, 0x08, 0x00, 0x00, 0x02,
  0x00, 0x0F, 0x0F, 0x00, 0x00, 0x01,
  0x00, 0x08, 0x08, 0x00, 0x00, 0x02,
};

const unsigned char lut_ww[] ={
  0x50, 0x08, 0x08, 0x00, 0x00, 0x02,
  0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,
  0xA0, 0x08, 0x08, 0x00, 0x00, 0x02,
};

const unsigned char lut_bw[] ={
  0x50, 0x08, 0x08, 0x00, 0x00, 0x02,
  0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,
  0xA0, 0x08, 0x08, 0x00, 0x00, 0x02,
};

const unsigned char lut_bb[] ={
  0xA0, 0x08, 0x08, 0x00, 0x00, 0x02,
  0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,
  0x50, 0x08, 0x08, 0x00, 0x00, 0x02,
};

const unsigned char lut_wb[] ={
  0x20, 0x08, 0x08, 0x00, 0x00, 0x02,
  0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,
  0x10, 0x08, 0x08, 0x00, 0x00, 0x02,
};

/**************************** partial ********************************/
const unsigned char partial_lut_vcom[] ={
  0x00, 0x08, 0x08, 0x00, 0x00, 0x01,
};

const unsigned char partial_lut_ww[] ={
  0x50, 0x08, 0x08, 0x00, 0x00, 0x01,
};

const unsigned char partial_lut_bw[] ={
  0x50, 0x08, 0x08, 0x00, 0x00, 0x01,
};

const unsigned char partial_lut_wb[] ={
  0xA0, 0x08, 0x08, 0x00, 0x00, 0x01,
};

const unsigned char partial_lut_bb[] ={
  0x20, 0x08, 0x08, 0x00, 0x00, 0x01,
};

/* END OF FILE */
