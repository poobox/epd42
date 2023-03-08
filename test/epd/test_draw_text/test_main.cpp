#include "epd4in2.h"
#include "epdpaint.h"
#include <unity.h>
#include "pstr_helper.h"
#include "test_helper.h"

P_STR(HELLO_WORLD,"Hello World!");
P_STR(FONT_8_M,"8p font is hard to read, but fits a lot.");
P_STR(FONT_12_M,"12p font is more legible and still fits a lot of text.");
P_STR(FONT_16_M,"16p font is pretty nice.");
P_STR(FONT_20_M, "20p font is large for text.");
P_STR(FONT_24_M, "24p font is BIG.");

void setUp(void) {}

void tearDown(void)
{
  Epd epd;
  epd.Init();
  epd.ClearFrame();
  epd.DisplayFrame();
  epd.Sleep();
}

void test_hello_world(void)
{
  Epd epd;
  epd.Init();

  epd.ClearFrame();
  unsigned char image[1201];
  Paint paint(image, 400, 24);

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(10, 1, PSTR_USE(HELLO_WORLD), &Font24, COLORED);
  epd.SetPartialWindow(paint.GetImage(), 0, 28, paint.GetWidth(), paint.GetHeight());

  epd.DisplayFrame();

  TEST_ASSERT_TRUE(AskUserForTestSuccess());
}

void test_fonts(void)
{
  Epd epd;
  epd.Init();

  epd.ClearFrame();
  unsigned char image[1201];
  Paint paint(image, 400, 24);

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 0, PSTR_USE(FONT_24_M), &Font24, COLORED);
  epd.SetPartialWindow(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 0, PSTR_USE(FONT_20_M), &Font20, COLORED);
  epd.SetPartialWindow(paint.GetImage(), 0, 28, paint.GetWidth(), paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 0, PSTR_USE(FONT_16_M), &Font16, COLORED);
  epd.SetPartialWindow(paint.GetImage(), 0, 28*2, paint.GetWidth(), paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 0, PSTR_USE(FONT_12_M), &Font12, COLORED);
  epd.SetPartialWindow(paint.GetImage(), 0, 28*3, paint.GetWidth(), paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 0, PSTR_USE(FONT_8_M), &Font8, COLORED);
  epd.SetPartialWindow(paint.GetImage(), 0, 28*4, paint.GetWidth(), paint.GetHeight());

  epd.DisplayFrame();
  TEST_ASSERT_TRUE(AskUserForTestSuccess());
}

void setup()
{
  UNITY_BEGIN(); // IMPORTANT LINE!
  RUN_TEST(test_hello_world);
  RUN_TEST(test_fonts);
  UNITY_END(); // stop unit testing
}

void loop() {}
