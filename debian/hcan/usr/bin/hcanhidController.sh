#!/bin/sh

TC=telican

validate_hcanhid()
{
  echo "lampe" | $TC | awk '
  BEGIN {ersteNutzzeilenNr=0}
  {
    if(ersteNutzzeilenNr > 0)
    {
      if($1 != "") # Gruppennummer
	print "hcanhid_ok"
      else
	print "hcanhid_ng"
      
      exit
    }
    else if($2 == "Name:" || $3 == "Name:" || $4 == "Name:") ersteNutzzeilenNr=NR+1
  }'
}


start_hcanhid()
{
  parallelPortOut 0
  switchUSB 0
  sleep 2
  parallelPortOut 1
  switchUSB 1
  sleep 4
  hcanhid &
  sleep 20
}

getPIDofTelican()
{
  aLine=$(pidof $TC)
  set -- $aLine # Zeile in $1, $2, ... wandeln
  echo "$1" # nur das erstes Element interessiert
}

# test()
# {
#   PID_TC=$(getPIDofTelican)
#   echo "PID=$PID_TC"
# }

start_hcanhid
$TC --timed &
PID_TC=$(getPIDofTelican)

while true
do
  T=`validate_hcanhid`
  if [ "hcanhid_ok" != "$T" ]
  then
    kill $PID_TC
	echo "vorStartHcanhid"    
	start_hcanhid
    $TC --timed &
    PID_TC=$(getPIDofTelican)
  fi
  sleep 300
done

# $* # falls wir eine Methode mal direkt aufrufen wollen (z. B. zu Testzwecken: "sh ./hcanhidController.sh test")

