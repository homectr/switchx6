# Homie compatible MQTT 6 Channel Switch

Node|Property|Description
----|--------|-----------
device|seq|Property used for sending sequence definitions.<br/>`mqtt topic=homie/device/seq/set`
device|seqStatus|Property exposing sequence processing status 0-100% <br/>`mqtt topic=homie/device/seqStatus`
device|cmd|Property used for sending commands to device.<br/>`mqtt topic=homie/device/cmd/set`
switch|1-6|Property for accessing specific switch 1-6.<br/>`mqtt topic=homie/switch/1`<br/>`mqtt topic=homie/switch/1/set`

## Custom Settings
Homie configuration file can be used to set switches (ids "s1" & "s2") to momentary mode (not latching).
It is possible to set a timeout for each switch after which such switch will be released.

**Example**
Following example sets switches s1 (gpio 14) & s2 (gpio 12) to momentary mode, releasing after 1000ms resp. 100ms.
Switches s3 (gpio 13) & s4 (gpio 4) are non-momentary/latching.

```
    "settings": {
        "item1":"switch:s1:14:m:1000",
        "item2":"switch:s2:12:m:100",
        "item3":"switch:s3:13",
        "item4":"switch:s4:4",
        "item5":"pwm:pwm1:5",
        "item6":"pwm:pwm2:15"
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

