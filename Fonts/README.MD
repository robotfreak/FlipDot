# Fonts

some of the Fonts (16x9, 12x8) has been created from scratch using the following tools:
* RasterFontEditor http://www.cylog.org/ 
* LCDAssistant Tool http://en.radzio.dxp.pl/bitmap_converter/

The workflow for creating fonts from scratch:
* User RasterFontEditor to create the font of the desired size.
* Create each character for the font
* Save as Bitmap
* Load the Bitmap in LCDAssisant
* Edit the given parameters for font size until they match with the bitmap size
* Convert Bitmap to C Header File
* Write a C-Function to load a character from the font


More fonts can be found in the following thread:
https://www.mikrocontroller.net/topic/54860
