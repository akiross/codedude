/*
 This is a very, _very_ simple code formatting tool for Google Docs,
 made by Alessandro "akiross" Re - ale-re.net
 
 This work is provided "as-is", distributed under the Creative Commons
 Attribution-NonCommercial-ShareAlike 3.0, available at the following URL:
   http://creativecommons.org/licenses/by-nc-sa/3.0/
 
 You're free to use, modify and distribute this work, as long as you keep
 the same license and you give credits to the original author.
 
 This script will add a menu entry to format your text.
 Just select the text you like to format and chose the language you want.
 The code will be colored, font changed to Courier New and Docs-style quotes
 will be replaced with ' and " (so that the code should be copypaste-able).
 
 Changelog
 v 0.1 - initial code, minimal support for Python3 and micro support for C.
 v 0.2 - updated keywords and functions for python.
*/

function onOpen() {
  DocumentApp.getUi().createMenu('Code Formatter')
      .addSubMenu(DocumentApp.getUi().createMenu('Highlight')
          .addItem('Python3', 'hiliPy3')
          .addItem('C', 'hiliPy3')
      ).addToUi();
}

function replace_quotes(text) {
  var replacements = {'‘': "'", '’': "'", '“': '"', '”': '"'};
  for (var k in replacements) {
    text.replaceText(k, replacements[k]);
  }
}

function colorize_keyword(text, keyword, color) {
  var s = null;
  replace_quotes(text);
  
  keyword = "(\\W|^)" + keyword + "(\\W|$)"; // Ensure it is a token
  // FIXME: note that start and end may include \W characters like ( or ].
  var valids = '()[]{}<>.,:;@+-*/%^&|';//’”';
  while (true) {
    var res = text.findText(keyword, s);
    if (res) {
      start = res.getStartOffset();
      end = res.getEndOffsetInclusive();
      var matched = text.getText().substring(start, end + 1);
      var m0 = matched[0];
      var mL = matched[matched.length - 1];
      Logger.log("Matched:" + matched + '--' + m0 + '--' + mL);
      if (valids.indexOf(m0) > -1) start += 1;
      if (valids.indexOf(mL) > -1) end -= 1;
      text.setForegroundColor(start, end, color);
      s = res;
    }
    else
      break;
  }
}

function highlighter(colors) {
  var selection = DocumentApp.getActiveDocument().getSelection();
  if (selection) {
    var elements = selection.getSelectedElements();
    for (e in elements) {
      var elem = elements[e].getElement();
      var type = elem.getType();
      var text;
      if (type == DocumentApp.ElementType.PARAGRAPH) {
        text = elem.editAsText();
      }
      else if (type == DocumentApp.ElementType.TEXT) {
        text = elem;
      }
      text.setFontFamily(DocumentApp.FontFamily.COURIER_NEW);
      for (col in colors) {
        var expr = colors[col];
        for (i in expr) {
          colorize_keyword(text, expr[i], col);
        }
      }
    }
  } else {
    DocumentApp.getUi().alert('Nothing is selected. Please select some text to highlight.');
  }
}

function hiliPy3() {
  var keys = ['and', 'as', 'assert', 'break', 'class',
              'continue', 'def', 'del', 'elif', 'else', 'except', 'finally',
              'for', 'from', 'global', 'if', 'import', 'in', 'is', 'lambda',
              'nonlocal', 'not', 'or', 'pass', 'raise', 'return', 'try',
              'while', 'with', 'yield'];
  var funcs = ['abs', 'dir', 'hex', 'next', 'slice', 'all', 'divmod', 'id',
               'object', 'sorted', 'any', 'enumerate', 'input', 'oct',
               'staticmethod', 'ascii', 'eval', 'int', 'open', 'str', 'bin',
               'exec', 'isinstance', 'ord', 'sum', 'bool', 'filter',
               'issubclass', 'pow', 'super', 'bytearray', 'float', 'iter',
               'print', 'tuple', 'bytes', 'format', 'len', 'property', 'type',
               'chr', 'frozenset', 'list', 'range', 'vars', 'classmethod',
               'getattr', 'locals', 'repr', 'zip', 'compile', 'globals', 'map',
               'reversed', '__import__', 'complex', 'hasattr', 'max', 'round',
               'delattr', 'hash', 'memoryview', 'set', 'dict', 'help', 'min',
               'setattr'];
  var modules = ['sys', 'os', 'urllib', 'random', 'math', 'argparse'];
  var comments = ['#.*$'];
  var strings = ['"[^"]*"', "'[^']*'", '[^”]*”', '[‘’“”][^‘’“”]*[‘’“”]'];
  var literals = ['True', 'False', 'None', '\\d+', '\\d*\\.\\d+',
                  '0x[a-fA-F0-9]+'];
  var colors = {'#FF0000': keys, '#0000FF': modules, '#0088FF': funcs,
                '#FF00FF': literals, '#777777': comments, '#00AA00': strings};
  highlighter(colors);
}

function hiliC() {
  var keys = ['typedef', 'enum', 'if', 'else', 'return'];
  var funcs = ['printf'];
  var comments = ['#.*$', '//.*$'];
  var strings = ['"[^"]*"', "'[^']*'", '"'];
  var literals = ['\\d+', '\\d*\\.\\d+', '0x[a-fA-F0-9]+'];
  var colors = {'#FF0000': keys, '#0088FF': funcs, '#FF00FF': literals, '#777777': comments, '#00AA00': strings};
  highlighter(colors);
}

