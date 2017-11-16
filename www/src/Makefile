CFLAGS=-g -Wall
CC=arm-linux-gcc
AR=arm-linux-ar
RANLIB=arm-linux-ranlib
LIBS=-L./ -lcgic -lsqlite3 -L/opt/sqlite3/sqlite3-3.14.1/install-arm/lib


all: libcgic.a siteconfig.cgi server.cgi pols.cgi devices.cgi ctlparam.cgi polsalarm.cgi alarm.cgi dailydata.cgi fault.cgi hourlydata.cgi minutedata.cgi rtdata.cgi dailyruntime.cgi runstatus.cgi sbparam.cgi sitesbs.cgi sitepols.cgi update_server.cgi update_pols.cgi update_devices.cgi update_sitepols.cgi update_sitesbs.cgi update_sbparam.cgi update_ctlparam.cgi update_polsalarm.cgi

install: libcgic.a
	cp libcgic.a /usr/local/lib
	cp cgic.h /usr/local/include
	@echo libcgic.a is in /usr/local/lib. cgic.h is in /usr/local/include.

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

siteconfig.cgi: siteconfig.o libcgic.a prefile.h
	$(CC) siteconfig.o -o siteconfig.cgi ${LIBS}

server.cgi: server.o libcgic.a prefile.h
	$(CC) server.o -o server.cgi ${LIBS}

pols.cgi:pols.o libcgic.a prefile.h
	$(CC) pols.o -o pols.cgi ${LIBS}

devices.cgi:devices.o libcgic.a prefile.h
	$(CC) devices.o -o devices.cgi ${LIBS}

alarm.cgi:alarm.o libcgic.a prefile.h
	$(CC) alarm.o -o alarm.cgi ${LIBS}

dailydata.cgi:dailydata.o libcgic.a prefile.h
	$(CC) dailydata.o -o dailydata.cgi ${LIBS}

fault.cgi:fault.o libcgic.a prefile.h
	$(CC) fault.o -o fault.cgi ${LIBS}

hourlydata.cgi:hourlydata.o libcgic.a prefile.h
	$(CC) hourlydata.o -o hourlydata.cgi ${LIBS}

minutedata.cgi:minutedata.o libcgic.a prefile.h
	$(CC) minutedata.o -o minutedata.cgi ${LIBS}

rtdata.cgi:rtdata.o libcgic.a prefile.h
	$(CC) rtdata.o -o rtdata.cgi ${LIBS}

dailyruntime.cgi:dailyruntime.o libcgic.a prefile.h
	$(CC) dailyruntime.o -o dailyruntime.cgi ${LIBS}

runstatus.cgi:runstatus.o libcgic.a prefile.h
	$(CC) runstatus.o -o runstatus.cgi ${LIBS}

sbparam.cgi:sbparam.o libcgic.a prefile.h
	$(CC) sbparam.o -o sbparam.cgi ${LIBS}

sitesbs.cgi:sitesbs.o libcgic.a prefile.h
	$(CC) sitesbs.o -o sitesbs.cgi ${LIBS}

sitepols.cgi:sitepols.o libcgic.a prefile.h
	$(CC) sitepols.o -o sitepols.cgi ${LIBS}

ctlparam.cgi:ctlparam.o libcgic.a prefile.h
	$(CC) ctlparam.o -o ctlparam.cgi ${LIBS}

polsalarm.cgi:polsalarm.o libcgic.a prefile.h
	$(CC) polsalarm.o -o polsalarm.cgi ${LIBS}

update_server.cgi:update_server.o libcgic.a prefile.h
	$(CC) update_server.o -o update_server.cgi ${LIBS}

update_pols.cgi:update_pols.o libcgic.a prefile.h
	$(CC) update_pols.o -o update_pols.cgi ${LIBS}

update_devices.cgi:update_devices.o libcgic.a prefile.h
	$(CC) update_devices.o -o update_devices.cgi ${LIBS}

update_sitepols.cgi:update_sitepols.o libcgic.a prefile.h
	$(CC) update_sitepols.o -o update_sitepols.cgi ${LIBS}

update_sitesbs.cgi:update_sitesbs.o libcgic.a prefile.h
	$(CC) update_sitesbs.o -o update_sitesbs.cgi ${LIBS}

update_sbparam.cgi:update_sbparam.o libcgic.a prefile.h
	$(CC) update_sbparam.o -o update_sbparam.cgi ${LIBS}

update_ctlparam.cgi:update_ctlparam.o libcgic.a prefile.h
	$(CC) update_ctlparam.o -o update_ctlparam.cgi ${LIBS}

update_polsalarm.cgi:update_polsalarm.o libcgic.a prefile.h
	$(CC) update_polsalarm.o -o update_polsalarm.cgi ${LIBS}
clean:
	rm -f *.o *.a *.cgi *~

