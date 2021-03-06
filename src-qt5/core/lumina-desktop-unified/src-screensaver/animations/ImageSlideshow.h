//===========================================
//  Lumina-DE source code
//  Copyright (c) 2015, Ken Moore
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef _LUMINA_DESKTOP_SCREEN_SAVER_IMAGESLIDESHOW_ANIMATION_H
#define _LUMINA_DESKTOP_SCREEN_SAVER_IMAGESLIDESHOW_ANIMATION_H

#include "global-includes.h"
#include "BaseAnimGroup.h"

class ImageSlideshow: public QParallelAnimationGroup{
	Q_OBJECT
private:
	QLabel *image;
	QPropertyAnimation *bounce, *fading;
	QPixmap pixmap;
	QStringList imageFiles;
	QString imagePath;
	QSize screenSize;
	bool animate;

private:
	void setupAnimation() {
	  //Choose between starting from top or bottom at random
	  if(qrand() % 2) {
		  bounce->setKeyValueAt(0, QPoint(0,screenSize.height()-image->height()));
		  bounce->setKeyValueAt(0.25, QPoint((screenSize.width()-image->width())/2,0));
		  bounce->setKeyValueAt(0.5, QPoint(screenSize.width()-image->width(),screenSize.height()-image->height()));
		  bounce->setKeyValueAt(0.75, QPoint((screenSize.width()-image->width())/2,0));
		  bounce->setKeyValueAt(1, QPoint(0,screenSize.height()-image->height()));
	  }else{
		  bounce->setKeyValueAt(0, QPoint(0,0));
		  bounce->setKeyValueAt(0.25, QPoint((screenSize.width()-image->width())/2,screenSize.height()-image->height()));
		  bounce->setKeyValueAt(0.5, QPoint(screenSize.width()-image->width(),0));
		  bounce->setKeyValueAt(0.75, QPoint((screenSize.width()-image->width())/2,screenSize.height()-image->height()));
		  bounce->setKeyValueAt(1, QPoint(0,0));
	  }
	}

	void chooseImage() {
		QString randomFile = imagePath+imageFiles[qrand() % imageFiles.size()];

		//Choose a new file if the chosen one is not an image
	 	while(QImageReader::imageFormat(randomFile).isEmpty())
		  randomFile = imagePath+imageFiles[qrand() % imageFiles.size()];
		pixmap.load(imagePath+imageFiles[qrand() % imageFiles.size()]);

	    //If the image is larger than the screen, then shrink the image down to 3/4 it's size (so there's still some bounce)
		//Scale the pixmap to keep the aspect ratio instead of resizing the label itself
	    if(pixmap.width() > screenSize.width() or pixmap.height() > screenSize.height())
		  pixmap = pixmap.scaled(screenSize*(3.0/4.0), Qt::KeepAspectRatio);

		//Set pixmap to the image label
		image->setPixmap(pixmap);
	    image->resize(pixmap.size());
	}

private slots:
	void LoopChanged(){
	  //Load a new random image. Resize the label based on the image's size
	  chooseImage();
	  setupAnimation();
	}
	void stopped(){ qDebug() << "Image Stopped"; image->hide();}

public:
	ImageSlideshow(QWidget *parent, QString path, bool animate) : QParallelAnimationGroup(parent){
	  imagePath = path;
	  image = new QLabel(parent);
	  screenSize = parent->size();
	  this->animate = animate;
	  
	  //Generate the list of files in the directory
	  imageFiles = QDir(imagePath).entryList(QDir::Files);
	  if(imageFiles.empty())
		  qDebug() << "Current image file path has no files.";

	  //Change some default settings for the image. If scaledContents is false, the image will be cut off if resized
	  image->setScaledContents(true);
	  image->setAlignment(Qt::AlignHCenter);

	  //Load a random initial image
	  chooseImage();

	  //Create the animation that moves the image across the screen
	  bounce = new QPropertyAnimation(image, "pos", parent);

	  //Add the animation that fades the image in and out
	  QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(parent);
	  image->setGraphicsEffect(eff);
	  fading = new QPropertyAnimation(eff,"opacity");
	  fading->setKeyValueAt(0, 0);
	  fading->setKeyValueAt(0.20, 1);
	  fading->setKeyValueAt(0.80, 1);
	  fading->setKeyValueAt(1, 0);
	  this->addAnimation(fading);

	  setupAnimation();
	  image->show();
	  //Only add the animation if set in the configuration file
	  if(animate)
		  this->addAnimation(bounce);
	  else
		  //If no animation, center the image in the middle of the screen
		  image->move(QPoint((parent->width()-image->width())/2,(parent->height()-image->height())/2));

	  //Loop through 30 times for a total for 4 minutes
	  this->setLoopCount(30);
	  bounce->setDuration(8000);
	  fading->setDuration(8000);

	  connect(this, SIGNAL(currentLoopChanged(int)), this, SLOT(LoopChanged()) );
	  connect(this, SIGNAL(finished()), this, SLOT(stopped()) );
	}
	~ImageSlideshow(){}

};

class ImageAnimation: public BaseAnimGroup{
	Q_OBJECT
public:
	ImageAnimation(QWidget *parent, QSettings *set) : BaseAnimGroup(parent, set){}
	~ImageAnimation(){
	  this->stop();
	}

	void LoadAnimations(){
	  canvas->setStyleSheet("background: black;");
	  //Load the path of the images from the configuration file (default /usr/local/backgrounds/)
	  QString imagePath = settings->value("imageSlideshow/path","/usr/local/backgrounds/").toString();
	  //Load whether to animate the image (default true)
	  bool animate = settings->value("imageSlideshow/animate", true).toBool();
	  ImageSlideshow *tmp = new ImageSlideshow(canvas, imagePath, animate);
	  this->addAnimation(tmp);
	}

};
#endif
