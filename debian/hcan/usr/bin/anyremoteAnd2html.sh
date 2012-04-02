#!/bin/bash
USER_NAME=sh whoami
#echo $USER_NAME

# anyremote @ ido.dyndns.info:
(su -c "/usr/bin/anyremote2html -w 8078 -a 5000 -r 15 | /usr/bin/anyremote -password -f /usr/share/anyremote/cfg-data/Server-mode/hcan.cfg -s socket:5000 -http" $USER_NAME) &
# anyremote2html:
# -i /usr/share/pixmaps/anyremote2html/ (default icons)
# -s 48 (48er icons verwenden)

# anyremote @ localhost:
(su -c "/usr/bin/anyremote2html -w 8079 -a 5001 -r 15 -s 64 | /usr/bin/anyremote -f /usr/share/anyremote/cfg-data/Server-mode/hcan.cfg -s socket:5001 -http" $USER_NAME) &
