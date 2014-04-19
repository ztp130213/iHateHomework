#############################################################################
#                     U N R E G I S T E R E D   C O P Y
# 
# You are on day 24 of your 30 day trial period.
# 
# This file was produced by an UNREGISTERED COPY of Parser Generator. It is
# for evaluation purposes only. If you continue to use Parser Generator 30
# days after installation then you are required to purchase a license. For
# more information see the online help or go to the Bumble-Bee Software
# homepage at:
# 
# http://www.bumblebeesoftware.com
# 
# This notice must remain present in the file. It cannot be removed.
#############################################################################

#############################################################################
# myparser.v
# YACC verbose file generated from myparser.y.
# 
# Date: 12/20/10
# Time: 23:58:47
# 
# AYACC Version: 2.07
#############################################################################


##############################################################################
# Rules
##############################################################################

    0  $accept : lines $end

    1  lines : lines expr '\n'
    2        | lines '\n'
    3        |
    4        | error '\n'

    5  expr : expr '+' term
    6       | expr '-' term
    7       | term

    8  term : term '*' factor
    9       | term '/' factor
   10       | factor

   11  factor : '(' expr ')'
   12         | '(' expr error
   13         | '-' factor
   14         | NUMBER


##############################################################################
# States
##############################################################################

state 0
	$accept : . lines $end
	lines : .  (3)

	error  shift 1
	.  reduce 3

	lines  goto 2


state 1
	lines : error . '\n'

	'\n'  shift 3


state 2
	$accept : lines . $end  (0)
	lines : lines . expr '\n'
	lines : lines . '\n'

	$end  accept
	'\n'  shift 4
	'('  shift 5
	'-'  shift 6
	NUMBER  shift 7

	expr  goto 8
	term  goto 9
	factor  goto 10


state 3
	lines : error '\n' .  (4)

	.  reduce 4


state 4
	lines : lines '\n' .  (2)

	.  reduce 2


state 5
	factor : '(' . expr ')'
	factor : '(' . expr error

	'('  shift 5
	'-'  shift 6
	NUMBER  shift 7

	expr  goto 11
	term  goto 9
	factor  goto 10


state 6
	factor : '-' . factor

	'('  shift 5
	'-'  shift 6
	NUMBER  shift 7

	factor  goto 12


state 7
	factor : NUMBER .  (14)

	.  reduce 14


state 8
	lines : lines expr . '\n'
	expr : expr . '+' term
	expr : expr . '-' term

	'\n'  shift 13
	'+'  shift 14
	'-'  shift 15


state 9
	expr : term .  (7)
	term : term . '*' factor
	term : term . '/' factor

	'*'  shift 16
	'/'  shift 17
	.  reduce 7


state 10
	term : factor .  (10)

	.  reduce 10


state 11
	expr : expr . '+' term
	expr : expr . '-' term
	factor : '(' expr . ')'
	factor : '(' expr . error

	')'  shift 18
	'+'  shift 14
	'-'  shift 15
	error  shift 19


state 12
	factor : '-' factor .  (13)

	.  reduce 13


state 13
	lines : lines expr '\n' .  (1)

	.  reduce 1


state 14
	expr : expr '+' . term

	'('  shift 5
	'-'  shift 6
	NUMBER  shift 7

	term  goto 20
	factor  goto 10


state 15
	expr : expr '-' . term

	'('  shift 5
	'-'  shift 6
	NUMBER  shift 7

	term  goto 21
	factor  goto 10


state 16
	term : term '*' . factor

	'('  shift 5
	'-'  shift 6
	NUMBER  shift 7

	factor  goto 22


state 17
	term : term '/' . factor

	'('  shift 5
	'-'  shift 6
	NUMBER  shift 7

	factor  goto 23


state 18
	factor : '(' expr ')' .  (11)

	.  reduce 11


state 19
	factor : '(' expr error .  (12)

	.  reduce 12


state 20
	expr : expr '+' term .  (5)
	term : term . '*' factor
	term : term . '/' factor

	'*'  shift 16
	'/'  shift 17
	.  reduce 5


state 21
	expr : expr '-' term .  (6)
	term : term . '*' factor
	term : term . '/' factor

	'*'  shift 16
	'/'  shift 17
	.  reduce 6


state 22
	term : term '*' factor .  (8)

	.  reduce 8


state 23
	term : term '/' factor .  (9)

	.  reduce 9


##############################################################################
# Summary
##############################################################################

10 token(s), 5 nonterminal(s)
15 grammar rule(s), 24 state(s)


##############################################################################
# End of File
##############################################################################
