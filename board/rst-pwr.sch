EESchema Schematic File Version 4
LIBS:rst-pwr-cache
EELAYER 28 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "Power/Reset Switch Circuit - Host Side"
Date "2018-09-04"
Rev "5.0"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector_Generic:Conn_01x04 J2
U 1 1 5B8EC0AD
P 4100 4200
F 0 "J2" H 4020 3775 50  0000 C CNN
F 1 "Molex" H 4020 3866 50  0000 C CNN
F 2 "MiniSPOX-Horizontal:Connector" H 4100 4200 50  0001 C CNN
F 3 "~" H 4100 4200 50  0001 C CNN
	1    4100 4200
	-1   0    0    1   
$EndComp
Wire Wire Line
	4800 4100 4300 4100
Wire Wire Line
	4300 4200 4800 4200
NoConn ~ 9350 1300
$Comp
L Connector_Generic:Conn_02x02_Odd_Even J3
U 1 1 5B8EB6FB
P 4800 5100
F 0 "J3" V 4804 5180 50  0000 L CNN
F 1 "RST" V 4895 5180 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x02_P2.54mm_Vertical" H 4800 5100 50  0001 C CNN
F 3 "~" H 4800 5100 50  0001 C CNN
	1    4800 5100
	0    1    1    0   
$EndComp
Wire Wire Line
	4700 5400 4700 4900
Wire Wire Line
	4800 4900 4800 5400
Connection ~ 4800 4900
$Comp
L Connector_Generic:Conn_02x02_Odd_Even J1
U 1 1 5B8EB8F3
P 4800 3200
F 0 "J1" V 4804 3280 50  0000 L CNN
F 1 "PWR" V 4895 3280 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x02_P2.54mm_Vertical" H 4800 3200 50  0001 C CNN
F 3 "~" H 4800 3200 50  0001 C CNN
	1    4800 3200
	0    1    1    0   
$EndComp
Wire Wire Line
	4700 3500 4700 3000
Wire Wire Line
	4800 3000 4800 3500
Connection ~ 4800 3500
$Comp
L Device:D_TVS D1
U 1 1 5BA269EF
P 4650 3755
F 0 "D1" H 4650 3631 50  0000 C CNN
F 1 "~" H 4650 3630 50  0001 C CNN
F 2 "Diode_SMD:D_SOD-323_HandSoldering" H 4650 3755 50  0001 C CNN
F 3 "~" H 4650 3755 50  0001 C CNN
	1    4650 3755
	-1   0    0    1   
$EndComp
Wire Wire Line
	4300 4000 4500 4000
Wire Wire Line
	4700 3500 4500 3500
Connection ~ 4700 3500
Wire Wire Line
	4800 3500 4800 3755
Wire Wire Line
	4500 3500 4500 3755
Wire Wire Line
	4300 4300 4500 4300
Wire Wire Line
	4700 4900 4500 4900
Connection ~ 4700 4900
Wire Wire Line
	4500 4615 4500 4900
Wire Wire Line
	4500 4300 4500 4615
Connection ~ 4500 4615
Wire Wire Line
	4800 4200 4800 4615
Connection ~ 4800 4615
$Comp
L Device:D_TVS D2
U 1 1 5BA27949
P 4650 4615
F 0 "D2" H 4650 4491 50  0000 C CNN
F 1 "~" H 4650 4490 50  0001 C CNN
F 2 "Diode_SMD:D_SOD-323_HandSoldering" H 4650 4615 50  0001 C CNN
F 3 "~" H 4650 4615 50  0001 C CNN
	1    4650 4615
	-1   0    0    1   
$EndComp
Wire Wire Line
	4800 4615 4800 4900
Connection ~ 4800 3755
Wire Wire Line
	4800 3755 4800 4100
Connection ~ 4500 3755
Wire Wire Line
	4500 3755 4500 4000
$EndSCHEMATC
