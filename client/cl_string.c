/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// cl_string.c
// string drawing and formatting functions

#include "client.h"


/*
================
StringSetParams
================
*/
qboolean StringSetParams (char modifier, int32_t *red, int32_t *green, int32_t *blue, qboolean *bold, qboolean *shadow, qboolean *italic, qboolean *reset)
{
	if (!alt_text_color)
		alt_text_color = Cvar_Get ("alt_text_color", "2", CVAR_ARCHIVE);

	switch (modifier)
	{
		case 'R':
		case 'r':
			*reset = true;
			return true;
		case 'B':
		case 'b':
			if (*bold) 
				*bold = false;
			else 
				*bold=true;
			return true;
		case 'S':
		case 's':
			if (*shadow) 
				*shadow=false; 
			else 
				*shadow=true;
			return true;
		case 'I':
		case 'i':
			if (*italic) 
				*italic=false; 
			else 
				*italic=true;
			return true;
		case C_COLOR_RED:
		case C_COLOR_GREEN:
		case C_COLOR_YELLOW:
		case C_COLOR_BLUE:
		case C_COLOR_CYAN:
		case C_COLOR_MAGENTA:
		case C_COLOR_WHITE:
		case C_COLOR_BLACK:
		case C_COLOR_ORANGE:
		case C_COLOR_GRAY:
			ColorLookup(atoi(&modifier), red, green, blue);
			return true;
		case 'A':	//alt text color
		case 'a':
			ColorLookup((int32_t)alt_text_color->value, red, green, blue);
			return true;
	}
	
	return false;
}


/*
================
DrawStringGeneric
================
*/
void DrawStringGeneric (int32_t x, int32_t y, const char *string, int32_t alpha, textscaletype_t scaleType, qboolean altBit)
{
	uint32_t i, j;
	uint8_t modifier, character;
	int32_t len, red, green, blue;
	qboolean italic, shadow, bold, reset, modified = false;
	float textSize, textScale;

	// defaults
	red = 255;
	green = 255;
	blue = 255;
	italic = false;
	shadow = false;
	bold = false;

	len = strlen( string );
	for ( i = 0, j = 0; i < len; i++ )
	{
		modifier = string[i];
		if (modifier&128) modifier &= ~128;

		if (modifier == '^' && i < len)
		{
			i++;

			reset = false;
			modifier = string[i];
			if (modifier&128) modifier &= ~128;

			if (modifier!='^')
			{
				modified = StringSetParams(modifier, &red, &green, &blue, &bold, &shadow, &italic, &reset);

				if (reset)
				{
					red = 255;
					green = 255;
					blue = 255;
					italic = false;
					shadow = false;
					bold = false;
				}
				if (modified)
					continue;
				else
					i--;
			}
		}
		j++;

		character = string[i];
		if (bold && character < 128)
			character += 128;
		else if (bold && character > 128)
			character -= 128;

		switch (scaleType)
		{
		case SCALETYPE_MENU:
			textSize = SCR_ScaledVideo(MENU_FONT_SIZE);
			textScale = SCR_VideoScale();
			break;
		case SCALETYPE_HUD:
			textSize = scaledHud(HUD_FONT_SIZE);
			textScale = HudScale();
			// hack for alternate text color
			if (altBit) {
				if (!modified) {
					character ^= 0x80;
					if (character & 128)
						ColorLookup((int32_t)alt_text_color->value, &red, &green, &blue);
				}
			}
			else {
				if (!modified && (character & 128))
					ColorLookup((int32_t)alt_text_color->value, &red, &green, &blue);
			}
			break;
		case SCALETYPE_CONSOLE:
		default:
			textSize = FONT_SIZE;
			textScale = FONT_SIZE/8.0;
			// hack for alternate text color
			if (!modified && (character & 128))
				ColorLookup((int32_t)alt_text_color->value, &red, &green, &blue);
			break;
		}

		if (shadow)
			R_DrawChar( ( x + (j-1)*textSize+textSize/4 ), y+(textSize/8), 
				character, textScale, 0, 0, 0, alpha, italic, false );
		R_DrawChar( ( x + (j-1)*textSize ), y,
			character, textScale, red, green, blue, alpha, italic, (i==(len-1)) );
	}
}
