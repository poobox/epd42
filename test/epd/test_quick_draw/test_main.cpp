#include "epd4in2.h"
#include "epdpaint.h"
#include <unity.h>
#include "pstr_helper.h"
#include "test_helper.h"

P_STR(MOVING_TEXT,"Hello Karl!");

void setUp(void)
{
  Epd epd;
  epd.Init();
  epd.ClearFrame();
  epd.DisplayFrame();
  // set stuff up here
}

void tearDown(void)
{
  // clean stuff up here
  Epd epd;
  epd.Init();
  epd.ClearFrame();
  for (int i=0; i < 3; i++) {
    epd.DisplayFrame();
  }
  epd.Sleep();
}

void test_move_text(void)
{
  Epd epd;
  epd.InitPartial();
  epd.ClearFrame();

  unsigned char image2[601];
  Paint paint2(image2, 200, 24);
  paint2.Clear(UNCOLORED);
  paint2.DrawStringAt(0, 0, PSTR_USE(MOVING_TEXT), &Font24, COLORED);

  for (int i = 0; i < 15; i++) {
    epd.SetPartialWindow(paint2.GetImage(), 5 + (i*5), 10 + (i * 10),
                         paint2.GetWidth(), paint2.GetHeight());
    epd.QuickDisplayFrame();
    epd.FillPartialWindowWhite(5 + (i*5), 10 + (i * 10), paint2.GetWidth(), paint2.GetHeight());
  }

  TEST_ASSERT_TRUE(AskUserForTestSuccess());
}

void setup()
{
  UNITY_BEGIN();
  RUN_TEST(test_move_text);
  UNITY_END();
}

void loop()
{}
