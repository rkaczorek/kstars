/***************************************************************************
                          viewopsdialog.cpp  -  K Desktop Planetarium
                             -------------------
    begin                : Sat Apr 28 2001
    copyright            : (C) 2001 by Jason Harris
    email                : jharris@30doradus.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <qtextstream.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qbutton.h>
#include <qcolor.h>
#include <qfont.h>
#include <qcolordialog.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qpushbutton.h>
//#include <qinputdialog.h>

#include <klocale.h>
#include <knuminput.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "kstars.h"
#include "ksutils.h"
#include "magnitudespinbox.h"
#include "timestepbox.h"
#include "addcatdialog.h"
#include "viewopsdialog.h"

#include "viewopsdialog.moc"

ViewOpsDialog::ViewOpsDialog( QWidget *parent )
	: KDialogBase( KDialogBase::Plain, i18n( "Display Options" ), Ok|Cancel, Ok, parent ) {
	QFrame *page = plainPage();
	ksw = (KStars *)parent;

	// Layout manager for entire dialog
	hlay = new QHBoxLayout( page, 2, 2 );
	hlay->setSpacing( 2 );
	hlay->setMargin( 4 );

	DisplayBox = new QGroupBox( page, "DisplayBox" );
	DisplayBox->setMinimumWidth( 230 );
	DisplayBox->setTitle( i18n( "Display Options" ) );

	DisplayBoxLayout = new QVBoxLayout( DisplayBox );
	DisplayBoxLayout->setSpacing( 6 );
	DisplayBoxLayout->setMargin( 11 );

	DisplayTabs = new QTabWidget( DisplayBox, "DisplayTabs" );
	DisplayTabs->move( 10, 24 );
	DisplayTabs->setMinimumSize( 220, 300 );

	CoordsGroup = new QButtonGroup( 1, Qt::Vertical, i18n( "Coordinate system" ), DisplayBox );

	EquatRadio = new QRadioButton( i18n( "The Equatorial (RA/Dec) coordinate system", "Equatorial" ), CoordsGroup );
	EquatRadio->setChecked( !ksw->options()->useAltAz );

	AltAzRadio = new QRadioButton( i18n( "The Horizontal (Alt/Az) coordinate system", "Horizontal" ), CoordsGroup );
	AltAzRadio->setChecked( ksw->options()->useAltAz );

	DisplayBoxLayout->addSpacing( 10 );
	DisplayBoxLayout->addWidget( DisplayTabs );
	DisplayBoxLayout->addWidget( CoordsGroup );


//*** Construct Catalogs tab ***//
  //This Tab now includes Stars (18 Feb 2002; JH)
	CatalogTab = new QWidget( DisplayTabs, "CatalogTab" );
	vlayCatTab = new QVBoxLayout( CatalogTab );
	vlayCatTab->setSpacing( 6 );
	vlayCatTab->setMargin( 11 );

//Place stars-related widgets in a group box
	StarsBox = new QGroupBox( i18n( "Stars" ), CatalogTab, "StarsBox" );
  vlayStarsBox = new QVBoxLayout( StarsBox );
	vlayStarsBox->setSpacing( 6 );
	vlayStarsBox->setMargin( 11 );

	showSAO = new QCheckBox( i18n( "Smithsonian Astrophysical Observatory (SAO) star catalog", "SAO Star Catalog" ), StarsBox );
	QFont stdFont(  showSAO->font() );
	showSAO->setFont( stdFont );
	showSAO->setChecked( ksw->options()->drawSAO );

	// Spin box : show stars brighter than magnitude limit
	int intMagLimitDrawStars = int ( 10.0 * ksw->options()->magLimitDrawStar );
	magSpinBoxDrawStars = new MagnitudeSpinBox( 0, 79, StarsBox );	// max mag = 7.9
	magSpinBoxDrawStars->setFont( stdFont );
	magSpinBoxDrawStars->setValue( intMagLimitDrawStars );

	textLabelMagStars = new QLabel( StarsBox, "LabelMagStars" );
	textLabelMagStars->setText( i18n( "Show stars brighter than" ) );
	textLabelMagStars->setFont( stdFont );

	// Spin box : show stars names for stars brighter than magnitude limit
	int intMagLimitDrawStarInfo = 10*((int)(ksw->options()->magLimitDrawStarInfo));
	magSpinBoxDrawStarInfo = new MagnitudeSpinBox( 0, 90, StarsBox );
	magSpinBoxDrawStarInfo->setFont( stdFont );
	magSpinBoxDrawStarInfo->setValue( intMagLimitDrawStarInfo );

	textLabelMagStarInfo = new QLabel( StarsBox, "LabelMagStarNames" );
	textLabelMagStarInfo->setText( i18n( "for stars brighter than" ) );
	textLabelMagStarInfo->setFont( stdFont );

	showStarNames = new QCheckBox( i18n( "Show name" ), StarsBox );
	showStarNames->setFont( stdFont );
	showStarNames->setChecked( ksw->options()->drawStarName );

	showStarMagnitude = new QCheckBox( i18n( "Show magnitude" ), StarsBox );
	showStarMagnitude->setFont( stdFont );
	showStarMagnitude->setChecked( ksw->options()->drawStarMagnitude );

	showStarNames->setEnabled( showSAO->isChecked() );
	showStarMagnitude->setEnabled( showSAO->isChecked() );
	magSpinBoxDrawStars->setEnabled( showSAO->isChecked() );
	textLabelMagStars->setEnabled( showSAO->isChecked() );
	magSpinBoxDrawStarInfo->setEnabled( showSAO->isChecked() );
	textLabelMagStarInfo->setEnabled( showSAO->isChecked() );

	QSpacerItem *spacerDrawLim = new QSpacerItem( 10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum );
	QSpacerItem *spacerLabelLim = new QSpacerItem( 10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum );

	hlayDrawLimit = new QHBoxLayout( 4 );
	hlayDrawLimit->addWidget( textLabelMagStars );
	hlayDrawLimit->addWidget( magSpinBoxDrawStars );
	hlayDrawLimit->addItem( spacerDrawLim );

	hlayLabelLimit = new QHBoxLayout( 4 );
	hlayLabelLimit->addWidget( textLabelMagStarInfo );
	hlayLabelLimit->addWidget( magSpinBoxDrawStarInfo );
	hlayLabelLimit->addItem( spacerLabelLim );

	vlayStarsBox->addSpacing( 10 );
	vlayStarsBox->addWidget( showSAO );
	vlayStarsBox->addLayout( hlayDrawLimit );
	vlayStarsBox->addSpacing( 10 );
	vlayStarsBox->addLayout( hlayLabelLimit );
	vlayStarsBox->addWidget( showStarNames );
	vlayStarsBox->addWidget( showStarMagnitude );

//Deep-sky catalogs
//Store these in a list box, so that custom catalogs can be added
	CatalogList = new QListView( CatalogTab, "catalog_list" );
	CatalogList->addColumn( i18n( "Other Catalogs" ) );

//Incompatible with KDE2.2:
//	CatalogList->setResizeMode( QListView::LastColumn );
  CatalogList->setVScrollBarMode( QScrollView::AlwaysOn );
  CatalogList->setSorting( -1 );
  CatalogList->setMaximumHeight( 128 );

	showIC = new QCheckListItem( CatalogList, i18n( "Index Catalog (IC)" ), QCheckListItem::CheckBox );
	showIC->setOn( ksw->options()->drawIC );

	showNGC = new QCheckListItem( CatalogList, i18n( "New General Catalog (NGC)" ), QCheckListItem::CheckBox );
	showNGC->setOn( ksw->options()->drawNGC );

	showMessImages = new QCheckListItem( CatalogList, i18n( "Messier Catalog (images)" ), QCheckListItem::CheckBox );
	showMessImages->setOn( ksw->options()->drawMessImages );

	showMessier = new QCheckListItem( CatalogList, i18n( "Messier Catalog (symbols)" ), QCheckListItem::CheckBox );
	showMessier->setOn( ksw->options()->drawMessier );

//Add custom catalogs, if necessary
	for ( unsigned int i=0; i<ksw->options()->CatalogCount; ++i ) { //loop over custom catalogs
		QCheckListItem *newItem = new QCheckListItem( CatalogList, ksw->options()->CatalogName[i], QCheckListItem::CheckBox );
		newItem->setOn( ksw->options()->drawCatalog[i] );
	}

	hlayCatButtons = new QHBoxLayout( 4 );

	AddCatalog = new QPushButton( i18n( "Add Custom Catalog" ), CatalogTab );
	AddCatalog->setFont( stdFont );

	RemoveCatalog = new QPushButton( i18n( "Remove Custom Catalog" ), CatalogTab );
	RemoveCatalog->setFont( stdFont );
	RemoveCatalog->setEnabled( false );

	hlayCatButtons->addWidget( AddCatalog );
	hlayCatButtons->addWidget( RemoveCatalog );

	QSpacerItem *spacerCatTab = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );

	vlayCatTab->addWidget( StarsBox );
	vlayCatTab->addSpacing( 20 );
	vlayCatTab->addWidget( CatalogList );
	vlayCatTab->addLayout( hlayCatButtons );
	vlayCatTab->addItem( spacerCatTab );

	DisplayTabs->insertTab( CatalogTab, i18n( "Catalogs" ) );


//*** Construct Planets tab //
	PlanetTab = new QWidget( DisplayTabs, "PlanetTab" );
	vlayPlanetTab = new QVBoxLayout( PlanetTab );
	vlayPlanetTab->setSpacing( 6 );
	vlayPlanetTab->setMargin( 11 );
	glayPlanetTab = new QGridLayout( 7, 2 );

	showSun = new QCheckBox( i18n( "The Sun" ), PlanetTab, "show_sun" );
	showSun->setFont( stdFont );
	showSun->setChecked( ksw->options()->drawSun );

	showMoon = new QCheckBox( i18n( "The Moon" ), PlanetTab, "show_moon" );
  showMoon->setFont( stdFont );
  showMoon->setChecked( ksw->options()->drawMoon );
  showMoon->setChecked( ksw->options()->drawMoon );

	showMercury = new QCheckBox( i18n( "Mercury" ), PlanetTab, "show_mercury" );
	showMercury->setFont( stdFont );
	showMercury->setChecked( ksw->options()->drawMercury );

	showVenus = new QCheckBox( i18n( "Venus" ), PlanetTab, "show_venus" );
	showVenus->setFont( stdFont );
	showVenus->setChecked( ksw->options()->drawVenus );

	showMars = new QCheckBox( i18n( "Mars" ), PlanetTab, "show_mars" );
	showMars->setFont( stdFont );
	showMars->setChecked( ksw->options()->drawMars );

	showJupiter = new QCheckBox( i18n( "Jupiter" ), PlanetTab, "show_jupiter" );
	showJupiter->setFont( stdFont );
	showJupiter->setChecked( ksw->options()->drawJupiter );

	showSaturn = new QCheckBox( i18n( "Saturn" ), PlanetTab, "show_saturn" );
	showSaturn->setFont( stdFont );
	showSaturn->setChecked( ksw->options()->drawSaturn );

	showUranus = new QCheckBox( i18n( "Uranus" ), PlanetTab, "show_uranus" );
	showUranus->setFont( stdFont );
	showUranus->setChecked( ksw->options()->drawUranus );

	showNeptune = new QCheckBox( i18n( "Neptune" ), PlanetTab, "show_neptune" );
	showNeptune->setFont( stdFont );
	showNeptune->setChecked( ksw->options()->drawNeptune );

	showPluto = new QCheckBox( i18n( "Pluto" ), PlanetTab, "show_pluto" );
	showPluto->setFont( stdFont );
	showPluto->setChecked( ksw->options()->drawPluto );

	showAll = new QPushButton( i18n( "Show All Planets", "Show All" ), PlanetTab, "show_all_planets" );
	showAll->setFont( stdFont );

	showNone = new QPushButton( i18n( "Hide All Planets", "Show None" ), PlanetTab, "show_none_planets" );
	showNone->setFont( stdFont );

	QSpacerItem *smallSpacerLeft  = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum );
	QSpacerItem *smallSpacerRight = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum );
	QSpacerItem *smallSpacerVert = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum );

	showPlanetNames = new QCheckBox( i18n( "Show planet names" ), PlanetTab, "show_planet_names" );
	showPlanetNames->setFont( stdFont );
	showPlanetNames->setChecked( ksw->options()->drawPlanetName );

	showPlanetImages = new QCheckBox( i18n( "Show planet images" ), PlanetTab, "show_planet_images" );
	showPlanetImages->setFont( stdFont );
	showPlanetImages->setChecked( ksw->options()->drawPlanetImage );

	QSpacerItem *spacerPlanetTab  = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );

	glayPlanetTab->addWidget( showSun, 0, 0 );
	glayPlanetTab->addWidget( showMoon, 1, 0 );
	glayPlanetTab->addWidget( showMercury, 2, 0 );
	glayPlanetTab->addWidget( showVenus, 3, 0 );
	glayPlanetTab->addWidget( showMars, 4, 0 );
	glayPlanetTab->addWidget( showJupiter, 0, 1 );
	glayPlanetTab->addWidget( showSaturn, 1, 1 );
	glayPlanetTab->addWidget( showUranus, 2, 1 );
	glayPlanetTab->addWidget( showNeptune, 3, 1 );
	glayPlanetTab->addWidget( showPluto, 4, 1 );
	glayPlanetTab->addItem( smallSpacerLeft, 5, 0 );
	glayPlanetTab->addItem( smallSpacerRight, 5, 1 );
	glayPlanetTab->addWidget( showAll, 6, 0 );
	glayPlanetTab->addWidget( showNone, 6, 1 );

	vlayPlanetTab->addLayout( glayPlanetTab );
	vlayPlanetTab->addItem( smallSpacerVert );
	vlayPlanetTab->addWidget( showPlanetNames );
	vlayPlanetTab->addWidget( showPlanetImages );
	vlayPlanetTab->addItem( spacerPlanetTab );

	DisplayTabs->insertTab( PlanetTab, i18n( "Solar System" ) );


//*** Construct Guides tab: ***//
	GuideTab = new QWidget( DisplayTabs, "GuideTab" );
	vlayGuideTab = new QVBoxLayout( GuideTab );
	vlayGuideTab->setSpacing( 6 );
	vlayGuideTab->setMargin( 11 );

	showConstellLines = new QCheckBox( i18n( "Constellation lines" ), GuideTab );
	showConstellLines->setFont( stdFont );
	showConstellLines->setChecked( ksw->options()->drawConstellLines );

	showConstellNames = new QCheckBox( i18n( "Constellation names" ), GuideTab );
	showConstellNames->setFont( stdFont );
	showConstellNames->setChecked( ksw->options()->drawConstellNames );
	ConstellOptions = new QButtonGroup( 1, Qt::Vertical, i18n( "Constellation Name Options" ), GuideTab );
	useLatinConstellNames = new QRadioButton( i18n( "Standard Latin constellation names (e.g., 'Lyra')", "Latin" ), ConstellOptions );
	useLatinConstellNames->setFont ( stdFont );
	useLatinConstellNames->setChecked( ksw->options()->useLatinConstellNames );
  useLocalConstellNames = new QRadioButton( i18n( "Local translated constellation names (e.g., en='The Lyre'", "Localized" ), ConstellOptions );
	useLocalConstellNames->setFont ( stdFont );
	useLocalConstellNames->setChecked( ksw->options()->useLocalConstellNames );
  useAbbrevConstellNames = new QRadioButton( i18n( "Standard IAU constellation abbreviation (e.g., 'Lyr')", "Abbreviated" ), ConstellOptions );
	useAbbrevConstellNames->setFont ( stdFont );
	useAbbrevConstellNames->setChecked( ksw->options()->useAbbrevConstellNames );
	ConstellOptions->setEnabled( showConstellNames->isChecked() );

	showMilkyWay = new QCheckBox( i18n( "refers to the band of stars in the sky due to the Galactic plane", "Milky Way" ), GuideTab );
	showMilkyWay->setFont( stdFont );
	showMilkyWay->setChecked( ksw->options()->drawMilkyWay );

	showMilkyWayFilled = new QCheckBox( i18n( "display the Milky way as a solid polygon (instead of just an outline)", "Fill Milky Way" ), GuideTab );
	showMilkyWayFilled->setFont( stdFont );
	showMilkyWayFilled->setChecked( ksw->options()->fillMilkyWay );
  showMilkyWayFilled->setEnabled( showMilkyWay->isChecked() );

	showGrid = new QCheckBox( i18n( "Coordinate grid" ), GuideTab );
	showGrid->setFont( stdFont );
	showGrid->setChecked( ksw->options()->drawGrid );

	showEquator = new QCheckBox( i18n( "Celestial equator" ), GuideTab );
	showEquator->setFont( stdFont );
	showEquator->setChecked( ksw->options()->drawEquator );

	showEcliptic = new QCheckBox( i18n( "Ecliptic" ), GuideTab );
	showEcliptic->setFont( stdFont );
	showEcliptic->setChecked( ksw->options()->drawEcliptic );

	showHorizon = new QCheckBox( i18n( "Horizon (line)" ), GuideTab );
	showHorizon->setFont( stdFont );
	showHorizon->setChecked( ksw->options()->drawHorizon );

	showGround = new QCheckBox( i18n( "display the ground as solid, opaque green", "Opaque Ground" ), GuideTab );
	showGround->setFont( stdFont );
	showGround->setChecked( ksw->options()->drawGround );
	if ( !ksw->options()->useAltAz ) showGround->setEnabled( false );

	QSpacerItem *spacerGuideTab = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
	QSpacerItem *smallspacer = new QSpacerItem( 20, 12, QSizePolicy::Minimum, QSizePolicy::Minimum );
	QSpacerItem *smallspacer2= new QSpacerItem( 20, 12, QSizePolicy::Minimum, QSizePolicy::Minimum );

	vlayGuideTab->addWidget( showConstellLines );
	vlayGuideTab->addWidget( showConstellNames );
	vlayGuideTab->addWidget( ConstellOptions );
	vlayGuideTab->addItem( smallspacer );
	vlayGuideTab->addWidget( showMilkyWay );
	vlayGuideTab->addWidget( showMilkyWayFilled );
	vlayGuideTab->addItem( smallspacer2 );
	vlayGuideTab->addWidget( showGrid );
	vlayGuideTab->addWidget( showEquator );
	vlayGuideTab->addWidget( showEcliptic );
	vlayGuideTab->addWidget( showHorizon );
	vlayGuideTab->addWidget( showGround );
	vlayGuideTab->addItem( spacerGuideTab );

	DisplayTabs->insertTab( GuideTab, i18n( "Guides are imaginary lines displayed in the sky map", "Guides" ) );


//*** Construct the Color Tab ***//
	ColorTab = new QWidget( DisplayTabs, "ColorTab" );
	LeftBox = new QGroupBox( ColorTab, "LeftBox" );
	LeftBox->setTitle( i18n( "current color scheme settings", "Current Colors" ) );

	RightBox = new QGroupBox( ColorTab, "RightBox" );
	RightBox->setTitle( i18n( "list of defined color schemes", "Preset Color Schemes" ) );

  hlayColorTab = new QHBoxLayout( ColorTab );
	vlayLeftBox = new QVBoxLayout( LeftBox );
  hlayLeftBox = new QHBoxLayout( 4 );
  hlayLeftBox2 = new QHBoxLayout( 4 );
	vlayRightBox = new QVBoxLayout( RightBox );

	hlayColorTab->setSpacing( 4 );
	hlayColorTab->setMargin( 8 );
	vlayLeftBox->setSpacing( 6 );
	vlayLeftBox->setMargin( 12 );
	vlayRightBox->setSpacing( 6 );
	vlayRightBox->setMargin( 12 );

	ColorPalette = new QListBox( LeftBox );
  ColorPalette->setSelectionMode( QListBox::Single );

	SkyColor = new QPixmap( 30, 20 );
	SkyColor->fill( QColor( ksw->options()->colorSky ) );
	ColorPalette->insertItem( *SkyColor, i18n( "Sky" ) );

	MessColor = new QPixmap( 30, 20 );
	MessColor->fill( QColor( ksw->options()->colorMess ) );
	ColorPalette->insertItem( *MessColor, i18n( "Messier Object" ) );

	NGCColor = new QPixmap( 30, 20 );
	NGCColor->fill( QColor( ksw->options()->colorNGC ) );
	ColorPalette->insertItem( *NGCColor, i18n( "New General Catalog object", "NGC Object" ) );

	ICColor = new QPixmap( 30, 20 );
	ICColor->fill( QColor( ksw->options()->colorIC ) );
	ColorPalette->insertItem( *ICColor, i18n( "Index Catalog object", "IC Object" ) );

	LinksColor = new QPixmap( 30, 20 );
	LinksColor->fill( QColor( ksw->options()->colorHST ) );
	ColorPalette->insertItem( *LinksColor, i18n( "Object with extra attached URLs", "Object w/ Links" ) );

	SNameColor = new QPixmap( 30, 20 );
	SNameColor->fill( QColor( ksw->options()->colorSName ) );
	ColorPalette->insertItem( *SNameColor, i18n( "Star Name" ) );

	PNameColor = new QPixmap( 30, 20 );
	PNameColor->fill( QColor( ksw->options()->colorPName ) );
	ColorPalette->insertItem( *PNameColor, i18n( "Planet Name" ) );

	CNameColor = new QPixmap( 30, 20 );
	CNameColor->fill( QColor( ksw->options()->colorCName ) );
	ColorPalette->insertItem( *CNameColor, i18n( "Contstellation Name", "Constell. Name" ) );

	CLineColor = new QPixmap( 30, 20 );
	CLineColor->fill( QColor( ksw->options()->colorCLine ) );
	ColorPalette->insertItem( *CLineColor, i18n( "Constellation Line", "Constell. Line" ) );

	MWColor = new QPixmap( 30, 20 );
	MWColor->fill( QColor( ksw->options()->colorMW ) );
	ColorPalette->insertItem( *MWColor, i18n( "refers to the band of stars in the sky due to the Galactic plane", "Milky Way" ) );

	EqColor = new QPixmap( 30, 20 );
	EqColor->fill( QColor( ksw->options()->colorEq ) );
	ColorPalette->insertItem( *EqColor, i18n( "Equator" ) );

	EclColor = new QPixmap( 30, 20 );
	EclColor->fill( QColor( ksw->options()->colorEcl ) );
	ColorPalette->insertItem( *EclColor, i18n( "Ecliptic" ) );

	HorzColor = new QPixmap( 30, 20 );
	HorzColor->fill( QColor( ksw->options()->colorHorz ) );
	ColorPalette->insertItem( *HorzColor, i18n( "Horizon" ) );

	GridColor = new QPixmap( 30, 20 );
	GridColor->fill( QColor( ksw->options()->colorGrid ) );
	ColorPalette->insertItem( *GridColor, i18n( "Coordinate Grid" ) );

  // the spinbox for colorintensity of stars
	IntensityBox = new KIntSpinBox (0, 10, 1, ksw->options()->starColorIntensity, 10, LeftBox);
	QLabel *IntensityLabel = new QLabel (IntensityBox, i18n ( "scale for the color saturation of stars", "Star Color &Intensity" ), LeftBox);
	IntensityLabel->setAlignment ( AlignRight | AlignVCenter );

  // the QComboBox for the starcolor mode
	StarColorMode = new QComboBox( LeftBox );
	QLabel *ColorModeLabel = new QLabel( StarColorMode, i18n( "How stars are colored", "Star Color &Mode" ), LeftBox );
	ColorModeLabel->setAlignment ( AlignRight | AlignVCenter );
	StarColorMode->insertItem( i18n( "use realistic star colors", "Real Colors" ) );
	StarColorMode->insertItem( i18n( "show stars as red circles", "Solid Red" ) );
	StarColorMode->insertItem( i18n( "show stars as black circles", "Solid Black" ) );
	StarColorMode->insertItem( i18n( "show stars as white circles", "Solid White" ) );

  //The list of preset color schemes
	PresetBox = new QListBox( RightBox );
  PresetBox->setSelectionMode( QListBox::Single );
	PresetBox->insertItem( i18n( "use default color scheme", "Default Colors" ) );
	PresetBox->insertItem( i18n( "use 'star chart' color scheme", "Star Chart" ) );
	PresetBox->insertItem( i18n( "use 'night vision' color scheme", "Night Vision" ) );

	PresetFileList.append( "default.colors" );
	PresetFileList.append( "chart.colors" );
	PresetFileList.append( "night.colors" );

	QFile file;
	QString line, schemeName, filename;
	file.setName( locateLocal( "appdata", "colors.dat" ) ); //determine filename in local user KDE directory tree.
	if ( file.open( IO_ReadOnly ) ) {
		QTextStream stream( &file );

  	while ( !stream.eof() ) {
			line = stream.readLine();
			schemeName = line.left( line.find( ':' ) );
			filename = line.mid( line.find( ':' ) +1, line.length() );
			PresetBox->insertItem( schemeName );
			PresetFileList.append( filename );
	  }
		file.close();
	}

	AddPreset = new QPushButton( i18n( "Save Current Colors" ), RightBox );
	RemovePreset = new QPushButton( i18n( "Remove Color Scheme" ), RightBox );
	RemovePreset->setEnabled( false );

  //Pack the widgets into the layouts
	hlayLeftBox2->addWidget ( IntensityLabel );
	hlayLeftBox2->addWidget ( IntensityBox );

	hlayLeftBox->addWidget ( ColorModeLabel );
	hlayLeftBox->addWidget ( StarColorMode );

	QSpacerItem *LeftSpacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	QSpacerItem *RightSpacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

	vlayLeftBox->addItem( LeftSpacer );
	vlayLeftBox->addWidget( ColorPalette );
	vlayLeftBox->addLayout( hlayLeftBox );
	vlayLeftBox->addLayout( hlayLeftBox2 );

	vlayRightBox->addItem( RightSpacer );
	vlayRightBox->addWidget( PresetBox );
	vlayRightBox->addWidget( AddPreset );
	vlayRightBox->addWidget( RemovePreset );

	hlayColorTab->addWidget ( LeftBox );
	hlayColorTab->addWidget ( RightBox );

	DisplayTabs->insertTab( ColorTab, i18n( "Color configuration options (tab label)", "Colors" ) );


//*** Construct the Advanced Tab ***//
	AdvancedTab = new QWidget( DisplayTabs, "AdvancedTab" );
	vlayAdvancedTab = new QVBoxLayout( AdvancedTab );
	vlayAdvancedTab->setSpacing( 6 );
	vlayAdvancedTab->setMargin( 11 );

	useRefraction = new QCheckBox( i18n( "Correct for atmospheric refraction" ), AdvancedTab );
	useRefraction->setFont( stdFont );
	useRefraction->setChecked( ksw->options()->useRefraction );

	animateSlewing = new QCheckBox( i18n( "Use animated slewing" ), AdvancedTab );
	animateSlewing->setFont( stdFont );
	animateSlewing->setChecked( ksw->options()->useAnimatedSlewing );

	hideSpinBox = new TimeStepBox( AdvancedTab, "HideSpinBox" );
	hideSpinBox->tsbox()->changeScale( ksw->options()->slewTimeScale );
	QLabel *hsbLabel = new QLabel( AdvancedTab, "HSBLabel" );
	hsbLabel->setText( i18n( "also hide if Time Scale greater than: " ) );

	hideObjects = new QCheckBox( i18n( "Hide objects while moving" ), AdvancedTab );
	hideObjects->setFont( stdFont );
	hideObjects->setChecked( ksw->options()->hideOnSlew );

	HideBox = new QGroupBox( AdvancedTab, "HideBox" );
	HideBox->setTitle( i18n( "Configure Hidden Objects" ) );

	hideStars = new QCheckBox( i18n( "Hide stars fainter than XX magnitude", "Stars fainter than " ), HideBox );
	hideStars->setFont( stdFont );
	hideStars->setChecked( ksw->options()->hideStars );

	hidePlanets = new QCheckBox( i18n( "Planets" ), HideBox );
	hidePlanets->setFont( stdFont );
	hidePlanets->setChecked( ksw->options()->hidePlanets );

	hideMess = new QCheckBox( i18n( "Messier Objects" ), HideBox );
	hideMess->setFont( stdFont );
	hideMess->setChecked( ksw->options()->hideMess );

	hideNGC = new QCheckBox( i18n( "NGC Objects" ), HideBox );
	hideNGC->setFont( stdFont );
	hideNGC->setChecked( ksw->options()->hideNGC );

	hideIC = new QCheckBox( i18n( "IC Objects" ), HideBox );
	hideIC->setFont( stdFont );
	hideIC->setChecked( ksw->options()->hideIC );

	hideMW = new QCheckBox( i18n( "Milky Way" ), HideBox );
	hideMW->setFont( stdFont );
	hideMW->setChecked( ksw->options()->hideMW );

	hideCNames = new QCheckBox( i18n( "Constellation names" ), HideBox );
	hideCNames->setFont( stdFont );
	hideCNames->setChecked( ksw->options()->hideCNames );

	hideCLines = new QCheckBox( i18n( "Constellation lines" ), HideBox );
	hideCLines->setFont( stdFont );
	hideCLines->setChecked( ksw->options()->hideCLines );

	hideGrid = new QCheckBox( i18n( "Coordinate grid" ), HideBox );
	hideGrid->setFont( stdFont );
	hideGrid->setChecked( ksw->options()->hideGrid );

	int intMagLimitHideStars = int ( 10.0 * ksw->options()->magLimitHideStar );
	magSpinBoxHideStars = new MagnitudeSpinBox( 0, 79, HideBox );	// max mag = 7.9
	magSpinBoxHideStars->setFont( stdFont );
	magSpinBoxHideStars->setValue( intMagLimitHideStars );
	magSpinBoxHideStars->setMinimumWidth( 20 );

	QSpacerItem *spacerAdvTab = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
	QSpacerItem *spacerAdvHStars = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	QSpacerItem *spacerAdvHTime = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

	//Construct layouts and add widgets to them
	vlayAdvHideObj = new QVBoxLayout( HideBox );
	vlayAdvHideObj->setSpacing( 6 );
	vlayAdvHideObj->setMargin( 11 );
	vlayAdvHideObj->addSpacing( 10 ); //Otherwise first widget overlaps title

	hlayAdvHideStars = new QHBoxLayout( 4 );  //add to vlayAdvHideObj
	hlayAdvHideTimeScale = new QHBoxLayout( 4 );  //add to vlayAdvHideObj
	glayAdvHideObj = new QGridLayout( 4, 2 ); //add to vlayAdvHideObj

	hlayAdvHideStars->addWidget( hideStars );
	hlayAdvHideStars->addWidget( magSpinBoxHideStars );
	hlayAdvHideStars->addItem( spacerAdvHStars );

	hlayAdvHideTimeScale->addWidget( hsbLabel );
	hlayAdvHideTimeScale->addWidget( hideSpinBox );
	hlayAdvHideTimeScale->addItem( spacerAdvHTime );

	glayAdvHideObj->addWidget( hidePlanets, 0, 0 );
	glayAdvHideObj->addWidget( hideMess, 1, 0 );
	glayAdvHideObj->addWidget( hideNGC, 2, 0 );
	glayAdvHideObj->addWidget( hideIC, 3, 0 );
	glayAdvHideObj->addWidget( hideMW, 0, 1 );
	glayAdvHideObj->addWidget( hideCNames, 1, 1 );
	glayAdvHideObj->addWidget( hideCLines, 2, 1 );
	glayAdvHideObj->addWidget( hideGrid, 3, 1 );
	
	vlayAdvHideObj->addLayout( hlayAdvHideStars );
	vlayAdvHideObj->addLayout( glayAdvHideObj );

	vlayAdvancedTab->addWidget( useRefraction );
	vlayAdvancedTab->addWidget( animateSlewing );
	vlayAdvancedTab->addSpacing( 20 );
	vlayAdvancedTab->addWidget( hideObjects );
	vlayAdvancedTab->addLayout( hlayAdvHideTimeScale );
	vlayAdvancedTab->addWidget( HideBox );
	vlayAdvancedTab->addItem( spacerAdvTab );

	DisplayTabs->insertTab( AdvancedTab, i18n( "Advanced configuration options (tab label)", "Advanced" ) );

//*** Done Making Tabs ***//

//Activate main layout	
	hlay->addWidget( DisplayBox );
	hlay->activate();

//Connect Signals to Slots.  Each modifiable option widget shoudl be connected to updateDisplay()
//Dialog buttons
  connect( this, SIGNAL( okClicked() ), this, SLOT( accept() ) ) ;
  connect( this, SIGNAL( cancelClicked() ), this, SLOT( reject() ) );

//Catalog Tab
	connect( showSAO, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( CatalogList, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( updateDisplay() ) );
	connect( CatalogList, SIGNAL( selectionChanged() ), this, SLOT( selectCatalog() ) );
	connect( showStarNames, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showStarMagnitude, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( magSpinBoxDrawStars, SIGNAL( valueChanged( int ) ), SLOT( changeMagDrawStars( int) ) );
	connect( magSpinBoxDrawStarInfo, SIGNAL( valueChanged( int ) ), SLOT( changeMagDrawInfo( int) ) );
	connect( AddCatalog, SIGNAL( clicked() ), this, SLOT( slotAddCatalog() ) );
	connect( RemoveCatalog, SIGNAL( clicked() ), this, SLOT( slotRemoveCatalog() ) );

//Solar System Tab
	connect( showSun, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showMoon, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showMercury, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showVenus, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showMars, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showJupiter, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showSaturn, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showUranus, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showNeptune, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showPluto, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showAll, SIGNAL( clicked() ), this, SLOT( markPlanets() ) );
	connect( showNone, SIGNAL( clicked() ), this, SLOT( unMarkPlanets() ) );
	connect( showPlanetNames, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showPlanetImages, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );

//Guide Tab
	connect( showConstellLines, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showConstellNames, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( useLatinConstellNames, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( useLatinConstellNames, SIGNAL( clicked() ), this, SLOT( sendClearCache() ) );
	connect( useLocalConstellNames, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( useLocalConstellNames, SIGNAL( clicked() ), this, SLOT( sendClearCache() ) );
	connect( useAbbrevConstellNames, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( useAbbrevConstellNames, SIGNAL( clicked() ), this, SLOT( sendClearCache() ) );
	connect( showMilkyWay, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showMilkyWayFilled, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showGrid, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showEquator, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showEcliptic, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showHorizon, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( showGround, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );

//Color Tab
	connect( ColorPalette, SIGNAL( clicked( QListBoxItem* ) ), this, SLOT( newColor( QListBoxItem* ) ) );
	connect( PresetBox, SIGNAL( highlighted( int ) ), this, SLOT( slotPreset( int ) ) );
	connect( AddPreset, SIGNAL( clicked() ), this, SLOT( slotAddPreset() ) );
	connect( RemovePreset, SIGNAL( clicked() ), this, SLOT( slotRemovePreset() ) );
	connect( IntensityBox, SIGNAL (valueChanged (int)), SLOT (changeStarColorIntensity(int)) );
	connect( StarColorMode, SIGNAL( activated( int ) ), this, SLOT( changeStarColorMode( int ) ) );

//Advanced Tab
	connect( useRefraction, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( animateSlewing, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( hideSpinBox, SIGNAL( scaleChanged( float ) ), this, SLOT( changeSlewTimeScale( float ) ) );
	connect( hideObjects, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( hideStars, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( hidePlanets, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( hideMess, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( hideNGC, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( hideIC, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( hideMW, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( hideCNames, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( hideCLines, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( hideGrid, SIGNAL( clicked() ), this, SLOT( updateDisplay() ) );
	connect( magSpinBoxHideStars, SIGNAL( valueChanged( int ) ), SLOT( changeMagHideStars( int) ) );

//Coordinate Systems
	connect( EquatRadio, SIGNAL( clicked() ), this, SLOT( changeCoordSys() ) );
	connect( AltAzRadio, SIGNAL( clicked() ), this, SLOT( changeCoordSys() ) );

}

ViewOpsDialog::~ViewOpsDialog(){
	delete SkyColor;  delete MessColor;  delete NGCColor;  delete ICColor;
	delete LinksColor;  delete SNameColor;  delete CLineColor;  delete CNameColor;
	delete EqColor;  delete EclColor;  delete HorzColor;  delete GridColor;
	delete MWColor;
}

void ViewOpsDialog::changeMagDrawStars( int newValue )
{
	float fNewValue = ( newValue * 1.0) / 10.0;

	ksw->data()->setMagnitude( fNewValue );

	// force redraw
	ksw->map()->Update();
}

void ViewOpsDialog::changeMagHideStars( int newValue )
{
	float fNewValue = ( newValue * 1.0) / 10.0;
	ksw->options()->magLimitHideStar = fNewValue;

//no need to redraw
//	ksw->map()->Update();
}

void ViewOpsDialog::changeMagDrawInfo( int newValue )
{
    kdDebug()<<"magnitude limit draw star info "<<newValue<<endl;
	float fNewValue = ( newValue * 1.0) / 10.0;
	ksw->options()->magLimitDrawStarInfo = fNewValue;
	// force redraw
	ksw->map()->Update();
}

void ViewOpsDialog::newColor( QListBoxItem *item ) {
	QPixmap *temp = new QPixmap( 30, 20 );
	QColor newColor;
	if ( item->text() == i18n( "color of sky", "Sky" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorSky ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorSky = newColor.name();
			temp->fill( QColor( ksw->options()->colorSky ) );
		}
  } else if ( item->text() == i18n( "Messier Object" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorMess ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorMess = newColor.name();
			temp->fill( QColor( ksw->options()->colorMess ) );
		}
  } else if ( item->text() == i18n( "NGC Object" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorNGC ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorNGC = newColor.name();
			temp->fill( QColor( ksw->options()->colorNGC ) );
		}
  } else if ( item->text() == i18n( "IC Object" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorIC ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorIC = newColor.name();
			temp->fill( QColor( ksw->options()->colorIC ) );
		}
  } else if ( item->text() == i18n( "Object with extra attached URLs", "Object w/ Links" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorHST ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorHST = newColor.name();
			temp->fill( QColor( ksw->options()->colorHST ) );
		}
  } else if ( item->text() == i18n( "Milky Way" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorMW ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorMW = newColor.name();
			temp->fill( QColor( ksw->options()->colorMW ) );
		}
  } else if ( item->text() == i18n( "Equator" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorEq ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorEq = newColor.name();
			temp->fill( QColor( ksw->options()->colorEq ) );
		}
  } else if ( item->text() == i18n( "Ecliptic" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorEcl ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorEcl = newColor.name();
			temp->fill( QColor( ksw->options()->colorEcl ) );
		}
  } else if ( item->text() == i18n( "Horizon" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorHorz ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorHorz = newColor.name();
			temp->fill( QColor( ksw->options()->colorHorz ) );
		}
  } else if ( item->text() == i18n( "Coordinate Grid" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorGrid ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorGrid = newColor.name();
			temp->fill( QColor( ksw->options()->colorGrid ) );
		}
  } else if ( item->text() == i18n( "Star Name" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorSName ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorSName = newColor.name();
			temp->fill( QColor( ksw->options()->colorSName ) );
		}
  } else if ( item->text() == i18n( "Planet Name" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorPName ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorPName = newColor.name();
			temp->fill( QColor( ksw->options()->colorPName ) );
		}
  } else if ( item->text() == i18n( "constellation line", "Constell. Line" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorCLine ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorCLine = newColor.name();
			temp->fill( QColor( ksw->options()->colorCLine ) );
		}
  } else if ( item->text() == i18n( "constellation name", "Constell. Name" ) ) {
		newColor = QColorDialog::getColor( QColor( ksw->options()->colorCName ) );
		if ( newColor.isValid() ) {
			ksw->options()->colorCName = newColor.name();
			temp->fill( QColor( ksw->options()->colorCName ) );
		}
	}

	if ( newColor.isValid() )
		ColorPalette->changeItem( *temp, item->text(), ColorPalette->index( item ) );

	delete temp;
	ksw->map()->Update();
}

void ViewOpsDialog::slotPreset( int index ) {
	QStringList::Iterator it = PresetFileList.at( index );
	bool result = setColors( *it );
	if (!result) {
		QString message = i18n( "The specified color scheme file could not be found, or it was corrupted." );
		KMessageBox::sorry( 0, message, i18n( "Could not set Color Scheme" ) );
	}
}

void ViewOpsDialog::slotAddPreset( void ) {
	QFile file;
	bool okPressed = false;

//KDE3-only version (incompatible w/KDE2 because of different arguments)
//  QString schemename = KLineEditDlg::getText( i18n( "New Color Scheme" ),
//											i18n( "Enter a name for the new color scheme" ),
//											QString::null, &okPressed, 0 );
	KLineEditDlg schemeDlg( i18n( "Enter a name for the new color scheme" ),
											QString::null, 0 );
	int result = schemeDlg.exec();
	if ( result ) {
		QString filename = schemeDlg.text().lower().stripWhiteSpace();
		if ( !filename.isEmpty() ) {
			for( int i=0; i<filename.length(); ++i)
				if ( filename.at(i)==' ' ) filename.replace( i, 1, "-" );

			filename = filename.append( ".colors" );
			file.setName( locateLocal( "appdata", filename ) ); //determine filename in local user KDE directory tree.

			if ( file.exists() || !file.open( IO_ReadWrite | IO_Append ) ) {
				QString message = i18n( "Local color scheme file could not be opened.\nScheme cannot be recorded." );
				KMessageBox::sorry( 0, message, i18n( "Could not Open File" ) );
			} else {
				QTextStream stream( &file );
				stream << ksw->options()->starColorMode << endl;
				stream << ksw->options()->colorSky << " :colorSky" << endl;
				stream << ksw->options()->colorMess << " :colorMess" << endl;
				stream << ksw->options()->colorNGC << " :colorNGC" << endl;
				stream << ksw->options()->colorIC << " :colorIC" << endl;
				stream << ksw->options()->colorHST << " :colorHST" << endl;
				stream << ksw->options()->colorSName << " :colorSName" << endl;
				stream << ksw->options()->colorPName << " :colorPName" << endl;
				stream << ksw->options()->colorCName << " :colorCName" << endl;
				stream << ksw->options()->colorCLine << " :colorCLine" << endl;
				stream << ksw->options()->colorMW << " :colorMW" << endl;
				stream << ksw->options()->colorEq << " :colorEq" << endl;
				stream << ksw->options()->colorEcl << " :colorEcl" << endl;
				stream << ksw->options()->colorHorz << " :colorHorz" << endl;
				stream << ksw->options()->colorGrid << " :colorGrid" << endl;
				file.close();
			}

			file.setName( locateLocal( "appdata", "colors.dat" ) ); //determine filename in local user KDE directory tree.

			if ( !file.open( IO_ReadWrite | IO_Append ) ) {
				QString message = i18n( "Local color scheme index file could not be opened.\nScheme cannot be recorded." );
				KMessageBox::sorry( 0, message, i18n( "Could not Open File" ) );
			} else {
				QTextStream stream( &file );
				stream << schemeDlg.text() << ":" << filename << endl;
				file.close();
			}

			PresetBox->insertItem( schemeDlg.text() );
			PresetFileList.append( filename );
		}
	}
}

void ViewOpsDialog::slotRemovePreset( void ) {
	QString name = PresetBox->currentText();
	QString filename = PresetFileList[ PresetBox->currentItem() ];
	QFile cdatFile;
	cdatFile.setName( locateLocal( "appdata", "colors.dat" ) ); //determine filename in local user KDE directory tree.

	//Remove entry from the ListBox and from the QStringList holding filenames.
	PresetFileList.remove( PresetFileList.at( PresetBox->currentItem() ) );
	PresetBox->removeItem( PresetBox->currentItem() );

	if ( !cdatFile.exists() || !cdatFile.open( IO_ReadWrite ) ) {
		QString message = i18n( "Local color scheme index file could not be opened.\nScheme cannot be removed." );
		KMessageBox::sorry( 0, message, i18n( "Could not Open File" ) );
	} else {
		//Read the contents of colors.dat into a QStringList, except for the entry to be removed.
		QTextStream stream( &cdatFile );
		QStringList slist;
		bool removed = false;

		while ( !stream.eof() ) {
			QString line = stream.readLine();
//DEBUG
			kdWarning() << line << endl;
//END_DEBUG
			if ( line.left( line.find(':') ) != name ) slist.append( line );
			else removed = true;
		}

		if ( removed ) { //Entry was removed; delete the corresponding .colors file.
			QFile colorFile;
			colorFile.setName( locateLocal( "appdata", filename ) ); //determine filename in local user KDE directory tree.
			if ( !colorFile.remove() ) {
				QString message = i18n( "Could not delete the file: %1" ).arg( colorFile.name() );
				KMessageBox::sorry( 0, message, i18n( "Error Deleting File" ) );
			}

			//remove the old colors.dat file, and rebuild it with the modified string list.
			cdatFile.remove();
			cdatFile.open( IO_ReadWrite );
			QTextStream stream2( &cdatFile );
			for( unsigned int i=0; i<slist.count(); ++i )
				stream << slist[i] << endl;
		} else {
			QString message = i18n( "Could not find an entry named %1 in colors.dat" ).arg( name );
			KMessageBox::sorry( 0, message, i18n( "Scheme not Found in colors.dat" ) );
		}
		cdatFile.close();
	}
}

bool ViewOpsDialog::setColors( QString filename ) {
	QPixmap *temp = new QPixmap( 30, 20 );
	QFile file;
	int i=0;
	bool colonLineFound = false;

	if ( !KSUtils::openDataFile( file, filename ) ) {
		return false;
	} else {
		//Is the colorscheme file stored in the user directory?
		//If so, enable the RemovePreset button.
		QFile test;
		test.setName( locateLocal( "appdata", filename ) ); //try filename in local user KDE directory tree.
		if ( test.exists() ) RemovePreset->setEnabled( true );
		else RemovePreset->setEnabled( false );
	}

	QTextStream stream( &file );
	QString line;

	//first line is the star-color mode
  line = stream.readLine();
	int newmode = line.left(1).toInt();
	ksw->options()->starColorMode = newmode;
	if ( ksw->map()->starColorMode() != newmode )
		ksw->map()->setStarColorMode( newmode );
	StarColorMode->setCurrentItem( newmode );

//More flexible method for reading in color values.  Any order is acceptable, and
//missing entries are ignored.
	while ( !stream.eof() ) {
		line = stream.readLine();

		if ( line.contains(':')==1 ) { //the new color preset format contains a ":" in each line, followed by the name of the color
     colonLineFound = true;

			if ( i > 0 ) return false; //we read at least one line without a colon...file is corrupted.

			if ( line.mid( line.find(':')+1 ).contains( "colorSky" ) ) {
				ksw->options()->colorSky = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorSky ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(0)->text(), 0 );
			} else if ( line.mid( line.find(':')+1 ).contains( "colorMess" ) ) {
				ksw->options()->colorMess = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorMess ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(1)->text(), 1 );
			} else if ( line.mid( line.find(':')+1 ).contains( "colorNGC" ) ) {
				ksw->options()->colorNGC = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorNGC ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(2)->text(), 2 );
			} else if ( line.mid( line.find(':')+1 ).contains( "colorIC" ) ) {
				ksw->options()->colorIC = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorIC ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(3)->text(), 3 );
			} else if ( line.mid( line.find(':')+1 ).contains( "colorHST" ) ) {
				ksw->options()->colorHST = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorHST ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(4)->text(), 4 );
			} else if ( line.mid( line.find(':')+1 ).contains( "colorSName" ) ) {
				ksw->options()->colorSName = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorSName ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(5)->text(), 5 );
			} else if ( line.mid( line.find(':')+1 ).contains( "colorPName" ) ) {
				ksw->options()->colorPName = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorPName ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(6)->text(), 6 );
			} else if ( line.mid( line.find(':')+1 ).contains( "colorCName" ) ) {
				ksw->options()->colorCName = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorCName ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(7)->text(), 7 );
			} else if ( line.mid( line.find(':')+1 ).contains( "colorCLine" ) ) {
				ksw->options()->colorCLine = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorCLine ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(8)->text(), 8 );
			} else if ( line.mid( line.find(':')+1 ).contains( "colorMW" ) ) {
				ksw->options()->colorMW = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorMW ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(9)->text(), 9 );
			} else if ( line.mid( line.find(':')+1 ).contains( "colorEq" ) ) {
				ksw->options()->colorEq = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorEq ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(10)->text(), 10 );
			} else if ( line.mid( line.find(':')+1 ).contains( "colorEcl" ) ) {
				ksw->options()->colorEcl = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorEcl ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(11)->text(), 11 );
			} else if ( line.mid( line.find(':')+1 ).contains( "colorHorz" ) ) {
				ksw->options()->colorHorz = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorHorz ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(12)->text(), 12 );
			} else if ( line.mid( line.find(':')+1 ).contains( "colorGrid" ) ) {
				ksw->options()->colorGrid = line.left( line.find(':')-1 );
				temp->fill( QColor( ksw->options()->colorGrid ) );
				ColorPalette->changeItem( *temp, ColorPalette->item(13)->text(), 13 );
			}

		} else { // no ':' seen in the line, so we must assume the old format

			if ( colonLineFound ) return false; //a previous line had a colon, this line doesn't.  File is corrupted.

			switch (i) {
				case 0: //sky
					ksw->options()->colorSky = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorSky ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
				case 1: //Messier objects
					ksw->options()->colorMess = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorMess ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
				case 2:
					ksw->options()->colorNGC = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorNGC ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
				case 3:
					ksw->options()->colorIC = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorIC ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
				case 4:
					ksw->options()->colorHST = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorHST ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
				case 5:
					ksw->options()->colorSName = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorSName ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
				case 6:
					ksw->options()->colorPName = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorPName ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
				case 7:
					ksw->options()->colorCName = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorCName ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
				case 8:
					ksw->options()->colorCLine = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorCLine ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
				case 9:
					ksw->options()->colorMW = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorMW ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
				case 10:
					ksw->options()->colorEq = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorEq ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
				case 11:
					ksw->options()->colorEcl = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorEcl ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
				case 12:
					ksw->options()->colorHorz = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorHorz ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
				case 13:
					ksw->options()->colorGrid = line.left( 7 );
					temp->fill( QColor( ksw->options()->colorGrid ) );
					ColorPalette->changeItem( *temp, ColorPalette->item(i)->text(), i );
					i++;
					break;
			}
		}
	}
	ksw->map()->Update();
	return true;
}

void ViewOpsDialog::updateDisplay( void ) {
//Set the drawPlanets or drawDeepSky meta-options to true if the options which
//these substitute for are altered.
	if ( sender()->name() == QString( "catalog_name" ) )
		ksw->options()->drawDeepSky = true;

	if ( sender()->name() == QString( "show_sun" ) ||
			sender()->name() == QString( "show_moon" ) ||
			sender()->name() == QString( "show_mercury" ) ||
			sender()->name() == QString( "show_venus" ) ||
			sender()->name() == QString( "show_mars" ) ||
			sender()->name() == QString( "show_jupiter" ) ||
			sender()->name() == QString( "show_saturn" ) ||
			sender()->name() == QString( "show_uranus" ) ||
			sender()->name() == QString( "show_neptune" ) ||
			sender()->name() == QString( "show_pluto" ) ||
			sender()->name() == QString( "show_planet_names" ) ||
			sender()->name() == QString( "show_planet_images" ) ||
			sender()->name() == QString( "show_all_planets" ) ||
			sender()->name() == QString( "show_none_planets" ) )
		ksw->options()->drawPlanets = true;

//Set options according to current settings
//Catalogs Tab
	ksw->options()->drawSAO = showSAO->isChecked();
	ksw->options()->drawMessier = showMessier->isOn();
	ksw->options()->drawMessImages = showMessImages->isOn();
	ksw->options()->drawNGC = showNGC->isOn();
	ksw->options()->drawIC = showIC->isOn();
	for ( unsigned int i=0; i<ksw->options()->CatalogCount; ++i ) {
		//KDE2.2 has no findItem function
		//QCheckListItem *item = (QCheckListItem*)(CatalogList->findItem( ksw->options()->CatalogName[i], 0 ));
		for ( QCheckListItem *item = (QCheckListItem*)CatalogList->firstChild(); item; item = (QCheckListItem*)item->nextSibling() )
			if ( item->text( 0 ) == ksw->options()->CatalogName[i] )
				ksw->options()->drawCatalog[i] = item->isOn();
	}

	ksw->options()->drawStarName = showStarNames->isChecked();
	ksw->options()->drawStarMagnitude = showStarMagnitude->isChecked();
	//star options enabled only if showSAO is checked...
	showStarNames->setEnabled( showSAO->isChecked() );
	showStarMagnitude->setEnabled( showSAO->isChecked() );
	magSpinBoxDrawStars->setEnabled( showSAO->isChecked() );
	textLabelMagStars->setEnabled( showSAO->isChecked() );
	magSpinBoxDrawStarInfo->setEnabled( showSAO->isChecked() );
	textLabelMagStarInfo->setEnabled( showSAO->isChecked() );

//Planets Tab
	ksw->options()->drawSun = showSun->isChecked();
	ksw->options()->drawMoon = showMoon->isChecked();
	ksw->options()->drawMercury = showMercury->isChecked();
	ksw->options()->drawVenus = showVenus->isChecked();
	ksw->options()->drawMars = showMars->isChecked();
	ksw->options()->drawJupiter = showJupiter->isChecked();
	ksw->options()->drawSaturn = showSaturn->isChecked();
	ksw->options()->drawUranus = showUranus->isChecked();
	ksw->options()->drawNeptune = showNeptune->isChecked();
	ksw->options()->drawPluto = showPluto->isChecked();
	ksw->options()->drawPlanetName = showPlanetNames->isChecked();
	ksw->options()->drawPlanetImage = showPlanetImages->isChecked();

//Guides Tab
	ksw->options()->drawConstellLines = showConstellLines->isChecked();
	ksw->options()->drawConstellNames = showConstellNames->isChecked();
	ksw->options()->useLatinConstellNames = useLatinConstellNames->isChecked();
	ksw->options()->useLocalConstellNames = useLocalConstellNames->isChecked();
	ksw->options()->useAbbrevConstellNames = useAbbrevConstellNames->isChecked();
	ksw->options()->drawMilkyWay = showMilkyWay->isChecked();
	ksw->options()->fillMilkyWay = showMilkyWayFilled->isChecked();
	ksw->options()->drawGrid = showGrid->isChecked();
	ksw->options()->drawEquator = showEquator->isChecked();
	ksw->options()->drawEcliptic = showEcliptic->isChecked();
	ksw->options()->drawHorizon = showHorizon->isChecked();
	ksw->options()->drawGround = showGround->isChecked();
	//constellation name options enabled only if showConstellationNames is checked...
	ConstellOptions->setEnabled( showConstellNames->isChecked() );
	//fill MW checkbox enabled only if showMilkyWay is checked...
	showMilkyWayFilled->setEnabled( showMilkyWay->isChecked() );

	//Advanced Tab
	ksw->options()->useRefraction = useRefraction->isChecked();
	ksw->options()->useAnimatedSlewing = animateSlewing->isChecked();
	ksw->options()->hideOnSlew = hideObjects->isChecked();
	ksw->options()->hideStars = hideStars->isChecked();
	ksw->options()->hidePlanets = hidePlanets->isChecked();
	ksw->options()->hideMess = hideMess->isChecked();
	ksw->options()->hideNGC = hideNGC->isChecked();
	ksw->options()->hideIC = hideIC->isChecked();
	ksw->options()->hideMW = hideMW->isChecked();
	ksw->options()->hideCNames = hideCNames->isChecked();
	ksw->options()->hideCLines = hideCLines->isChecked();
	ksw->options()->hideGrid = hideGrid->isChecked();
	//HideBox widgets enabled only if hideObjects is checked...
	hideSpinBox->setEnabled( hideObjects->isChecked() );
	hideStars->setEnabled( hideObjects->isChecked() );
	magSpinBoxHideStars->setEnabled( hideObjects->isChecked() );
	hidePlanets->setEnabled( hideObjects->isChecked() );
	hideMess->setEnabled( hideObjects->isChecked() );
	hideNGC->setEnabled( hideObjects->isChecked() );
	hideIC->setEnabled( hideObjects->isChecked() );
	hideMW->setEnabled( hideObjects->isChecked() );
	hideCNames->setEnabled( hideObjects->isChecked() );
	hideCLines->setEnabled( hideObjects->isChecked() );
	hideGrid->setEnabled( hideObjects->isChecked() );

	ksw->map()->Update();
}

void ViewOpsDialog::changeCoordSys( void ) {
	ksw->options()->useAltAz = AltAzRadio->isChecked();
	showGround->setEnabled( AltAzRadio->isChecked() );

	if ( EquatRadio->isChecked() ) {
		showGround->setChecked( false );
	}
	ksw->options()->drawGround = showGround->isChecked();
	ksw->map()->Update();
}

void ViewOpsDialog::changeStarColorIntensity( int newValue ) {
	ksw->map()->setStarColorIntensity( newValue );
	ksw->options()->starColorIntensity = ksw->map()->starColorIntensity();
	ksw->map()->Update();
}

void ViewOpsDialog::changeStarColorMode( int newValue ) {
	ksw->map()->setStarColorMode( newValue );
	ksw->options()->starColorMode = ksw->map()->starColorMode();
	if (newValue) IntensityBox->setEnabled( false );
	else IntensityBox->setEnabled( true );
	ksw->map()->Update();
}

void ViewOpsDialog::markPlanets( void ) {
	showSun->setChecked( true );
	showMoon->setChecked( true );
	showMercury->setChecked( true );
	showVenus->setChecked( true );
	showMars->setChecked( true );
	showJupiter->setChecked( true );
	showSaturn->setChecked( true );
	showUranus->setChecked( true );
	showNeptune->setChecked( true );
	showPluto->setChecked( true );
	updateDisplay();
}

void ViewOpsDialog::unMarkPlanets( void ) {
	showSun->setChecked( false );
	showMoon->setChecked( false );
	showMercury->setChecked( false );
	showVenus->setChecked( false );
	showMars->setChecked( false );
	showJupiter->setChecked( false );
	showSaturn->setChecked( false );
	showUranus->setChecked( false );
	showNeptune->setChecked( false );
	showPluto->setChecked( false );
	updateDisplay();
}

void ViewOpsDialog::sendClearCache() {
	emit clearCache();
}

void ViewOpsDialog::slotAddCatalog() {
	AddCatDialog ac(this);
	if ( ac.exec()==QDialog::Accepted ) {
		//compute Horizontal coords for custom objects:
		for ( unsigned int i=0; i < ac.objectList().count(); ++i )
			ac.objectList().at(i)->EquatorialToHorizontal( ksw->LSTh(), ksw->geo()->lat() );

		//Add new custom catalog, based on the list of SkyObjects we just parsed
		ksw->data()->addCatalog( ac.name(), ac.objectList() );
		QCheckListItem *newCat = new QCheckListItem( CatalogList, ac.name(), QCheckListItem::CheckBox );
		newCat->setOn( true );
		CatalogList->insertItem( newCat );

		ksw->options()->CatalogCount = ++ksw->options()->CatalogCount;
		ksw->options()->CatalogName.append( ac.name() );
		ksw->options()->CatalogFile.append( ac.filename() );
		ksw->options()->drawCatalog.append( true );
//		kdWarning() << "CatalogCount: " << ksw->options()->CatalogCount << endl;
//		kdWarning() << "CatalogName.count(): " << ksw->options()->CatalogName.count() << endl;
		ksw->map()->Update();
	}
}

void ViewOpsDialog::slotRemoveCatalog() {
//Remove CatalogName, CatalogFile, and drawCatalog entries, and decrement CatalogCount
	for ( unsigned int i=0; i < ksw->options()->CatalogName.count(); ++i ) {
		if ( CatalogList->currentItem()->text( 0 ) == ksw->options()->CatalogName[i] ) {
			ksw->options()->CatalogName.remove( ksw->options()->CatalogName.at(i) );
			ksw->options()->CatalogFile.remove( ksw->options()->CatalogFile.at(i) );
			ksw->options()->drawCatalog.remove( ksw->options()->drawCatalog.at(i) );
			--ksw->options()->CatalogCount;
			break;
		}
	}

//Remove entry in the QListView
//	int i = CatalogList->itemPos( CatalogList->currentItem() );
	CatalogList->takeItem( CatalogList->currentItem() );
}

void ViewOpsDialog::selectCatalog() {
//If selected item is a custom catalog, enable the remove button (otherwise, disable it)
	RemoveCatalog->setEnabled( false );
	for ( unsigned int i=0; i < ksw->options()->CatalogName.count(); ++i ) {
		if ( CatalogList->currentItem()->text( 0 ) == ksw->options()->CatalogName[i] ) {
			RemoveCatalog->setEnabled( true );
			break;
		}
	}
}

void ViewOpsDialog::changeSlewTimeScale( float f ) {
	if ( f < 0.0 ) {
		f = 0.0;
		hideSpinBox->tsbox()->changeScale( 0.0 );
	}
	ksw->options()->slewTimeScale = f;
}
