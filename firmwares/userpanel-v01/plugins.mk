# Ist manuell angepasst, bis auch die userpanel Firmware direkt
# Plugins verwendet. Bis dahin muessen nur die Plugins dem Linker "angeboten"
# sein, damit er die noetigen Funktionen (u.a. timezone_matches etc.) 
# dazulinken kann

PLUGINS = heizung timeservice zeitzone

PLUGINS_LD = ../plugins/heizung/plugin.a \
			../plugins/timeservice/plugin.a \
			../plugins/zeitzone/plugin.a 

plugins:
	make -C ../plugins/heizung
	make -C ../plugins/timeservice
	make -C ../plugins/zeitzone

plugins-clean:
	make -C ../plugins/heizung clean
	make -C ../plugins/timeservice clean
	make -C ../plugins/zeitzone clean

