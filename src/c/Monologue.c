#include <pebble.h>
#include "Monologue.h"
#include "utils/weekday.h"
#include "utils/MathUtils.h"
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/fpath.h>
#include <pebble-fctx/ffont.h>

// Constants for improved readability
#define SECONDS_TICK_INTERVAL_MS 1000

// Main window and layers
static Window *s_window;
static Layer *s_canvas_layer;
static Layer *s_bg_layer;
static Layer *s_dial_layer;
//static Layer *s_dial_digits_layer;
static Layer *s_date_battery_logo_layer;
static Layer *s_canvas_second_hand;
static Layer *s_canvas_bt_icon;
static Layer *s_canvas_qt_icon;
static Layer *s_canvas_battery;
static GRect bounds;
static GRect bounds_seconds;
// Fonts
static GFont
    #ifdef PBL_BW
    FontDate,
    FontBattery,
    FontLogo,
    FontHour,
    #endif
    FontBTQTIcons;

//FFont* Digits_Font;
FFont* Date_Font;
// Time and date variables
static struct tm *prv_tick_time;
static int current_date;
static int s_weekday;
static int minutes;
static int hours;   //12h modulo
static int s_hours; //24h version
static int seconds;
static ClaySettings settings;
static bool showSeconds;

// Date position struct for different platforms
typedef struct {
  int BottomXPosition;
  int DateBottomYPosition;
  int BTQTBottomYPosition;
  int LeftxPosition;
  int xOffset;
  int yOffset;
  int xOffsetFctxWeekday;
  int xOffsetFctxDate;
  int yOffsetFctx;
  int xyMaskOffset;
  int xWeekdayOffset;
  int yWeekdayDateOffset;
  int xDateOffset;
  int ShadowAndMaskWidth;
  int ShadowAndMaskHeight;
  int WeekdayWidth;
  int DateWidth;
  int WeekdayDateHeight;
  int BTIconYOffset;
  int QTIconYOffset;
  int BatteryYOffset;
  int BatteryYOffset2;
  int BatteryYOffset3;
  int BatteryLineYOffset;
  int BatteryLineYOffset2;
  int LogoYOffset;
  int LogoYOffset2;
  int LogoYOffset3;
  int font_size_digits;
  int font_size_battery;
  int font_size_date;
  int font_size_logo;
  int six_pos_x;
  int six_pos_y;
  int twelve_pos_x;
  int twelve_pos_y;
  int date_pos_x;
  int date_pos_y;
  int battery_pos_z;
  int battery_pos_y;
  int battery_line;
  int second_hand_a;
  int second_hand_b;
  int second_hand_c;
  int second_hand_d;
  int hour_hand_a;
  int min_hand_a;
  int circle_radius_adj;
  int tick_mask_radius_adj;
  int hands_shadow;
  int QTIconXOffset2;
  int BTIconXOffset2;
  int QTIconYOffset2;
  int BTIconYOffset2;
  int corner_radius_secondshand;
  int corner_radius_majortickrect;
  int corner_radius_minortickrect;
  int majortickrect_w;
  int majortickrect_h;
  int minortickrect_w;
  int minortickrect_h;
  int tick_inset_outer;
  GRect dial_digits_mask_a[1];
  GRect dial_digits_mask_b[1];
  GRect dial_digits_mask_c[1];
} UIConfig;

#ifdef PBL_PLATFORM_EMERY
static const UIConfig config = {
.BottomXPosition = 46,
.DateBottomYPosition = 168,
.BTQTBottomYPosition = 168,
.LeftxPosition = -5,
.xOffset = 22,
.yOffset = -8,
.xOffsetFctxWeekday = 38/2  -1,
.xOffsetFctxDate = 22/2 + 1,
.yOffsetFctx = 3,
.xyMaskOffset = 6,
.xWeekdayOffset = 2,
.yWeekdayDateOffset = 1,
.xDateOffset = 41,
.ShadowAndMaskWidth = 64,
.ShadowAndMaskHeight = 17,
.WeekdayWidth = 38,
.DateWidth = 22,
.WeekdayDateHeight = 13,
.BTIconYOffset = -21,
.QTIconYOffset = 21,
.BatteryYOffset = 53-4 +3-1,
.BatteryYOffset2 = -9,
.BatteryYOffset3 = -9,
.BatteryLineYOffset = 66,
.BatteryLineYOffset2 = -8+114,
.LogoYOffset = 69+1,
.LogoYOffset2 = -8,
.LogoYOffset3 = -8+114,
.font_size_digits = 36,
.font_size_battery = 14,
.font_size_date = 12,
.font_size_logo = 10,
.six_pos_x = 2,
.six_pos_y = -40 + 9,
.twelve_pos_x = - 2 + 1,
.twelve_pos_y = -3,
.battery_pos_z = -2,
.battery_pos_y = 4,
.battery_line = 63, //sized to the width of the default logo TITANIUM
.second_hand_a = 1,  //was 20
.second_hand_b = 28,
.second_hand_c = 6,
.second_hand_d = 14,
.hour_hand_a = 45,
.min_hand_a = 2,  //was 20
.circle_radius_adj = 18,
.tick_mask_radius_adj = 12,
.hands_shadow = 2,
.QTIconXOffset2 =0,
.QTIconYOffset2 = 0,
.BTIconXOffset2 = 0,
.BTIconYOffset2 = 0,
.corner_radius_secondshand = 24,
.corner_radius_majortickrect = 20,
.corner_radius_minortickrect = 28,
.majortickrect_w = 86,
.majortickrect_h = 100,
.minortickrect_w = 90,
.minortickrect_h = 104,
.tick_inset_outer = -10,
.dial_digits_mask_a = {{{100-15,23},{39,7}}},
.dial_digits_mask_b = {{{100-19,0},{39,27}}},
.dial_digits_mask_c = {{{100-15,228-27},{31,27}}}
};
#elif defined(PBL_PLATFORM_GABBRO)
static const UIConfig config = {
.BottomXPosition = 46+30,
.DateBottomYPosition = 168+22+3,
.BTQTBottomYPosition = 168+22+3,
.LeftxPosition = 16+2,
.xOffset = 22,
.yOffset = -8,
.xOffsetFctxWeekday = 38/2  -1,
.xOffsetFctxDate = 22/2 + 1,
.yOffsetFctx = 3,
.xyMaskOffset = 6,
.xWeekdayOffset = 2,
.yWeekdayDateOffset = 1,
.xDateOffset = 41,
.ShadowAndMaskWidth = 64,
.ShadowAndMaskHeight = 17,
.WeekdayWidth = 38,
.DateWidth = 22,
.WeekdayDateHeight = 13,
.BTIconYOffset = -21,
.QTIconYOffset = 21,
.BatteryYOffset = 53-4 +3-1,
.BatteryYOffset2 = -9,
.BatteryYOffset3 = -1,
.BatteryLineYOffset = 66,
.BatteryLineYOffset2 = -8+130,
.LogoYOffset = 69+1+1,
.LogoYOffset2 = -8,
.LogoYOffset3 = -8+130,
.font_size_digits = 46,
.font_size_battery = 14,
.font_size_date = 12,
.font_size_logo = 10,
.six_pos_x = 2,
.six_pos_y = - 40 + 9,
.twelve_pos_x = - 2 + 1,
.twelve_pos_y = -3,
.battery_pos_z = -2,
.battery_pos_y = 4,
.battery_line = 63,
.second_hand_a = 1,
.second_hand_b = 28,
.hour_hand_a = 60,
.min_hand_a = 2,
.circle_radius_adj = 17,
.tick_mask_radius_adj = 12,
.hands_shadow = 2,
.QTIconXOffset2 =0,
.QTIconYOffset2 = 0,
.BTIconXOffset2 = 0,
.BTIconYOffset2 = 0,
.second_hand_c = 40,
.dial_digits_mask_a = {{{130-15,23-2},{39,7+2}}},
.dial_digits_mask_b = {{{130-19,0},{39,27}}},
.dial_digits_mask_c = {{{130-15,260-27},{31,27}}}
};
#elif defined(PBL_BW)
static const UIConfig config = {
.BottomXPosition = 38,
.DateBottomYPosition = 126-3+5,
.BTQTBottomYPosition = 126-3,
.LeftxPosition = 6 - 2,
.xOffset = 10,
.yOffset = -7,
.xOffsetFctxWeekday = 30/2 ,
.xOffsetFctxDate = 16/2 +1,
.yOffsetFctx = 2,
.xyMaskOffset = 3,
.xWeekdayOffset = 1,
.yWeekdayDateOffset = 2,
.xDateOffset = 31,
.ShadowAndMaskWidth = 48,
.ShadowAndMaskHeight = 15,
.WeekdayWidth = 30,
.DateWidth = 16,
.WeekdayDateHeight = 11,
.BTIconYOffset = -20,
.QTIconYOffset = 18,
.BatteryYOffset = 40-4,
.BatteryYOffset2 = -5,
.BatteryYOffset3 = -5,
.BatteryLineYOffset = 49,
.BatteryLineYOffset2 = -8+84,
.LogoYOffset = 52+1,
.LogoYOffset2 = -5,
.LogoYOffset3 = -5+84,
.font_size_digits = 24,
.font_size_battery = 10,
.font_size_date = 9,
.font_size_logo = 8,
.six_pos_x = 2,
.six_pos_y = - 38 + 9,
.twelve_pos_x = - 2 + 1,
.twelve_pos_y = -2,
.battery_pos_z = 0,
.battery_pos_y = 4,
.battery_line = 51,
.second_hand_a = 1,
.second_hand_b = 22,
.second_hand_c = 6,
.second_hand_d = 11,
.hour_hand_a = 35,
.min_hand_a = 22,
.circle_radius_adj = 18,
.tick_mask_radius_adj = 12,
.hands_shadow = 2,
.QTIconXOffset2 = 42,
.QTIconYOffset2 = 23,
.BTIconXOffset2 = -29,
.BTIconYOffset2 = 23,
.corner_radius_secondshand = 18,
.corner_radius_majortickrect = 15,
.corner_radius_minortickrect = 21,
.majortickrect_w = 62,
.majortickrect_h = 72,
.minortickrect_w = 66,
.minortickrect_h = 76,
.tick_inset_outer = -10,
.dial_digits_mask_a = {{{72-14,22},{36,7}}},
.dial_digits_mask_b = {{{72-18,0},{36,26}}},
.dial_digits_mask_c = {{{72-13,168-26},{28,26}}}
};
#elif defined(PBL_ROUND)
static const UIConfig config = {
.BottomXPosition = 52,
.DateBottomYPosition = 132,
.BTQTBottomYPosition = 132,
.LeftxPosition = 16,
.xOffset = 14,
.yOffset = -7,
.xOffsetFctxWeekday = 30/2 -1,
.xOffsetFctxDate = 16/2 +1,
.yOffsetFctx = 2,
.xyMaskOffset = 4,
.xWeekdayOffset = 1,
.yWeekdayDateOffset = 2,
.xDateOffset = 31,
.ShadowAndMaskWidth = 48,
.ShadowAndMaskHeight = 15,
.WeekdayWidth = 30,
.DateWidth = 16,
.WeekdayDateHeight = 11,
.BTIconYOffset = -20,
.QTIconYOffset = 18,
.BatteryYOffset = 40-4,
.BatteryYOffset2 = -4,
.BatteryYOffset3 = -4,
.BatteryLineYOffset = 49,
.BatteryLineYOffset2 = -8+90+4,
.LogoYOffset = 52+1,
.LogoYOffset2 = -8,
.LogoYOffset3 = -8+90+4,
.font_size_digits = 28,
.font_size_battery = 10,
.font_size_date = 9,
.font_size_logo = 8,
.six_pos_x = 2,
.six_pos_y = - 38 + 9,
.twelve_pos_x = - 2 + 1,
.twelve_pos_y = -2,
.battery_pos_z = 0,
.battery_pos_y = 4,
.battery_line = 51,
.second_hand_a = 1,
.second_hand_b = 22,
.hour_hand_a = 50,
.min_hand_a = 34,
.circle_radius_adj = 17,
.tick_mask_radius_adj = 12,
.hands_shadow = 2,
.QTIconXOffset2 =0,
.QTIconYOffset2 = 0,
.BTIconXOffset2 = 0,
.BTIconYOffset2 = 0,
.second_hand_c = 28,
.dial_digits_mask_a = {{{90-14,22},{36,7}}},
.dial_digits_mask_b = {{{90-18,0},{36,26}}},
.dial_digits_mask_c = {{{90-13,180-26},{28,26}}}
};
#else // Default for other platforms
static const UIConfig config = {
.BottomXPosition = 38,
.DateBottomYPosition = 126-3+5,
.BTQTBottomYPosition = 126-3,
.LeftxPosition = 7 - 2,
.xOffset = 10,
.yOffset = -7,
.xOffsetFctxWeekday = 30/2 -1,
.xOffsetFctxDate = 16/2 +1,
.yOffsetFctx = 2,
.xyMaskOffset = 4,
.xWeekdayOffset = 1,
.yWeekdayDateOffset = 2,
.xDateOffset = 31,
.ShadowAndMaskWidth = 48,
.ShadowAndMaskHeight = 15,
.WeekdayWidth = 30,
.DateWidth = 16,
.WeekdayDateHeight = 11,
.BTIconYOffset = -20,
.QTIconYOffset = 18,
.BatteryYOffset = 40-4,
.BatteryYOffset2 = -7,
.BatteryYOffset3 = -7,
.BatteryLineYOffset = 49,
.BatteryLineYOffset2 = -8+84+1,
.LogoYOffset = 52+1,
.LogoYOffset2 = -8,
.LogoYOffset3 = -8+84,
.font_size_digits = 24,
.font_size_battery = 10,
.font_size_date = 9,
.font_size_logo = 8,
.six_pos_x = 2,
.six_pos_y = - 38 + 9,
.twelve_pos_x = - 2 + 1,
.twelve_pos_y = -2,
.battery_pos_z = 0,
.battery_pos_y = 4,
.battery_line = 51,
.second_hand_a = 1,
.second_hand_b = 22,
.second_hand_c = 6,
.second_hand_d = 11,
.hour_hand_a = 35,
.min_hand_a = 22,
.circle_radius_adj = 18,
.tick_mask_radius_adj = 12,
.hands_shadow = 2,
.QTIconXOffset2 = 42,
.QTIconYOffset2 = 23,
.BTIconXOffset2 = -29,
.BTIconYOffset2 = 23,
.corner_radius_secondshand = 18,
.corner_radius_majortickrect = 15,
.corner_radius_minortickrect = 21,
.majortickrect_w = 62,
.majortickrect_h = 72,
.minortickrect_w = 66,
.minortickrect_h = 76,
.tick_inset_outer = -10,
.dial_digits_mask_a = {{{72-14,22},{36,7}}},
.dial_digits_mask_b = {{{72-18,0},{36,26}}},
.dial_digits_mask_c = {{{72-13,168-26},{28,26}}}
};
#endif

bool connected = true;
bool ignore_next_tap = false;

//function prototypes



static void prv_save_settings(void);
static void prv_default_settings(void);
static void prv_load_settings(void);
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context);
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void bg_update_proc(Layer *layer, GContext *ctx);
//static void layer_update_proc_dial_digits_mask(Layer *layer, GContext * ctx);
static void update_logo_date_battery_fctx_layer(Layer *layer, GContext * ctx);
static void layer_update_proc_battery_line(Layer *layer, GContext * ctx);
//static void layer_update_proc_seconds_hand(Layer *layer, GContext * ctx);
static void hour_min_hands_canvas_update_proc(Layer *layer, GContext *ctx);
static void layer_update_proc_qt(Layer *layer, GContext *ctx);
static void layer_update_proc_bt(Layer *layer, GContext *ctx);
// static void draw_fancy_hand_hour(GContext *ctx, int angle, int length, GColor fill_color, GColor border_color);
// static void draw_fancy_hand_min(GContext *ctx, int angle, int length, GColor fill_color, GColor border_color);
static void draw_line_hand(GContext *ctx, int angle, int length, int back_length, GColor color);
//static void draw_center(GContext *ctx, GColor minutes_border, GColor minutes_color);
static void draw_seconds_center(GContext *ctx, GColor outer_color, GColor inner_color);
static void prv_window_load(Window *window);
static void prv_window_unload(Window *window);
static void prv_init(void);
static void prv_deinit(void);

// Save settings to persistent storage
static void prv_save_settings(void) {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}


// Set default settings
static void prv_default_settings(void) {
  settings.EnableSecondsHand = true;
  settings.SecondsVisibleTime = 135;
  settings.EnableDate = true;
  settings.EnableBattery = true;
  settings.EnableBatteryLine = true;
  settings.EnableLogo = true;
  snprintf(settings.LogoText, sizeof(settings.LogoText), "%s", "monologue");
  settings.BackgroundColor1 = GColorWhite;
  settings.BackgroundColor2 = GColorBabyBlueEyes;
  settings.TextColor1 = GColorWhite;
  settings.TextColor2 = GColorBabyBlueEyes;
  settings.TextColor3 = GColorDarkGray;
  settings.DateColor = GColorDarkGray;
  settings.HoursHandColor = GColorCobaltBlue;
  settings.HoursHandBorderColor = GColorDarkGray;
  settings.MinutesHandColor = GColorCobaltBlue;
  settings.MinutesHandBorderColor = GColorCobaltBlue;
  settings.TickColor = GColorCobaltBlue;
  settings.SecondsHandColor = GColorOrange;
  settings.BatteryLineColor = GColorOrange;
  settings.BWDateColor = GColorBlack;
  settings.BWBackgroundColor1 = GColorWhite;
  settings.BWBackgroundColor2 = GColorDarkGray;
  settings.BWTextColor1 = GColorBlack;
  settings.BWTextColor2 = GColorBlack;
  settings.BWTextColor3 = GColorBlack;
  settings.BTQTColor = GColorBlack;
  settings.BWBTQTColor = GColorBlack;
  settings.showMajorTick = true;
  settings.showMinorTick = true;
  // settings.BWThemeSelect = "wh";
  // settings.ThemeSelect = "wh";
  snprintf(settings.BWThemeSelect, sizeof(settings.BWThemeSelect), "%s", "wh");
  snprintf(settings.ThemeSelect, sizeof(settings.ThemeSelect), "%s", "wh");
  snprintf(settings.PosLeft, sizeof(settings.PosLeft), "%s", "hr");
  snprintf(settings.PosRight, sizeof(settings.PosRight), "%s", "ap");
  snprintf(settings.PosTop, sizeof(settings.PosTop), "%s", "lo");
  snprintf(settings.PosBottom, sizeof(settings.PosBottom), "%s", "dt");
  settings.BWShadowOn = true;
  settings.ShadowOn = true;
  settings.Font = 1;
  settings.VibeOn = false;
  settings.AddZero12h = false;
  settings.RemoveZero24h = false;
  settings.showlocalAMPM = true;
  settings.ForegroundShape = true;  //true = round, false = rect
}

// Quiet time icon handler
static void quiet_time_icon () {
    layer_set_hidden(s_canvas_qt_icon, !quiet_time_is_active());
}


static AppTimer *s_timeout_timer;


static void timeout_handler(void *context) {
  // showSeconds = false;

  // // Unsubscribe from second ticks to save power
  // //// tick_timer_service_unsubscribe();
  // tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // layer_mark_dirty(s_canvas_second_hand);
  // s_timeout_timer = NULL; // Set the handle to NULL after the timer expires

  // //APP_LOG(APP_LOG_LEVEL_DEBUG, "timeout event");

}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  //  if (ignore_next_tap) {
  //      ignore_next_tap = false; // Reset the flag for the next tap
  //      return;
  //    }

  // // Only handle if the seconds hand setting is enabled and not already always on
  // if (settings.EnableSecondsHand && settings.SecondsVisibleTime < 135) {
  //     // If a timer is already running, cancel it
  //     if (s_timeout_timer) {
  //       app_timer_cancel(s_timeout_timer);
  //       s_timeout_timer = NULL;
  //     }

  //     // Only subscribe to second ticks if not already subscribed
  //     if (!showSeconds) {
  //     //   tick_timer_service_unsubscribe();
  //        tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  //     //   layer_mark_dirty(s_canvas_second_hand);
  //     }
      
  //     showSeconds = true;

  //     // Register a new timer to hide the seconds hand
  //     s_timeout_timer = app_timer_register(SECONDS_TICK_INTERVAL_MS * settings.SecondsVisibleTime, timeout_handler, NULL);
  //     layer_mark_dirty(s_canvas_second_hand);
  // }
}

static void bluetooth_vibe_icon (bool connected) {


   layer_set_hidden(s_canvas_bt_icon, connected);

  if((!connected && !quiet_time_is_active()) ||(!connected && quiet_time_is_active() && settings.VibeOn)) {
    if (settings.SecondsVisibleTime > 0 && settings.SecondsVisibleTime < 135) {
      // Unsubscribe from accel_tap before the vibe
      accel_tap_service_unsubscribe();
      showSeconds = false;
    }

    // Issue a vibrating alert
    #ifdef PBL_PLATFORM_DIORITE
    vibes_short_pulse();
    ignore_next_tap = true;

    #else
    vibes_double_pulse();
    // Set the flag to ignore the next tap
    ignore_next_tap = false;
    #endif

    if (settings.SecondsVisibleTime > 0 && settings.SecondsVisibleTime < 135) {
      accel_tap_service_subscribe(accel_tap_handler);
    }
  }


}

// Load settings from persistent storage
static void prv_load_settings(void) {
  prv_default_settings();
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// AppMessage inbox handler
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
#ifdef LOG
  APP_LOG(APP_LOG_LEVEL_INFO, "Received message");
#endif

  bool settings_changed = false;
  bool theme_settings_changed = false;

  Tuple *vibe_t = dict_find(iter, MESSAGE_KEY_VibeOn);
  Tuple *enable_seconds_t = dict_find(iter, MESSAGE_KEY_EnableSecondsHand);
  Tuple *enable_secondsvisible_t = dict_find(iter, MESSAGE_KEY_SecondsVisibleTime);
  Tuple *enable_date_t = dict_find(iter, MESSAGE_KEY_EnableDate);
  Tuple *enable_battery_t = dict_find(iter, MESSAGE_KEY_EnableBattery);
  Tuple *enable_battery_line_t = dict_find(iter, MESSAGE_KEY_EnableBatteryLine);
  Tuple *enable_logo_t = dict_find(iter, MESSAGE_KEY_EnableLogo);
  Tuple *logotext_t = dict_find(iter, MESSAGE_KEY_LogoText);
  Tuple *bwthemeselect_t = dict_find(iter, MESSAGE_KEY_BWThemeSelect);
  Tuple *themeselect_t = dict_find(iter, MESSAGE_KEY_ThemeSelect);
  Tuple *bg_color1_t = dict_find(iter, MESSAGE_KEY_BackgroundColor1);
  Tuple *bg_color2_t = dict_find(iter, MESSAGE_KEY_BackgroundColor2);
  Tuple *text_color1_t = dict_find(iter, MESSAGE_KEY_TextColor1);
  Tuple *text_color2_t = dict_find(iter, MESSAGE_KEY_TextColor2);
  Tuple *text_color3_t = dict_find(iter, MESSAGE_KEY_TextColor3);
  Tuple *date_color_t = dict_find(iter, MESSAGE_KEY_DateColor);
  Tuple *bwdate_color_t = dict_find(iter, MESSAGE_KEY_BWDateColor);
  Tuple *hours_color_t = dict_find(iter, MESSAGE_KEY_HoursHandColor);
  Tuple *hours_border_t = dict_find(iter, MESSAGE_KEY_HoursHandBorderColor);
  Tuple *minutes_color_t = dict_find(iter, MESSAGE_KEY_MinutesHandColor);
  Tuple *minutes_border_t = dict_find(iter, MESSAGE_KEY_MinutesHandBorderColor);
  Tuple *tick_color_t = dict_find(iter, MESSAGE_KEY_TickColor);
  Tuple *seconds_color_t = dict_find(iter, MESSAGE_KEY_SecondsHandColor);
  Tuple *battery_line_color_t = dict_find(iter, MESSAGE_KEY_BatteryLineColor);
  Tuple *bwbg_color1_t = dict_find(iter, MESSAGE_KEY_BWBackgroundColor1);
  Tuple *bwbg_color2_t = dict_find(iter, MESSAGE_KEY_BWBackgroundColor2);
  Tuple *bwtext_color1_t = dict_find(iter, MESSAGE_KEY_BWTextColor1);
  Tuple *bwtext_color2_t = dict_find(iter, MESSAGE_KEY_BWTextColor2);
  Tuple *bwtext_color3_t = dict_find(iter, MESSAGE_KEY_BWTextColor3);
  Tuple *btqt_color_t = dict_find(iter, MESSAGE_KEY_BTQTColor);
  Tuple *bwbtqt_color_t = dict_find(iter, MESSAGE_KEY_BWBTQTColor);
  Tuple *bwshadowon_t = dict_find(iter, MESSAGE_KEY_BWShadowOn);
  Tuple *shadowon_t = dict_find(iter, MESSAGE_KEY_ShadowOn);
  Tuple *addzero12_t = dict_find(iter, MESSAGE_KEY_AddZero12h);
  Tuple *remzero24_t = dict_find(iter, MESSAGE_KEY_RemoveZero24h);
  Tuple *localampm_t = dict_find(iter, MESSAGE_KEY_showlocalAMPM);
  Tuple *posleft_t = dict_find(iter, MESSAGE_KEY_PosLeft);
  Tuple *posright_t = dict_find(iter, MESSAGE_KEY_PosRight);
  Tuple *postop_t = dict_find(iter, MESSAGE_KEY_PosTop);
  Tuple *posbottom_t = dict_find(iter, MESSAGE_KEY_PosBottom);
  Tuple *majort_t = dict_find(iter, MESSAGE_KEY_showMajorTick);
  Tuple *minort_t = dict_find(iter, MESSAGE_KEY_showMinorTick);
  Tuple *fg_shape_t = dict_find(iter, MESSAGE_KEY_ForegroundShape);

  if (fg_shape_t) {
    settings.ForegroundShape = fg_shape_t->value->int32 == 1;
    layer_mark_dirty(s_bg_layer);
    layer_mark_dirty(s_canvas_layer);
  }

  if(majort_t){
    settings.showMajorTick = majort_t->value->int32 != 0;
    layer_mark_dirty(s_bg_layer);
  } 
  
  if(minort_t){
    settings.showMinorTick = minort_t->value->int32 != 0;
    layer_mark_dirty(s_bg_layer);
  }

  if(posleft_t){
    snprintf(settings.PosLeft, sizeof(settings.PosLeft), "%s", posleft_t -> value -> cstring);
    layer_mark_dirty(s_date_battery_logo_layer);
  }

  if(posright_t){
    snprintf(settings.PosRight, sizeof(settings.PosRight), "%s", posright_t -> value -> cstring);
    layer_mark_dirty(s_date_battery_logo_layer);
  }

  if(postop_t){
    snprintf(settings.PosTop, sizeof(settings.PosTop), "%s", postop_t -> value -> cstring);
    layer_mark_dirty(s_date_battery_logo_layer);
  }

  if(posbottom_t){
    snprintf(settings.PosBottom, sizeof(settings.PosBottom), "%s", posbottom_t -> value -> cstring);
    layer_mark_dirty(s_date_battery_logo_layer);
  }

  if (vibe_t){
    if (vibe_t -> value -> int32 == 0){
      settings.VibeOn = false;
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Vibe off");
    } else {
      settings.VibeOn = true;
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Vibe on");
    }
    layer_mark_dirty(s_canvas_bt_icon);
  }

  if (addzero12_t) {
    settings.AddZero12h = addzero12_t->value->int32 != 0;
    layer_mark_dirty(s_date_battery_logo_layer);
  }

  if (remzero24_t) {
    settings.RemoveZero24h = remzero24_t->value->int32 != 0;
     layer_mark_dirty(s_date_battery_logo_layer);
  }

  if (localampm_t) {
    settings.showlocalAMPM = localampm_t->value->int32 == 1;
    layer_mark_dirty(s_date_battery_logo_layer);
  }

  if (enable_seconds_t) {
    settings.EnableSecondsHand = enable_seconds_t->value->int32 == 1;
    // Unsubscribe from any existing tick services
    tick_timer_service_unsubscribe();
    accel_tap_service_unsubscribe();
    // Always subscribe to MINUTE_UNIT by default for efficiency
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    layer_mark_dirty(s_canvas_second_hand);
  }

  if (enable_date_t) {
    settings.EnableDate = enable_date_t->value->int32 == 1;
    layer_mark_dirty(s_canvas_layer);
    layer_mark_dirty(s_date_battery_logo_layer);
  }

  if (enable_logo_t) {
    settings.EnableLogo = enable_logo_t->value->int32 == 1;

    // Check if the logo is enabled and the custom text string is not empty
    if (settings.EnableLogo && logotext_t && strlen(logotext_t->value->cstring) > 0) {
      // If the custom text field is not blank, use the user's text
      snprintf(settings.LogoText, sizeof(settings.LogoText), "%s", logotext_t->value->cstring);
    } else if (settings.EnableLogo && strlen(logotext_t->value->cstring) == 0) {
      // If the custom text field is blank but the logo is enabled, use the default text
      snprintf(settings.LogoText, sizeof(settings.LogoText), "%s", "monologue");
    }
    else {
      snprintf(settings.LogoText, sizeof(settings.LogoText), "%s", "");
    }

    layer_mark_dirty(s_date_battery_logo_layer);

  }



  if (enable_battery_t) {
    settings.EnableBattery = enable_battery_t->value->int32 == 1;
    layer_mark_dirty(s_canvas_battery);
    layer_mark_dirty(s_date_battery_logo_layer);
    }

  if (enable_battery_line_t) {
    settings.EnableBatteryLine = enable_battery_line_t->value->int32 == 1;
    layer_mark_dirty(s_canvas_battery);
  }

  if (enable_secondsvisible_t) {
    settings.SecondsVisibleTime = (int) enable_secondsvisible_t->value->int32;
    // Cancel and re-register timer if it was running
    if (s_timeout_timer) {
      app_timer_cancel(s_timeout_timer);
      s_timeout_timer = NULL;
    }

    // Handle "Always On" vs. "Timeout" behavior for the seconds hand
    if (settings.SecondsVisibleTime == 135) {
      // "Always On" logic: show seconds, and don't register a timer
      showSeconds = true;
      if (settings.EnableSecondsHand) {
        tick_timer_service_unsubscribe();
        tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
        // Unsubscribe from accel_tap_service as it's not needed
        accel_tap_service_unsubscribe();
      }
    } else if (settings.SecondsVisibleTime > 0) {
      // "Timeout" logic: start with seconds shown, register a timer
      showSeconds = true;
      if (settings.EnableSecondsHand) {
        tick_timer_service_unsubscribe();
        tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
        s_timeout_timer = app_timer_register(SECONDS_TICK_INTERVAL_MS * settings.SecondsVisibleTime, timeout_handler, NULL);
        // Subscribe to accel_tap_service to reset the timer
        accel_tap_service_subscribe(accel_tap_handler);
      }
    } else {
      // "Disabled" logic: don't show seconds, ensure we're on minute ticks
      showSeconds = false;
      tick_timer_service_unsubscribe();
      tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
      // Unsubscribe from accel_tap_service
      accel_tap_service_unsubscribe();
    }
    layer_mark_dirty(s_canvas_second_hand);
  }




  if (bwthemeselect_t) {
          // Compare the string value received from the phone
          if (strcmp(bwthemeselect_t->value->cstring, "wh") == 0) {
              // Set the theme and other settings for "wh"
                    settings.BWDateColor = GColorBlack;
                    if (bwshadowon_t) {
                      settings.BWShadowOn = bwshadowon_t->value->int32 == 1;
                    }
                        if(settings.BWShadowOn){
                          settings.BWBackgroundColor2 = GColorDarkGray;
                        }
                        else {
                        settings.BWBackgroundColor2 = GColorWhite;
                        }
                    settings.BWBackgroundColor1 = GColorWhite;
                    settings.BWTextColor1 = GColorBlack;
                    settings.BWTextColor2 = GColorBlack;
                    settings.BWTextColor3 = GColorBlack;
                    settings.BWBTQTColor = GColorBlack;
                      theme_settings_changed = true;
                    //    APP_LOG(APP_LOG_LEVEL_DEBUG, "Theme white selected");
          } else if (strcmp(bwthemeselect_t->value->cstring, "bl") == 0) {
              // Set the theme and other settings for "bl"
                    settings.BWDateColor = GColorWhite;
                    settings.BWBackgroundColor1 = GColorBlack;
                    if (bwshadowon_t) {
                      settings.BWShadowOn = bwshadowon_t->value->int32 == 1;
                    }
                        if(settings.BWShadowOn){
                          settings.BWBackgroundColor2 = GColorDarkGray;
                        }
                        else {
                        settings.BWBackgroundColor2 = GColorBlack;
                        }
                    settings.BWTextColor1 = GColorWhite;
                    settings.BWTextColor2 = GColorWhite;
                    settings.BWTextColor3 = GColorWhite;
                    settings.BWBTQTColor = GColorWhite;
                      theme_settings_changed = true;
                    //    APP_LOG(APP_LOG_LEVEL_DEBUG, "Theme black selected");
          } else if (strcmp(bwthemeselect_t->value->cstring, "cu") == 0) {
              // Set the theme for "cu" and handle custom colors
              settings.BWDateColor = GColorFromHEX(bwdate_color_t->value->int32);
                    layer_mark_dirty(s_canvas_layer);

                  if (bwbg_color1_t) {
                    settings.BWBackgroundColor1 = GColorFromHEX(bwbg_color1_t->value->int32);
                    settings_changed = true;
                  }

                  if (bwshadowon_t) {
                    settings.BWShadowOn = bwshadowon_t->value->int32 == 1;

                      if(settings.BWShadowOn){
                        if (bwbg_color2_t) {
                          settings.BWBackgroundColor2 = GColorFromHEX(bwbg_color2_t->value->int32);
                          settings_changed = true;
                        }
                      }
                      else {
                      settings.BWBackgroundColor2 = settings.BWBackgroundColor1;
                      }
                  }

                  if (bwtext_color1_t) {
                    settings.BWTextColor1 = GColorFromHEX(bwtext_color1_t->value->int32);
                    settings_changed = true;
                  }
                  if (bwtext_color2_t) {
                    settings.BWTextColor2 = GColorFromHEX(bwtext_color2_t->value->int32);
                    settings_changed = true;
                  }
                  if (bwtext_color3_t) {
                    settings.BWTextColor3 = GColorFromHEX(bwtext_color3_t->value->int32);
                    settings_changed = true;
                  }
                  if (bwbtqt_color_t) {
                    settings.BWBTQTColor = GColorFromHEX(bwbtqt_color_t->value->int32);
                    layer_mark_dirty(s_canvas_bt_icon);
                    layer_mark_dirty(s_canvas_qt_icon);
                  }
                  theme_settings_changed = true;
                  //  APP_LOG(APP_LOG_LEVEL_DEBUG, "Theme custom selected");
                }
          }
/////////////////////////////////////
  if (themeselect_t) {
          // Compare the string value received from the phone
          if (strcmp(themeselect_t->value->cstring, "wh") == 0) {
              // Set the theme and other settings for "wh"
                    if (shadowon_t) {
                      settings.ShadowOn = shadowon_t->value->int32 == 1;
                    }
                        if(settings.ShadowOn){
                          settings.BackgroundColor2 = GColorBabyBlueEyes;
                        }
                        else {
                        settings.BackgroundColor2 = GColorWhite;
                        }
                    settings.BackgroundColor1 = GColorWhite;
                    settings.TextColor1 = GColorWhite;
                    settings.TextColor2 = GColorBabyBlueEyes;
                    settings.TextColor3 = GColorDarkGray;
                    settings.DateColor = GColorDarkGray;
                    settings.HoursHandColor = GColorCobaltBlue;
                    settings.HoursHandBorderColor = GColorDarkGray;
                    settings.MinutesHandColor = GColorCobaltBlue;
                    settings.MinutesHandBorderColor = GColorCobaltBlue;
                    settings.TickColor = GColorCobaltBlue;
                    settings.SecondsHandColor = GColorOrange;
                    settings.BatteryLineColor = GColorOrange;
                    settings.BTQTColor = GColorBabyBlueEyes;
                      theme_settings_changed = true;
                    //    APP_LOG(APP_LOG_LEVEL_DEBUG, "Theme white selected");
          } else if (strcmp(themeselect_t->value->cstring, "bl") == 0) {
              // Set the theme and other settings for "bl"

                    settings.BackgroundColor1 = GColorBlack;
                    if (shadowon_t) {
                      settings.ShadowOn = shadowon_t->value->int32 == 1;
                    }
                        if(settings.ShadowOn){
                          settings.BackgroundColor2 = GColorDarkGray;
                        }
                        else {
                        settings.BackgroundColor2 = GColorBlack;
                        }
                    settings.TextColor1 = GColorBlack;
                    settings.TextColor2 = GColorDarkGray;
                    settings.TextColor3 = GColorWindsorTan;
                    settings.DateColor = GColorWindsorTan;
                    settings.HoursHandColor = GColorYellow;
                    settings.HoursHandBorderColor = GColorYellow;
                    settings.MinutesHandColor = GColorYellow;
                    settings.MinutesHandBorderColor = GColorYellow;
                    settings.TickColor = GColorYellow;
                    settings.SecondsHandColor = GColorYellow;
                    settings.BatteryLineColor = GColorYellow;
                    settings.BTQTColor = GColorDarkGray;
                      theme_settings_changed = true;
                      //  APP_LOG(APP_LOG_LEVEL_DEBUG, "Theme black selected");
          } else if (strcmp(themeselect_t->value->cstring, "bu") == 0) {
              // Set the theme and other settings for "bl"

                    settings.BackgroundColor1 = GColorDukeBlue;
                    if (shadowon_t) {
                      settings.ShadowOn = shadowon_t->value->int32 == 1;
                    }
                        if(settings.ShadowOn){
                          settings.BackgroundColor2 = GColorOxfordBlue;
                        }
                        else {
                        settings.BackgroundColor2 = GColorDukeBlue;
                        }
                    settings.TextColor1 = GColorBlack;
                    settings.TextColor2 = GColorPictonBlue;
                    settings.TextColor3 = GColorIcterine;
                    settings.DateColor = GColorWhite;
                    settings.HoursHandColor = GColorYellow;
                    settings.HoursHandBorderColor = GColorYellow;
                    settings.MinutesHandColor = GColorYellow;
                    settings.MinutesHandBorderColor = GColorYellow;
                    settings.TickColor = GColorYellow;
                    settings.SecondsHandColor = GColorRed;
                    settings.BatteryLineColor = GColorRed;
                    settings.BTQTColor = GColorPictonBlue;
                      theme_settings_changed = true;
                      //  APP_LOG(APP_LOG_LEVEL_DEBUG, "Theme blue selected");
          } else if (strcmp(themeselect_t->value->cstring, "pl") == 0) {
              // Set the theme and other settings for "bl"

                    settings.BackgroundColor1 = GColorPurple;
                    if (shadowon_t) {
                      settings.ShadowOn = shadowon_t->value->int32 == 1;
                    }
                        if(settings.ShadowOn){
                          settings.BackgroundColor2 = GColorImperialPurple;
                        }
                        else {
                        settings.BackgroundColor2 = GColorPurple;
                        }
                    settings.TextColor1 = GColorRichBrilliantLavender;
                    settings.TextColor2 = GColorImperialPurple;
                    settings.TextColor3 = GColorRichBrilliantLavender;
                    settings.DateColor = GColorRichBrilliantLavender;
                    settings.HoursHandColor = GColorRichBrilliantLavender;
                    settings.HoursHandBorderColor = GColorRichBrilliantLavender;
                    settings.MinutesHandColor = GColorRichBrilliantLavender;
                    settings.MinutesHandBorderColor = GColorBlack;
                    settings.TickColor = GColorRichBrilliantLavender;
                    settings.SecondsHandColor = GColorBulgarianRose;
                    settings.BatteryLineColor = GColorBulgarianRose;
                    settings.BTQTColor = GColorImperialPurple;
                      theme_settings_changed = true;
                      //  APP_LOG(APP_LOG_LEVEL_DEBUG, "Theme purple selected");
          } else if (strcmp(themeselect_t->value->cstring, "gr") == 0) {
              // Set the theme and other settings for "gr"

                    settings.BackgroundColor1 = GColorBlack;
                    if (shadowon_t) {
                      settings.ShadowOn = shadowon_t->value->int32 == 1;
                    }
                        if(settings.ShadowOn){
                          settings.BackgroundColor2 = GColorDarkGreen;
                        }
                        else {
                        settings.BackgroundColor2 = GColorBlack;
                        }
                    settings.TextColor1 = GColorBlack;
                    settings.TextColor2 = GColorDarkGreen;
                    settings.TextColor3 = GColorBrightGreen;
                    settings.DateColor = GColorGreen;
                    settings.HoursHandColor = GColorBrightGreen;
                    settings.HoursHandBorderColor = GColorBrightGreen;
                    settings.MinutesHandColor = GColorBrightGreen;
                    settings.MinutesHandBorderColor = GColorBrightGreen;
                    settings.TickColor = GColorBrightGreen;
                    settings.SecondsHandColor = GColorPastelYellow;
                    settings.BatteryLineColor = GColorPastelYellow;
                    settings.BTQTColor = GColorDarkGreen;
                      theme_settings_changed = true;
                      //  APP_LOG(APP_LOG_LEVEL_DEBUG, "Theme black & green selected");
          } else if (strcmp(themeselect_t->value->cstring, "cu") == 0) {
              // Set the theme for "cu" and handle custom colors
              // settings.DateColor = GColorFromHEX(date_color_t->value->int32);
              //       layer_mark_dirty(s_canvas_layer);

                  if (bg_color1_t) {
                    settings.BackgroundColor1 = GColorFromHEX(bg_color1_t->value->int32);
                    settings_changed = true;
                  }

                  if (shadowon_t) {
                    settings.ShadowOn = shadowon_t->value->int32 == 1;

                      if(settings.ShadowOn){
                        if (bg_color2_t) {
                          settings.BackgroundColor2 = GColorFromHEX(bg_color2_t->value->int32);
                          settings_changed = true;
                        }
                      }
                      else {
                      settings.BackgroundColor2 = settings.BackgroundColor1;
                      }
                  }

                  if (text_color1_t) {
                    settings.TextColor1 = GColorFromHEX(text_color1_t->value->int32);
                    layer_mark_dirty(s_dial_layer);
                    //layer_mark_dirty(s_dial_digits_layer);
                    layer_mark_dirty(s_date_battery_logo_layer);
                  }
                  if (text_color2_t) {
                    settings.TextColor2 = GColorFromHEX(text_color2_t->value->int32);
                    layer_mark_dirty(s_bg_layer);
                  }
                  if (text_color3_t) {
                    settings.TextColor3 = GColorFromHEX(text_color3_t->value->int32);
                    layer_mark_dirty(s_dial_layer);
                   // layer_mark_dirty(s_dial_digits_layer);
                    layer_mark_dirty(s_date_battery_logo_layer);
                  }
                  if (date_color_t) {
                    settings.DateColor = GColorFromHEX(date_color_t->value->int32);
                    layer_mark_dirty(s_canvas_layer);
                    layer_mark_dirty(s_date_battery_logo_layer);
                  }
                  if (hours_color_t) {
                    settings.HoursHandColor = GColorFromHEX(hours_color_t->value->int32);
                    layer_mark_dirty(s_canvas_layer);
                    layer_mark_dirty(s_canvas_second_hand);
                  }
                  if (hours_border_t) {
                    settings.HoursHandBorderColor = GColorFromHEX(hours_border_t->value->int32);
                    layer_mark_dirty(s_canvas_layer);
                  }
                  if (minutes_color_t) {
                    settings.MinutesHandColor = GColorFromHEX(minutes_color_t->value->int32);
                    layer_mark_dirty(s_canvas_layer);
                    layer_mark_dirty(s_canvas_second_hand);
                    layer_mark_dirty(s_date_battery_logo_layer);
                  }
                  if (minutes_border_t) {
                    settings.MinutesHandBorderColor = GColorFromHEX(minutes_border_t->value->int32);
                    layer_mark_dirty(s_canvas_layer);
                    layer_mark_dirty(s_date_battery_logo_layer);
                  }
                  if (tick_color_t) {
                    settings.TickColor = GColorFromHEX(tick_color_t->value->int32);
                    layer_mark_dirty(s_canvas_layer);
                    layer_mark_dirty(s_date_battery_logo_layer);
                  }
                  if (seconds_color_t) {
                    settings.SecondsHandColor = GColorFromHEX(seconds_color_t->value->int32);
                    layer_mark_dirty(s_canvas_second_hand);
                  }
                  if (battery_line_color_t) {
                    settings.BatteryLineColor = GColorFromHEX(battery_line_color_t->value->int32);
                    layer_mark_dirty(s_canvas_battery);
                  }
                  if (btqt_color_t) {
                    settings.BTQTColor = GColorFromHEX(btqt_color_t->value->int32);
                    layer_mark_dirty(s_canvas_bt_icon);
                    layer_mark_dirty(s_canvas_qt_icon);
                  }
                  theme_settings_changed = true;
                //    APP_LOG(APP_LOG_LEVEL_DEBUG, "Theme custom selected");
                }
          }

                  ///////////////////////////////

  // if (bg_color1_t) {
  //   settings.BackgroundColor1 = GColorFromHEX(bg_color1_t->value->int32);
  //   layer_mark_dirty(s_bg_layer);
  //   layer_mark_dirty(s_canvas_layer);
  //   layer_mark_dirty(s_dial_layer);
  //   layer_mark_dirty(s_dial_digits_layer);
  //   layer_mark_dirty(s_date_battery_logo_layer);
  //   layer_mark_dirty(s_canvas_second_hand);
  // }

  // if (shadowon_t) {
  //   settings.ShadowOn = shadowon_t->value->int32 == 1;
  //   APP_LOG(APP_LOG_LEVEL_DEBUG, "color shadow is %d",settings.ShadowOn);
  //
  //   if(settings.ShadowOn){
  //     if (bg_color2_t) {
  //       settings.BackgroundColor2 = GColorFromHEX(bg_color2_t->value->int32);
  //       settings_changed = true;
  //     }
  //   }
  //   else {
  //   settings.BackgroundColor2 = settings.BackgroundColor1;
  //   }
  //   layer_mark_dirty(s_bg_layer);
  //   layer_mark_dirty(s_canvas_layer);
  //   layer_mark_dirty(s_dial_layer);
  //   layer_mark_dirty(s_dial_digits_layer);
  //   layer_mark_dirty(s_date_battery_logo_layer);
  //   layer_mark_dirty(s_canvas_second_hand);
  // }

  // if (text_color1_t) {
  //   settings.TextColor1 = GColorFromHEX(text_color1_t->value->int32);
  //   layer_mark_dirty(s_dial_layer);
  //   layer_mark_dirty(s_dial_digits_layer);
  //   layer_mark_dirty(s_date_battery_logo_layer);
  // }
  // if (text_color2_t) {
  //   settings.TextColor2 = GColorFromHEX(text_color2_t->value->int32);
  //   layer_mark_dirty(s_bg_layer);
  // }
  // if (text_color3_t) {
  //   settings.TextColor3 = GColorFromHEX(text_color3_t->value->int32);
  //   layer_mark_dirty(s_dial_layer);
  //   layer_mark_dirty(s_dial_digits_layer);
  //   //layer_mark_dirty(s_date_battery_logo_layer);
  // }
  // if (date_color_t) {
  //   settings.DateColor = GColorFromHEX(date_color_t->value->int32);
  //   layer_mark_dirty(s_canvas_layer);
  //   layer_mark_dirty(s_date_battery_logo_layer);
  // }
  //
  // if (hours_color_t) {
  //   settings.HoursHandColor = GColorFromHEX(hours_color_t->value->int32);
  //   layer_mark_dirty(s_canvas_layer);
  //   layer_mark_dirty(s_canvas_second_hand);
  // }
  // if (hours_border_t) {
  //   settings.HoursHandBorderColor = GColorFromHEX(hours_border_t->value->int32);
  //   layer_mark_dirty(s_canvas_layer);
  // }
  // if (minutes_color_t) {
  //   settings.MinutesHandColor = GColorFromHEX(minutes_color_t->value->int32);
  //   layer_mark_dirty(s_canvas_layer);
  //   layer_mark_dirty(s_canvas_second_hand);
  // }
  // if (minutes_border_t) {
  //   settings.MinutesHandBorderColor = GColorFromHEX(minutes_border_t->value->int32);
  //   layer_mark_dirty(s_canvas_layer);
  // }
  // if (seconds_color_t) {
  //   settings.SecondsHandColor = GColorFromHEX(seconds_color_t->value->int32);
  //   layer_mark_dirty(s_canvas_second_hand);
  // }
  // if (battery_line_color_t) {
  //   settings.BatteryLineColor = GColorFromHEX(battery_line_color_t->value->int32);
  //   layer_mark_dirty(s_canvas_battery);
  // }
  // if (btqt_color_t) {
  //   settings.BTQTColor = GColorFromHEX(btqt_color_t->value->int32);
  //   layer_mark_dirty(s_canvas_bt_icon);
  //   layer_mark_dirty(s_canvas_qt_icon);
  // }

  if (settings_changed) {
    layer_mark_dirty(s_bg_layer);
    layer_mark_dirty(s_canvas_layer);
    layer_mark_dirty(s_dial_layer);
   // layer_mark_dirty(s_dial_digits_layer);
    layer_mark_dirty(s_date_battery_logo_layer);
    layer_mark_dirty(s_canvas_second_hand);
    layer_mark_dirty(s_canvas_battery);
  }

  if (theme_settings_changed) {
    layer_mark_dirty(s_bg_layer);
    layer_mark_dirty(s_canvas_layer);
    layer_mark_dirty(s_dial_layer);
  //  layer_mark_dirty(s_dial_digits_layer);
    layer_mark_dirty(s_date_battery_logo_layer);
    layer_mark_dirty(s_canvas_second_hand);
    layer_mark_dirty(s_canvas_qt_icon);
    layer_mark_dirty(s_canvas_bt_icon);
    layer_mark_dirty(s_canvas_battery);
  }

  prv_save_settings();

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  time_t temp = time(NULL);
  prv_tick_time = localtime(&temp);

  // Update hour and minute hands and the date on minute change
  if (units_changed & MINUTE_UNIT) {
    minutes = tick_time->tm_min;
    hours = tick_time->tm_hour % 12;
    s_hours = tick_time->tm_hour;
    layer_mark_dirty(s_canvas_layer);
    layer_mark_dirty(s_date_battery_logo_layer);
    if (settings.EnableDate && tick_time->tm_mday != current_date) {
      current_date = tick_time->tm_mday;
      s_weekday = tick_time->tm_wday;
    }
  }


}



///second hand
static void draw_line_hand(GContext *ctx, int angle, int length, int back_length, GColor color) {
  GPoint origin = GPoint(bounds.size.w / 2, bounds.size.h / 2);
  GPoint origin_offset = GPoint(origin.x + config.hands_shadow, origin.y + config.hands_shadow);
  GPoint p1;
  GPoint p2;
  
  #ifdef PBL_ROUND
    p1 = polar_to_point_offset(origin, angle + 180, back_length);
    p2 = polar_to_point_offset(origin, angle, length);
  #else
    if(settings.ForegroundShape){
      p1 = polar_to_point_offset(origin, angle + 180, back_length);
      p2 = polar_to_point_offset(origin, angle, length);
    }
    else{
      GRect r = GRect(0, 0, bounds.size.w, bounds.size.h);
      //p1 = point_from_edge(origin, angle+180, r, back_length);
      p1 = polar_to_point_offset(origin, angle + 180, back_length);
      p2 = angle_to_rounded_rect_edge(origin, angle, bounds.size.w/2 - config.second_hand_c, bounds.size.h/2 - config.second_hand_c, config.corner_radius_secondshand);

    }
  #endif
  // Define shadow color
  GColor shadow_color = PBL_IF_BW_ELSE(settings.BWBackgroundColor2,settings.BackgroundColor2);


  // Set the antialiasing
  graphics_context_set_antialiased(ctx, true);

  // Draw the shadow first, with a small offset
  graphics_context_set_stroke_color(ctx, shadow_color);
  graphics_context_set_stroke_width(ctx, 1); // Same width as the hand
  graphics_draw_line(ctx, GPoint(p1.x + config.hands_shadow, p1.y + config.hands_shadow), GPoint(p2.x + config.hands_shadow, p2.y + config.hands_shadow));
  graphics_context_set_fill_color(ctx, shadow_color);
  GPoint origin_back_offset = GPoint(p1.x + config.hands_shadow, p1.y + config.hands_shadow);
  graphics_fill_circle(ctx, origin_back_offset, 4);
  graphics_fill_circle(ctx, origin_offset, 7); //started as 4



  // Now draw the main second hand on top
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, 2);
  graphics_draw_line(ctx, p1, p2);

  graphics_context_set_fill_color(ctx, color);
  GPoint origin_back = GPoint(p1.x, p1.y);
  graphics_fill_circle(ctx, origin_back, 4);

}



static void draw_seconds_center(GContext *ctx, GColor outer_color, GColor inner_color) {
  GPoint origin = GPoint(bounds.size.w / 2, bounds.size.h / 2);
  graphics_context_set_antialiased(ctx, true);

  graphics_context_set_fill_color(ctx, outer_color);
  graphics_fill_circle(ctx, origin, 7); //started as 4
  
  graphics_context_set_fill_color(ctx, inner_color);
  graphics_fill_circle(ctx, origin, 1); //started as 2

}


static void draw_major_tick (GContext *ctx, int angle, int length, GColor fill_color, GColor border_color) {
    // GPoint origin = GPoint(bounds.size.w / 2, bounds.size.h / 2);

    // // The tick starts away from the center
    // GPoint p1 = polar_to_point_offset(origin, angle, bounds.size.h / 2 - 16 );
    // // The tick ends at a fixed length from p1
    // GPoint p2 = polar_to_point_offset(origin, angle, bounds.size.h / 2 - 16 + length);
 

    GPoint origin = GPoint(bounds.size.w / 2, bounds.size.h / 2);
      GPoint p1;
      GPoint p2;

      #ifdef PBL_ROUND
        p1 = polar_to_point_offset(origin, angle, bounds.size.h / 2 - 16 );
        p2 = polar_to_point_offset(origin, angle, bounds.size.h / 2 - 16 + length);
      #else
        if(settings.ForegroundShape){
          p1 = polar_to_point_offset(origin, angle, bounds.size.h / 2 - 16 );
          p2 = polar_to_point_offset(origin, angle, bounds.size.h / 2 - 16 + length);
        }
        else{
          GRect r = GRect(0, 0, bounds.size.w, bounds.size.h);
          GPoint edge = angle_to_rect_edge(origin, angle, r);
          int32_t dx = cos_lookup(DEG_TO_TRIGANGLE(angle));
          int32_t dy = sin_lookup(DEG_TO_TRIGANGLE(angle));
          p2 = GPoint(edge.x - (int)((dx * config.tick_inset_outer) / TRIG_MAX_ANGLE),
                            edge.y - (int)((dy * config.tick_inset_outer) / TRIG_MAX_ANGLE));
          p1 = angle_to_rounded_rect_edge(origin, angle, config.majortickrect_w, config.majortickrect_h, config.corner_radius_majortickrect);
        }
      #endif
 
    graphics_context_set_antialiased(ctx, true);
    graphics_context_set_stroke_color(ctx, border_color);
    graphics_context_set_stroke_width(ctx, 3);
    graphics_draw_line(ctx, p1, p2);
}

static void draw_minor_tick(GContext *ctx, int angle, GColor border_color) {
  GPoint origin = GPoint(bounds.size.w / 2, bounds.size.h / 2);
      GPoint p1;
      GPoint p2;

      #ifdef PBL_ROUND
          // The tick starts away from the center of the watch face.
          p1 = polar_to_point_offset(origin, angle, bounds.size.h / 2 - 8);
          // The tick ends closer to the edge.
          p2 = polar_to_point_offset(origin, angle, bounds.size.h / 2 );
      #else
        if(settings.ForegroundShape){
            p1 = polar_to_point_offset(origin, angle, bounds.size.h / 2 - 8);
            p2 = polar_to_point_offset(origin, angle, bounds.size.h / 2 );
          }
          else{
            GRect r = GRect(0, 0, bounds.size.w, bounds.size.h);
            GPoint edge = angle_to_rect_edge(origin, angle, r);
            int32_t dx = cos_lookup(DEG_TO_TRIGANGLE(angle));
            int32_t dy = sin_lookup(DEG_TO_TRIGANGLE(angle));
            p2 = GPoint(edge.x - (int)((dx * config.tick_inset_outer) / TRIG_MAX_ANGLE),
                              edge.y - (int)((dy * config.tick_inset_outer) / TRIG_MAX_ANGLE));
            p1 = angle_to_rounded_rect_edge(origin, angle, config.minortickrect_w, config.minortickrect_h, config.corner_radius_minortickrect);
          }
      #endif

  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_color(ctx, border_color);
  graphics_context_set_stroke_width(ctx, 1); // A thin line for minor ticks
  graphics_draw_line(ctx, p1, p2);
}


#ifdef PBL_BW  //DON'T use FCTX a second time on Aplite: also use on Diorite and Flint as fctx is less efficient
static void update_logo_date_battery_fctx_layer (Layer *layer, GContext *ctx) {
  
  GRect bounds = layer_get_bounds(s_date_battery_logo_layer);

  //draw ampm markers

  
  

  //draw battery value
  if(settings.EnableBattery ){
     if(strcmp(settings.PosTop, "lo") == 0){
      int s_battery_level = battery_state_service_peek().charge_percent;
      char BatterytoDraw[6];
            snprintf(BatterytoDraw,sizeof(BatterytoDraw),"%d",s_battery_level);

      if (settings.EnableBatteryLine) {
        GRect BatteryRect = GRect((bounds.size.w / 2) - 18, config.BatteryYOffset, 36, 40);
        graphics_context_set_text_color(ctx, settings.BWDateColor);
        graphics_draw_text(ctx, BatterytoDraw, FontBattery, BatteryRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
      else{
        GRect BatteryRect = GRect((bounds.size.w / 2) - 18, config.BatteryYOffset + 4, 36, 40);
        graphics_context_set_text_color(ctx, settings.BWDateColor);
        graphics_draw_text(ctx, BatterytoDraw, FontBattery, BatteryRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
    }

    if(strcmp(settings.PosBottom, "lo") == 0){
      int s_battery_level = battery_state_service_peek().charge_percent;
      char BatterytoDraw[6];
            snprintf(BatterytoDraw,sizeof(BatterytoDraw),"%d",s_battery_level);

      if (settings.EnableBatteryLine) {
        GRect BatteryRect = GRect((bounds.size.w / 2) - 18, config.BatteryYOffset + (bounds.size.h/2) -8, 36, 40);
        graphics_context_set_text_color(ctx, settings.BWDateColor);
        graphics_draw_text(ctx, BatterytoDraw, FontBattery, BatteryRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
      else{
        GRect BatteryRect = GRect((bounds.size.w / 2) - 18, config.BatteryYOffset + 4 + (bounds.size.h/2) -8, 36, 40);
        graphics_context_set_text_color(ctx, settings.BWDateColor);
        graphics_draw_text(ctx, BatterytoDraw, FontBattery, BatteryRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
    }

    if(strcmp(settings.PosLeft, "lo") == 0){
      int s_battery_level = battery_state_service_peek().charge_percent;
      char BatterytoDraw[6];
            snprintf(BatterytoDraw,sizeof(BatterytoDraw),"%d",s_battery_level);

      if (settings.EnableBatteryLine) {
        GRect BatteryRect = GRect(18, (bounds.size.h/2)  - 8 - 5, 36, 40);
        graphics_context_set_text_color(ctx, settings.BWDateColor);
        graphics_draw_text(ctx, BatterytoDraw, FontBattery, BatteryRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
      else{
        GRect BatteryRect = GRect(18, (bounds.size.h/2) -8 - 5 + 4, 36, 40);
        graphics_context_set_text_color(ctx, settings.BWDateColor);
        graphics_draw_text(ctx, BatterytoDraw, FontBattery, BatteryRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
    }

    if(strcmp(settings.PosRight, "lo") == 0){
      int s_battery_level = battery_state_service_peek().charge_percent;
      char BatterytoDraw[6];
            snprintf(BatterytoDraw,sizeof(BatterytoDraw),"%d",s_battery_level);

      if (settings.EnableBatteryLine) {
        GRect BatteryRect = GRect((bounds.size.w/2) + 18, (bounds.size.h/2)  - 8 - 5, 36, 40);
        graphics_context_set_text_color(ctx, settings.BWDateColor);
        graphics_draw_text(ctx, BatterytoDraw, FontBattery, BatteryRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
      else{
        GRect BatteryRect = GRect((bounds.size.w/2) + 18, (bounds.size.h/2) -8 - 5 + 4, 36, 40);
        graphics_context_set_text_color(ctx, settings.BWDateColor);
        graphics_draw_text(ctx, BatterytoDraw, FontBattery, BatteryRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
    }
  }

  //draw weekday and date text
  if (settings.EnableDate ) {
    minutes = prv_tick_time->tm_min;
    hours = prv_tick_time->tm_hour % 12;
    ///use below for testing and for screenshots
     //int seconds = 8;
       // minutes = 30;
       // hours = 9;

    int xPosition;
    int yPosition;
    int xOffset = config.xOffset;
    int yOffset = config.yOffset;
    ///int yOffsetFctx = config.yOffsetFctx;
    int xWeekdayOffset = config.xWeekdayOffset;
    int yWeekdayDateOffset = config.yWeekdayDateOffset;
    int xDateOffset = config.xDateOffset;
    int xyMaskOffset = config.xyMaskOffset;
    int ShadowAndMaskWidth = config.ShadowAndMaskWidth;
    int ShadowAndMaskHeight = config.ShadowAndMaskHeight;
    int WeekdayWidth = config.WeekdayWidth;
    int DateWidth = config.DateWidth;
    int WeekdayDateHeight = config.WeekdayDateHeight;

    // if (((hours > 7 && hours < 11) && (minutes > 7 && minutes < 23))||
    // ((hours <5 && hours >1) && (minutes > 37 && minutes < 53))){
          //bottom position
    
    ////////Draw hour digits in one of 4 positions

    if(strcmp(settings.PosBottom, "hr") == 0){

       graphics_context_set_antialiased(ctx, true);
           int hourdraw;
              //hourdraw = prv_tick_time->tm_hour;
              //s_hours = prv_tick_time->tm_hour % 12;
              char hournow[4];
              if (clock_is_24h_style()) {
                hourdraw = s_hours;
                snprintf(hournow, sizeof(hournow), settings.RemoveZero24h ? "%d" : "%02d", hourdraw);
              } else {
                if (s_hours == 0 || s_hours == 12) hourdraw = 12;
                else 
                hourdraw = s_hours % 12;
                snprintf(hournow, sizeof(hournow), settings.AddZero12h ? "%02d" : "%d", hourdraw);
              }

            GRect hour_rect = GRect(0, bounds.size.h * 3/4 - 15, bounds.size.w, 20);
             graphics_context_set_text_color(ctx, settings.BWTextColor2);
             graphics_draw_text(ctx, hournow, FontHour, hour_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
   }
   
   if(strcmp(settings.PosTop, "hr") == 0){

         graphics_context_set_antialiased(ctx, true);
           int hourdraw;
              //hourdraw = prv_tick_time->tm_hour;
              //s_hours = prv_tick_time->tm_hour % 12;
              char hournow[4];
              if (clock_is_24h_style()) {
                hourdraw = s_hours;
                snprintf(hournow, sizeof(hournow), settings.RemoveZero24h ? "%d" : "%02d", hourdraw);
              } else {
                if (s_hours == 0 || s_hours == 12) hourdraw = 12;
                else 
                hourdraw = s_hours % 12;
                snprintf(hournow, sizeof(hournow), settings.AddZero12h ? "%02d" : "%d", hourdraw);
              }

            GRect hour_rect = GRect(0, bounds.size.h/4 - 15, bounds.size.w, 20);
             graphics_context_set_text_color(ctx, settings.BWTextColor2);
             graphics_draw_text(ctx, hournow, FontHour, hour_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

    }


    if(strcmp(settings.PosRight, "hr") == 0){
         graphics_context_set_antialiased(ctx, true);
           int hourdraw;
              //hourdraw = prv_tick_time->tm_hour;
              //s_hours = prv_tick_time->tm_hour % 12;
              char hournow[4];
              if (clock_is_24h_style()) {
                hourdraw = s_hours;
                snprintf(hournow, sizeof(hournow), settings.RemoveZero24h ? "%d" : "%02d", hourdraw);
              } else {
                if (s_hours == 0 || s_hours == 12) hourdraw = 12;
                else 
                hourdraw = s_hours % 12;
                snprintf(hournow, sizeof(hournow), settings.AddZero12h ? "%02d" : "%d", hourdraw);
              }

            GRect hour_rect = GRect(bounds.size.w/2, bounds.size.h/2 - 15, bounds.size.w/2, 20);
             graphics_context_set_text_color(ctx, settings.BWTextColor2);
             graphics_draw_text(ctx, hournow, FontHour, hour_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);


    }
    
    if(strcmp(settings.PosLeft, "hr") == 0){

         graphics_context_set_antialiased(ctx, true);
           int hourdraw;
              //hourdraw = prv_tick_time->tm_hour;
              //s_hours = prv_tick_time->tm_hour % 12;
              char hournow[4];
              if (clock_is_24h_style()) {
                hourdraw = s_hours;
                snprintf(hournow, sizeof(hournow), settings.RemoveZero24h ? "%d" : "%02d", hourdraw);
              } else {
                if (s_hours == 0 || s_hours == 12) hourdraw = 12;
                else 
                hourdraw = s_hours % 12;
                snprintf(hournow, sizeof(hournow), settings.AddZero12h ? "%02d" : "%d", hourdraw);
              }

            GRect hour_rect = GRect(0, bounds.size.h/2 - 15, bounds.size.w/2, 20);
             graphics_context_set_text_color(ctx, settings.BWTextColor2);
             graphics_draw_text(ctx, hournow, FontHour, hour_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }

///////Draw date in one of 4 positions


    if(strcmp(settings.PosBottom, "dt") == 0){
        // int xPosition;
        // int yPosition;
          xPosition = config.BottomXPosition;
          yPosition = config.DateBottomYPosition;

          graphics_context_set_antialiased(ctx, true);

            const char * sys_locale = i18n_get_system_locale();
            char weekday[5];
            fetchwday(s_weekday, sys_locale, weekday);

            char weekdaydraw[10];
            snprintf(weekdaydraw, sizeof(weekdaydraw), "%s", weekday);

            char daynow[5];
            snprintf(daynow, sizeof(daynow), "%d", current_date);

            GRect WeekdayRect =
                GRect(xPosition + xOffset + xWeekdayOffset, yPosition + yOffset +yWeekdayDateOffset, WeekdayWidth, WeekdayDateHeight);

            GRect DateRect =
                GRect(xPosition + xOffset + xDateOffset, yPosition + yOffset +yWeekdayDateOffset, DateWidth, WeekdayDateHeight);

            graphics_context_set_text_color(ctx, settings.BWDateColor);
            graphics_draw_text(ctx, weekdaydraw, FontDate, WeekdayRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
            graphics_draw_text(ctx, daynow, FontDate, DateRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
           

    }

    if(strcmp(settings.PosLeft, "dt") == 0){
        // int xPosition;
        // int yPosition;
          xPosition = config.LeftxPosition;
          yPosition = bounds.size.h/2;

          graphics_context_set_antialiased(ctx, true);

    

            const char * sys_locale = i18n_get_system_locale();
            char weekday[5];
            fetchwday(s_weekday, sys_locale, weekday);

            char weekdaydraw[10];
            snprintf(weekdaydraw, sizeof(weekdaydraw), "%s", weekday);

            char daynow[5];
            snprintf(daynow, sizeof(daynow), "%d", current_date);

            GRect WeekdayRect =
                GRect(xPosition + xOffset + xWeekdayOffset, yPosition + yOffset +yWeekdayDateOffset, WeekdayWidth, WeekdayDateHeight);

            GRect DateRect =
                GRect(xPosition + xOffset + xDateOffset, yPosition + yOffset +yWeekdayDateOffset, DateWidth, WeekdayDateHeight);

            graphics_context_set_text_color(ctx, settings.BWDateColor);
            graphics_draw_text(ctx, weekdaydraw, FontDate, WeekdayRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
            graphics_draw_text(ctx, daynow, FontDate, DateRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }

    if(strcmp(settings.PosRight, "dt") == 0){
      // int xPosition;
      // int yPosition;
         xPosition = bounds.size.w/2;
         yPosition = bounds.size.h/2;

         graphics_context_set_antialiased(ctx, true);

    

            const char * sys_locale = i18n_get_system_locale();
            char weekday[5];
            fetchwday(s_weekday, sys_locale, weekday);

            char weekdaydraw[10];
            snprintf(weekdaydraw, sizeof(weekdaydraw), "%s", weekday);

            char daynow[5];
            snprintf(daynow, sizeof(daynow), "%d", current_date);


            GRect WeekdayRect =
                GRect(xPosition + xOffset + xWeekdayOffset, yPosition + yOffset +yWeekdayDateOffset, WeekdayWidth, WeekdayDateHeight);

            GRect DateRect =
                GRect(xPosition + xOffset + xDateOffset, yPosition + yOffset +yWeekdayDateOffset, DateWidth, WeekdayDateHeight);


            graphics_context_set_text_color(ctx, settings.BWDateColor);
            graphics_draw_text(ctx, weekdaydraw, FontDate, WeekdayRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
            graphics_draw_text(ctx, daynow, FontDate, DateRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

      }

      if(strcmp(settings.PosTop, "dt") == 0){
      // int xPosition;
      // int yPosition;
        xPosition = config.BottomXPosition;
        yPosition = bounds.size.h/4;

        graphics_context_set_antialiased(ctx, true);

    

            const char * sys_locale = i18n_get_system_locale();
            char weekday[5];
            fetchwday(s_weekday, sys_locale, weekday);

            char weekdaydraw[10];
            snprintf(weekdaydraw, sizeof(weekdaydraw), "%s", weekday);

            char daynow[5];
            snprintf(daynow, sizeof(daynow), "%d", current_date);

            GRect WeekdayRect =
                GRect(xPosition + xOffset + xWeekdayOffset, yPosition + yOffset +yWeekdayDateOffset, WeekdayWidth, WeekdayDateHeight);

            GRect DateRect =
                GRect(xPosition + xOffset + xDateOffset, yPosition + yOffset +yWeekdayDateOffset, DateWidth, WeekdayDateHeight);

            graphics_context_set_text_color(ctx, settings.BWDateColor);
            graphics_draw_text(ctx, weekdaydraw, FontDate, WeekdayRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
            graphics_draw_text(ctx, daynow, FontDate, DateRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
    
    
  }

//////////draw logo at top or bottom (4 positions)

  if (settings.EnableLogo) {

    if(strcmp(settings.PosRight, "lo") == 0){
      //draw logo text
      GRect LogoRect = GRect((bounds.size.w / 2)+4, bounds.size.h/2 + 3 , bounds.size.w / 2-8, 40);

      char logodraw [20];
      snprintf(logodraw, sizeof(logodraw), "%s", settings.LogoText);

      graphics_context_set_text_color(ctx, settings.BWDateColor);
      graphics_draw_text(ctx, logodraw, FontLogo, LogoRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }

    if(strcmp(settings.PosLeft, "lo") == 0){
      //draw logo text
      GRect LogoRect = GRect(4, bounds.size.h/2 + 3 , bounds.size.w / 2-8, 40);

      char logodraw [20];
      snprintf(logodraw, sizeof(logodraw), "%s", settings.LogoText);

      graphics_context_set_text_color(ctx, settings.BWDateColor);
      graphics_draw_text(ctx, logodraw, FontLogo, LogoRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }

    if(strcmp(settings.PosTop, "lo") == 0){
      //draw logo text
      GRect LogoRect = GRect((bounds.size.w / 2) - 34, config.LogoYOffset - 1 , 68, 40);

      char logodraw [20];
      snprintf(logodraw, sizeof(logodraw), "%s", settings.LogoText);

      graphics_context_set_text_color(ctx, settings.BWDateColor);
      graphics_draw_text(ctx, logodraw, FontLogo, LogoRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }

    if(strcmp(settings.PosBottom, "lo") == 0){
      //draw logo text
      GRect LogoRect = GRect((bounds.size.w / 2) - 34, config.LogoYOffset - 1 + (bounds.size.h/2) - 8, 68, 40);

      char logodraw [20];
      snprintf(logodraw, sizeof(logodraw), "%s", settings.LogoText);

      graphics_context_set_text_color(ctx, settings.BWDateColor);
      graphics_draw_text(ctx, logodraw, FontLogo, LogoRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }

  }
if(settings.showlocalAMPM){

    if(strcmp(settings.PosTop, "ap") == 0){
      char local_ampm_string[5];
      strftime(local_ampm_string, sizeof(local_ampm_string), "%p", prv_tick_time);

      if (settings.showlocalAMPM && !clock_is_24h_style()) {
        bool is_am = (prv_tick_time->tm_hour < 12);
        // int ampm_y = is_am
        //     ? 74 - config.other_text_font_size - 2 - 3 
        //     : 74 + 21 + 2 - 6;

        // GRect ampm_rect = GRect(50, ampm_y, 45, config.other_text_font_size + 2);

        // graphics_draw_text(ctx, local_ampm_string, ampm_font_aplite, ampm_rect,
        //     GTextOverflowModeFill, GTextAlignmentCenter, NULL);

        GRect ampm_rect = GRect(0, bounds.size.h/4 - 6, bounds.size.w, 6);
        graphics_context_set_text_color(ctx, settings.BWTextColor2);
        graphics_draw_text(ctx, local_ampm_string, FontBattery, ampm_rect, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
      }
    }

    if(strcmp(settings.PosBottom, "ap") == 0){
      char local_ampm_string[5];
      strftime(local_ampm_string, sizeof(local_ampm_string), "%p", prv_tick_time);

      if (settings.showlocalAMPM && !clock_is_24h_style()) {
        bool is_am = (prv_tick_time->tm_hour < 12);
        // int ampm_y = is_am
        //     ? 74 - config.other_text_font_size - 2 - 3 
        //     : 74 + 21 + 2 - 6;

        // GRect ampm_rect = GRect(50, ampm_y, 45, config.other_text_font_size + 2);

        // graphics_draw_text(ctx, local_ampm_string, ampm_font_aplite, ampm_rect,
        //     GTextOverflowModeFill, GTextAlignmentCenter, NULL);

        GRect ampm_rect = GRect(0, bounds.size.h * 3/4 - 6, bounds.size.w, 6);
        graphics_context_set_text_color(ctx, settings.BWTextColor2);
        graphics_draw_text(ctx, local_ampm_string, FontBattery, ampm_rect, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
      }
    }

    if(strcmp(settings.PosRight, "ap") == 0){
      char local_ampm_string[5];
      strftime(local_ampm_string, sizeof(local_ampm_string), "%p", prv_tick_time);

      if (settings.showlocalAMPM && !clock_is_24h_style()) {
        bool is_am = (prv_tick_time->tm_hour < 12);
        // int ampm_y = is_am
        //     ? 74 - config.other_text_font_size - 2 - 3 
        //     : 74 + 21 + 2 - 6;

        // GRect ampm_rect = GRect(50, ampm_y, 45, config.other_text_font_size + 2);

        // graphics_draw_text(ctx, local_ampm_string, ampm_font_aplite, ampm_rect,
        //     GTextOverflowModeFill, GTextAlignmentCenter, NULL);

        GRect ampm_rect = GRect(bounds.size.w/2, bounds.size.h/2 - 6, bounds.size.w/2, 6);
        graphics_context_set_text_color(ctx, settings.BWTextColor2);
        graphics_draw_text(ctx, local_ampm_string, FontBattery, ampm_rect, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
        }
      }
    
      if(strcmp(settings.PosLeft, "ap") == 0){
      char local_ampm_string[5];
      strftime(local_ampm_string, sizeof(local_ampm_string), "%p", prv_tick_time);

      if (settings.showlocalAMPM && !clock_is_24h_style()) {
        bool is_am = (prv_tick_time->tm_hour < 12);
        // int ampm_y = is_am
        //     ? 74 - config.other_text_font_size - 2 - 3 
        //     : 74 + 21 + 2 - 6;

        // GRect ampm_rect = GRect(50, ampm_y, 45, config.other_text_font_size + 2);

        // graphics_draw_text(ctx, local_ampm_string, ampm_font_aplite, ampm_rect,
        //     GTextOverflowModeFill, GTextAlignmentCenter, NULL);

        GRect ampm_rect = GRect(0, bounds.size.h/2 - 6, bounds.size.w/2, 6);
        graphics_context_set_text_color(ctx, settings.BWTextColor2);
        graphics_draw_text(ctx, local_ampm_string, FontBattery, ampm_rect, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
        }
      }
    }
}
#else   //use FCTX to antialise the digits better on all colour watches, still refers to B&W in case I change my mind later on non-APLITE watches
static void update_logo_date_battery_fctx_layer (Layer *layer, GContext *ctx) {
  
  GRect bounds = layer_get_bounds(s_date_battery_logo_layer);

  FContext fctx;
  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);
  #ifdef PBL_COLOR
   fctx_enable_aa(true);
  #endif

  if(strcmp(settings.PosLeft, "hr") == 0){
   fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWTextColor2, settings.HoursHandColor));
   FPoint hour_pos;

   hour_pos.x = INT_TO_FIXED((bounds.size.w / 4));
   hour_pos.y = INT_TO_FIXED((bounds.size.h / 2));

   int hourdraw;
      char hournow[4];
      if (clock_is_24h_style()) {
        hourdraw = s_hours;
        snprintf(hournow, sizeof(hournow), settings.RemoveZero24h ? "%d" : "%02d", hourdraw);
      } else {
        if (s_hours == 0 || s_hours == 12) hourdraw = 12;
        else 
        hourdraw = s_hours % 12;
        snprintf(hournow, sizeof(hournow), settings.AddZero12h ? "%02d" : "%d", hourdraw);
      }

    fctx_begin_fill(&fctx);
    fctx_set_text_em_height(&fctx, Date_Font, config.font_size_digits);

          fctx_set_offset(&fctx, hour_pos);
          fctx_draw_string(&fctx, hournow, Date_Font, GTextAlignmentCenter, FTextAnchorMiddle);
          fctx_end_fill(&fctx);
    }

  if(strcmp(settings.PosRight, "hr") == 0){
   fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWTextColor2, settings.HoursHandColor));
   FPoint hour_pos;

   hour_pos.x = INT_TO_FIXED((bounds.size.w*3 / 4));
   hour_pos.y = INT_TO_FIXED((bounds.size.h / 2));

   int hourdraw;
      char hournow[4];
      if (clock_is_24h_style()) {
        hourdraw = s_hours;
        snprintf(hournow, sizeof(hournow), settings.RemoveZero24h ? "%d" : "%02d", hourdraw);
      } else {
        if (s_hours == 0 || s_hours == 12) hourdraw = 12;
        else 
        hourdraw = s_hours % 12;
        snprintf(hournow, sizeof(hournow), settings.AddZero12h ? "%02d" : "%d", hourdraw);
      }

    fctx_begin_fill(&fctx);
    fctx_set_text_em_height(&fctx, Date_Font, config.font_size_digits);

          fctx_set_offset(&fctx, hour_pos);
          fctx_draw_string(&fctx, hournow, Date_Font, GTextAlignmentCenter, FTextAnchorMiddle);
          fctx_end_fill(&fctx);
  }

  if(strcmp(settings.PosTop, "hr") == 0){
   fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWTextColor2, settings.HoursHandColor));
   FPoint hour_pos;

   hour_pos.x = INT_TO_FIXED((bounds.size.w / 2));
   hour_pos.y = INT_TO_FIXED((bounds.size.h / 4));

   int hourdraw;
      char hournow[4];
      if (clock_is_24h_style()) {
        hourdraw = s_hours;
        snprintf(hournow, sizeof(hournow), settings.RemoveZero24h ? "%d" : "%02d", hourdraw);
      } else {
        if (s_hours == 0 || s_hours == 12) hourdraw = 12;
        else 
        hourdraw = s_hours % 12;
        snprintf(hournow, sizeof(hournow), settings.AddZero12h ? "%02d" : "%d", hourdraw);
      }

    fctx_begin_fill(&fctx);
    fctx_set_text_em_height(&fctx, Date_Font, config.font_size_digits);

          fctx_set_offset(&fctx, hour_pos);
          fctx_draw_string(&fctx, hournow, Date_Font, GTextAlignmentCenter, FTextAnchorMiddle);
          fctx_end_fill(&fctx);
  }

  if(strcmp(settings.PosBottom, "hr") == 0){
   fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWTextColor2, settings.HoursHandColor));
   FPoint hour_pos;

   hour_pos.x = INT_TO_FIXED((bounds.size.w / 2));
   hour_pos.y = INT_TO_FIXED((bounds.size.h*3 / 4));

   int hourdraw;
      char hournow[4];
      if (clock_is_24h_style()) {
        hourdraw = s_hours;
        snprintf(hournow, sizeof(hournow), settings.RemoveZero24h ? "%d" : "%02d", hourdraw);
      } else {
        if (s_hours == 0 || s_hours == 12) hourdraw = 12;
        else 
        hourdraw = s_hours % 12;
        snprintf(hournow, sizeof(hournow), settings.AddZero12h ? "%02d" : "%d", hourdraw);
      }

    fctx_begin_fill(&fctx);
    fctx_set_text_em_height(&fctx, Date_Font, config.font_size_digits);

          fctx_set_offset(&fctx, hour_pos);
          fctx_draw_string(&fctx, hournow, Date_Font, GTextAlignmentCenter, FTextAnchorMiddle);
          fctx_end_fill(&fctx);
  }

    //fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWDateColor, settings.DateColor));   
  //draw battery value
  if(settings.EnableBattery ){
        if(strcmp(settings.PosTop, "lo") == 0){
            fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWDateColor, settings.DateColor));
            FPoint battery_pos;
            
            int font_size_battery = config.font_size_battery;
            
            int s_battery_level = battery_state_service_peek().charge_percent;
            fctx_begin_fill(&fctx);
            fctx_set_text_em_height(&fctx, Date_Font, font_size_battery);

                // Formats and draws the battery percentage text
              if (settings.EnableBatteryLine) {

                  battery_pos.x = INT_TO_FIXED((bounds.size.w / 2));
                  battery_pos.y = INT_TO_FIXED(config.BatteryYOffset + config.yOffsetFctx + config.battery_pos_z);

                  char BatterytoDraw[6];
                  snprintf(BatterytoDraw,sizeof(BatterytoDraw),"%d",s_battery_level);

                  fctx_set_offset(&fctx, battery_pos);
                  fctx_draw_string(&fctx, BatterytoDraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                  fctx_end_fill(&fctx);
                }
                else { //shift the percentage text slightly when there's no battery line

                  battery_pos.x = INT_TO_FIXED((bounds.size.w / 2) );
                  battery_pos.y = INT_TO_FIXED(config.BatteryYOffset + config.yOffsetFctx + config.battery_pos_y);

                  char BatterytoDraw[6];
                  snprintf(BatterytoDraw,sizeof(BatterytoDraw),"%d",s_battery_level);

                  fctx_set_offset(&fctx, battery_pos);
                  fctx_draw_string(&fctx, BatterytoDraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                  fctx_end_fill(&fctx);
                }
      }

      if(strcmp(settings.PosBottom, "lo") == 0){
            fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWDateColor, settings.DateColor));
            FPoint battery_pos;
            
            int font_size_battery = config.font_size_battery;
            
            int s_battery_level = battery_state_service_peek().charge_percent;
            fctx_begin_fill(&fctx);
            fctx_set_text_em_height(&fctx, Date_Font, font_size_battery);

                // Formats and draws the battery percentage text
              if (settings.EnableBatteryLine) {

                  battery_pos.x = INT_TO_FIXED((bounds.size.w / 2));
                  battery_pos.y = INT_TO_FIXED((bounds.size.h / 2) + config.BatteryYOffset2 + config.BatteryYOffset + config.yOffsetFctx + config.battery_pos_z);

                  char BatterytoDraw[6];
                  snprintf(BatterytoDraw,sizeof(BatterytoDraw),"%d",s_battery_level);

                  fctx_set_offset(&fctx, battery_pos);
                  fctx_draw_string(&fctx, BatterytoDraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                  fctx_end_fill(&fctx);
                }
                else { //shift the percentage text slightly when there's no battery line

                  battery_pos.x = INT_TO_FIXED((bounds.size.w / 2) );
                  battery_pos.y = INT_TO_FIXED((bounds.size.h / 2) + config.BatteryYOffset2 + config.BatteryYOffset + config.yOffsetFctx + config.battery_pos_y);

                  char BatterytoDraw[6];
                  snprintf(BatterytoDraw,sizeof(BatterytoDraw),"%d",s_battery_level);

                  fctx_set_offset(&fctx, battery_pos);
                  fctx_draw_string(&fctx, BatterytoDraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                  fctx_end_fill(&fctx);
                }
      }

      if(strcmp(settings.PosLeft, "lo") == 0){
            fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWDateColor, settings.DateColor));
            FPoint battery_pos;
            
            int font_size_battery = config.font_size_battery;
            
            int s_battery_level = battery_state_service_peek().charge_percent;
            fctx_begin_fill(&fctx);
            fctx_set_text_em_height(&fctx, Date_Font, font_size_battery);

                // Formats and draws the battery percentage text
              if (settings.EnableBatteryLine) {

                  battery_pos.x = INT_TO_FIXED((bounds.size.w / 4));
                  battery_pos.y = INT_TO_FIXED((bounds.size.h / 4) + config.BatteryYOffset3 +config.BatteryYOffset + config.yOffsetFctx + config.battery_pos_z);

                  char BatterytoDraw[6];
                  snprintf(BatterytoDraw,sizeof(BatterytoDraw),"%d",s_battery_level);

                  fctx_set_offset(&fctx, battery_pos);
                  fctx_draw_string(&fctx, BatterytoDraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                  fctx_end_fill(&fctx);
                }
                else { //shift the percentage text slightly when there's no battery line

                  battery_pos.x = INT_TO_FIXED((bounds.size.w / 4) );
                  battery_pos.y = INT_TO_FIXED((bounds.size.h / 4) + config.BatteryYOffset3 + config.BatteryYOffset + config.yOffsetFctx + config.battery_pos_y);

                  char BatterytoDraw[6];
                  snprintf(BatterytoDraw,sizeof(BatterytoDraw),"%d",s_battery_level);

                  fctx_set_offset(&fctx, battery_pos);
                  fctx_draw_string(&fctx, BatterytoDraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                  fctx_end_fill(&fctx);
                }
      }

      if(strcmp(settings.PosRight, "lo") == 0){
            fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWDateColor, settings.DateColor));
            FPoint battery_pos;
            
            int font_size_battery = config.font_size_battery;
            
            int s_battery_level = battery_state_service_peek().charge_percent;
            fctx_begin_fill(&fctx);
            fctx_set_text_em_height(&fctx, Date_Font, font_size_battery);

                // Formats and draws the battery percentage text
              if (settings.EnableBatteryLine) {

                  battery_pos.x = INT_TO_FIXED((bounds.size.w*3 / 4));
                  battery_pos.y = INT_TO_FIXED((bounds.size.h / 4) + config.BatteryYOffset3 +config.BatteryYOffset + config.yOffsetFctx + config.battery_pos_z);

                  char BatterytoDraw[6];
                  snprintf(BatterytoDraw,sizeof(BatterytoDraw),"%d",s_battery_level);

                  fctx_set_offset(&fctx, battery_pos);
                  fctx_draw_string(&fctx, BatterytoDraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                  fctx_end_fill(&fctx);
                }
                else { //shift the percentage text slightly when there's no battery line

                  battery_pos.x = INT_TO_FIXED((bounds.size.w*3 / 4) );
                  battery_pos.y = INT_TO_FIXED((bounds.size.h / 4) + config.BatteryYOffset3 + config.BatteryYOffset + config.yOffsetFctx + config.battery_pos_y);

                  char BatterytoDraw[6];
                  snprintf(BatterytoDraw,sizeof(BatterytoDraw),"%d",s_battery_level);

                  fctx_set_offset(&fctx, battery_pos);
                  fctx_draw_string(&fctx, BatterytoDraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                  fctx_end_fill(&fctx);
                }
      }
  }


  //draw weekday and date text
  if (settings.EnableDate ) {

    if(strcmp(settings.PosRight, "dt") == 0){
    fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWDateColor, settings.DateColor));

    minutes = prv_tick_time->tm_min;
    hours = prv_tick_time->tm_hour % 12;

    ///use below for testing and for screenshots
    //  int seconds = 8;
       // minutes = 30;
       // hours = 9;

    FPoint weekday_pos;
    FPoint date_pos;

    int font_size_date = config.font_size_date;
   
    int xOffset = config.xOffset;
    int yOffset = config.yOffset;
    int yOffsetFctx = config.yOffsetFctx;
    int xWeekdayOffset = config.xWeekdayOffset;
    int yWeekdayDateOffset = config.yWeekdayDateOffset;
    int xDateOffset = config.xDateOffset;
    int xyMaskOffset = config.xyMaskOffset;
    int ShadowAndMaskWidth = config.ShadowAndMaskWidth;
    int ShadowAndMaskHeight = config.ShadowAndMaskHeight;
    int xOffsetFctxWeekday = config.xOffsetFctxWeekday;
    int xOffsetFctxDate = config.xOffsetFctxDate;

      weekday_pos.x = INT_TO_FIXED(bounds.size.w/2 + xOffset + xOffsetFctxWeekday + xWeekdayOffset);
      weekday_pos.y = INT_TO_FIXED(bounds.size.h/2 + yOffset + yOffsetFctx + yWeekdayDateOffset);
      date_pos.x = INT_TO_FIXED(bounds.size.w/2 + xOffset + xOffsetFctxDate + xDateOffset);
      date_pos.y = INT_TO_FIXED(bounds.size.h/2 + yOffset + yOffsetFctx + yWeekdayDateOffset);

    fctx_begin_fill(&fctx);
    fctx_set_text_em_height(&fctx, Date_Font, font_size_date);

    const char * sys_locale = i18n_get_system_locale();
    char weekday[5];
    fetchwday(s_weekday, sys_locale, weekday);

    char weekdaydraw[10];
    snprintf(weekdaydraw, sizeof(weekdaydraw), "%s", weekday);
  //  snprintf(weekdaydraw, sizeof(weekdaydraw), "%s", "WED");  //use for testing instead of line above (WED is widest text for weekday)

    fctx_set_offset(&fctx, weekday_pos);
    fctx_draw_string(&fctx, weekdaydraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
    fctx_end_fill(&fctx);



    fctx_begin_fill(&fctx);
    fctx_set_text_em_height(&fctx, Date_Font, font_size_date);

    char daynow[5];
    snprintf(daynow, sizeof(daynow), "%d", current_date);
  //  snprintf(daynow, sizeof(daynow), "%s", "30"); //use for testing instead of line above (30 is widest text for date)

    fctx_set_offset(&fctx, date_pos);
    fctx_draw_string(&fctx, daynow, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
    fctx_end_fill(&fctx);


    }



    if(strcmp(settings.PosLeft, "dt") == 0){
    fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWDateColor, settings.DateColor));

    minutes = prv_tick_time->tm_min;
    hours = prv_tick_time->tm_hour % 12;

    ///use below for testing and for screenshots
    //  int seconds = 8;
       // minutes = 30;
       // hours = 9;

    FPoint weekday_pos;
    FPoint date_pos;

    int font_size_date = config.font_size_date;
   
    int xOffset = config.xOffset;
    int yOffset = config.yOffset;
    int yOffsetFctx = config.yOffsetFctx;
    int xWeekdayOffset = config.xWeekdayOffset;
    int yWeekdayDateOffset = config.yWeekdayDateOffset;
    int xDateOffset = config.xDateOffset;
    int xyMaskOffset = config.xyMaskOffset;
    int ShadowAndMaskWidth = config.ShadowAndMaskWidth;
    int ShadowAndMaskHeight = config.ShadowAndMaskHeight;
    int xOffsetFctxWeekday = config.xOffsetFctxWeekday;
    int xOffsetFctxDate = config.xOffsetFctxDate;

      weekday_pos.x = INT_TO_FIXED(config.LeftxPosition + xOffset + xOffsetFctxWeekday + xWeekdayOffset);
      weekday_pos.y = INT_TO_FIXED(bounds.size.h/2 + yOffset + yOffsetFctx+ yWeekdayDateOffset);
      date_pos.x = INT_TO_FIXED(config.LeftxPosition + xOffset + xOffsetFctxDate + xDateOffset);
      date_pos.y = INT_TO_FIXED(bounds.size.h/2 + yOffset + yOffsetFctx + yWeekdayDateOffset);
 
    fctx_begin_fill(&fctx);
    fctx_set_text_em_height(&fctx, Date_Font, font_size_date);

    const char * sys_locale = i18n_get_system_locale();
    char weekday[5];
    fetchwday(s_weekday, sys_locale, weekday);

    char weekdaydraw[10];
    snprintf(weekdaydraw, sizeof(weekdaydraw), "%s", weekday);
  //  snprintf(weekdaydraw, sizeof(weekdaydraw), "%s", "WED");  //use for testing instead of line above (WED is widest text for weekday)

    fctx_set_offset(&fctx, weekday_pos);
    fctx_draw_string(&fctx, weekdaydraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
    fctx_end_fill(&fctx);



    fctx_begin_fill(&fctx);
    fctx_set_text_em_height(&fctx, Date_Font, font_size_date);

    char daynow[5];
    snprintf(daynow, sizeof(daynow), "%d", current_date);
  //  snprintf(daynow, sizeof(daynow), "%s", "30"); //use for testing instead of line above (30 is widest text for date)

    fctx_set_offset(&fctx, date_pos);
    fctx_draw_string(&fctx, daynow, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
    fctx_end_fill(&fctx);


    }




    if(strcmp(settings.PosBottom, "dt") == 0){
    fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWDateColor, settings.DateColor));

    minutes = prv_tick_time->tm_min;
    hours = prv_tick_time->tm_hour % 12;

    ///use below for testing and for screenshots
    //  int seconds = 8;
       // minutes = 30;
       // hours = 9;

    FPoint weekday_pos;
    FPoint date_pos;

    int font_size_date = config.font_size_date;

    int xOffset = config.xOffset;
    int yOffset = config.yOffset;
    int yOffsetFctx = config.yOffsetFctx;
    int xWeekdayOffset = config.xWeekdayOffset;
    int yWeekdayDateOffset = config.yWeekdayDateOffset;
    int xDateOffset = config.xDateOffset;
    int xyMaskOffset = config.xyMaskOffset;
    int ShadowAndMaskWidth = config.ShadowAndMaskWidth;
    int ShadowAndMaskHeight = config.ShadowAndMaskHeight;
    int xOffsetFctxWeekday = config.xOffsetFctxWeekday;
    int xOffsetFctxDate = config.xOffsetFctxDate;

      weekday_pos.x = INT_TO_FIXED(config.BottomXPosition + xOffset + xOffsetFctxWeekday + xWeekdayOffset);
      weekday_pos.y = INT_TO_FIXED(config.DateBottomYPosition + yOffset + yOffsetFctx + yWeekdayDateOffset);
      date_pos.x = INT_TO_FIXED(config.BottomXPosition + xOffset + xOffsetFctxDate + xDateOffset);
      date_pos.y = INT_TO_FIXED(config.DateBottomYPosition + yOffset + yOffsetFctx + yWeekdayDateOffset);

    fctx_begin_fill(&fctx);
    fctx_set_text_em_height(&fctx, Date_Font, font_size_date);

    const char * sys_locale = i18n_get_system_locale();
    char weekday[5];
    fetchwday(s_weekday, sys_locale, weekday);

    char weekdaydraw[10];
    snprintf(weekdaydraw, sizeof(weekdaydraw), "%s", weekday);
  //  snprintf(weekdaydraw, sizeof(weekdaydraw), "%s", "WED");  //use for testing instead of line above (WED is widest text for weekday)

    fctx_set_offset(&fctx, weekday_pos);
    fctx_draw_string(&fctx, weekdaydraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
    fctx_end_fill(&fctx);



    fctx_begin_fill(&fctx);
    fctx_set_text_em_height(&fctx, Date_Font, font_size_date);

    char daynow[5];
    snprintf(daynow, sizeof(daynow), "%d", current_date);
  //  snprintf(daynow, sizeof(daynow), "%s", "30"); //use for testing instead of line above (30 is widest text for date)

    fctx_set_offset(&fctx, date_pos);
    fctx_draw_string(&fctx, daynow, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
    fctx_end_fill(&fctx);


    }

    if(strcmp(settings.PosTop, "dt") == 0){
    fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWDateColor, settings.DateColor));

    minutes = prv_tick_time->tm_min;
    hours = prv_tick_time->tm_hour % 12;

    FPoint weekday_pos;
    FPoint date_pos;

    int font_size_date = config.font_size_date;
   
    int xOffset = config.xOffset;
    int yOffset = config.yOffset;
    int yOffsetFctx = config.yOffsetFctx;
    int xWeekdayOffset = config.xWeekdayOffset;
    int yWeekdayDateOffset = config.yWeekdayDateOffset;
    int xDateOffset = config.xDateOffset;
    int xyMaskOffset = config.xyMaskOffset;
    int ShadowAndMaskWidth = config.ShadowAndMaskWidth;
    int ShadowAndMaskHeight = config.ShadowAndMaskHeight;
    int xOffsetFctxWeekday = config.xOffsetFctxWeekday;
    int xOffsetFctxDate = config.xOffsetFctxDate;

      weekday_pos.x = INT_TO_FIXED(config.BottomXPosition + xOffset + xOffsetFctxWeekday + xWeekdayOffset);
      weekday_pos.y = INT_TO_FIXED(bounds.size.h/4);
      date_pos.x = INT_TO_FIXED(config.BottomXPosition + xOffset + xOffsetFctxDate + xDateOffset);
      date_pos.y = INT_TO_FIXED(bounds.size.h/4);
    
   
    fctx_begin_fill(&fctx);
    fctx_set_text_em_height(&fctx, Date_Font, font_size_date);

    const char * sys_locale = i18n_get_system_locale();
    char weekday[5];
    fetchwday(s_weekday, sys_locale, weekday);

    char weekdaydraw[10];
    snprintf(weekdaydraw, sizeof(weekdaydraw), "%s", weekday);
 
    fctx_set_offset(&fctx, weekday_pos);
    fctx_draw_string(&fctx, weekdaydraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
    fctx_end_fill(&fctx);



    fctx_begin_fill(&fctx);
    fctx_set_text_em_height(&fctx, Date_Font, font_size_date);

    char daynow[5];
    snprintf(daynow, sizeof(daynow), "%d", current_date);
  //  snprintf(daynow, sizeof(daynow), "%s", "30"); //use for testing instead of line above (30 is widest text for date)

    fctx_set_offset(&fctx, date_pos);
    fctx_draw_string(&fctx, daynow, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
    fctx_end_fill(&fctx);


    }
  }


    if (settings.EnableLogo) {  //draw logo text
      if(strcmp(settings.PosTop, "lo") == 0){
          fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWDateColor, settings.DateColor));
          FPoint logo_pos;

          logo_pos.x = INT_TO_FIXED((bounds.size.w / 2));
          logo_pos.y = INT_TO_FIXED(config.LogoYOffset);

            int font_size_logo = config.font_size_logo;
            #ifdef PBL_PLATFORM_EMERY
              char logodraw[20];
            #elif defined (PBL_PLATFORM_GABBRO)
              char logodraw[20];
            #else
              char logodraw[20];
            #endif
            snprintf(logodraw, sizeof(logodraw), "%s", settings.LogoText);

            

            #ifdef PBL_PLATFORM_EMERY
              #define LOGO_WRAP_AT 8
            #elif defined (PBL_PLATFORM_GABBRO)
              #define LOGO_WRAP_AT 18
            #else
              #define LOGO_WRAP_AT 12
            #endif

            char *line2 = NULL;
            if (strlen(logodraw) > LOGO_WRAP_AT) {
                char *split = NULL;
                for (int i = LOGO_WRAP_AT; i >= 0; i--) {
                    if (logodraw[i] == ' ') {
                        split = &logodraw[i];
                        break;
                    }
                }
                if (split) {
                    *split = '\0';
                    line2 = split + 1;
                }
                // No space found = single word, leave line2 NULL and draw as one line
            }

          if (line2) {
                // Draw line 1
                fctx_begin_fill(&fctx);
                fctx_set_text_em_height(&fctx, Date_Font, font_size_logo);
                fctx_set_offset(&fctx, logo_pos);   
                fctx_draw_string(&fctx, logodraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                fctx_end_fill(&fctx);

                // Draw line 2
            logo_pos.y = INT_TO_FIXED(config.LogoYOffset + font_size_logo + 2);
                  fctx_begin_fill(&fctx);
                  fctx_set_text_em_height(&fctx, Date_Font, font_size_logo);
                  fctx_set_offset(&fctx, logo_pos);   
                  fctx_draw_string(&fctx, line2, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                  fctx_end_fill(&fctx);

            } else {
                fctx_begin_fill(&fctx);
                fctx_set_text_em_height(&fctx, Date_Font, font_size_logo);
                fctx_set_offset(&fctx, logo_pos);
                fctx_draw_string(&fctx, logodraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                fctx_end_fill(&fctx);
           }


            
      }

      if(strcmp(settings.PosBottom, "lo") == 0){
          fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWDateColor, settings.DateColor));
          FPoint logo_pos;
          logo_pos.x = INT_TO_FIXED((bounds.size.w / 2));
          logo_pos.y = INT_TO_FIXED(config.LogoYOffset + config.LogoYOffset3);
            int font_size_logo = config.font_size_logo;
            char logodraw[20];
            snprintf(logodraw, sizeof(logodraw), "%s", settings.LogoText);
            char *line2 = NULL;
            if (strlen(logodraw) > LOGO_WRAP_AT) {
                char *split = NULL;
                for (int i = LOGO_WRAP_AT; i >= 0; i--) {
                    if (logodraw[i] == ' ') { split = &logodraw[i]; break; }
                }
                if (split) { *split = '\0'; line2 = split + 1; }
            }
            if (line2) {
                fctx_begin_fill(&fctx);
                fctx_set_text_em_height(&fctx, Date_Font, font_size_logo);
                fctx_set_offset(&fctx, logo_pos);
                fctx_draw_string(&fctx, logodraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                fctx_end_fill(&fctx);
                logo_pos.y = INT_TO_FIXED(config.LogoYOffset + config.LogoYOffset3 + font_size_logo + 2);
                fctx_begin_fill(&fctx);
                fctx_set_text_em_height(&fctx, Date_Font, font_size_logo);
                fctx_set_offset(&fctx, logo_pos);
                fctx_draw_string(&fctx, line2, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                fctx_end_fill(&fctx);
            } else {
                fctx_begin_fill(&fctx);
                fctx_set_text_em_height(&fctx, Date_Font, font_size_logo);
                fctx_set_offset(&fctx, logo_pos);
                fctx_draw_string(&fctx, logodraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                fctx_end_fill(&fctx);
            }
      }

      if(strcmp(settings.PosLeft, "lo") == 0){
          fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWDateColor, settings.DateColor));
          FPoint logo_pos;
          logo_pos.x = INT_TO_FIXED((bounds.size.w / 4));
          logo_pos.y = INT_TO_FIXED(bounds.size.h / 2 - config.LogoYOffset2/2);
            int font_size_logo = config.font_size_logo;
            char logodraw[20];
            snprintf(logodraw, sizeof(logodraw), "%s", settings.LogoText);
            char *line2 = NULL;
            if (strlen(logodraw) > LOGO_WRAP_AT) {
                char *split = NULL;
                for (int i = LOGO_WRAP_AT; i >= 0; i--) {
                    if (logodraw[i] == ' ') { split = &logodraw[i]; break; }
                }
                if (split) { *split = '\0'; line2 = split + 1; }
            }
            if (line2) {
                fctx_begin_fill(&fctx);
                fctx_set_text_em_height(&fctx, Date_Font, font_size_logo);
                fctx_set_offset(&fctx, logo_pos);
                fctx_draw_string(&fctx, logodraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                fctx_end_fill(&fctx);
                logo_pos.y = INT_TO_FIXED(bounds.size.h / 2 - config.LogoYOffset2/2 + font_size_logo + 2);
                fctx_begin_fill(&fctx);
                fctx_set_text_em_height(&fctx, Date_Font, font_size_logo);
                fctx_set_offset(&fctx, logo_pos);
                fctx_draw_string(&fctx, line2, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                fctx_end_fill(&fctx);
            } else {
                fctx_begin_fill(&fctx);
                fctx_set_text_em_height(&fctx, Date_Font, font_size_logo);
                fctx_set_offset(&fctx, logo_pos);
                fctx_draw_string(&fctx, logodraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                fctx_end_fill(&fctx);
            }
      }
      if(strcmp(settings.PosRight, "lo") == 0){
          fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWDateColor, settings.DateColor));
          FPoint logo_pos;
          logo_pos.x = INT_TO_FIXED((bounds.size.w*3 / 4));
          logo_pos.y = INT_TO_FIXED(bounds.size.h / 2 - config.LogoYOffset2/2);
            int font_size_logo = config.font_size_logo;
            char logodraw[20];
            snprintf(logodraw, sizeof(logodraw), "%s", settings.LogoText);
            char *line2 = NULL;
            if (strlen(logodraw) > LOGO_WRAP_AT) {
                char *split = NULL;
                for (int i = LOGO_WRAP_AT; i >= 0; i--) {
                    if (logodraw[i] == ' ') { split = &logodraw[i]; break; }
                }
                if (split) { *split = '\0'; line2 = split + 1; }
            }
            if (line2) {
                fctx_begin_fill(&fctx);
                fctx_set_text_em_height(&fctx, Date_Font, font_size_logo);
                fctx_set_offset(&fctx, logo_pos);
                fctx_draw_string(&fctx, logodraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                fctx_end_fill(&fctx);
                logo_pos.y = INT_TO_FIXED(bounds.size.h / 2 - config.LogoYOffset2/2 + font_size_logo + 2);
                fctx_begin_fill(&fctx);
                fctx_set_text_em_height(&fctx, Date_Font, font_size_logo);
                fctx_set_offset(&fctx, logo_pos);
                fctx_draw_string(&fctx, line2, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                fctx_end_fill(&fctx);
            } else {
                fctx_begin_fill(&fctx);
                fctx_set_text_em_height(&fctx, Date_Font, font_size_logo);
                fctx_set_offset(&fctx, logo_pos);
                fctx_draw_string(&fctx, logodraw, Date_Font, GTextAlignmentCenter, FTextAnchorTop);
                fctx_end_fill(&fctx);
            }
      }
    }

    if(settings.showlocalAMPM && !clock_is_24h_style()){

      if(strcmp(settings.PosBottom, "ap") == 0){
          fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWTextColor2, settings.HoursHandColor));
          FPoint ampm_pos;

          ampm_pos.x = INT_TO_FIXED((bounds.size.w / 2));
          ampm_pos.y = INT_TO_FIXED((bounds.size.h*3 / 4));


          char local_ampm_string[5];
          strftime(local_ampm_string, sizeof(local_ampm_string), "%p", prv_tick_time);
   
            fctx_begin_fill(&fctx);
            fctx_set_text_em_height(&fctx, Date_Font, config.font_size_battery);
                  fctx_set_offset(&fctx, ampm_pos);
                  fctx_draw_string(&fctx, local_ampm_string, Date_Font, GTextAlignmentCenter, FTextAnchorMiddle);
                  fctx_end_fill(&fctx);
       }

       if(strcmp(settings.PosTop, "ap") == 0){
          fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWTextColor2, settings.HoursHandColor));
          FPoint ampm_pos;

          ampm_pos.x = INT_TO_FIXED((bounds.size.w / 2));
          ampm_pos.y = INT_TO_FIXED((bounds.size.h / 4));


          char local_ampm_string[5];
          strftime(local_ampm_string, sizeof(local_ampm_string), "%p", prv_tick_time);
   
            fctx_begin_fill(&fctx);
            fctx_set_text_em_height(&fctx, Date_Font, config.font_size_battery);
                  fctx_set_offset(&fctx, ampm_pos);
                  fctx_draw_string(&fctx, local_ampm_string, Date_Font, GTextAlignmentCenter, FTextAnchorMiddle);
                  fctx_end_fill(&fctx);
       }

       if(strcmp(settings.PosLeft, "ap") == 0){
          fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWTextColor2, settings.HoursHandColor));
          FPoint ampm_pos;

          ampm_pos.x = INT_TO_FIXED((bounds.size.w / 4));
          ampm_pos.y = INT_TO_FIXED((bounds.size.h / 2));


          char local_ampm_string[5];
          strftime(local_ampm_string, sizeof(local_ampm_string), "%p", prv_tick_time);
   
            fctx_begin_fill(&fctx);
            fctx_set_text_em_height(&fctx, Date_Font, config.font_size_battery);
                  fctx_set_offset(&fctx, ampm_pos);
                  fctx_draw_string(&fctx, local_ampm_string, Date_Font, GTextAlignmentCenter, FTextAnchorMiddle);
                  fctx_end_fill(&fctx);
       }

      if(strcmp(settings.PosRight, "ap") == 0){
          fctx_set_fill_color(&fctx, PBL_IF_BW_ELSE(settings.BWTextColor2, settings.HoursHandColor));
          FPoint ampm_pos;

          ampm_pos.x = INT_TO_FIXED((bounds.size.w * 3 / 4));
          ampm_pos.y = INT_TO_FIXED((bounds.size.h / 2));


          char local_ampm_string[5];
          strftime(local_ampm_string, sizeof(local_ampm_string), "%p", prv_tick_time);
   
            fctx_begin_fill(&fctx);
            fctx_set_text_em_height(&fctx, Date_Font, config.font_size_battery);
                  fctx_set_offset(&fctx, ampm_pos);
                  fctx_draw_string(&fctx, local_ampm_string, Date_Font, GTextAlignmentCenter, FTextAnchorMiddle);
                  fctx_end_fill(&fctx);
       }
    } 




    fctx_deinit_context(&fctx);
      
}
#endif

// static void layer_update_proc_dial_digits_mask(Layer *layer, GContext *ctx) {   //background squares and shadows for the 12 and 6 digits
//   GRect bounds = layer_get_bounds(s_dial_layer);
//   graphics_context_set_antialiased(ctx, true);
//   graphics_context_set_fill_color(ctx, PBL_IF_BW_ELSE(settings.BWBackgroundColor2, settings.BackgroundColor2));

//   GRect dial_digits_mask_a = config.dial_digits_mask_a[0]; //shadow mask for behind the 12 digit
//   graphics_fill_rect(ctx, dial_digits_mask_a, 1, GCornersAll);

//   graphics_context_set_fill_color(ctx, PBL_IF_BW_ELSE(settings.BWBackgroundColor1, settings.BackgroundColor1));


// GRect dial_digits_mask_b = config.dial_digits_mask_b[0];  //face colour mask for behind the 12 digit
// graphics_fill_rect(ctx, dial_digits_mask_b, 1, GCornersAll);

// GRect dial_digits_mask_c = config.dial_digits_mask_c[0]; //face color mask for behind the 6 digit
// graphics_fill_rect(ctx, dial_digits_mask_c, 1, GCornersAll);

// }

// Update procedure for the seconds hand layer
// static void layer_update_proc_seconds_hand(Layer *layer, GContext *ctx) {
//   if (!showSeconds || !prv_tick_time) {
//       // Do not draw the second hand if it should be hidden or if time data is not yet available
//       return;
//     }

//   GRect bounds = layer_get_bounds(layer);
//   seconds = prv_tick_time->tm_sec;
//   //for test & screenshots
//   //int
//   // seconds = 8;

//   if (!settings.EnableSecondsHand || !showSeconds) {
//     seconds = 0;
//   }

//   int seconds_angle = ((double)seconds / 60 * 360) - 90;

//   draw_line_hand(ctx, seconds_angle, bounds.size.w/2 - config.second_hand_a, config.second_hand_b,  PBL_IF_BW_ELSE(settings.BWTextColor2, settings.SecondsHandColor));
//   draw_seconds_center(ctx, PBL_IF_BW_ELSE(settings.BWTextColor1, settings.HoursHandColor), PBL_IF_BW_ELSE(settings.BWTextColor2, settings.SecondsHandColor));
// }

static void layer_update_proc_battery_line(Layer *layer, GContext *ctx) {
    // If neither element is enabled in config, stop.
    if (!settings.EnableBattery && !settings.EnableBatteryLine) {
        return;
    }

    int s_battery_level = battery_state_service_peek().charge_percent;

    // Draw battery line
    if(strcmp(settings.PosTop, "lo") == 0){
    if (settings.EnableBatteryLine) {
        int width_rect = (s_battery_level * config.battery_line) / 100;
        int rect_x_pos = (bounds.size.w/2) - (width_rect/2);

        GRect BatteryLineRect = GRect(rect_x_pos,config.BatteryLineYOffset,width_rect, 2);
        graphics_context_set_antialiased(ctx, true);
        graphics_context_set_fill_color(ctx, PBL_IF_BW_ELSE(settings.BWTextColor1, settings.BatteryLineColor));
        graphics_fill_rect(ctx,BatteryLineRect, 1, GCornersBottom);
    }
    }

    if(strcmp(settings.PosBottom, "lo") == 0){
    if (settings.EnableBatteryLine) {
        int width_rect = (s_battery_level * config.battery_line) / 100;
        int rect_x_pos = (bounds.size.w/2) - (width_rect/2);

        GRect BatteryLineRect = GRect(rect_x_pos,config.BatteryLineYOffset +config.BatteryLineYOffset2,width_rect, 2);
        graphics_context_set_antialiased(ctx, true);
        graphics_context_set_fill_color(ctx, PBL_IF_BW_ELSE(settings.BWTextColor1, settings.BatteryLineColor));
        graphics_fill_rect(ctx,BatteryLineRect, 1, GCornersBottom);
    }
    }

    if(strcmp(settings.PosLeft, "lo") == 0){
    if (settings.EnableBatteryLine) {
        int width_rect = (s_battery_level * config.battery_line) / 100;
        int rect_x_pos = (bounds.size.w/4) - (width_rect/2);

        GRect BatteryLineRect = GRect(rect_x_pos,(bounds.size.h/2),width_rect, 2);
        graphics_context_set_antialiased(ctx, true);
        graphics_context_set_fill_color(ctx, PBL_IF_BW_ELSE(settings.BWTextColor1, settings.BatteryLineColor));
        graphics_fill_rect(ctx,BatteryLineRect, 1, GCornersBottom);
    }
    }

    if(strcmp(settings.PosRight, "lo") == 0){
    if (settings.EnableBatteryLine) {
        int width_rect = (s_battery_level * config.battery_line) / 100;
        int rect_x_pos = (bounds.size.w*3/4) - (width_rect/2);

        GRect BatteryLineRect = GRect(rect_x_pos,(bounds.size.h/2),width_rect, 2);
        graphics_context_set_antialiased(ctx, true);
        graphics_context_set_fill_color(ctx, PBL_IF_BW_ELSE(settings.BWTextColor1, settings.BatteryLineColor));
        graphics_fill_rect(ctx,BatteryLineRect, 1, GCornersBottom);
    }
    }
}

//Update procedure for the Bluetooth Icon (shows when disconnected) layer
static void layer_update_proc_bt(Layer * layer, GContext * ctx){
  GRect bounds = layer_get_bounds(layer);
   minutes = prv_tick_time->tm_min;
   hours = prv_tick_time->tm_hour % 12;

//use this for testing
   // minutes = 30;
   // hours = 9;

      int xPosition;
      int yPosition;
      int textboxwidth;
      int BTIconYOffset;

      // if (((hours > 7 && hours < 11) && (minutes > 7 && minutes < 23))||
      // ((hours <5 && hours >1) && (minutes > 37 && minutes < 53))){
        // Bottom position
      
      #ifdef PBL_BW 
              xPosition = config.BottomXPosition + 2;
              yPosition = config.BTQTBottomYPosition;
              textboxwidth = config.ShadowAndMaskWidth/2;
              BTIconYOffset = config.BTIconYOffset;
      #elif defined (PBL_PLATFORM_BASALT)
              xPosition = config.BottomXPosition + 2;
              yPosition = config.BTQTBottomYPosition;
              textboxwidth = config.ShadowAndMaskWidth/2;
              BTIconYOffset = config.BTIconYOffset;
      #else
            if(quiet_time_is_active()){
              xPosition = config.BottomXPosition + 2;
              yPosition = config.BTQTBottomYPosition;
              textboxwidth = config.ShadowAndMaskWidth/2;
              BTIconYOffset = config.BTIconYOffset;
            }
            else{
              xPosition = config.BottomXPosition;
              yPosition = config.BTQTBottomYPosition;
              textboxwidth = config.ShadowAndMaskWidth;
              BTIconYOffset = config.BTIconYOffset;
            }
        #endif
      // }
      // else if ((minutes > 7 && minutes < 23)||
      // ((hours <5 && hours >1) && (minutes > 52 || minutes < 37))){
      //   // Left position
      //   xPosition = config.LeftxPosition;
      //   yPosition = bounds.size.h/2;
      //   textboxwidth = config.ShadowAndMaskWidth;
      //   BTIconYOffset = config.BTIconYOffset;
      // }
      // else{
      //   // Right position
      //   xPosition = bounds.size.w/2;
      //   yPosition = bounds.size.h/2;
      //   textboxwidth = config.ShadowAndMaskWidth;
      //   BTIconYOffset = config.BTIconYOffset;
      // }

  GRect BTIconRect =
    GRect(xPosition + config.xOffset + config.BTIconXOffset2, yPosition + config.yOffset + BTIconYOffset + config.BTIconYOffset2, textboxwidth, 20);


#ifdef PBL_COLOR
 graphics_context_set_text_color(ctx, settings.BTQTColor);
 #else
  graphics_context_set_text_color(ctx, settings.BWBTQTColor);
 #endif

 //bluetooth_callback(connection_service_peek_pebble_app_connection());
 graphics_context_set_antialiased(ctx, true);
 graphics_draw_text(ctx, "z", FontBTQTIcons, BTIconRect, GTextOverflowModeFill,GTextAlignmentCenter, NULL);


}

//Update procedure for the QT Icon layer (shows when Quiet time is active)
static void layer_update_proc_qt(Layer * layer, GContext * ctx){

  GRect bounds = layer_get_bounds(layer);
   minutes = prv_tick_time->tm_min;
   hours = prv_tick_time->tm_hour % 12;

//use this for testing
   // minutes = 30;
   // hours = 9;

      int xPosition;
      int yPosition;
      int textboxwidth;
      int QTIconYOffset;


   //   if (((hours > 7 && hours < 11) && (minutes > 7 && minutes < 23))||
   //   ((hours <5 && hours >1) && (minutes > 37 && minutes < 53))){
        // Bottom position
     // 

      #ifdef PBL_BW
        xPosition = config.BottomXPosition;
        yPosition = config.BTQTBottomYPosition;
        textboxwidth = config.ShadowAndMaskWidth;
        QTIconYOffset = 0 - config.QTIconYOffset;
      #elif defined (PBL_PLATFORM_BASALT)
        xPosition = config.BottomXPosition;
        yPosition = config.BTQTBottomYPosition;
        textboxwidth = config.ShadowAndMaskWidth;
        QTIconYOffset = 0 - config.QTIconYOffset;
      #else
       if(connection_service_peek_pebble_app_connection()){

        xPosition = config.BottomXPosition;
        yPosition = config.BTQTBottomYPosition -1;
        textboxwidth = config.ShadowAndMaskWidth;
        QTIconYOffset = 0 - config.QTIconYOffset;
       }
      else{
        xPosition = config.BottomXPosition + config.ShadowAndMaskWidth/2 - 2;
        yPosition = config.BTQTBottomYPosition -1 ;
        textboxwidth = config.ShadowAndMaskWidth/2;
        QTIconYOffset = 0 - config.QTIconYOffset;
      }
      #endif
      // }
      // else if ((minutes > 7 && minutes < 23)||
      // ((hours <5 && hours >1) && (minutes > 52 || minutes < 37))){
      //   // Left position
      //   xPosition = config.LeftxPosition;
      //   yPosition = bounds.size.h/2;
      //   textboxwidth = config.ShadowAndMaskWidth;
      //   QTIconYOffset = config.QTIconYOffset;
      // }
      // else{
      //   // Right position
      //   xPosition = bounds.size.w/2;
      //   yPosition = bounds.size.h/2;
      //   textboxwidth = config.ShadowAndMaskWidth;
      //   QTIconYOffset = config.QTIconYOffset;
      // }

  GRect QTIconRect =
    GRect(xPosition + config.xOffset + config.QTIconXOffset2, yPosition + config.yOffset + QTIconYOffset + config.QTIconYOffset2, textboxwidth, 20);

 quiet_time_icon(); //checks whether quiet time is active

 #ifdef PBL_COLOR
  graphics_context_set_text_color(ctx, settings.BTQTColor);
  #else
   graphics_context_set_text_color(ctx, settings.BWBTQTColor);
  #endif
  graphics_context_set_antialiased(ctx, true);
  graphics_draw_text(ctx, "\U0000E061", FontBTQTIcons, QTIconRect, GTextOverflowModeFill,GTextAlignmentCenter, NULL);

}

// Update procedure for the main canvas layer (hour & minute hands)
static void hour_min_hands_canvas_update_proc(Layer *layer, GContext *ctx) {

 GRect bounds = layer_get_bounds(layer);

//use these for live version

   minutes = prv_tick_time->tm_min;
   //hours = prv_tick_time->tm_hour % 12;


  ///use below for testing and for screenshots
  //  int seconds = 8;
    // int minutes = 30;
    // int hours = 9;
  //  APP_LOG(APP_LOG_LEVEL_DEBUG, "minutes in hour_min_hands_canvas_update_proc is %d", minutes);

  // #ifdef SECOND
  //   seconds = SECOND;
  // #endif

  // #ifdef HOUR
  //   hours = HOUR;
  // #endif

  #ifdef MINUTE
    minutes = MINUTE;
  #endif

  // int hours_angle = ((double)hours / 12 * 360) + ((double)minutes / 60 * 360 / 12) + /*((double)seconds / 60 * 360 / 60 / 12)*/  - 90;

  //draw_fancy_hand_hour(ctx, hours_angle, bounds.size.w / 2 - config.hour_hand_a, PBL_IF_BW_ELSE(settings.BWTextColor1,settings.HoursHandColor), PBL_IF_BW_ELSE(settings.BWTextColor2,settings.HoursHandBorderColor));

   int minutes_angle = ((double)minutes / 60 * 360) + /*((double)seconds / 60 * 360 / 60)*/ - 90;

   //draw_fancy_hand_min(ctx, minutes_angle, bounds.size.w / 2 - config.min_hand_a, PBL_IF_BW_ELSE(settings.BWTextColor1,settings.MinutesHandColor), PBL_IF_BW_ELSE(settings.BWTextColor2,settings.MinutesHandBorderColor));
  #ifdef PBL_ROUND
   draw_line_hand(ctx, minutes_angle,
       bounds.size.w/2 - config.second_hand_a, 
       config.second_hand_b,  
       PBL_IF_BW_ELSE(settings.BWTextColor1, settings.MinutesHandColor));
   draw_seconds_center(ctx, PBL_IF_BW_ELSE(settings.BWTextColor1, settings.MinutesHandColor), PBL_IF_BW_ELSE(settings.BWBackgroundColor1, settings.BackgroundColor1));
  #else
  if(settings.ForegroundShape){
   draw_line_hand(ctx, minutes_angle, 
        bounds.size.w/2 - config.second_hand_a, 
        config.second_hand_b,  
        PBL_IF_BW_ELSE(settings.BWTextColor1, settings.MinutesHandColor));
   draw_seconds_center(ctx, PBL_IF_BW_ELSE(settings.BWTextColor1, settings.MinutesHandColor), PBL_IF_BW_ELSE(settings.BWBackgroundColor1, settings.BackgroundColor1));
  }
  else{
    draw_line_hand(ctx, minutes_angle, 
        bounds.size.w/2 - config.second_hand_a, 
        config.second_hand_b,  
        PBL_IF_BW_ELSE(settings.BWTextColor1, settings.MinutesHandColor));
    draw_seconds_center(ctx, PBL_IF_BW_ELSE(settings.BWTextColor1, settings.MinutesHandColor), PBL_IF_BW_ELSE(settings.BWBackgroundColor1, settings.BackgroundColor1));
  }
  #endif
  

//  #ifdef PBL_COLOR
//    draw_center(ctx, settings.HoursHandBorderColor, settings.HoursHandColor);
//  #else
//    draw_center(ctx, settings.BWTextColor2, settings.BWTextColor1);
//  #endif
}

///update procedure for background
static void bg_update_proc(Layer *layer, GContext *ctx) {

  GRect bounds = layer_get_bounds(layer);

  GRect Background =
       GRect(0, 0, bounds.size.w, bounds.size.h);

   graphics_context_set_fill_color(ctx,PBL_IF_BW_ELSE(settings.BWBackgroundColor1, settings.BackgroundColor1) );
   graphics_fill_rect(ctx, Background,0,GCornersAll);

   // Draw the large circle with a shadow
      // GPoint center = GPoint(bounds.size.w / 2, bounds.size.h / 2);

      // int circle_radius = (bounds.size.h / 2) - config.circle_radius_adj; // Adjust struct value as needed
      // int tick_mask_radius = circle_radius + config.tick_mask_radius_adj; //adjust in structs

    //  #ifdef PBL_ROUND
    //   int circle_radius = (bounds.size.h / 2) - 17; // Adjust this value as needed
    //   int tick_mask_radius = circle_radius +12;
    //  #else
    //    int circle_radius = (bounds.size.h / 2) - 18; // Adjust this value as needed
    //    int tick_mask_radius = circle_radius +12;
    //  #endif

    // draw_circle_shadow(ctx, center, circle_radius, PBL_IF_BW_ELSE(settings.BWBackgroundColor1, settings.BackgroundColor1), PBL_IF_BW_ELSE(settings.BWBackgroundColor2, settings.BackgroundColor2));

      // for (int i = 0; i < 12; i++) {
      //   if (i == 6 || i == 12) continue;
      //   int angle = i * 30 - 90;
      //   draw_major_tick_shadow(ctx, angle, 15, PBL_IF_BW_ELSE(settings.BWTextColor1, settings.HoursHandColor), PBL_IF_BW_ELSE(settings.BWTextColor3, settings.BackgroundColor2));
      // }

      // draw_major_tick_mask(ctx, tick_mask_radius, PBL_IF_BW_ELSE(settings.BWBackgroundColor1, settings.BackgroundColor1));


    if(settings.showMinorTick){
        for (int i = 0; i < 60; i++) {
        //if (i % 5 == 0) continue;
        int angle = i * 6;
        draw_minor_tick(ctx, angle, PBL_IF_BW_ELSE(settings.BWTextColor3, settings.TextColor2));
      }
    }

    if(settings.showMajorTick){
      for (int i = 0; i < 12; i++) {
        if (i == 6 || i == 12 || i == 3 || i == 9 || i == 0) {
        int angle = i * 30 - 90;
        draw_major_tick(ctx, angle, 16, PBL_IF_BW_ELSE(settings.BWBackgroundColor1, settings.TickColor), PBL_IF_BW_ELSE(settings.BWTextColor3, settings.TickColor));
      }

      else {
        //if (i == 6 || i == 12) continue;
        int angle = i * 30 - 90;
        draw_major_tick(ctx, angle, 16, PBL_IF_BW_ELSE(settings.BWBackgroundColor1, settings.BackgroundColor1), PBL_IF_BW_ELSE(settings.BWTextColor3, settings.TextColor2));
      }
    }
    }

    


}


static void prv_window_load(Window *window) {
  time_t temp = time(NULL);
  prv_tick_time = localtime(&temp);
  current_date = prv_tick_time->tm_mday;
  s_weekday = prv_tick_time->tm_wday;
  minutes = prv_tick_time->tm_min;
  hours = prv_tick_time->tm_hour % 12;
  s_hours = prv_tick_time->tm_hour;

  Layer *window_layer = window_get_root_layer(window);
  bounds = layer_get_bounds(window_layer);

  Layer *seconds_root_layer = window_get_root_layer(window);
  bounds_seconds = layer_get_bounds(seconds_root_layer);

  // Load fctx ffonts
   // Digits_Font =  ffont_create_from_resource(RESOURCE_ID_FONT_DIGITS_FCTX);
    Date_Font =  ffont_create_from_resource(RESOURCE_ID_FONT_DATE_FCTX);
    FontBTQTIcons = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_16));
    //non-fctx custom fonts for B&W screens
    #ifdef PBL_BW 
    FontDate = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATE_9));
    FontBattery = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATE_10));
    FontLogo = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATE_8));
    FontHour = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATE_24));
    #endif
  // Subscribe to the connection service to get Bluetooth status updates.
  connection_service_subscribe((ConnectionHandlers){
    .pebble_app_connection_handler = bluetooth_vibe_icon
  });

  // Subscribe to the correct tick service based on settings
    if (settings.EnableSecondsHand) {
        if (settings.SecondsVisibleTime == 135) {
        tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
        } else {
        tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
        s_timeout_timer = app_timer_register(1000*settings.SecondsVisibleTime, timeout_handler,NULL);
        accel_tap_service_subscribe(accel_tap_handler);
        }
    }
    else {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    }
    showSeconds = settings.EnableSecondsHand;

  //create layers
  s_bg_layer = layer_create(bounds);
  s_dial_layer = layer_create(bounds);
  s_canvas_qt_icon = layer_create(bounds);
     quiet_time_icon();
  s_canvas_bt_icon = layer_create(bounds);
    bool is_connected = connection_service_peek_pebble_app_connection();
    layer_set_hidden(s_canvas_bt_icon, is_connected);
  s_canvas_battery = layer_create(bounds);
  s_canvas_layer = layer_create(bounds);
  s_canvas_second_hand = layer_create(bounds_seconds);
  //s_dial_digits_layer = layer_create(bounds);
  s_date_battery_logo_layer = layer_create(bounds);

  // Change the order here
  layer_add_child(window_layer, s_bg_layer); //backforound, circles, major tick shoadow &tickmask
  //layer_add_child(window_layer, s_dial_layer); //12 and 6 digit mask
  //layer_add_child(window_layer, s_dial_digits_layer); //12 and 6 text fctx
  layer_add_child(window_layer, s_canvas_bt_icon);
  layer_add_child(window_layer, s_canvas_qt_icon);
  layer_add_child(window_layer, s_date_battery_logo_layer); //fctx version of text
  layer_add_child(window_layer, s_canvas_battery); //battery line
  layer_add_child(window_layer, s_canvas_layer);  //hour and minute hands
  layer_add_child(seconds_root_layer, s_canvas_second_hand);  //second hand

  bluetooth_vibe_icon(connection_service_peek_pebble_app_connection());

  layer_set_update_proc(s_bg_layer, bg_update_proc);
  //layer_set_update_proc(s_dial_layer, layer_update_proc_dial_digits_mask);
  //layer_set_update_proc(s_dial_digits_layer, update_dial_digits_fctx_layer);
  layer_set_update_proc(s_canvas_bt_icon, layer_update_proc_bt);
  layer_set_update_proc(s_canvas_qt_icon, layer_update_proc_qt);
  layer_set_update_proc(s_date_battery_logo_layer, update_logo_date_battery_fctx_layer);
  layer_set_update_proc(s_canvas_battery, layer_update_proc_battery_line);
  layer_set_update_proc(s_canvas_layer, hour_min_hands_canvas_update_proc);
  //layer_set_update_proc(s_canvas_second_hand, layer_update_proc_seconds_hand);


}


static void prv_window_unload(Window *window) {
  if (s_timeout_timer) {
    app_timer_cancel(s_timeout_timer);
  }
  accel_tap_service_unsubscribe();
  connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  tick_timer_service_unsubscribe();
  layer_destroy(s_canvas_layer);
  layer_destroy(s_bg_layer);
  layer_destroy(s_dial_layer);
  layer_destroy(s_canvas_second_hand);
  layer_destroy(s_canvas_battery);
  layer_destroy(s_canvas_bt_icon);
  layer_destroy(s_canvas_qt_icon);
 // layer_destroy(s_dial_digits_layer);
  layer_destroy(s_date_battery_logo_layer);
 // ffont_destroy(Digits_Font);
  ffont_destroy(Date_Font);
  #ifdef PBL_BW
  fonts_unload_custom_font(FontDate);
  fonts_unload_custom_font(FontBattery);
  fonts_unload_custom_font(FontLogo);
  fonts_unload_custom_font(FontHour);
  #endif
  fonts_unload_custom_font(FontBTQTIcons);
}

static void prv_init(void) {
  prv_load_settings();

  // Open AppMessage and set the message handler
  app_message_open(512, 512);
  app_message_register_inbox_received(prv_inbox_received_handler);

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });

  window_stack_push(s_window, true);
}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}