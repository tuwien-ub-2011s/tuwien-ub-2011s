/**
 * @file test.c
 *
 * Testframework für das Übersetzerbau-Assembler-Beispiel B im 2011S.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

/**
 * Deklaration der in Assembler programmierten Funktion.
 *
 * @param x erstes Summandenarray (Länge n)
 * @param y zweites Summandenarray (Länge n)
 * @param sum Summenarray (Zielarray; Länge n+1)
 * @param n Länge der Arrays
 */
void asmb(unsigned long x[], unsigned long y[], unsigned long sum[], size_t n);

/**
 * Naive C-Implementierung desselben Algorithmus. Schamlos von der Angabe gemopst.
 *
 * @param x erstes Summandenarray (Länge n)
 * @param y zweites Summandenarray (Länge n)
 * @param sum Summenarray (Zielarray; Länge n+1)
 * @param n Länge der Arrays
 */
void naive(unsigned long x[], unsigned long y[], unsigned long sum[], size_t n)
{
	unsigned long carry, s;
	size_t i;
	carry = 0;

	for (i = 0; i < n; i++)
	{
		s = x[i] + y[i] + carry;
		carry = carry ? s <= x[i] : s < x[i];
		sum[i] = s;
	}
	sum[i] = carry;
}

/**
 * Gibt ein Array aus unsigned longs aus.
 * @param f stdio-Dateiobjekt, wohin die Ausgabe getätigt werden soll.
 * @param lbl Beschreibung des Arrays, die dem Array vorangestellt wird.
 * @param arr Das Array selbst.
 * @param arrlen Die Länge des Arrays (arr).
 */
static void print_ulong_array(FILE *f, const char *lbl, unsigned long *arr, size_t arrlen)
{
	size_t i;

	fprintf(f, "%s: { ", lbl);
	for (i = 0; i < arrlen; ++i)
	{
		fprintf(f, "%016lx", arr[i]);

		if (i < arrlen-1)
		{
			fprintf(f, ", ");
		}
	}
	fprintf(f, " }\n");
}

/**
 * Ruft asmb auf und prüft dabei, dass callee-preserved-Register nicht
 * modifiziert wurden.

 * @param x erstes Summandenarray (Länge n)
 * @param y zweites Summandenarray (Länge n)
 * @param sum Summenarray (Zielarray; Länge n+1)
 * @param n Länge der Arrays
 */
static void regcheck_asmb(unsigned long x[], unsigned long y[], unsigned long sum[], size_t n)
{
	unsigned long oldrbp, oldrbx, oldr12, oldr13, oldr14, oldr15;
	unsigned long newrbp, newrbx, newr12, newr13, newr14, newr15;

	__asm__(
		"movq %%rbp,%0\n"
		"movq %%rbx,%1\n"
		"movq %%r12,%2\n"
		"movq %%r13,%3\n"
		"movq %%r14,%4\n"
		"movq %%r15,%5\n"
		:
		"=m"(oldrbp), "=m"(oldrbx), "=m"(oldr12),
		"=m"(oldr13), "=m"(oldr14), "=m"(oldr15)
	);

	asmb(x, y, sum, n);

	__asm__(
		"movq %%rbp,%0\n"
		"movq %%rbx,%1\n"
		"movq %%r12,%2\n"
		"movq %%r13,%3\n"
		"movq %%r14,%4\n"
		"movq %%r15,%5\n"
		:
		"=m"(newrbp), "=m"(newrbx), "=m"(newr12),
		"=m"(newr13), "=m"(newr14), "=m"(newr15)
	);

#define CHECK_REGISTER(reg) do { \
	if (old##reg != new##reg) \
	{ \
		fprintf(stderr, "FAIL! asma modifizierte " #reg " (von 0x%016lx zu 0x%016lx)!\n", old##reg, new##reg); \
		exit(1); \
	} } while(0)

	CHECK_REGISTER(rbp);
	CHECK_REGISTER(rbx);
	CHECK_REGISTER(r12);
	CHECK_REGISTER(r13);
	CHECK_REGISTER(r14);
	CHECK_REGISTER(r15);
}

/**
 * Der vorgeschriebene Einsprungspunkt ins Programm.
 *
 * @return Exit-Code (0 falls OK, 1 bei Fehler).
 */
int main(void)
{
	unsigned int i;
	FILE *f;

	/* hol ein paar Zufallszahlen, yay! */
	f = fopen("/dev/urandom", "rb");
	if (f == NULL)
	{
		perror("fopen");
		return 1;
	}

	/* 20000 Testfälle sollten genug für jeden sein */
	for (i = 0; i < 20000; ++i)
	{
		size_t lng;
		unsigned long *x, *y, *xasm, *xnaive, *yasm, *ynaive, *sumasm, *sumnaive;

		/* wie lang sollen die Arrays sein? */
		if (fread(&lng, sizeof(size_t), 1, f) != 1)
		{
			perror("fread");
			fclose(f);
			return 1;
		}

		/* begrenze auf 0 bis 19 */
		lng %= 20;

/** Kürzel: alloziere ein Array mit lng unsigned longs */
#define LNGMALLOC() malloc(sizeof(unsigned long) * lng)

/** Kürzel: alloziere ein Array mit lng+1 unsigned longs */
#define LNGP1MALLOC() malloc(sizeof(unsigned long) * (lng+1))

/** Kürzel: Größe eines Arrays mit lng unsigned longs */
#define LNGULONGS (sizeof(unsigned long) * lng)

		/* alloziere Speicher */
		x = LNGMALLOC();
		y = LNGMALLOC();
		xasm = LNGMALLOC();
		xnaive = LNGMALLOC();
		yasm = LNGMALLOC();
		ynaive = LNGMALLOC();

		sumasm = LNGP1MALLOC();
		sumnaive = LNGP1MALLOC();

		/* wurde der Speicher erfolgreich alloziert? */
		if (
			x == NULL || y == NULL || xasm == NULL || xnaive == NULL ||
			yasm == NULL || ynaive == NULL || sumasm == NULL || sumnaive == NULL
		)
		{
			fputs("FAIL: Speicherallozierungsfehler.\n", stderr);
			return 1;
		}

		/* fülle das x- und y-Array mit zufälligen Daten */
		if (
			fread(x, sizeof(unsigned long), lng, f) != lng ||
			fread(y, sizeof(unsigned long), lng, f) != lng
		)
		{
			perror("fread");
			fclose(f);
			return 1;
		}

		/* kopiere zu Vergleichszwecken diese zufälligen Daten */
		memcpy(xasm, x, LNGULONGS);
		memcpy(xnaive, x, LNGULONGS);
		memcpy(yasm, y, LNGULONGS);
		memcpy(ynaive, y, LNGULONGS);

		/* rufe Assembler- sowie naive C-Implementierung nacheinander auf */
		regcheck_asmb(xasm, yasm, sumasm, lng);
		naive(xnaive, ynaive, sumnaive, lng);

		/* haben die Implementierungen die Eingangsarrays modifiziert? das soll nicht passieren */
		if (memcmp(x, xasm, LNGULONGS) != 0)
		{
			fputs("FAIL! asmb modifizierte x!\n", stderr);
			print_ulong_array(stderr, "urspruengliches x", x, lng);
			print_ulong_array(stderr, "x nach asmb      ", xasm, lng);
			return 1;
		}
		if (memcmp(y, yasm, LNGULONGS) != 0)
		{
			fputs("FAIL! asmb modifizierte y!\n", stderr);
			print_ulong_array(stderr, "urspruengliches y", y, lng);
			print_ulong_array(stderr, "y nach asmb      ", yasm, lng);
			return 1;
		}
		if (memcmp(x, xnaive, LNGULONGS) != 0)
		{
			fputs("FAIL! naive modifizierte x!\n", stderr);
			print_ulong_array(stderr, "urspruengliches x", x, lng);
			print_ulong_array(stderr, "x nach naive     ", xnaive, lng);
			return 1;
		}
		if (memcmp(y, ynaive, LNGULONGS) != 0)
		{
			fputs("FAIL! naive modifizierte y!\n", stderr);
			print_ulong_array(stderr, "urspruengliches y", y, lng);
			print_ulong_array(stderr, "y nach naive     ", ynaive, lng);
			return 1;
		}

		/* vergleiche die Summen */
		if (memcmp(sumasm, sumnaive, LNGULONGS+1) != 0)
		{
			fputs("FAIL! Summen sind ungleich!\n", stderr);
			print_ulong_array(stderr, "Summe laut asmb ", sumasm, lng+1);
			print_ulong_array(stderr, "Summe laut naive", sumnaive, lng+1);
			return 1;
		}

		/* gib Speicher zurück */
		free(x);
		free(y);
		free(xasm);
		free(xnaive);
		free(yasm);
		free(ynaive);
		free(sumasm);
		free(sumnaive);
	}

	fputs("PASS!\n", stderr);

	fclose(f);
	return 0;
}
