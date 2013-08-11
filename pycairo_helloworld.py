#!/usr/bin/env python3

from qkdrw import quickdraw
import cairo

@quickdraw()
def draw(widget, qc):
	qc.set_source_rgb(0, 0, 255)
	qc.select_font_face("Space Age", cairo.FONT_SLANT_ITALIC, cairo.FONT_WEIGHT_BOLD)
	qc.set_font_size(20)
	qc.move_to(20, 20)
	qc.show_text("Hello, World!")


