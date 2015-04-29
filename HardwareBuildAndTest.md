<br>Describe in detail how to build and test board, and options<br>
<br>
<br>= Introduction =<br>
<br>
Rough draft <br><br><br>
<br>= Details =<br>
<br>
<br>decide which style headers.<br>
<br>
<br>decide which way the LCD goes<br>
<br>use the right m/f connector for it<br>
<br>
<br>which will be wired (probes, test switch, power)<br>
<br>
<br>what power source?<br>
<br>
<br><b>ASSEMBLY ORDER</b>
<br>precision r first.<br>
<br>only one value at a time.<br>
<br>
<br>rest of r<br>
<br>diode<br>
<br>i.c. socket<br>
<br>leds<br>
<br>small c<br>
<br>headers<br>
<br>reg<br>
<br>q<br>
<br>elect c<br>
<br>
<br>
<br>===================================PARTS===================================================<br>
<br><b>BARE MINIMUM SYSTEM:</b>
<br>1x2 2pin +5V IN header, from external +5V supply.<br>
<br>LCD<br>
<br>LCD connector - do NOT solder in LCD if you want to be able to add options.<br>
<br>CPU<br>
cpu socket<br>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=1'>r1</a> precision for probe <a href='https://code.google.com/p/avr-component-tester/source/detail?r=1'>R1</a>-<a href='https://code.google.com/p/avr-component-tester/source/detail?r=6'>R6</a>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=1'>r1</a>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=3'>r3</a>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=4'>r4</a>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=5'>r5</a>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=6'>r6</a>
<br>c1 AREF decouple<br>
<br>c3 AVCC decouple<br>
<br>c6 +5v pwr filter<br>
<br>C5 +5v decouple<br>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=13'>r13</a> pullup mclr<br>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=11'>r11</a> contrast pot<br>
<br>1x3 3 pin header for probes, or wire directly<br>
<br>
<hr />
<br><b>BATTERY OPTION:</b> (makes it PORTABLE)<br>
<br>9V battery pigtail<br>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=12'>r12</a> batt voltage sense divider<br>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=21'>r21</a> batt voltage sense divider<br>
<br>d2  protection zener<br>
<br>ic2 5v regulator for internal +5V supply.<br>
<br>q3 9v switch<br>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=10'>r10</a> bias<br>
<br>c4 filter<br>
<br>d1 input protection diode<br>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=7'>r7</a>
<br>q1 keep-on-turn-off<br>
<br>led1 vdrop<br>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=8'>r8</a> base limiter<br>
<br>c2 noise filter<br>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=9'>r9</a> noise filter<br>
<br>9V pwr in header<br>
<br>
<br>------------------------------------------------------------<br>
<br><b>POWER INDICATOR LED OPTION:</b>
<br><a href='https://code.google.com/p/avr-component-tester/source/detail?r=22'>r22</a> 680r<br>
<br>pwr-led<br>
<br>
<br>------------------------------------------------------------<br>
<br><b>I/O EXPANSION OPTION:</b>
<br>3 pin header PB6-PB7<br>
<br>3 pin header PB3-PC4<br>
<br>
<br>------------------------------------------------------------<br>
<br><b>UART OPTION:</b>
<br>3 pin header UART<br>
<br>
<br>------------------------------------------------------------<br>
<br><b>ISP OPTION:</b>
<br>2x3 6 pin header<br>
<br>
<hr />
<br><b>5V OUT OPTION:</b>
<br>1x2 2 pin header - also used as +5V IN if not using 9V battery