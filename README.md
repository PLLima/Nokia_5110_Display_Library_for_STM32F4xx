# Important Files:

- **For a more detailed documentation, here's a [Nokia 5110's Static Library Documentation PDF](https://drive.google.com/file/d/1tTYV-_VoK2PKpsm6HHo3GUi-4JNY2zMa/view?usp=sharing);**
- **For the example images's files, here's [its folder](https://drive.google.com/drive/folders/1O_cOrA-rPLWu5BWAe0FCR24_E1Jp_-6t?usp=sharing);**
- **For the example's video, here's [its file](https://drive.google.com/file/d/1vXGnclWeVLx4x3PoLjOo3_7i568FhR_e/view?usp=sharing).**

# Nokia 5110's Static Library:

**Static library for the Nokia 5110 (with a PCD8544 controller) for the STM32F4xx boards (using HAL drivers).**

## Author:

@PLLima

## Version:

```
1.0
```

## Date:

```
November, 2021
```

## Table of Contents:

- [Overview](https://github.com/PLLima/Nokia_5110_Display_Library_for_STM32F4xx#overview);
- [Installation](https://github.com/PLLima/Nokia_5110_Display_Library_for_STM32F4xx#installation);
- [Display's Characteristics](https://github.com/PLLima/Nokia_5110_Display_Library_for_STM32F4xx#displays-characteristics);
- [Library's Features](https://github.com/PLLima/Nokia_5110_Display_Library_for_STM32F4xx#librarys-features);
- [Fonts](https://github.com/PLLima/Nokia_5110_Display_Library_for_STM32F4xx#fonts);
- [Images](https://github.com/PLLima/Nokia_5110_Display_Library_for_STM32F4xx#images);
- [Initial Recommendation for Starting an Application with this Library](https://github.com/PLLima/Nokia_5110_Display_Library_for_STM32F4xx#initial-recommendation-for-starting-an-application-with-this-library);
- [Examples](https://github.com/PLLima/Nokia_5110_Display_Library_for_STM32F4xx#examples).

## Overview:

- STM32F4xx MCUs' static library;
- Uses HAL drivers (functions, data types, etc.);
- Display pinout and SPI peripheral configurable on the fly;
- Supports more than one display's instance (scalability);
- Transmission mode control (blocking or non-blocking);
- Inverse screen mode, contrast control and temperature coefficient control;
- Position setting, clearing and inverse clearing functions;
- Sleep mode available for low power consumption;
- String and inverse string writing;
- Customizable ASCII fonts (user-created or included fonts);
- Drawing and inverse drawing functions;
- Function parameters' error feedback printed on the screen.

## Installation:

In order to install a static library inside a project, one must link it within the respective project's properties. It's strongly recommended to use this library with the STM32CubeIDE app. Here's a link to an online guide on how to include static and non-static libraries on the STM32CubeIDE app and one to include it on the Atollic TrueSTUDIO:

- [Include Libraries on Atollic TrueSTUDIO](http://www.pomad.fr/node/60);
- [Include Libraries on STM32CubeIDE](https://hackmd.io/@quency/rJzJXx2UP).

## Display's Characteristics:

The Nokia 5110 LCD Display is a very versatile device, with low power consumption, that works with a PCD8544 controller. That controller is a low power CMOS driver for LCD displays with 48 rows and 84 columns. With a total of 4,032 pixels, every 8 columns corresponds to a block of data (byte), counting up to 6 blocks per row. This way, there is at most 504 bytes of data on the screen at once. The addressing of the position in the display starts from the upper left corner and goes to the right row (as the X position increases) and to the bottom column (as the Y position increases). The figure below represents the data structure of the device:

![Structure of Data and Address of the Display](/Nokia_5110_Data_Structure.jpg)

Using the SPI protocol, the device's driver receives data from the SDIN (data input), SCLK (clock of the SPI) and SCE (chip select / slave enable) pins. This library requires the use of a SPI peripheral (hardware). The SPIx_MOSI (Master Out, Slave In), SPIx_SCK (Clock) and SPIx_NSS (Slave Select, whose signal type must be used by software) are pins used for the display's configuration. It works with SPI baud rates up to 4 Mb / s. To select which type of data is being received (data or commands), the device's D / C pin is used. The RES pin is used for restarting (active low) the PCD8544 controller and is not controlled by this library (it's up to the user to configure this pin). It's recommended to use the MCU's reset pin (as long as it's kept low for at least 100 ns). The VCC and GND pins are responsible for powering the device. Last but not least is the LED pin that controls the backlight of the display whose control is not included in this library. Since it's made of blue LEDs, it's recommended to keep its current from 10 to 30 mA. The pinout of the device is illustrated by the image below:

![Pinout of the Display](/Nokia_5110_Pinout.jpg)

### Attention:

- The display works with a voltage range from 2.7 to 5 V, however it's recommended to power it within 2.7 to   3.3 V;
- This device works within a temperature range from – 25 °C to 70 °C, but it's always recommended to keep its temperature around 25 °C.

## Library's Features:

The Nokia 5110's Static Library for the STM32F4xx MCUs using HAL drivers attends a variety of users. It features functions to configure all possible display's parameters. These are contrast (voltage operation + bias level), sleep mode (power down operation), invert screen mode and temperature coeffient control (for environments with very distinct temperatures). Furthermore, the user can choose the display's current coordinate / position in order to draw figures or write a text. It is also possible to get the current display's coordinate using the `LCD5110_Get_XY function`. The width of each coordinate depends on the current text's font (which will be detailed in the next paragraphs). To print things onto the display anew, there is a clear screen function (`LCD5110_Clear`) that turns off every pixel of the screen (turns them "white"). Beyond the display, this library gives the user the ability to choose which transmission method to use (blocking mode, with polling or non-blocking mode, with interruptions or DMA).

The user needs to define which transmission mode to use with the initialization function and can change it at anytime during the code's execution with the function that changes that mode (`LCD5110_Set_Mode`).

This library also counts with a text writing functions, `LCD5110_Write_String` that writes any string that fits the screen's size onto the display. If the string passed by the user as a parameter is larger than the screen's size (504 bytes / columns), it returns an error an writes it on the screen. There's the possibility to change the text's font. In
order to do that, one needs to include the _lcd5110_included_fonts.h_ file in the project or create and include one's own fonts' file, following the patterns specified by the [Fonts](https://github.com/PLLima/Nokia_5110_Display_Library_for_STM32F4xx#fonts) topic in this section.

The drawing functions included in this library allow the user to draw a line from a subcoordinate (pixel) to another subcoordinate (the x value of the first subcoordinate must be smaller than the second's x value). In addition to that, it gives the ability to draw retactangles and rounded rectangles from a vertex to another (diagonally, respecting the x and y values of the first subcoordinate being smaller than the second's x and y values). Last but not least, the user can draw a circle to the screen by specifing its center's subcoordinate and radius. Since all the drawing functions use a shared buffer, every time one draws a figure without clearing the screen (`LCD5110_Clear`), the previously drawn figures after the last calling of the clearing function will remain drawn. In other words, if the user calls the `LCD5110_Draw_Rect` to draw two different rectangles, for example, they will both appear in the screen at once.

All the drawing and text writing functions have an inverse mode version that draw and write text inversely onto the display (everything that is usually "black" turns "white" and vice-versa). The inverse text writing functions don't require the use of the clearing functions, since they turn every pixel surrounding each character on ("black") in a rectangular manner. For example, if a font has a width of 6 bytes / columns, the surrounding pixels of the character in a 6 x 8 (width x height, in pixels) area will be turned on. In order to use the inverse drawing functions, the user must call the inverse clearing function first (`LCD5110_InvClear`) and then call the respective drawing function (s). Of course, if the invert screen mode was called for an odd number of times in the code, everything that was drawn or written with the inverse functions will display as if they were drawn or written with the normal functions.

If the aforementioned drawing functionality isn't enough, there is also a block / image writing function that sends a user-defined number of bytes from a chosen array of bytes. What array is going to be sent can be included in the project by the user. For more information about its format, read the [Images](https://github.com/PLLima/Nokia_5110_Display_Library_for_STM32F4xx#images) section below.

All functions have parameter errors detection that stop the execution of the rest of this library's functions and prints a message indicating which parameters of the respective function that caused an error. The parameters' boundaries were set in order to avoid unexpected behaviour of the display using this library. If one wants to get the code for the respective error, there is an user-available `LCD5110_Get_Error()` function.

### Attention:

- Whilst using non-blocking modes, the `LCD5110_Init` and `LCD5110_Enable_Sleep` functions don't transmit all the information in interruption or DMA mode. It only sends the respective command in non-blocking mode after clearing the screen in blocking mode;
- It's recommended to use some sort of timing before calling any function in non-blocking mode. That's because the functions start sending data to the display whilst the code is running and, if the program reaches another function before all the previous data is sent, the function is skipped;
- The user **MUST** put the `LCD5110_TxCpltCallback` function inside the `HAL_SPI_TxCpltCallback` (function called after the SPI transmission in non-blocking mode) featured inside the HAL drivers when using any non-blocking mode.

## Fonts:

In order to create a custom font (s)' file, the user must first create a _.h_ file and include the _lcd5110.h_ file in the new file's code. Then, one shall create the new single dimensional arrays with the information for the new fonts. The type of data stored in those arrays is that of an unsigned char. Those arrays must contain sequential characters according to the ASCII table and can start from the character ' ' (0x20) or any other character after it and end at the last writable character on the table '∼' (0x7E) or any other character before it. After creating all the data for the new fonts, the user must create an `LCD5110_FontTypeDef` structure (that can be constant or not) for each of the arrays created. Its recommended to initialize each member of the structure with its respective value. Those are, in order: font's width, first character, last character and the first element of the respective array's address (name of the array). Then, once included the custom font (s)' file in the main program, the user can use the custom fonts by calling them in the `LCD5110_Set_Font` function. The _lcd5110_included_fonts.h_ file has some good examples showing how to create a custom font (s)' file. If the user wants to, they can create custom fonts inside the main program if they follow the aforementioned instructions.

## Images:

One can print any image to the display, as long as they are inside a single dimensional array of _uint8_t_ variables (single dimensional unsigned char arrays are equivalent). The display is capable of storing up to 504 bytes of data, it's recommended that the array has at maximum 504 elements. The user can create images and include them in the main program through _.h_ files or by implementing them in the final program. The _lcd5110_included_images.h_ has some good examples of how to create arrays of images to send to the LCD 5110. Once created, it's possible to print them to the display through the `LCD5110_Write_DataBlock` function.

## Initial Recommendation for Starting an Application with this Library:

1. Configure your SPI peripheral (s) to support the display's requirements and limitations (remember to use a software controlled SPI_NSS) and, it's recommended to use a timer to make the best use of the non-blocking functions;
2. Create a `LCD5110_HandleTypeDef` variable for each of the connected displays (it's not recommended to initialize them);
3. Call the `LCD5110_Config_Pins` function to establish the pins used for each the displays as well as the handler of the respective SPI peripheral (s);
4. Call the `LCD5110_Init function` to initialize each of the connected LCD 5110s and choose the transmission mode for each of them;
5. It's recommended to set the coordinates of each display to its next intended position before moving forward.

## Examples:

Here are some images that show different screens of the display created through this library:

![First image sent as a block of data](/Chip_Kit_Image.jpg)
![Second image sent as a block of data](/The_End_Image.jpg)
![Text written with the default font](/Default_Font.jpg)
![Text written inversely with the tiny font](/Font_1.jpg)
![Text written inversely with the thick font](/Font_2.jpg)
![Text written with the aurebesh font](/Font_3.jpg)
![Text written inversely with the seven segment font](/Font_4.jpg)
![Text written with the wide font](/Font_5.jpg)
![Drawn inverse lines](/Lines.jpg)
![Drawn rectangles](/Rectangles.jpg)
![Drawn inverse rounded rectangles](/Rounded_Rectangles.jpg)
![Drawn circles](/Circles.jpg)
