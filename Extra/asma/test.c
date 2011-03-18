/**
 * @file test.c
 *
 * Testframework für das Übersetzerbau-Assembler-Beispiel A im 2011S.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Kürzel für "die Größe zweier unsigned longs". */
#define TWOULONGS (sizeof(unsigned long)*2)

/**
 * Deklaration der in Assembler programmierten Funktion.
 * @param x erstes Summandenarray (Länge 2)
 * @param y zweites Summandenarray (Länge 2)
 * @param sum Summenarray (Zielarray; Länge 2)
 */
void asma(unsigned long x[], unsigned long y[], unsigned long sum[]);

/**
 * Naive C-Implementierung desselben Algorithmus. Schamlos von der Angabe gemopst.
 * @param x erstes Summandenarray (Länge 2)
 * @param y zweites Summandenarray (Länge 2)
 * @param sum Summenarray (Zielarray; Länge 2)
 */
void naive(unsigned long x[], unsigned long y[], unsigned long sum[])
{
	unsigned long carry, sum0;

	sum0 = x[0] + y[0];
	carry = sum0 < x[0];
	sum[0] = sum0;
	sum[1] = x[1] + y[1] + carry;
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
 * Ruft asma auf und prüft dabei, dass callee-preserved-Register nicht
 * modifiziert wurden.
 * @param x erstes Summandenarray (Länge 2)
 * @param y zweites Summandenarray (Länge 2)
 * @param sum Summenarray (Zielarray; Länge 2)
 */
static void regcheck_asma(unsigned long x[], unsigned long y[], unsigned long sum[])
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

	asma(x, y, sum);

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
	unsigned long x[2], xasm[2], xnaive[2];
	unsigned long y[2], yasm[2], ynaive[2];
	unsigned long sumasm[2], sumnaive[2];
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
		/* fülle das x- und y-Array mit zufälligen Daten */
		if (
			fread(x, sizeof(unsigned long), 2, f) != 2 ||
			fread(y, sizeof(unsigned long), 2, f) != 2
		)
		{
			perror("fread");
			fclose(f);
			return 1;
		}

		/* kopiere zu Vergleichszwecken diese zufälligen Daten */
		memcpy(xasm, x, TWOULONGS);
		memcpy(xnaive, x, TWOULONGS);
		memcpy(yasm, y, TWOULONGS);
		memcpy(ynaive, y, TWOULONGS);

		/* rufe Assembler- sowie naive C-Implementierung nacheinander auf */
		regcheck_asma(xasm, yasm, sumasm);
		naive(xnaive, ynaive, sumnaive);

		/* haben die Implementierungen die Eingangsarrays modifiziert? das soll nicht passieren */
		if (memcmp(x, xasm, TWOULONGS) != 0)
		{
			fputs("FAIL! asma modifizierte x!\n", stderr);
			print_ulong_array(stderr, "urspruengliches x", x, 2);
			print_ulong_array(stderr, "x nach asma      ", xasm, 2);
			return 1;
		}
		if (memcmp(y, yasm, TWOULONGS) != 0)
		{
			fputs("FAIL! asma modifizierte y!\n", stderr);
			print_ulong_array(stderr, "urspruengliches y", y, 2);
			print_ulong_array(stderr, "y nach asma      ", yasm, 2);
			return 1;
		}
		if (memcmp(x, xnaive, TWOULONGS) != 0)
		{
			fputs("FAIL! naive modifizierte x!\n", stderr);
			print_ulong_array(stderr, "urspruengliches x", x, 2);
			print_ulong_array(stderr, "x nach naive     ", xnaive, 2);
			return 1;
		}
		if (memcmp(y, ynaive, TWOULONGS) != 0)
		{
			fputs("FAIL! naive modifizierte y!\n", stderr);
			print_ulong_array(stderr, "urspruengliches y", y, 2);
			print_ulong_array(stderr, "y nach naive     ", ynaive, 2);
			return 1;
		}

		if (memcmp(sumasm, sumnaive, TWOULONGS) != 0)
		{
			fputs("FAIL! Summen sind ungleich!\n", stderr);
			print_ulong_array(stderr, "Summe laut asma ", sumasm, 2);
			print_ulong_array(stderr, "Summe laut naive", sumnaive, 2);
			return 1;
		}
	}

	fputs("PASS!\n", stderr);

	fclose(f);
	return 0;
}
