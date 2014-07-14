#!/usr/bin/tclsh
#
# Run this script using TCLSH to do a speed comparison between
# various versions of SQLite and PostgreSQL and MySQL
#

# Run a test
#
set cnt 1
proc runtest {title} {
  global cnt
  incr cnt
}

# Initialize the environment
#
expr srand(1)
set ones {zero one two three four five six seven eight nine
          ten eleven twelve thirteen fourteen fifteen sixteen seventeen
          eighteen nineteen}
set tens {{} ten twenty thirty forty fifty sixty seventy eighty ninety}
proc number_name {n} {
  if {$n>=1000} {
    set txt "[number_name [expr {$n/1000}]] thousand"
    set n [expr {$n%1000}]
  } else {
    set txt {}
  }
  if {$n>=100} {
    append txt " [lindex $::ones [expr {$n/100}]] hundred"
    set n [expr {$n%100}]
  }
  if {$n>=20} {
    append txt " [lindex $::tens [expr {$n/10}]]"
    set n [expr {$n%10}]
  }
  if {$n>0} {
    append txt " [lindex $::ones $n]"
  }
  set txt [string trim $txt]
  if {$txt==""} {set txt zero}
  return $txt
}



set fd [open test$cnt.sql w]
puts $fd "CREATE TABLE t1(a INTEGER, b INTEGER, c VARCHAR(100));"
for {set i 1} {$i<=100} {incr i} {
  set r [expr {int(rand()*100000)}]
  puts $fd "INSERT INTO t1 VALUES($i,$r,'[number_name $r]');"
}
close $fd
runtest {100 INSERTs}



set fd [open test$cnt.sql w]
puts $fd "BEGIN;"
puts $fd "CREATE TABLE t2(a INTEGER, b INTEGER, c VARCHAR(100));"
for {set i 1} {$i<=2500} {incr i} {
  set r [expr {int(rand()*500000)}]
  puts $fd "INSERT INTO t2 VALUES($i,$r,'[number_name $r]');"
}
puts $fd "COMMIT;"
close $fd
runtest {2500 INSERTs in a transaction}



set fd [open test$cnt.sql w]
for {set i 0} {$i<10} {incr i} {
  set lwr [expr {$i*100}]
  set upr [expr {($i+10)*100}]
  puts $fd "SELECT count(*), avg(b) FROM t2 WHERE b>=$lwr AND b<$upr;"
}
close $fd
runtest {10 SELECTs without an index}



set fd [open test$cnt.sql w]
for {set i 1} {$i<=10} {incr i} {
  puts $fd "SELECT count(*), avg(b) FROM t2 WHERE c LIKE '%[number_name $i]%';"
}
close $fd
runtest {10 SELECTs on a string comparison}



set fd [open test$cnt.sql w]
puts $fd {CREATE INDEX i2a ON t2(a);}
puts $fd {CREATE INDEX i2b ON t2(b);}
close $fd
runtest {Creating an index}



set fd [open test$cnt.sql w]
for {set i 0} {$i<500} {incr i} {
  set lwr [expr {$i*100}]
  set upr [expr {($i+1)*100}]
  puts $fd "SELECT count(*), avg(b) FROM t2 WHERE b>=$lwr AND b<$upr;"
}
close $fd
runtest {500 SELECTs with an index}



set fd [open test$cnt.sql w]
puts $fd "BEGIN;"
for {set i 0} {$i<100} {incr i} {
  set lwr [expr {$i*10}]
  set upr [expr {($i+1)*10}]
  puts $fd "UPDATE t1 SET b=b*2 WHERE a>=$lwr AND a<$upr;"
}
puts $fd "COMMIT;"
close $fd
runtest {100 UPDATEs without an index}



set fd [open test$cnt.sql w]
puts $fd "BEGIN;"
for {set i 1} {$i<=2500} {incr i} {
  set r [expr {int(rand()*500000)}]
  puts $fd "UPDATE t2 SET b=$r WHERE a=$i;"
}
puts $fd "COMMIT;"
close $fd
runtest {2500 UPDATEs with an index}


set fd [open test$cnt.sql w]
puts $fd "BEGIN;"
for {set i 1} {$i<=2500} {incr i} {
  set r [expr {int(rand()*500000)}]
  puts $fd "UPDATE t2 SET c='[number_name $r]' WHERE a=$i;"
}
puts $fd "COMMIT;"
close $fd
runtest {2500 text UPDATEs with an index}



set fd [open test$cnt.sql w]
puts $fd "BEGIN;"
puts $fd "INSERT INTO t1 SELECT * FROM t2;"
puts $fd "INSERT INTO t2 SELECT * FROM t1;"
puts $fd "COMMIT;"
close $fd
runtest {INSERTs from a SELECT}



set fd [open test$cnt.sql w]
puts $fd {DELETE FROM t2 WHERE c LIKE '%fifty%';}
close $fd
runtest {DELETE without an index}



set fd [open test$cnt.sql w]
puts $fd {DELETE FROM t2 WHERE a>10 AND a<20000;}
close $fd
runtest {DELETE with an index}



set fd [open test$cnt.sql w]
puts $fd {INSERT INTO t2 SELECT * FROM t1;}
close $fd
runtest {A big INSERT after a big DELETE}



set fd [open test$cnt.sql w]
puts $fd {BEGIN;}
puts $fd {DELETE FROM t1;}
for {set i 1} {$i<=300} {incr i} {
  set r [expr {int(rand()*100000)}]
  puts $fd "INSERT INTO t1 VALUES($i,$r,'[number_name $r]');"
}
puts $fd {COMMIT;}
close $fd
runtest {A big DELETE followed by many small INSERTs}

set fd [open test$cnt.sql w]
puts $fd {DROP TABLE t1;}
puts $fd {DROP TABLE t2;}
close $fd
runtest {DROP TABLE}
