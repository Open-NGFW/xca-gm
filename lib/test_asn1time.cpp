/* vi: set sw=4 ts=4:
 *
 * Copyright (C) 2023 Christian Hohnstaedt.
 *
 * All rights reserved.
 */

#include <QTest>
#include <QLocale>
#include <QTimeZone>
#include <QRegularExpression>

#include "asn1time.h"

#include <stdlib.h>
       #include <time.h>
class test_asn1time: public QObject
{
    Q_OBJECT
private slots:
    void construct_op();
    void output();
};

void test_asn1time::construct_op()
{
	a1time b, a("20191125153015Z");
	QVERIFY(a.isValid());
	QVERIFY(!a.isUndefined());
	QVERIFY(b != a);
	QVERIFY(b > a);
	b = a;
	QCOMPARE(b , a);
	a1time c(a.get());
	QCOMPARE(c , a);
	a1time d(a.get_utc());
	QCOMPARE(d , a.toUTC());
}

// The timezone abbreviation Qt prints for UTC ("GMT" vs "UTC") differs
// between Qt versions, system tzdata and platforms (macOS prints "GMT",
// Linux "UTC"). Normalise it so the test doesn't depend on the environment.
static QString prettyNormalizedZone(const QString &s)
{
	QString r = s;
	return r.replace(QRegularExpression(" (GMT|UTC)$"), " UTC");
}

void test_asn1time::output()
{
	QLocale::setDefault(QLocale::C);
#if !defined(Q_OS_WIN32)
	setenv("TZ","UTC", 1);
	tzset();
#endif

	a1time a("20191125153015Z");

	QCOMPARE(a.toString("yyyy MM"), "2019 11");
	QCOMPARE(a.toSortable(), "2019-11-25");

	QCOMPARE(a.toPlain(),  "20191125153015Z");
	QCOMPARE(a.toPlainUTC(), "191125153015Z");

#if !defined(Q_OS_WIN32)
	a.setTimeZone(QTimeZone("UTC"));
	QCOMPARE(prettyNormalizedZone(a.toPretty()),
		 QString("Monday, 25 November 2019 15:30:15 UTC"));

	a.setTimeZone(QTimeZone("Europe/Berlin"));
	QCOMPARE(prettyNormalizedZone(a.toPretty()),
		 QString("Monday, 25 November 2019 14:30:15 UTC"));

	a.setTimeZone(QTimeZone("UTC+07:00"));
	QCOMPARE(prettyNormalizedZone(a.toPretty()),
		 QString("Monday, 25 November 2019 08:30:15 UTC"));
#endif
}

QTEST_MAIN(test_asn1time)
#include "test_asn1time.moc"
