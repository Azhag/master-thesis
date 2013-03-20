#!/bin/csh
# global $HOME/.cshrc file -- humphrey@cs.ucsb.edu, 01/06/97
#
#   Updated by Don Voita -- 12 Jan 2005
#
# Users who invoke a C shell can expect this file to be sourced every time
# they do so.  For users whose login shell is the C shell, this file will
# be executed after /etc/.login but before $HOME/.login.

# Computer Science only supports SunOS and Linux architectures.  This test to see
# if the machine is on SunOS or Linux, and if not it defaults to using ECI scripts
switch ( `/bin/uname` )

    case  SunOS:
      breaksw

    case  Linux:
      breaksw

    default:
        if ( -e /eci/share/skel/.cshrc_sys ) then
                source /eci/share/skel/.cshrc_sys
        else
                echo "WARNING: No C-shell configuration for this machine\!"
        endif
        breaksw

endsw

# This stuff requires user interaction, and will break for shells not
# associated with a terminal, like those under CDE or XDM graphical login.
/bin/tty -s
set NO_TTY=$status

if ( ! ${?DT} && ! $NO_TTY ) then

	# Uncomment the next four lines (starting at the "if") to get
	# tcsh on most CS machines.  You'll also want to uncomment the
	# lines at the bottom of this file to re-source your .login

	# if ( ! ${?tcsh} && ! ${?SHLVL} && -x /usr/bin/tcsh ) then
	#	setenv SHELL /usr/bin/tcsh
	#	exec /usr/bin/tcsh $*
	# endif

        # This sets your prompt.
        set prompt=`/bin/uname -n`'% '

	# Put your personal, terminal-associated functions here.
	# End personal term functions.

	# Uncommment this stuff if your want your Xterm to be titled
        # with your login name and the machine you are on.
	#if ( ${?TERM} ) then
	#	if ( $TERM == xterm ) then
	#		alias xtitle "echo -n ']2;'\!:*''"
	#		alias xicon "echo -n ']1;'\!:*''"
	#      		alias rlogin "rlogin \!:* ; xtitle "${USER}@`uname -n`"; xicon "${USER}@`uname -n`""
        #		alias telnet "telnet \!:* ; xtitle "${USER}@`uname -n`"; xicon "${USER}@`uname -n`""
        #		set hostname=`uname -n`
        #		xtitle "${USER}@${hostname}"
        #		xicon "${USER}@${hostname}"
	#	endif
	#endif

	# This will usually make your backspace key work the way you expect.
	# stty erase ^H

endif
# End of protect from CDE/XDM section.

# If a program you run crashes, Unix writes a picture of its memory space
# to your home directory.  This limits the size of that file to 10MB.
limit coredumpsize 10m
 
# This limits the maximum size of any one file to 512MB.
#limit filesize 512m
 
# Keeps a record of the last 1000 commands you have entered.
set savehist=1000
 
# This prevents you from being logged out of terminals by pressing Control-D
set ignoreeof
 
# This prevents Unix from automatically overwriting files that already
# exist.
set noclobber

# This tells how many of your previous commands will be available to you
# when you use the history function.
set history=30
 
# When you use the "cd" command, will check to see if the directory
# you specify is a subdirectory of one of these, and then take you there.
set cdpath=( ./ $HOME )

set path = (/usr/bin /eci/bin /usr/local/bin /usr/pubsw/bin /usr/bin /usr/ccs/bin /bin /usr/ucb /usr/pubsw/X/bin /usr/X11R6/bin /usr/dt/bin /usr/openwin/bin /usr/local/j2sdk1.4.2/bin /eci/j2sdk1.4.0/bin . /fs/iguana/home/research/cse/hong/Maui/Java/bin /fs/iguana/home/research/cse/hong/product/SSA/C++/xerces-c_2_6_0-redhat_80-gcc_32/bin:/fs/iguana/home/research/cse/hong/product/phage/SBMLreader/withoutMaui/tt)

 
# If you type part of a file name ( for a file in the current directory )
# and then hit the ESCAPE key, this will complete the file name for you.
# If more than one match exists, hit Control-D for a list of options.
set filec
 
# Alias commands allow the user to substitute a shorter or more familiar
# string for a complex command.  The following are some useful aliases:

# Makes ls put * after commands, / after directories, and @ after links.
alias ls		ls -F

# Lists files in long format, shows permissions, bytes used, etc.
alias ll		ls -lF

# Lists all files, including hidden (.filename) files.
alias la		ls -alF

# Some more "common" aliases.
alias bye		exit
alias h			history
alias f			finger
alias m			more
alias rm		rm -i
alias so		source

# For the student info database.
alias gold '3270 ccnh.ucsb.edu'

# Note that if an alias has the same name as a command, you can
# temporarily override it by putting a \ in front of the command line.

# Put your personal aliases here.
# End personal aliases.

# Backward compatibilty aliases:  These are meant to help those who
# merge old version of $HOME/.login and $HOME/.cshrc files with the
# new, or who are having problems with the new binaries.

# Uncomment this alias if your scripts give the error "os: not found".
# alias os		uname -r

# Uncomment this alias if you are having trouble with TEX or LaTEX.
# ( It may not solve your problem... )
# alias tex		/usr/compsci/tex/bin/tex

# If you choose to uncomment the tcsh stuff above, you *must* 
# uncomment this stuff too.
#if ( ${?SHLVL} ) then
#        if ( $SHLVL == 1 && ${?tcsh} ) source $HOME/.login
#endif

setenv CONVERTER_HOME /cs/class/stochkit/code/StochKit/tools/SBML2StochKitConverter

setenv  LD_LIBRARY_PATH $CONVERTER_HOME/lib

