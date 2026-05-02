module.exports = [
  {
    "type": "heading",
    "defaultValue": "monologue"
  },
  {
    "type": "text",
    "defaultValue": "<p>by astosia</p>"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Features"
      },
      {
        "type": "toggle",
        "label": "Watchface Style",
        "messageKey": "ForegroundShape",
        "description": "Off = Rectangular, On = Round",
        "capabilities": [
          "RECT"
        ],
        "defaultValue": true
      },
      {
        "type": "toggle",
        "label": "Add leading zero to 12h time",
        "messageKey": "AddZero12h",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "label": "Remove leading zero from 24h time",
        "messageKey": "RemoveZero24h",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "label": "Date visible",
        "messageKey": "EnableDate",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "label": "Logo visible",
        "messageKey": "EnableLogo",
        "defaultValue": true
      },
      {
         "type": "input",
         "messageKey": "LogoText",
         "defaultValue": "monologue",
         "label": "Custom Logo Text",
         "attributes": {
           "placeholder": "monologue"
         }
      },
      {
        "type": "toggle",
        "label": "Battery Value visible",
        "messageKey": "EnableBattery",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "label": "Battery Line visible",
        "messageKey": "EnableBatteryLine",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "label": "Show Major Tickmarks",
        "messageKey": "showMajorTick",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "label": "Show Minor Tickmarks",
        "messageKey": "showMinorTick",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "messageKey": "VibeOn",
        "label": "Vibrate on Bluetooth disconnect during Quiet Time",
        "description": "If set to off, will still vibrate on disconnect, but not during quiet time",
        "defaultValue": false
      }
    ]
  },
   {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Layout"
      },
        {
        "type": "select",
        "messageKey": "PosTop",
        "label": "Top Position",
        "capabilities": ["NOT_PLATFORM_GABBRO", "NOT_PLATFORM_EMERY"],
            "defaultValue": "lo",
            "options": [
          {
            "label": "Empty",
            "value": "em"
          },
          {
            "label": "Hour Digits",
            "value": "hr"
          },
          {
            "label": "AM/PM marker",
            "value": "ap"
          },
          {
            "label": "Date",
            "value": "dt"
          },
          {
            "label": "Battery & Logo",
            "value": "lo"
          }
        ]
        },
        {
        "type": "select",
        "messageKey": "PosRight",
        "label": "Right Position",
        "capabilities": ["NOT_PLATFORM_GABBRO", "NOT_PLATFORM_EMERY"],
            "options": [
          {
            "label": "Empty",
            "value": "em"
          },
          {
            "label": "Hour Digits",
            "value": "hr"
          },
          {
            "label": "AM/PM marker",
            "value": "ap"
          },
          {
            "label": "Date",
            "value": "dt"
          },
          {
            "label": "Battery & Logo",
            "value": "lo"
          }
        ]
        },
        {
        "type": "select",
        "messageKey": "PosBottom",
        "label": "Bottom Position",
        "capabilities": ["NOT_PLATFORM_GABBRO", "NOT_PLATFORM_EMERY"],
        "defaultValue": "dt",
            "options": [
          {
            "label": "Empty",
            "value": "em"
          },
          {
            "label": "Hour Digits",
            "value": "hr"
          },
          {
            "label": "AM/PM marker",
            "value": "ap"
          },
          {
            "label": "Date",
            "value": "dt"
          },
          {
            "label": "Battery & Logo",
            "value": "lo"
          }
        ]
        },
        {
        "type": "select",
        "messageKey": "PosLeft",
        "label": "Left Position",
        "capabilities": ["NOT_PLATFORM_GABBRO", "NOT_PLATFORM_EMERY"],
        "defaultValue": "hr",
            "options": [
          {
            "label": "Empty",
            "value": "em"
          },
          {
            "label": "Hour Digits",
            "value": "hr"
          },
          {
            "label": "AM/PM marker",
            "value": "ap"
          },
          {
            "label": "Date",
            "value": "dt"
          },
          {
            "label": "Battery & Logo",
            "value": "lo"
          }
        ]
        },
        {
        "type": "select",
        "messageKey": "PosTop",
        "label": "Top Position",
        "capabilities": ["NOT_PLATFORM_APLITE", "NOT_PLATFORM_BASALT", "NOT_PLATFORM_CHALK", "NOT_PLATFORM_DIORITE", "NOT_PLATFORM_FLINT"],
        "defaultValue": "lo",
            "options": [
          {
            "label": "Empty",
            "value": "em"
          },
          {
            "label": "Hour Digits",
            "value": "hr"
          },
          {
            "label": "AM/PM marker",
            "value": "ap"
          },
          {
            "label": "Date",
            "value": "dt"
          },
          {
            "label": "Battery & Logo",
            "value": "lo"
          }
        ]
        },
        {
        "type": "select",
        "messageKey": "PosRight",
        "label": "Right Position",
        "capabilities": ["NOT_PLATFORM_APLITE", "NOT_PLATFORM_BASALT", "NOT_PLATFORM_CHALK", "NOT_PLATFORM_DIORITE", "NOT_PLATFORM_FLINT"],
        "defaultValue": "em",
            "options": [
          {
            "label": "Empty",
            "value": "em"
          },
          {
            "label": "Hour Digits",
            "value": "hr"
          },
          {
            "label": "AM/PM marker",
            "value": "ap"
          },
          {
            "label": "Date",
            "value": "dt"
          },
          {
            "label": "Battery & Logo",
            "value": "lo"
          }
        ]
        },
        {
        "type": "select",
        "messageKey": "PosBottom",
        "label": "Bottom Position",
        "capabilities": ["NOT_PLATFORM_APLITE", "NOT_PLATFORM_BASALT", "NOT_PLATFORM_CHALK", "NOT_PLATFORM_DIORITE", "NOT_PLATFORM_FLINT"],
        "defaultValue": "dt",
            "options": [
          {
            "label": "Empty",
            "value": "em"
          },
          {
            "label": "Hour Digits",
            "value": "hr"
          },
          {
            "label": "AM/PM marker",
            "value": "ap"
          },
          {
            "label": "Date",
            "value": "dt"
          },
          {
            "label": "Battery & Logo",
            "value": "lo"
          }
        ]
        },
        {
        "type": "select",
        "messageKey": "PosLeft",
        "label": "Left Position",
        "capabilities": ["NOT_PLATFORM_APLITE", "NOT_PLATFORM_BASALT", "NOT_PLATFORM_CHALK", "NOT_PLATFORM_DIORITE", "NOT_PLATFORM_FLINT"],
        "defaultValue": "hr",
            "options": [
          {
            "label": "Empty",
            "value": "em"
          },
          {
            "label": "Hour Digits",
            "value": "hr"
          },
          {
            "label": "AM/PM marker",
            "value": "ap"
          },
          {
            "label": "Date",
            "value": "dt"
          },
          {
            "label": "Battery & Logo",
            "value": "lo"
          }
        ]
        }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save"
  },
  {
    "type": "section",
    "capabilities": [ "COLOR" ],
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colours"
      },
      {
        "type": "toggle",
       "label": "Minute Hand Shadow On",
       "messageKey": "ShadowOn",
       "defaultValue": true
      },
      {
        "type": "radiogroup",
        "messageKey": "ThemeSelect",
        "defaultValue": "wh",
        "label": "COLOUR THEME SELECT",
        "options": [
          {
            "label": "White Background",
            "value": "wh"
          },
          {
            "label": "Black Background",
            "value": "bl"
          },
          {
              "label": "Blue Background",
              "value": "bu"
          },
          {
              "label": "Purple Background",
              "value": "pl"
          },
          {
              "label": "Black & Green",
              "value": "gr"
          },
          {
            "label": "Custom Colours",
            "value": "cu"
          }
        ]
      },
      {
           "type": "heading",
           "defaultValue": "Custom Colours"
      },
      {
        "type": "color",
        "label": "Background Colour",
        "messageKey": "BackgroundColor1",
        "defaultValue": "FFFFFF"
      },
      {
        "type": "color",
        "label": "Minute Hand Shadow Colour",
        "messageKey": "MinuteHandShadowColor",
        "defaultValue": "AAAAAA"
      },
      {
        "type": "color",
        "label": "Major Tickmark Colour",
        "messageKey": "MajorTickColor",
        "defaultValue": "000000"
      },
      {
        "type": "color",
        "label": "Minor Tickmark Colour",
        "messageKey": "MinorTickColor",
        "defaultValue": "000000"
      },
      {
        "type": "color",
        "label": "Hours Digits Colour",
        "messageKey": "HourDigitsColor",
        "defaultValue": "FFFFFF"
      },
      {
        "type": "color",
        "label": "Minute Hand Colour",
        "messageKey": "MinutesHandColor",
        "defaultValue": "FFFFFF"
      },
      {
        "type": "color",
        "label": "Date, Battery & Logo Text Colour",
        "messageKey": "DateColor",
        "defaultValue": "000000"
      },
      {
        "type": "color",
        "label": "Battery Line Colour",
        "messageKey": "BatteryLineColor",
        "defaultValue": "FF5500"
      },
      {
        "type": "color",
        "label": "Quiet Time and Bluetooth Icon Colour",
        "messageKey": "BTQTColor",
        "defaultValue": "000000"
      }
    ]
  },
  {
    "type": "section",
    "capabilities": [ "BW" ],
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colours"
      },
      {
        "type": "toggle",
       "label": "Minute Hand Shadow On",
       "messageKey": "BWShadowOn",
       "defaultValue": true
      },
       {
         "type": "radiogroup",
         "messageKey": "BWThemeSelect",
         "defaultValue": "wh",
         "label": "COLOUR THEME SELECT",
         "options": [
           {
             "label": "White Background",
             "value": "wh"
           },
           {
             "label": "Black Background",
             "value": "bl"
           },
           {
             "label": "Custom Colours",
             "value": "cu"
           }
         ]
       },
          {
            "type": "heading",
            "defaultValue": "Custom Colours"
          },
      {
        "type": "color",
        "label": "Background Colour",
        "messageKey": "BWBackgroundColor1",
        "defaultValue": "FFFFFF",
        "allowGray": true
      },
      {
        "type": "color",
        "label": "Minute Hand Shadow Colour",
        "messageKey": "BWMinuteHandShadowColor",
        "defaultValue": "AAAAAA",
        "allowGray": true
      },
      {
        "type": "color",
        "label": "Tickmark Colour",
        "messageKey": "BWMajorTickColor",
        "defaultValue": "000000"
      },
      {
        "type": "color",
        "label": "Minute Hand & Battery Line Colour",
        "messageKey": "BWMinHandBatLineColor",
        "defaultValue": "000000"
      },
      {
        "type": "color",
        "label": "Hours Digits Colour",
        "messageKey": "BWHourDigitsColor",
        "defaultValue": "000000"
      },
      {
        "type": "color",
        "label": "Date, Battery Value & Logo Text Colour",
        "messageKey": "BWDateColor",
        "defaultValue": "000000"
      },
      {
        "type": "color",
        "label": "Quiet Time and Bluetooth Icon Colour",
        "messageKey": "BWBTQTColor",
        "defaultValue": "000000"
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save"
  }
];