# Homie compatible MQTT Button x 6

Node|Property|Description
----|--------|-----------
device|seq|Property used for sending sequence definitions.<br/>`mqtt topic=homie/device/seq/set`
device|seqStatus|Property exposing sequence processing status 0-100% <br/>`mqtt topic=homie/device/seqStatus`
device|cmd|Property used forsending commands to device.<br/>`mqtt topic=homie/device/cmd/set`
button|1-6|Property for accessing specific button 1-6.<br/>`mqtt topic=homie/button/1`<br/>`mqtt topic=homie/button/1/set`

## Commands
`sequence_stop` - immediately stops currently running sequence

## Sequences
It is possible to perform series of button presses by sending a sequence to property `device/seq`.

Sequences are encoded as strings having specific syntax:
`tt:bb,dd:bb,dd:bb,dd`
Where

Code|Explanation
-|-
tt|Used time unit:<br/>h - hour,<br/>m - minute,<br/>s - second,<br/>ms - millisecond
bb|Button number: 1-6<br/>Button 0 means 'pause'.
dd|Button push duration in previously specified time units.

### Examples
Sequence|Explanation
-|-
`s:1,15:0,5:2,10`|Used time units `seconds`.<br/>Push button 1 for 15 seconds.<br/>Pause for 5 seconds<br/>Push button 2 for 10 seconds.

