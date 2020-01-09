/*!
 * @file AdafruitIO.h
 *
 * This is part of the Adafruit IO library for the Arduino platform.
 * 
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Tony DiCola, Todd Treece for Adafruit Industries
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef ADAFRUITIO_H
#define ADAFRUITIO_H

#include "AdafruitIO_Dashboard.h"
#include "AdafruitIO_Data.h"
#include "AdafruitIO_Definitions.h"
#include "AdafruitIO_Feed.h"
#include "AdafruitIO_Group.h"
#include "AdafruitIO_Time.h"
#include "Adafruit_MQTT.h"
#include "Arduino.h"
#include "ArduinoHttpClient.h"
#include "util/AdafruitIO_Board.h"

#ifndef ADAFRUIT_MQTT_VERSION_MAJOR
#error                                                                         \
    "This sketch requires Adafruit MQTT Library v1.0.0 or higher. Please install or upgrade using the Library Manager."
#endif

#if ADAFRUIT_MQTT_VERSION_MAJOR == 1 && ADAFRUIT_MQTT_VERSION_MINOR < 0
#error                                                                         \
    "This sketch requires Adafruit MQTT Library v1.0.0 or higher. Please install or upgrade using the Library Manager."
#endif

/**************************************************************************/
/*!
    @brief  Class for interacting with Adafruit IO
*/
/**************************************************************************/
class AdafruitIO {

  friend class AdafruitIO_Feed;
  friend class AdafruitIO_Group;
  friend class AdafruitIO_Dashboard;
  friend class AdafruitIO_Block;
  friend class AdafruitIO_Time;

public:
  AdafruitIO(const char *user, const char *key);
  virtual ~AdafruitIO();

  void connect();
  void wifi_disconnect();
  aio_status_t run(uint16_t busywait_ms = 0, bool fail_fast = false);

  AdafruitIO_Feed *feed(const char *name);
  AdafruitIO_Feed *feed(const char *name, const char *owner);
  AdafruitIO_Group *group(const char *name);
  AdafruitIO_Dashboard *dashboard(const char *name);
  AdafruitIO_Time *time(aio_time_format_t format);

  const __FlashStringHelper *statusText();

  aio_status_t status();
  virtual aio_status_t networkStatus() = 0;
  aio_status_t mqttStatus(bool fail_fast = false);

  char *boardID();
  const char *boardType();
  char *version();
  char *userAgent();
  virtual const char *connectionType() = 0;

protected:
  virtual void _connect() = 0;
  virtual void _disconnect() = 0;
  aio_status_t _status = AIO_IDLE;
  uint32_t _last_ping = 0;
  uint32_t _last_mqtt_connect = 0;

  Adafruit_MQTT *_mqtt;
  HttpClient *_http;

  char _version[10];

  const char *_host = "io.adafruit.com";
  uint16_t _mqtt_port = 8883;
  uint16_t _mqtt_eth_port = 1883;
  uint16_t _http_port = 443;

  uint16_t _packetread_timeout;

  const char *_username;
  const char *_key;

  char *_err_topic;
  char *_throttle_topic;
  char *_user_agent;

  Adafruit_MQTT_Subscribe *_err_sub;
  Adafruit_MQTT_Subscribe *_throttle_sub;

private:
  void _init();
};

#endif // ADAFRUITIO_H
