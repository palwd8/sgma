# -*- MakeFile -*-

install: sgma.c schedule.txt
	gcc -o sgma sgma.c
	cp -f sgma /usr/local/bin/
	chmod 755 /usr/local/bin/sgma
	mkdir -p /usr/share/sgma
	cp -f schedule.txt /usr/share/sgma/schedule.txt

uninstall:
	rm -rf /usr/local/bin/sgma /usr/share/sgma/schedule.txt

update:
	cp -f schedule.txt /usr/share/sgma/schedule.txt
