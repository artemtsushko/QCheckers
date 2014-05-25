#include "checkerspicture.h"

CheckersPicture::CheckersPicture(QWidget *parent) : QWidget(parent)
{
    QPalette palette;
    palette.setColor(QPalette::Light,QColor(Qt::gray));
    setPalette(palette);
    setAutoFillBackground(true);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setAttribute(Qt::WA_StaticContents);

    zoom = 32;
    curstate = NULL;
    n = 8;
    setMinimumSize(zoom*(n+1),zoom*(n+1));
}

CheckersPicture::~CheckersPicture()
{
}

CheckersState * CheckersPicture::state() {
	return curstate;
}

void CheckersPicture::setComputerColor(uint8 color) {
	this->color = color;
}

void CheckersPicture::setState(CheckersState * state) {
    if (state) {
		curstate = state;
		n = state->size();
		v.clear();
		update();
	} else {
		clear();
	}
}

void CheckersPicture::setSize(int n) {
	this->n = n;
	update();
}

void CheckersPicture::deleteVector() {
    if (!v.empty()) {
		v.clear();
		update();
	}
}

void CheckersPicture::setVector(std::vector <point> & v) {
	this->v = v;
	update();
}

void CheckersPicture::clear() {
	curstate = NULL;
	v.clear();
	update();
}

void CheckersPicture::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() && Qt::LeftButton && curstate) {
		qreal i = (event->pos().x() - p.x() + side/(2*n+2))*(n+1)/side - 1.0;
		qreal j = (double)n - (event->pos().y() - p.y() + side/(2*n+2))*(n+1)/side;
        if (color == BLACK)
			emit mouseClicked((int)i,(int)j);
		else
            emit mouseClicked(n - 1 - (int)i, n - 1 - (int)j);
	}
}

void CheckersPicture::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setViewport(p.x(),p.y(),side,side);
    painter.setWindow(0, 0, zoom*(n+1.0), zoom*(n+1.0));

    QColor border(109, 80, 50);

    painter.fillRect(QRect(0,0,zoom*(n+1.0),zoom*1.0), border);
    painter.fillRect(QRect(0,zoom*(n+0.5),zoom*(n+1.0),zoom*0.5), border);
    painter.fillRect(QRect(0,0,zoom*(0.5),zoom*(n+1.0)), border);
    painter.fillRect(QRect(zoom*(n+0.5),0,zoom*0.5,zoom*(n+1.0)), border);

    QColor dark(139, 115, 85);
    QColor light(255, 231, 186);

    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            QRect rect = pixelRect(i, j);
            if( !((i+j%2)%2) ) {
                    painter.fillRect(rect, dark);
            } else {
                painter.fillRect(rect, light);
            }
        }
    }

	int ix,jx;
    QColor endColor(110, 139, 61);
    QColor startColor(109, 80, 50);
    QColor capturedColor(178, 34, 34);
    QColor normalColor(162, 205, 90);
    if (v.size()) {
		int type;
        for (unsigned i = 0; i < v.size(); i++) {
            color == WHITE ? jx = n-1-v.at(i).y : jx = v.at(i).y;
            color == WHITE ? ix = n-1-v.at(i).x : ix = v.at(i).x;
			QRect rect = pixelRect(ix, jx);
			type = v.at(i).type;
            if (type == MOVEDFROM)
				painter.fillRect(rect, startColor);
            else if (type == MOVEDTO || type == TOKING)
				painter.fillRect(rect, endColor);
            else if (type == MOVEDTHROUGH)
				painter.fillRect(rect, normalColor);
            else if (type == DELETED)
				painter.fillRect(rect, capturedColor);
		}
	}

    int s = zoom * 0.4;
    int sd = zoom * 0.2;
    if (curstate) {
        painter.setPen(QPen(Qt::black,zoom*0.05));
		painter.setBrush(QBrush(Qt::white));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                color == WHITE ? jx = j+1 : jx = n-j;
                color == WHITE ? ix = n-i : ix = i+1;
                if (curstate->at(i,j) == WHITE)
					painter.drawEllipse(QPoint(zoom*(ix),zoom*(jx)),s,s);
                if (curstate->at(i,j) == WHITEKING) {
					painter.drawEllipse(QPoint(zoom*(ix),zoom*(jx)),s,s);
					painter.drawEllipse(QPoint(zoom*(ix),zoom*(jx)),sd,sd);
				}
			}
		}
		painter.setBrush(QBrush(Qt::black));
        painter.setPen(QPen(Qt::white,zoom*0.05));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                color == WHITE ? jx = j+1 : jx = n-j;
                color == WHITE ? ix = n-i : ix = i+1;
                if (curstate->at(i,j) == BLACK)
					painter.drawEllipse(QPoint(zoom*(ix),zoom*(jx)),s,s);
                if (curstate->at(i,j) == BLACKKING) {
					painter.drawEllipse(QPoint(zoom*(ix),zoom*(jx)),s,s);
					painter.drawEllipse(QPoint(zoom*(ix),zoom*(jx)),sd,sd);
				}
			}
		}
	}
}

void CheckersPicture::resizeEvent (QResizeEvent * event) {
    if (event->oldSize() != event->size()) {
		update();
		side = qMin(width(), height());
		p = QPoint((width() - side) / 2, (height() - side) / 2);
	} else {
		event->ignore();
	}
}

QRect CheckersPicture::pixelRect(int i, int j) const
{
	return QRect(zoom * i + zoom*0.5, zoom*(n-0.5) - zoom * j, zoom, zoom);
}


