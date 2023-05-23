#include "display.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Screenbuffer
static uint8_t Display_Buffer[DISPLAY_BUFFER_SIZE];

// Screen object
static DISPLAY_t DISPLAY;

// Send a byte to the command register
void display_write_command(uint8_t byte) {
    HAL_I2C_Mem_Write(&DISPLAY_I2C_PORT, DISPLAY_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
}

// Send data
void display_write_data(uint8_t* buffer, size_t buff_size) {
    HAL_I2C_Mem_Write(&DISPLAY_I2C_PORT, DISPLAY_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
}

/* Fills the Screenbuffer with values from a given buffer of a fixed length */
SSD1306_Error_t display_fill_framebuffer(uint8_t* buf, uint32_t len) {

    SSD1306_Error_t ret = SSD1306_ERR;

    if (len <= DISPLAY_BUFFER_SIZE) {

        memcpy(Display_Buffer, buf, len);
        ret = SSD1306_OK;
    }

    return ret;
}

// Initialize the oled screen
void display_init(void) {

    // Wait for the screen to boot
    HAL_Delay(100);

    // Init OLED
    display_set_on(0); //display off

    display_write_command(0x20); //Set Memory Addressing Mode
    display_write_command(0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                // 10b,Page Addressing Mode (RESET); 11b,Invalid

    display_write_command(0xB0); //Set Page Start Address for Page Addressing Mode,0-7

#ifdef SSD1306_MIRROR_VERT
    display_write_command(0xC0); // Mirror vertically
#else
    display_write_command(0xC8); //Set COM Output Scan Direction
#endif

    display_write_command(0x00); //---set low column address
    display_write_command(0x10); //---set high column address

    display_write_command(0x40); //--set start line address - CHECK

    display_set_contrast(0xFF);

#ifdef SSD1306_MIRROR_HORIZ
    display_write_command(0xA0); // Mirror horizontally
#else
    display_write_command(0xA1); //--set segment re-map 0 to 127 - CHECK
#endif

#ifdef SSD1306_INVERSE_COLOR
    display_write_command(0xA7); //--set inverse color
#else
    display_write_command(0xA6); //--set normal color
#endif

// Set multiplex ratio.
#if (DISPLAY_HEIGHT == 128)
    // Found in the Luma Python lib for SH1106.
    display_write_command(0xFF);
#else
    display_write_command(0xA8); //--set multiplex ratio(1 to 64) - CHECK
#endif

#if (DISPLAY_HEIGHT == 32)
    display_write_command(0x1F); //
#elif (DISPLAY_HEIGHT == 64)
    display_write_command(0x3F); //
#elif (DISPLAY_HEIGHT == 128)
    display_write_command(0x3F); // Seems to work for 128px high displays too.
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    display_write_command(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    display_write_command(0xD3); //-set display offset - CHECK
    display_write_command(0x00); //-not offset

    display_write_command(0xD5); //--set display clock divide ratio/oscillator frequency
    display_write_command(0xF0); //--set divide ratio

    display_write_command(0xD9); //--set pre-charge period
    display_write_command(0x22); //

    display_write_command(0xDA); //--set com pins hardware configuration - CHECK
#if (DISPLAY_HEIGHT == 32)
    display_write_command(0x02);
#elif (DISPLAY_HEIGHT == 64)
    display_write_command(0x12);
#elif (DISPLAY_HEIGHT == 128)
    display_write_command(0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    display_write_command(0xDB); //--set vcomh
    display_write_command(0x20); //0x20,0.77xVcc

    display_write_command(0x8D); //--set DC-DC enable
    display_write_command(0x14); //
    display_set_on(1); //--turn on SSD1306 panel

    // Clear screen
    display_fill(Black);

    // Flush buffer to screen
    display_update();

    // Set default values for screen object
    DISPLAY.CurrentX = 0;
    DISPLAY.CurrentY = 0;

    DISPLAY.Initialized = 1;
}

// Position the cursor
void display_set_cursor(uint8_t x, uint8_t y) {

	DISPLAY.CurrentX = x;
	DISPLAY.CurrentY = y;
}

// Fill the whole screen with the given color
void display_fill(DISPLAY_COLOR color) {

    /* Set memory */
    uint32_t i;

    for(i = 0; i < sizeof(Display_Buffer); i++) {
        Display_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

// Write the screenbuffer with changed to the screen
void display_update(void) {

    // Write data to each page of RAM. Number of pages
    // depends on the screen height:
    //
    //  * 32px   ==  4 pages
    //  * 64px   ==  8 pages
    //  * 128px  ==  16 pages
    for(uint8_t i = 0; i < DISPLAY_HEIGHT/8; i++) {
    	display_write_command(0xB0 + i); // Set the current RAM page address.
    	display_write_command(0x00);
    	display_write_command(0x10);
        display_write_data(&Display_Buffer[DISPLAY_WIDTH*i], DISPLAY_WIDTH);
    }
}

//    Draw one pixel in the screenbuffer
//    X => X Coordinate
//    Y => Y Coordinate
//    color => Pixel color
void display_draw_pixel(uint8_t x, uint8_t y, DISPLAY_COLOR color) {

    if(x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) {
        // Don't write outside the buffer
        return;
    }

    // Draw in the right color
    if(color == White) {
        Display_Buffer[x + (y / 8) * DISPLAY_WIDTH] |= 1 << (y % 8);
    }
    else {
        Display_Buffer[x + (y / 8) * DISPLAY_WIDTH] &= ~(1 << (y % 8));
    }
}

// Draw 1 char to the screen buffer
// ch       => char om weg te schrijven
// Font     => Font waarmee we gaan schrijven
// color    => Black or White
char display_write_char(char ch, FontDef Font, DISPLAY_COLOR color) {
    uint32_t i, b, j;

    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;

    // Check remaining space on current line
    if (DISPLAY_WIDTH < (DISPLAY.CurrentX + Font.FontWidth) || DISPLAY_HEIGHT < (DISPLAY.CurrentY + Font.FontHeight)) {
        // Not enough space on current line
        return 0;
    }

    // Use the font to write
    for(i = 0; i < Font.FontHeight; i++) {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for(j = 0; j < Font.FontWidth; j++) {
            if((b << j) & 0x8000)  {
            	display_draw_pixel(DISPLAY.CurrentX + j, (DISPLAY.CurrentY + i), (DISPLAY_COLOR) color);
            } else {
            	display_draw_pixel(DISPLAY.CurrentX + j, (DISPLAY.CurrentY + i), (DISPLAY_COLOR)!color);
            }
        }
    }

    // The current space is now taken
    DISPLAY.CurrentX += Font.FontWidth;

    // Return written char for validation
    return ch;
}

void display_set_contrast(const uint8_t value) {

    const uint8_t kSetContrastControlRegister = 0x81;
    display_write_command(kSetContrastControlRegister);
    display_write_command(value);
}

void display_set_on(const uint8_t on) {

    uint8_t value;
    if (on) {
        value = 0xAF;   // Display on
        DISPLAY.DisplayOn = 1;
    } else {
        value = 0xAE;   // Display off
        DISPLAY.DisplayOn = 0;
    }
    display_write_command(value);
}

uint8_t display_get_on() {

    return DISPLAY.DisplayOn;
}
