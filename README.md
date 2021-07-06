# Homie compatible MQTT 6 Channel Switch

Node|Property|Description
----|--------|-----------
device|seq|Property used for sending sequence definitions.<br/>`mqtt topic=homie/device/seq/set`
device|seqStatus|Property exposing sequence processing status 0-100% <br/>`mqtt topic=homie/device/seqStatus`
device|cmd|Property used forsending commands to device.<br/>`mqtt topic=homie/device/cmd/set`
switch|1-6|Property for accessing specific switch 1-6.<br/>`mqtt topic=homie/switch/1`<br/>`mqtt topic=homie/switch/1/set`

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

