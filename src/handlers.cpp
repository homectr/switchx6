#include "handlers.h"
#include "Thing.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

extern Thing *thing;

bool sequenceHandler(const HomieRange &range, const String &value)
{
  unsigned long totalDuration = 0;
  unsigned int i = 0;
  int j = 0;
  String t;
  unsigned int multiple = 1;

  DEBUG_PRINT("handling new sequence=%s\n", value.c_str());

  // parse program: time:IO:duration:IO:duration....
  // example s:1:10: 2:15:

  j = value.indexOf(':', i);
  if (j < 0)
    return false;

  t = value.substring(i, j);
  DEBUG_PRINT("time specified in %s\n", t.c_str());
  if (t == "ms")
    multiple = 1;
  if (t == "s")
    multiple = 1000;
  if (t == "m")
    multiple = 60000;
  if (t == "h")
    multiple = 3600000;

  i = j + 1;
  while (i <= value.length())
  {
    j = value.indexOf(',', i);
    if (j < 0)
    {
      DEBUG_PRINT("break\n");
      break;
    }
    DEBUG_PRINT("Found i=%d j=%d end port %s\n", i, j, value.substring(i, j).c_str());
    unsigned char port = value.substring(i, j).toInt();
    i = j + 1;

    j = value.indexOf(':', i);
    DEBUG_PRINT("Found i=%d j=%d end dur %s\n", i, j, value.substring(i, j).c_str());
    unsigned long duration = value.substring(i, j).toInt() * multiple;
    totalDuration += duration;

    if (port > 0)
    {
      DEBUG_PRINT("Adding to sequence: port=%d duration=%lu\n", port, duration);
      thing->sequence.add(port, duration);
    }
    else
    {
      DEBUG_PRINT("Adding pause to sequence: duration=%lu\n", duration);
      thing->sequence.addPause(duration);
    }

    if (j < 0)
    {
      DEBUG_PRINT("break\n");
      break;
    }

    i = j + 1;
  }

  if (totalDuration > 0)
  {
    thing->homieDevice.setProperty("seq").send(String(totalDuration)); // send total sequence duraton in ms back
    Homie.getLogger() << "Total sequence duration is " << totalDuration << endl;
    thing->sequence.start();
  }

  return totalDuration > 0;
}

bool cmdHandler(const HomieRange &range, const String &value)
{
  bool updated = false;

  if (value == "seqence_stop")
  {
    thing->sequence.stop();
    updated = true;
  }

  if (updated)
  {
    thing->homieDevice.setProperty("cmd").send(value); // Update the state of the led
    Homie.getLogger() << "Cmd is " << value << endl;
  }

  return updated;
}

bool updateHandler(const HomieNode &node, const HomieRange &range, const String &property, const String &value)
{
  DEBUG_PRINT("[updHandler] node=%s prop=%s val=%s\n", node.getId(), property.c_str(), value.c_str());
  bool updated = false;
  String newValue = value;

  if (strcmp(node.getId(), "switch") == 0)
  {
    GPIOSwitch *s = thing->switches.get(property.c_str());
    if (!s)
    {
      DEBUG_PRINT("Switch not found id=%s\n", property.c_str());
      return false; // no such proprety
    }
    newValue = value == "true" ? "true" : "false";
    if (value == "true")
      s->on();
    else
      s->off();

    updated = true;
  }

  if (strcmp(node.getId(), "pwm") == 0)
  {
    PWMPort *p = thing->pwm.get(property.c_str());
    if (!p)
    {
      DEBUG_PRINT("Port not found id=%s\n", property.c_str());
      return false; // no such property
    }
    int dc = value.toInt();

    if (dc > 0)
      p->setDutyCycle(dc);
    else
      p->off();
    newValue = String(p->getDutyCycle());

    updated = true;
  }

  if (updated)
  {
    node.setProperty(property).send(newValue);
    Homie.getLogger() << "Node '" << node.getId() << "' property '" << property << "' set to " << newValue << endl;
  }

  return updated;
}

void handleSequenceStart()
{
  DEBUG_PRINT("[handleSeqStart] START\n");
  thing->homieDevice.setProperty("seqStatus").send("1");
}

void handleSequenceStop()
{
  DEBUG_PRINT("[handleSeqStop] STOP\n");
  thing->homieDevice.setProperty("seqStatus").send("0");
  thing->sequence.clear();
}

void handleStepStart(SeqStep<unsigned char> *step)
{
  digitalWrite(step->item, 1);
  DEBUG_PRINT("[handleStepStart] step=%d START\n", step->id);
  thing->homieSwitches.setProperty(String(step->item)).send("true");
}

void handleStepStop(SeqStep<unsigned char> *step)
{
  digitalWrite(step->item, 0);
  DEBUG_PRINT("[handleStepStop] step=%d STOP\n", step->id);
  thing->homieSwitches.setProperty(String(step->item)).send("false");
}

void handleSwitchOn(Switch *s)
{
  DEBUG_PRINT("[handleSwitch] id=%s ON\n", s->getId());
  thing->homieSwitches.setProperty(s->getId()).send("true");
}

void handleSwitchOff(Switch *s)
{
  DEBUG_PRINT("[handleSwitch] id=%s OFF\n", s->getId());
  thing->homieSwitches.setProperty(s->getId()).send("false");
}

void handlePWM(PWMPort *p)
{
  DEBUG_PRINT("[handlePWM] id=%s dc=%d\n", p->getId(), p->getDutyCycle());
  thing->homiePWM.setProperty(p->getId()).send(String(p->getDutyCycle()));
}

void onHomieEvent(const HomieEvent &event)
{
  switch (event.type)
  {
  case HomieEventType::STANDALONE_MODE:
    // Do whatever you want when standalone mode is started
    break;
  case HomieEventType::CONFIGURATION_MODE:
    // Do whatever you want when configuration mode is started
    break;
  case HomieEventType::NORMAL_MODE:
    // Do whatever you want when normal mode is started
    break;
  case HomieEventType::OTA_STARTED:
    // Do whatever you want when OTA is started
    break;
  case HomieEventType::OTA_PROGRESS:
    // Do whatever you want when OTA is in progress

    // You can use event.sizeDone and event.sizeTotal
    break;
  case HomieEventType::OTA_FAILED:
    // Do whatever you want when OTA is failed
    break;
  case HomieEventType::OTA_SUCCESSFUL:
    // Do whatever you want when OTA is successful
    break;
  case HomieEventType::ABOUT_TO_RESET:
    // Do whatever you want when the device is about to reset
    break;
  case HomieEventType::WIFI_CONNECTED:
    // Do whatever you want when Wi-Fi is connected in normal mode

    // You can use event.ip, event.gateway, event.mask
    break;
  case HomieEventType::WIFI_DISCONNECTED:
    // Do whatever you want when Wi-Fi is disconnected in normal mode

    // You can use event.wifiReason
    /*
      Wi-Fi Reason (souce: https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientEvents/WiFiClientEvents.ino)
      0  SYSTEM_EVENT_WIFI_READY               < ESP32 WiFi ready
      1  SYSTEM_EVENT_SCAN_DONE                < ESP32 finish scanning AP
      2  SYSTEM_EVENT_STA_START                < ESP32 station start
      3  SYSTEM_EVENT_STA_STOP                 < ESP32 station stop
      4  SYSTEM_EVENT_STA_CONNECTED            < ESP32 station connected to AP
      5  SYSTEM_EVENT_STA_DISCONNECTED         < ESP32 station disconnected from AP
      6  SYSTEM_EVENT_STA_AUTHMODE_CHANGE      < the auth mode of AP connected by ESP32 station changed
      7  SYSTEM_EVENT_STA_GOT_IP               < ESP32 station got IP from connected AP
      8  SYSTEM_EVENT_STA_LOST_IP              < ESP32 station lost IP and the IP is reset to 0
      9  SYSTEM_EVENT_STA_WPS_ER_SUCCESS       < ESP32 station wps succeeds in enrollee mode
      10 SYSTEM_EVENT_STA_WPS_ER_FAILED        < ESP32 station wps fails in enrollee mode
      11 SYSTEM_EVENT_STA_WPS_ER_TIMEOUT       < ESP32 station wps timeout in enrollee mode
      12 SYSTEM_EVENT_STA_WPS_ER_PIN           < ESP32 station wps pin code in enrollee mode
      13 SYSTEM_EVENT_AP_START                 < ESP32 soft-AP start
      14 SYSTEM_EVENT_AP_STOP                  < ESP32 soft-AP stop
      15 SYSTEM_EVENT_AP_STACONNECTED          < a station connected to ESP32 soft-AP
      16 SYSTEM_EVENT_AP_STADISCONNECTED       < a station disconnected from ESP32 soft-AP
      17 SYSTEM_EVENT_AP_STAIPASSIGNED         < ESP32 soft-AP assign an IP to a connected station
      18 SYSTEM_EVENT_AP_PROBEREQRECVED        < Receive probe request packet in soft-AP interface
      19 SYSTEM_EVENT_GOT_IP6                  < ESP32 station or ap or ethernet interface v6IP addr is preferred
      20 SYSTEM_EVENT_ETH_START                < ESP32 ethernet start
      21 SYSTEM_EVENT_ETH_STOP                 < ESP32 ethernet stop
      22 SYSTEM_EVENT_ETH_CONNECTED            < ESP32 ethernet phy link up
      23 SYSTEM_EVENT_ETH_DISCONNECTED         < ESP32 ethernet phy link down
      24 SYSTEM_EVENT_ETH_GOT_IP               < ESP32 ethernet got IP from connected AP
      25 SYSTEM_EVENT_MAX
    */
    break;
  case HomieEventType::MQTT_READY:
    // Do whatever you want when MQTT is connected in normal mode
    break;
  case HomieEventType::MQTT_DISCONNECTED:
    // Do whatever you want when MQTT is disconnected in normal mode

    // You can use event.mqttReason
    /*
      MQTT Reason (source: https://github.com/marvinroger/async-mqtt-client/blob/master/src/AsyncMqttClient/DisconnectReasons.hpp)
      0 TCP_DISCONNECTED
      1 MQTT_UNACCEPTABLE_PROTOCOL_VERSION
      2 MQTT_IDENTIFIER_REJECTED
      3 MQTT_SERVER_UNAVAILABLE
      4 MQTT_MALFORMED_CREDENTIALS
      5 MQTT_NOT_AUTHORIZED
      6 ESP8266_NOT_ENOUGH_SPACE
      7 TLS_BAD_FINGERPRINT
    */
    break;
  case HomieEventType::MQTT_PACKET_ACKNOWLEDGED:
    // Do whatever you want when an MQTT packet with QoS > 0 is acknowledged by the broker

    // You can use event.packetId
    break;
  case HomieEventType::READY_TO_SLEEP:
    // After you've called `prepareToSleep()`, the event is triggered when MQTT is disconnected
    break;
  case HomieEventType::SENDING_STATISTICS:
    // Do whatever you want when statistics are sent in normal mode
    break;
  }
}