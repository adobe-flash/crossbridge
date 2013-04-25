# Must be run from the conform directory as special/compilation-speed.sh
#
# Compare the time for this to the time you get when you load
# compilation-speed.html into a decent web browser.  There will be
# noise that prevents exact comparison, but the difference will be in
# the ballpark of the number we're interested in.

time ( cd ../../build
       ./esc.sh ../test/conform/special/compilation-speed1.es
       ./esc.sh ../test/conform/special/compilation-speed2.es
       ./esc.sh ../test/conform/special/compilation-speed3.es
       ./esc.sh ../test/conform/special/compilation-speed4.es
       ./esc.sh ../test/conform/special/compilation-speed5.es
       ./esc.sh ../test/conform/special/compilation-speed6.es
       ./esc.sh ../test/conform/special/compilation-speed7.es
       ./esc.sh ../test/conform/special/compilation-speed8.es
       ./esc.sh ../test/conform/special/compilation-speed9.es
       ./esc.sh ../test/conform/special/compilation-speed10.es )
