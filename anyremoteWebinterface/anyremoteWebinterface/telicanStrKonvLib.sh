#!/bin/sh
#
TC=telican
#telican -a 192.168.1.22

getInput() # from telican
{
  # Echte Hausdaten: 
  echo $1 | $TC
  
  # Simulation mit lampe.txt, sonstige.txt, heizung.txt,  tempsensor.txt, reedkontakt.txt, rolladen.txt:
#  cat $1.txt
}

setOutput() # to telican
{
  #echo "" > outputToTelican.txt

  while read line ; do
    # An die Haussteuerung: 
    echo $line | $TC
  
    
    # Debugging: 
    echo "OutputToTelican: $line"
    #echo $line >> outputToTelican.txt

    sleep 1 # mehr als einen Befehl an telican leicht verzögern
  done
}
######################################################################
sagParameter()
{
	echo "$1" | espeak -v de
}
######################################################################
getListe_heizung() # $4 = aus | auto | therm
{
  getInput heizung | awk '
  BEGIN {ersteNutzzeilenNr=0; Tsoll=0; format="%s%3.1f %s"; Trenner=","; inklAlle="--- Alle,"}
  {
    if(ersteNutzzeilenNr > 0)
    {
      if($4 == "") exit ### FERTIG ###

      if($8 != "|") Tsoll=$8
      else Tsoll=0 # Wieder ruecksetzen ist wichtig!
    
      if(NR == ersteNutzzeilenNr) 	printf format, inklAlle, Tsoll, $(NF) # Trennzeichen Tsoll Bez
      else				printf format, Trenner,  Tsoll, $(NF) # Trennzeichen Tsoll Bez
    }
    else if($0 == "+-----+---------+--------+---------+-------+-----------+----------") ersteNutzzeilenNr=NR+1
  }'
}

getListe_heizung_sortiert() # Bisher nicht verwendet!
{
  getListe_heizung_zeilenumbruch | sort -bif
}

getListe_heizung_zeilenumbruch() # Achtung: ohne "--- Alle\n"
{
  getInput heizung | awk '
  BEGIN {ersteNutzzeilenNr=0; Tsoll=0; format="%3.1f %s%s"; Trenner="\n"; inklAlle=""}
  {
    if(ersteNutzzeilenNr > 0)
    {
      if($4 == "") exit ### FERTIG ###

      if($8 != "|") Tsoll=$8
      else Tsoll=0 # Wieder ruecksetzen ist wichtig!
    
      if(NR == ersteNutzzeilenNr) 	printf "%s", inklAlle
      
      printf format, Tsoll, $(NF), Trenner # Tsoll Bez Trennzeichen
    }
    else if($0 == "+-----+---------+--------+---------+-------+-----------+----------") ersteNutzzeilenNr=NR+1
  }'
}

heizung() # $1=Tsoll|---, $2=Raum-Bezeichung|Alle, $3=ein|aus|auto|plus2Grad|minus2Grad
{
  mode=$3 # in mode sichern, da dieser in getListe_heizung_zeilenumbruch nicht existiert
  
  if [ $2 = "Alle" ]; then
    getListe_heizung_zeilenumbruch | 
    {
      while read aLine ; do
	set -- $aLine # Umsetzung der Zeile in $1, $2...
	heizung_output $1 $2 $mode
      done
    }
  # Einzelraum-Ansteuerung:
  else
    heizung_output $1 $2 $mode
  fi
}

heizung_output() # $1=Tsoll|---, $2=Raum-Bezeichung|Alle, $3=ein|aus|auto|plus2Grad|minus2Grad
{
  if ([ $3 = "aus" ] || [ $3 = "auto" ]); then
    echo "heizung $2 $3"
  
  elif [ $3 = "ein" ]; then
    echo "heizung $2 therm 19.0 180" # 19°C fuer 3 Std.
  
  elif [ $3 = "plus2Grad" ]; then
    echo "heizung $2 therm $(echo "$1 + 2" | bc) 180" # aktuelle Solltemp. + 2°C fuer 3 Std.
  
  elif [ $3 = "minus2Grad" ]; then
    echo "heizung $2 therm $(echo "$1 - 2" | bc) 180" # aktuelle Solltemp. - 2°C fuer 3 Std.
  
  fi | setOutput
}
######################################################################
toggle() # $1=lampe|sonstige, $2=---|ein, $3=Bezeichung
{
  if   [ $2 = "ein" ]; then echo "$1 $3 aus" | setOutput
  elif [ $2 = "---" ]; then echo "$1 $3 ein" | setOutput
  fi
}

getListe() # $1=lampe|sonstige|rolladen|reedkontakt
{
  getInput $1 | awk '
  BEGIN {ersteNutzzeilenNr=0}
  {
    if(ersteNutzzeilenNr > 0)
    {
      if($3 == "") exit ### FERTIG ###

      if($2 == "-") {$2 = "---"}
      if(NR == ersteNutzzeilenNr) 	printf  "%-6s %s", $2, $3  # Erste Nutzzeile
      else				printf ",%-6s %s", $2, $3
    }
    else if($2 == "Name:" || $3 == "Name:" || $4 == "Name:") ersteNutzzeilenNr=NR+1
  }'
}

getEingeschaltete() # $1=lampe|sonstige
{
#  getInput $1 | less # Testausgabe
  getInput $1 | awk '
  # $1=gruppe, $2=Status ein|-, $3=Bezeichung
  BEGIN {ersteNutzzeilenNr=0; activ=0; Einleitungstext="eingeschaltet ist: "}
  {
    if(ersteNutzzeilenNr > 0)
    {
      if($3 == "") exit ### FERTIG ###
      
      if($2 == "-") next # nicht activ ueberspringen
      else activ++

      if(activ == 1) print Einleitungstext
      if(activ > 1) print " und "
      printf "%s", $3
    }
    else if($2 == "Name:" || $3 == "Name:" || $4 == "Name:") ersteNutzzeilenNr=NR+1
  }'
}
######################################################################
getOffene_reedkontakt()
{
  getInput reedkontakt | awk '
  # $1=gruppe, $2=Status ein|-, $3=Bezeichung
  BEGIN {ersteNutzzeilenNr=0; activ=0; Einleitungstext="offen ist: "}
  {
    if(ersteNutzzeilenNr > 0)
    {
      if($3 == "") exit ### FERTIG ###
      
      if($2 == "-") next # nicht activ ueberspringen
      else activ++

      if(activ == 1) print Einleitungstext
      if(activ > 1) print " und "
      printf "%s", $3
    }
    else if($2 == "Name:" || $3 == "Name:" || $4 == "Name:") ersteNutzzeilenNr=NR+1
  }'
}
######################################################################
getListe_tempsensor()
{
  getInput tempsensor | awk '
  # $1=gruppe, $2=Status ein|-, $3=Bezeichung
  BEGIN {ersteNutzzeilenNr=0}
  {
    if(ersteNutzzeilenNr > 0)
    {
      if($3 == "") exit ### FERTIG ###

      if(NR == ersteNutzzeilenNr) 	printf  "%3.1f %s", $2, $3  # Erste Nutzzeile
      else				printf ",%3.1f %s", $2, $3
    }
    else if($2 == "Name:" || $3 == "Name:" || $4 == "Name:") ersteNutzzeilenNr=NR+1
  }'
}
######################################################################
rolladen_posAnfahren()
{
  echo "rolladen $2 $3" | setOutput # z.B. $2=K1-Einzel-Fenster $3=17
}
######################################################################
# echo "$*"
# read stdInput
# echo stdInput
$*

test "$?" != "0" && {
  echo "FEHLER IN $*"
}
