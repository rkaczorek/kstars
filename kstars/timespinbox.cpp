/***************************************************************************
                          timespinbox.cpp  -  description
                             -------------------
    begin                : Sun Mar 31 2002
    copyright            : (C) 2002 by Jason Harris
    email                : kstars@30doradus.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//Time steps:
//0-9:   0 sec, 0.1, 0.25, 0.5, 1, 2, 5, 10, 20, 30 sec
//10-14: 1 minute, 2, 5, 10, 15, 30 min
//15-19: 1 hour, 2, 4, 6, 12 hours
//20-23: 1 day, 2, 3, 5 days
//24-26: 1 week, 2, 3 weeks
//27-32: 1 month, 2, 3, 4, 6, 9 months
//33-41: 1 year, 2, 3, 4, 5, 10, 25, 50, 100 years

#include <qlineedit.h>
#include <kdebug.h>
#include <klocale.h>
#include <stdlib.h>
#include <math.h>

#include "timespinbox.h"

TimeSpinBox::TimeSpinBox( QWidget *parent, const char *name )
	: QSpinBox ( -41, 41, 1 /* step */, parent, name )
{
	setMinimumWidth( 80 );
	setMaximumWidth( 80 );
	setValidator( 0 );
	setButtonSymbols( QSpinBox::PlusMinus );
	editor()->setReadOnly( true );
	setValue( 4 ); //1 second (real time)

	TimeScale[0] = 0.0;
	TimeScale[1] = 0.1;
	TimeScale[2] = 0.25;
	TimeScale[3] = 0.5;
	TimeScale[4] = 1.0;
	TimeScale[5] = 2.0;
	TimeScale[6] = 5.0;
	TimeScale[7] = 10.0;
	TimeScale[8] = 20.0;
	TimeScale[9] = 30.0;
	TimeScale[10] = 60.0;
	TimeScale[11] = 120.0;
	TimeScale[12] = 300.0;
	TimeScale[13] = 600.0;
	TimeScale[14] = 900.0;
	TimeScale[15] = 1800.0;
	TimeScale[16] = 3600.0;
	TimeScale[17] = 7200.0;
	TimeScale[18] = 10800.0;
	TimeScale[19] = 21600.0;
	TimeScale[20] = 43200.0;
	TimeScale[21] = 86400.0;
	TimeScale[22] = 172800.0;
	TimeScale[23] = 259200.0;
	TimeScale[24] = 432000.0;
	TimeScale[25] = 604800.0;
	TimeScale[26] = 1209600.0;
	TimeScale[27] = 1814400.0;
//Months aren't a simple measurement of time; I'll just use fractions of a year
	TimeScale[28] = 31556909.0/12.0;
	TimeScale[29] = 31556909.0/6.0;
	TimeScale[30] = 0.25*31556909.0;
	TimeScale[31] = 31556909.0/3.0;
	TimeScale[32] = 0.5*31556909.0;
	TimeScale[33] = 0.75*31556909.0;
	TimeScale[34] = 31556909.0;
	TimeScale[35] = 2.0*31556909.0;
	TimeScale[36] = 3.0*31556909.0;
	TimeScale[37] = 5.0*31556909.0;
	TimeScale[38] = 10.0*31556909.0;
	TimeScale[39] = 25.0*31556909.0;
	TimeScale[40] = 50.0*31556909.0;
	TimeScale[41] = 100.0*31556909.0;

	TimeString.append( "0 " + i18n( "seconds", "secs" ));
	TimeString.append( "0.1 " + i18n( "seconds", "secs" ));
	TimeString.append( "0.25 " + i18n( "seconds", "secs" ));
	TimeString.append( "0.5 " + i18n( "seconds", "secs" ));
	TimeString.append( "1 " + i18n( "second", "sec" ));
	TimeString.append( "2 " + i18n( "seconds", "secs" ));
	TimeString.append( "5 " + i18n( "seconds", "secs" ));
	TimeString.append( "10 " + i18n( "seconds", "secs" ));
	TimeString.append( "20 " + i18n( "seconds", "secs" ));
	TimeString.append( "30 " + i18n( "seconds", "secs" ));
	TimeString.append( "1 " + i18n( "minute", "min" ));
	TimeString.append( "2 " + i18n( "minutes", "mins" ));
	TimeString.append( "5 " + i18n( "minutes", "mins" ));
	TimeString.append( "10 " + i18n( "minutes", "mins" ));
	TimeString.append( "15 " + i18n( "minutes", "mins" ));
	TimeString.append( "30 " + i18n( "minutes", "mins" ));
	TimeString.append( "1 " + i18n( "hour" ));
	TimeString.append( "2 " + i18n( "hours", "hrs" ));
	TimeString.append( "3 " + i18n( "hours", "hrs" ));
	TimeString.append( "6 " + i18n( "hours", "hrs" ));
	TimeString.append( "12 " + i18n( "hours", "hrs" ));
	TimeString.append( "1 " + i18n( "day" ));
	TimeString.append( "2 " + i18n( "days" ));
	TimeString.append( "3 " + i18n( "days" ));
	TimeString.append( "5 " + i18n( "days" ));
	TimeString.append( "1 " + i18n( "week" ));
	TimeString.append( "2 " + i18n( "weeks", "wks" ));
	TimeString.append( "3 " + i18n( "weeks", "wks" ));
	TimeString.append( "1 " + i18n( "month" ));
	TimeString.append( "2 " + i18n( "months", "mths" ));
	TimeString.append( "3 " + i18n( "months", "mths" ));
	TimeString.append( "4 " + i18n( "months", "mths" ));
	TimeString.append( "6 " + i18n( "months", "mths" ));
	TimeString.append( "9 " + i18n( "months", "mths" ));
	TimeString.append( "1 " + i18n( "year" ));
	TimeString.append( "2 " + i18n( "years", "yrs" ));
	TimeString.append( "3 " + i18n( "years", "yrs" ));
	TimeString.append( "5 " + i18n( "years", "yrs" ));
	TimeString.append( "10 " + i18n( "years", "yrs" ));
	TimeString.append( "25 " + i18n( "years", "yrs" ));
	TimeString.append( "50 " + i18n( "years", "yrs" ));
	TimeString.append( "100 " + i18n( "years", "yrs" ));

	connect( this, SIGNAL( valueChanged( int ) ), this, SLOT( reportChange() ) );
	updateDisplay();
}

int TimeSpinBox::mapTextToValue( bool *ok ) {
	*ok = true;
	for ( unsigned int i=0; i<42; ++i ) {
		if ( text() == TimeString[i] ) { return i; }
		if ( text().mid(1,text().length()) == TimeString[i] ) { return -1*i; }
	}

	*ok = false;
	return 0;
}

QString TimeSpinBox::mapValueToText( int value ) {
	QString neg("-"), result;
	int posval( abs( value ) );

	result = TimeString[ posval ];

	if ( value<0 ) { result = "-" + result; }
	return result;
}

void TimeSpinBox::changeScale( float x ) {
	//Pick the closest value
	int imin;
	float dx, dxlast, dxmin(10000000000.0);
	for ( unsigned int i=0; i<42; ++i ) {
		dx = fabs( TimeScale[i] - fabs(x) );
		if ( dx < dxmin ) { imin = i; dxmin = dx; }
		if ( dx > dxlast ) break; //we have passed the minimum dx
		dxlast = dx;
	}

	if ( x < 0.0 ) imin *= -1;
	setValue( imin );
}

float TimeSpinBox::timeScale( void ) {
	return value() > 0 ? TimeScale[ value() ] : -1.*TimeScale[ abs(value()) ];
}

void TimeSpinBox::reportChange() {
	kdDebug() << i18n( "Reporting new timestep value: " ) << timeScale() << endl;
	emit scaleChanged( timeScale() );
}

#include "timespinbox.moc"
