/*
 **************************************
 * – Author:  Pedro Lubaszewski Lima; *
 * – Version: 1.0;                    *
 * – Date:    November, 2021.         *
 **************************************
 */

/* Only fills this file with data if this flag wasn't used yet */
#ifndef __LCD5110_H
#define __LCD5110_H

/* Includes */

#include "stm32f4xx.h" /* Library with all base functions for the STM32 boards */
#include "stm32f4xx_hal_def.h" /* Library with basic HAL definitions */
#include "stm32f4xx_hal_spi.h" /* Library with all functions for the SPI peripherals */
#include "stm32f4xx_hal_gpio.h" /* Library with all functions for the GPIO peripherals */

/* Defines */

/* Defines the screen size in bytes */
#define LCD5110_SCREEN_SIZE 504

/* TypeDefs */

/* LCD 5110 State enumeration definition */
typedef enum __LCD5110_StateTypeDef
{

	LCD5110_STATE_RESET   = 0,
	LCD5110_STATE_READY   = 1,
	LCD5110_STATE_BUSY    = 2,
	LCD5110_STATE_ASLEEP  = 3,
	LCD5110_STATE_ERROR   = 4

} LCD5110_StateTypeDef;

/* LCD 5110 Initial Instructions enumeration definition */
typedef enum __LCD5110_InstructionOrderTypeDef
{

	LCD5110_FIRST_COMMAND  = 0,
	LCD5110_SECOND_COMMAND = 1,
	LCD5110_THIRD_COMMAND  = 2,
	LCD5110_FOURTH_COMMAND = 3,
	LCD5110_FIFTH_COMMAND  = 4,
	LCD5110_SIXTH_COMMAND  = 5

} LCD5110_InstructionOrderTypeDef;

/* LCD 5110 Mode enumeration definition */
typedef enum __LCD5110_ModeTypeDef
{
	LCD5110_MODE_DEFAULT = 0, // The command used to select the blocking mode (polling)
	LCD5110_MODE_IT      = 1, // The command used to select the interruption mode (non-blocking mode)
	LCD5110_MODE_DMA     = 2  // The command used to select the DMA (direct memory access) mode (non-blocking mode)

} LCD5110_ModeTypeDef;

/* LCD 5110 Temperature Coefficients commands definition */
typedef enum __LCD5110_TempCoefficientsTypeDef
{

	LCD5110_TEMPERATURE_COEFFICIENT_0 = 0, // The command used to select the temperature coefficient 0 (for lower temperatures and higher screen contrast)
	LCD5110_TEMPERATURE_COEFFICIENT_1 = 1, // The command used to select the temperature coefficient 1 (LCD's typical voltage curve)
	LCD5110_TEMPERATURE_COEFFICIENT_2 = 2, // The command used to select the temperature coefficient 2 (temperature coefficient of the display's IC)
	LCD5110_TEMPERATURE_COEFFICIENT_3 = 3  // The command used to select the temperature coefficient 3 (for higher temperatures and lower screen contrast)

} LCD5110_TempCoefficientsTypeDef;

/* LCD 5110 Coordinates structure definition */
typedef struct __LCD5110_CoordinatesTypeDef
{

	uint8_t	XPosition;
	uint8_t	YPosition;

} LCD5110_CoordinatesTypeDef;

/* LCD 5110 Font structure definition */
typedef struct __LCD5110_FontTypeDef
{

	uint8_t         FontWidth;  // The width of each character of the font in pixels (including the padding)
	char			FirstChar;  // The first character of the font (according to ASCII standards)
	char			LastChar;   // The last character of the font (according to ASCII standards)
	unsigned char  *pFontTable; // Pointer to the first adress of the font's table

} LCD5110_FontTypeDef;

/* LCD 5110 Error List enumeration definition */
typedef enum __LCD5110_ErrorListTypeDef
{

	LCD5110_ERROR_NONE				                       =  0,
	LCD5110_INVALID_TEMPERATURE_COEFFICIENT                =  1,
	LCD5110_INVALID_CONTRAST_PERCENTAGE                    =  2,
	LCD5110_INVALID_XY_COORDINATES                         =  3,
	LCD5110_INVALID_DATA_BLOCK_SIZE                        =  4,
	LCD5110_INVALID_FONT_INDEX			       	           =  5,
	LCD5110_INVALID_CHARACTER                              =  6,
	LCD5110_INVALID_STRING_LENGTH	                       =  7,
	LCD5110_INVALID_XY_SUBCOORDINATES		               =  8,
	LCD5110_INVALID_ORIGIN_DESTINATION_SUBCOORDINATES      =  9,
	LCD5110_INVALID_FIRST_AND_SECOND_VERTEX_SUBCOORDINATES = 10,
	LCD5110_INVALID_EDGES_LENGTH						   = 11,
	LCD5110_INVALID_CIRCLE_RADIUS_CENTER				   = 12

} LCD5110_ErrorListTypeDef;

/* LCD 5110 Handler definition */
typedef struct __LCD5110_HandleTypeDef
{

	SPI_HandleTypeDef              *phspi; // Pointer to a handler of an SPI peripheral responsible for generalizing the calling of this peripheral

	GPIO_TypeDef                   *Port_DC; // Pointer to all GPIO Ports responsible for generalizing the port that has the D / C pin
	uint16_t                        Pin_DC; // Variable responsible for generalizing the D / C pin

	GPIO_TypeDef                   *Port_SCE; // Pointer to all GPIO Ports responsible for generalizing the port that has the SCE pin
	uint16_t                        Pin_SCE; // Variable responsible for generalizing the SCE pin

	HAL_StatusTypeDef             (*pWriteFunction) (struct __LCD5110_HandleTypeDef *, GPIO_PinState, uint8_t *, uint16_t); // Pointer to the write function used in the current mode

	LCD5110_StateTypeDef            State; // State of the display

	uint8_t                         Vop; // Current operation voltage
	LCD5110_TempCoefficientsTypeDef TempCoefficient; // Current temperature coefficient
	uint8_t                         BiasLevel; // Current bias level
	uint8_t                         ScreenMode; // Current screen's mode
	LCD5110_CoordinatesTypeDef      Coordinates; // Current X and Y coordinates on the display

	LCD5110_FontTypeDef 		    TextFont; // Current font used in the display

	uint8_t					        DrawingBuffer [LCD5110_SCREEN_SIZE]; // Buffer used for storing drawings

	LCD5110_ErrorListTypeDef        ErrorCode; // In case of error, it points to the corresponding occurence

} LCD5110_HandleTypeDef;

/* Functions' Prototypes */

/* Function used for selecting the respective pins of a LCD 5110:
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – phspi is a pointer to the SPI Handler used for the display;
 * – Port_DC is the GPIO Port of the D / C pin;
 * – Pin_DC is the D / C GPIO Pin;
 * – Port_SCE it the GPIO Port of the SCE pin;
 * – Pin_SCE it the SCE GPIO Pin;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Config_Pins (LCD5110_HandleTypeDef *hlcd5110, SPI_HandleTypeDef *phspi, GPIO_TypeDef *Port_DC, uint16_t Pin_DC, GPIO_TypeDef *Port_SCE, uint16_t Pin_SCE);

/* Function that must be called inside the HAL_SPI_TxCpltCallback in case of non-blocking mode operation:
 *
 * If using either the interruption or DMA mode, the calling of this function inside the HAL_SPI_TxCpltCallback
 * is necessary for the rest of the next functions related to the display to work.
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – hspi is a pointer to a SPI Handler;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_TxCpltCallback (LCD5110_HandleTypeDef *hlcd5110, SPI_HandleTypeDef *hspi);

/* Sends a block of data (bytes) to the display's screen:
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – pDataBlock is a pointer to the beginning of the data block;
 * – Size is the size of the data block in bytes (maximum of 504 bytes);
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Write_DataBlock (LCD5110_HandleTypeDef *hlcd5110, uint8_t *pDataBlock, uint16_t Size);

/* Clears the whole screen (fills it with zeros):
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Clear (LCD5110_HandleTypeDef *hlcd5110);

/* Clears the whole screen inversely (fills it with ones):
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_InvClear (LCD5110_HandleTypeDef *hlcd5110);

/* Function responsible for initializing the display with default values and configuring its mode (blocking or non-blocking):
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – Mode is the LCD 5110 Mode structure with the desired transmission mode;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Init (LCD5110_HandleTypeDef *hlcd5110, LCD5110_ModeTypeDef Mode);

/* Function responsible for putting the display to sleep (low power consumption, clearing the screen):
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Enable_Sleep (LCD5110_HandleTypeDef *hlcd5110);

/* Function responsible for disabling the display's sleep (low power consumption mode):
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Disable_Sleep (LCD5110_HandleTypeDef *hlcd5110);

/* Sets the Temperature Coefficient of the display (that affects the screen contrast);
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – TempCoefficient is the temperature coefficient to be used (see LCD5110_TemperatureCoefficientsTypeDef for more information);
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Set_TempCoefficient (LCD5110_HandleTypeDef *hlcd5110, LCD5110_TempCoefficientsTypeDef TempCoefficient);

/* Sets the mode used for transmiting data to the display (blocking of non-blocking):
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – Mode is the LCD 5110 Mode structure with the desired transmission mode;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Set_Mode (LCD5110_HandleTypeDef *hlcd5110, LCD5110_ModeTypeDef Mode);

/* Changes the screen contrast:
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – Contrast is a percentage from 0 % to 100 % (without the '%' sign);
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Set_Contrast (LCD5110_HandleTypeDef *hlcd5110, float Contrast);

/* Inverts the contents of the screen (what was 'on' turns 'off' and vice-versa):
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Inv_Screen (LCD5110_HandleTypeDef *hlcd5110);

/* Sets the font used for the text-writing and coordinate-setting functions:
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – TextFont is a LCD 5110 Font structure with the font's information;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Set_Font (LCD5110_HandleTypeDef *hlcd5110, LCD5110_FontTypeDef TextFont);

/* Sets the X and Y text position on the display:
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – X is the position in the X-axis of the display (from 0 to the maximum number of characters in a line with the last selected font);
 * – Y is the position in the Y-axis of the display (from 0 to 5);
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Set_XY (LCD5110_HandleTypeDef *hlcd5110, uint8_t X, uint8_t Y);

/* Function used for getting the LCD 5110's current coordinates:
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – Returns the LCD 5110's Coordinates.
 */
LCD5110_CoordinatesTypeDef LCD5110_Get_XY (LCD5110_HandleTypeDef *hlcd5110);

/* Function that sends a string to the display:
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – pString is the string to be written;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Write_String (LCD5110_HandleTypeDef *hlcd5110, char *pString);

/* Function that sends a string with inverted characters to the display:
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – pString is the string to be written;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Write_InvString (LCD5110_HandleTypeDef *hlcd5110, char *pString);

/* Function that draws a line from a pixel's subcoordinate to another pixel's subcoordinate:
 *
 * This function sends the drawing buffer with all the previously drawn figures plus the figure
 * drawn by this function. In order to send a single figure, use the LCD5110_Clear function first, then
 * use the respective drawing function.
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – x0 is the origin's x value of the subcoordinate;
 * – y0 is the origin's y value of the subcoordinate;
 * – x1 is the final x value of the subcoordinate (must be bigger or equal to x0);
 * – y1 is the final y value of the subcoordinate;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Draw_Line (LCD5110_HandleTypeDef *hlcd5110, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/* Function that draws an inverse line from a pixel's subcoordinate to another pixel's subcoordinate:
 *
 * This function sends the drawing buffer with all the previously drawn figures plus the figure
 * drawn by this function. In order to send a single figure, use the LCD5110_Clear function first, then
 * use the respective drawing function.
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – x0 is the origin's x value of the subcoordinate;
 * – y0 is the origin's y value of the subcoordinate;
 * – x1 is the final x value of the subcoordinate (must be bigger or equal to x0);
 * – y1 is the final y value of the subcoordinate;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Draw_InvLine (LCD5110_HandleTypeDef *hlcd5110, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/* Function that draws a rectangle from a vertex's subcoordinate to another vertex's subcoordinate:
 *
 * This function sends the drawing buffer with all the previously drawn figures plus the figure
 * drawn by this function. In order to send a single figure, use the LCD5110_Clear function first, then
 * use the respective drawing function.
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – x0 is the first vertex's x value of the subcoordinate;
 * – y0 is the first vertex's y value of the subcoordinate;
 * – x1 is the second vertex's x value of the subcoordinate (must be bigger or equal to x0);
 * – y1 is the second vertex's y value of the subcoordinate (must be bigger or equal to y0);
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Draw_Rect (LCD5110_HandleTypeDef *hlcd5110, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/* Function that draws an inverse rectangle from a vertex's subcoordinate to another vertex's subcoordinate:
 *
 * This function sends the drawing buffer with all the previously drawn figures plus the figure
 * drawn by this function. In order to send a single figure, use the LCD5110_Clear function first, then
 * use the respective drawing function.
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – x0 is the first vertex's x value of the subcoordinate;
 * – y0 is the first vertex's y value of the subcoordinate;
 * – x1 is the second vertex's x value of the subcoordinate (must be bigger or equal to x0);
 * – y1 is the second vertex's y value of the subcoordinate (must be bigger or equal to y0);
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Draw_InvRect (LCD5110_HandleTypeDef *hlcd5110, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/* Function that draws a rounded rectangle from a vertex's subcoordinate to another vertex's subcoordinate (at least 5 pixels of edge's length):
 *
 * This function sends the drawing buffer with all the previously drawn figures plus the figure
 * drawn by this function. In order to send a single figure, use the LCD5110_Clear function first, then
 * use the respective drawing function.
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – x0 is the first vertex's x value of the subcoordinate;
 * – y0 is the first vertex's y value of the subcoordinate;
 * – x1 is the second vertex's x value of the subcoordinate (must be bigger or equal to x0);
 * – y1 is the second vertex's y value of the subcoordinate (must be bigger or equal to y0);
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Draw_RoundRect (LCD5110_HandleTypeDef *hlcd5110, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/* Function that draws an inverted rounded rectangle from a vertex's subcoordinate to another vertex's subcoordinate (at least 5 pixels of edge's length):
 *
 * This function sends the drawing buffer with all the previously drawn figures plus the figure
 * drawn by this function. In order to send a single figure, use the LCD5110_Clear function first, then
 * use the respective drawing function.
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – x0 is the first vertex's x value of the subcoordinate;
 * – y0 is the first vertex's y value of the subcoordinate;
 * – x1 is the second vertex's x value of the subcoordinate (must be bigger or equal to x0);
 * – y1 is the second vertex's y value of the subcoordinate (must be bigger or equal to y0);
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Draw_InvRoundRect (LCD5110_HandleTypeDef *hlcd5110, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/* Function that draws a circle from the center's subcoordinate and radius:
 *
 * This function sends the drawing buffer with all the previously drawn figures plus the figure
 * drawn by this function. In order to send a single figure, use the LCD5110_Clear function first, then
 * use the respective drawing function.
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – x is the center's x value of the subcoordinate;
 * – y is the certer's y value of the subcoordinate;
 * – Radius is the radius of the circle in pixels;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Draw_Circle (LCD5110_HandleTypeDef *hlcd5110, uint8_t x, uint8_t y, uint8_t Radius);

/* Function that draws an inverted circle from the center's subcoordinate and radius:
 *
 * This function sends the drawing buffer with all the previously drawn figures plus the figure
 * drawn by this function. In order to send a single figure, use the LCD5110_Clear function first, then
 * use the respective drawing function.
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – x is the center's x value of the subcoordinate;
 * – y is the certer's y value of the subcoordinate;
 * – Radius is the radius of the circle in pixels;
 * – Returns the HAL Status.
 */
HAL_StatusTypeDef LCD5110_Draw_InvCircle (LCD5110_HandleTypeDef *hlcd5110, uint8_t x, uint8_t y, uint8_t Radius);

/* Function used for getting the LCD 5110's error code:
 *
 * – hlcd5110 is a pointer to a LCD 5110 Handler;
 * – Returns the HAL Status.
 */
LCD5110_ErrorListTypeDef LCD5110_Get_Error (LCD5110_HandleTypeDef *hlcd5110);

#endif
