# Testfallsammlung für Übersetzerbau 2011S

TU Wien, Ertl/Krall

## Tutorial für die g0

Herunterladen:

    $ git clone git://github.com/tuwien-ub-2011s/tuwien-ub-2011s ~/test

Aktualisieren:

    $ cd ~/test && git pull

Die Testskripts befinden sich unter

    /usr/ftp/pub/ubvl/test/beispiel/test

wo _beispiel_ durch den entsprechenden Beispielnamen (scanner, parser, ag, codea, codeb, gesamt) zu ersetzen ist.

## Namenskonventionen

Die Erweiterungen der Eingabedateien sollen jeweils eine Zahl sein, die den Exit-Code darstellt. `ravu_correct1.0` ist also korrekt, `ravu_badtoken3.1` enthält ungültige Tokens, `ravu_synerror8.2` einen Syntaxfehler, und `ravu_unknownvar.3` einen semantischen Fehler (beispielsweise das Auslesen einer unbekannten Variable). Natürlich kommen bei manchen Beispielen nicht alle Exit-Codes zur Geltung; das Scanner-Beispiel etwa sieht nur `0` und `1` vor.

Beim Scanner-Beispiel ist die Ausgabe noch mit einer Referenzausgabe zu vergleichen; diese soll wie die Eingabedatei heißen, nur statt `.0` die Endung `.out` besitzen (`ravu_correct1.out`).

Bei den Codegen-Beispielen (`codea`, `codeb` und `gesamt`) muss für alle `.0`-Dateien auch eine entsprechende Aufrufdatei, die die Rückgabewerte überprüft, erstellt werden. Diese hat die Endung `.call` (`ravu_sum.call`). Falls die Laufzeit-Typüberprüfung bei einem Beispiel fehlschlagen soll, soll die Codedatei statt `.0` die Endung `.xin` haben (`ravu_intpluslist.xin`); eine `.call`-Datei ist weiterhin notwendig!

Damit es zu keinen Konflikten zwischen gleich benannten Dateien kommt, soll vor jede Bezeichnung im Dateinamen noch der eigene Name oder Nickname und ein Underscore hinzugefügt werden, wie auch oben in den Beispielen gezeigt.

## beitragen

### mit git und Github-Account

1. Schreib mir eine E-Mail (siehe weiter unten) und ich füge dich der *Organisation* hinzu.

2. Klone die Repository:

        $ git clone git@github.com:tuwien-ub-2011s/tuwien-ub-2011s.git

3. *commit*te und *push*e wie üblich.

        $ git add scanner/ravu_*.{0,out}
        $ git commit -m "ein paar Scanner-Testfaelle"
        $ git push

Alternativ kannst du diese Repository auch forken und Pull-Requests aufmachen.

### mit git aber ohne Github-Account

1. Klone die Repository:

        $ git clone git://github.com/tuwien-ub-2011s/tuwien-ub-2011s.git

2. *commit*e wie üblich.

        $ git add scanner/ravu_*.{0,out}
        $ git commit -m "ein paar Scanner-Testfaelle"

3. Erstelle einen Patch.

        $ git format-patch

4. Schicke ihn mir per Mail (siehe weiter unten).

### ohne git

Testcases, die mir per E-Mail (siehe weiter unten) übermittelt werden, werden ebenfalls hochgeladen. Falls du mir mehrere schickst, wäre ich sehr dankbar, wenn du sie in eine `zip`- oder `tar.gz`-Datei verpacken würdest.

## Kontakt

### E-Mail-Adressen

    ondrej PUNKT hosek KLAMMERAFFE student PUNKT tuwien PUNKT ac PUNKT at

## Danksagungen

Vielen Dank an lewurm, der diese Idee hatte und schon letztes Semester erfolgreich umsetzte.

Danke auch an alle, die Testcases beigesteuert haben. :-)
