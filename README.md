# moodlamp
The moodlamp allows to change color in RGB colorspace.

Some art has been done with the help of moodlamps:
- ACAB (AllColourAreBeautiful) https://wiki.muc.ccc.de/acab:start
- Die Drei C https://wiki.muc.ccc.de/diedreic

# r3 version and earlier versions
Earlier versions are developed with ATmega324P, ATmega644P, ... For RS485 usage, change the MCU section in the file ubloader/Makefile.

# r4 version (2th quarter 2015)
Latest r4 version is designed with ATmega324A. If you get a warning from avrdude "device signature [..] expected signature [..] or use -F to override this check", put the following lines in your avrdude.conf:
```diff
 #------------------------------------------------------------
+# ATmega324A
+#------------------------------------------------------------
+
+# similar to ATmega324P
+
+part parent "m324p"
+    id               = "m324a";
+    desc             = "ATmega324A";
+    signature        = 0x1e 0x95 0x15;
+
+    ocdrev           = 3;
+  ;
+
+#------------------------------------------------------------
 # ATmega324PA
 #------------------------------------------------------------
```
For RS485 usage, also double check the MCU part in the file ubloader/Makefile.
