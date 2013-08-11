#!/usr/bin/env python3

from gi.repository import Gtk
import cairo

def quickdraw(title='Quick Context', start_time=-1, end_time=-1):
	'''Decorator which launches a Gtk window with a Cairo context in it. The
	decorated function will receive the widget and context as parameters and
	will be used to draw on the screen.
	The function is *used directly* in the decoration process and no call is
	needed. This decorator returns None if the main loop quit.
	
	Example usage:
	
	from qkdrw import quickdraw
	@quickdraw()
	def draw(widget, ctx):
		ctx.move_to(20, 20)
		ctx.show_text("Hello, World!")
	'''
	def _wrapper(draw_func):
		class QuickDrawWindow(Gtk.Window):
			def __init__(self):
				Gtk.Window.__init__(self, title=title)
				darea = Gtk.DrawingArea()
				darea.connect('draw', draw_func)
				self.add(darea)
		win = QuickDrawWindow()
		win.connect("delete-event", Gtk.main_quit)
		win.show_all()
		Gtk.main()
		return None
	return _wrapper

# TODO
class QuickDrawEditor(Gtk.Window):
	'''Open a window with a drawing context and a text editor.
	The code in the editor is evaluated to draw the screen.'''
	def __init__(self):
		pass 
