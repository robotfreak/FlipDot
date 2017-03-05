## Flipdot Controller Revisionen

### V0 – 30.05.2016:

* erster Prototyp

### V1 - 23.10.2016:
* zweiter Prototyp (Reset Problematik beseitigt)
* Schieberegister /RES Pin an Arduino Pin D6 (SHRES)
* COL Transistor (BC547 T20) an Arduino Pin D4 (COL-IN)
* COMM Transistor (BC557, T19)  an Arduino Pin D5 entfällt
* COMM Transistor (BC557, T19) über Schieberegister (IC6) / ULN2803 (IC3) Ausgang 6
* FRAME Signal über Schieberegister (IC6) / ULN2803 (IC3) Ausgang 7
* STROBE Signal über Schieberegister (IC6) / ULN2803 (IC3) Ausgang 8
* R21 10k Pull Up an COMM Transistor  (BC557, T19)
* R17 10k Pull Down an COL Transistor (BC547 T20)
* R22 10k Pull Down an Schiebregister /RES Pin 
* R23 10k Pull Up an Schiebregister /OE Pin  

### V1.3 – 05.03.2017:
* Umstellung von Target v18 (PCB-Pool) auf Target v14 smart
* COMM-IN Signal von Schieberegister auf Pin D5 geändert
* SHRES Signal von Pin D6 auf Pin D2 geändert
* RGB LED Strip Connector an Pin D3, D6, D7
* COL Treiber Umstellung auf P-FET
* COMM Treiber Umstellung auf N-FET


