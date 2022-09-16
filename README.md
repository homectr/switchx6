# Homie compatible MQTT 6 Channel Switch

Node|Property|Description
----|--------|-----------
device|seq|Property used for sending sequence definitions.<br/>`mqtt topic=homie/device/seq/set`
device|seqStatus|Property exposing sequence processing status 0-100% <br/>`mqtt topic=homie/device/seqStatus`
device|cmd|Property used for sending commands to device.<br/>`mqtt topic=homie/device/cmd/set`
switch|{id}|Property for accessing specific switch.<br/>`mqtt topic=homie/switch/s1`<br/>`mqtt topic=homie/switch/s1/set`
pwm|{id}|Property for accessing specific pwm port. Allowed values are 0-100.<br/>`mqtt topic=homie/pwm/pwm1`<br/>`mqtt topic=homie/pwm/pwm1/set`

## Pins/GPIOs

GPIO|Usage
----|-----
0|Config mode button - if LOW for more than 10s config mode is started
2|Status LED - slow blinking = connecting to Wifi,<br/>rapid blinking = connecting to MQTT

For use in your applications, please, check GPIO usage in ESP documentation.
Usefull info about which GPIOs can be used for what can be found also here https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/

## Custom Settings

Homie configuration file can be used to set switches to momentary mode (not latching).
It is possible to set a timeout for each mementary switch after which such switch will be released.
If no timeout is specified, momentary switch releases after 10ms.
All switches are by default non-momentary (latching).

**Example**

* switches s1 (gpio 14) & s2 (gpio 12) to momentary mode, releasing after 1000ms resp. default 10ms.
* switches s3 (gpio 13) & s4 (gpio 4) are non-momentary.
* switches s1 and s4 are set to inverse mode: 0 => ON, 1 => OFF
* switch s4 is also set to ON during start. Switches are set to OFF unless parameter `ison` is specified.
* PWM is configured on gpio 5 and 16.

```
    "settings": {
        "item1":"switch:s1:14:inv:m:1000",
        "item2":"switch:s2:12:m",
        "item3":"switch:s3:13",
        "item4":"switch:s4:4:inv:ison",
        "item5":"pwm:pwm1:5",
        "item6":"pwm:pwm2:16"
    }
```

## Commands
`sequence_stop` - immediately stops currently running sequence

## Sequences
It is possible to perform series of switch presses by sending a sequence to property `device/seq`.

Sequences are encoded as strings having specific syntax:
`tt:ss,dd:ss,dd:ss,dd`
Where

Code|Explanation
-|-
tt|Used time unit:<br/>h - hour,<br/>m - minute,<br/>s - second,<br/>ms - millisecond
ss|Switch number: 1-6<br/>Switch 0 means 'pause'.
dd|Switch push duration in previously specified time units.

### Examples
Sequence|Explanation
-|-
`s:1,15:0,5:2,10`|Used time units `seconds`.<br/>Switch 1 is ON for 15 seconds.<br/>Pause for 5 seconds<br/>Switch 2 is ON 10 seconds.
`ms:3,5:1,5:2,5:3,10:6,1`|Used time units `milliseconds`.<br/>Switch 3 is ON for 5ms.<br/>Switch 1 is ON for 5ms<br/>Switch 2 is ON for 5ms.<br/>Switch 3 is ON for 10ms.<br/>Switch 6 is ON for 1ms.

