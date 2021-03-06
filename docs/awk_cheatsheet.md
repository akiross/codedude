# AkiRoss' AWK cheatsheet
by Alessandro [AkiRoss] Re -> www.ale-re.net

## Big helps:
 * http://www.vectorsite.net/tsawk.html
 * http://www.gnu.org/manual/gawk/html_node/index.html

## My help (a cheatsheet):
 * AWK stands for Aho, Weinberger, Kernighan.
 * Quick console usage
   * awk -f script_file input_data
   * awk 'pattern { action }' input_data
   * awk 'rules' -			Read input from stdin
 * Script are made of rules:
   * pattern { action }
   * action is executed if pattern is matched
   * you can omit pattern to execute that action on every row
   * you can omit action to print the row that match your pattern
 * Syntax is really similar to C one
   * { block }
   * if (cond) ... else ...
   * # Comments
   * while, for, do, switch, break, continue, next, exit
 * BEGIN and END patterns are fixed and executed when script begin and end
 * Usual operators
   * = (assign)
   * == (compare)
   * ++, +=, ...
 * Strings are "as usual", but AWK can evaluate them as integer
   * "123" == 123
 * In addition there are regexps, quoted between /s
   * /^[Rr]eg[Ee]xp?$/
   * metachars doesn't need quoting
   * usual metachars
 * Patterns are automagically divided into fields
   * Fields are separated by a given char (default is ' ') or regex
   * $0						Is the whole line
   * $n						Is the n-th field (n > 0)
   * Field variables aren't read only, you can write into them
   * Modify the field separator with -F on the command line or use the FS var
   * FS						Can be a char or a regex.
     						In this case, regex is quoted with "" instead of //
 * Patterns are evaluated expressions:
   * /regex/					matches lines with contains regex
     						(e.g. regex, regexASD, ASDregexASD)
   * /^regex$/ 				to match exactly that regex
   * "matchall"				matches all lines
   * ""						matches no line
   * "123" == 123 			is a valid pattern
   * ($1 == "hello") &&		boolean composition
   * ($2 == "folk")
 * There is the ~ operator for matching regexp
   * $1 ~ /regex/			first field match regex
   * $2 !~ /regex/			second field doesn't match regex
 * You can use variables in your script
   * They don't need to be declared
   * Default value is 0 or "" for unitialized variables
   * Auto conversion between math and strings
   * There are pre-defined variables
     * Some of them are:
     * NR					Keeps how many rows in input has been processed
     * FS					Field separator variable, can be char or regex (default spaces)
     * OFS					Output field separator (default spaces)
     * NF					Number of fields on this line
     * FILENAME			Guess...
 * You can use some functions for your actions
   * print is the default action. Does newline.
     * print 				behaves like print $0
     * print param			print the parameters
     * print a,b,c 		print a, b and c separed by OFS
   * printf formatted print. Works like in C
     * printf("Your output, Mr %s\n", "John");
   * sprintf return a string with printf output
   * sqrt(), log(), exp(), int()
   * substr(), split(), index()
 * Redirections and pipes
   * print > "file"			Print on file (create and overwrite)
   * print >> "file"			Print on file (create and append)
   * print | "tool opts"		Pipes the output to a shell tool
   * they works on outputs, not variables or return values
