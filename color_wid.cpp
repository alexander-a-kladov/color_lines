#include "color_wid.h"
#include <QPainter>
#include <QGradient>
#include <QIcon>
#include <iostream>

QColor color[7]={Qt::white,Qt::blue,Qt::yellow,Qt::red,Qt::magenta,Qt::cyan,Qt::green};

void ColorWid::timeOut()
{
    for (int i=0;i<cell_number;i++) {
	if (field[i].size!=ball_width) {
	    field[i].size=field[i].size/2;
	    if (!field[i].size) field[i].color=0;
	}
    }
    
    if (mouse_move_sel&&(animate_ball==-1)) {
    if (memcmp(&mpos,&opos,sizeof(mpos))==0) {
	if (mcount<7) mcount++;
	if (mcount==7) {clickAction(opos);mcount++;}
    }
    else {
	opos = mpos;
	mcount = 0;
    }
    }
    
    if (animate_ball==0) {
    	    field[path[0].ii*field_size+path[0].jj]=mem;
	    
	    if (!destroyLines()) {addBalls();randBalls();destroyLines();}
	    if (game_over) {
	    }
	    drawTitle();
	    animate_ball = -1;
    }
    if (animate_ball>0) {
	animate_ball--;
    }
    repaint();
    return;
}

void ColorWid::mousePressEvent(QMouseEvent *ev)
{
    TPos pos;
    pos.ii = ev->y()/cell_width;
    pos.jj = ev->x()/cell_width;    
    clickAction(pos);
    return;
}

void ColorWid::clickAction(TPos pos)
{
    if (animate_ball>=0) return;

    if (game_over) {
	newGame();
	repaint();
	return;
    }
    
    if (isFree(pos.ii,pos.jj)&&selected)  {
	bool res;
	res = findPath(select_pos.ii,select_pos.jj,pos.ii,pos.jj);
	if (res) getPath(pos.ii,pos.jj,select_pos.ii,select_pos.jj);
	else path.resize(0);
	cleanMinus();
	if (res) {
	    selected=false;
	    animate_ball=path.size()-1;
	}
	else field[select_pos.ii*field_size+select_pos.jj]=mem;
    } else {
	if (!isFree(pos.ii,pos.jj)) {
	selected = true;
	select_pos.ii = pos.ii;
	select_pos.jj = pos.jj;
	}
    }
    repaint();
    return;
}

void ColorWid::mouseMoveEvent(QMouseEvent *ev)
{
    mpos.ii = ev->y()/cell_width;
    mpos.jj = ev->x()/cell_width;
    return;
}

void ColorWid::wheelEvent(QWheelEvent *ev)
{
    TPos pos;
    pos.ii = ev->y()/cell_width;
    pos.jj = ev->x()/cell_width;    
    clickAction(pos);
    return;
}

void ColorWid::drawTitle()
{
    QString qs;
    qs=QString("CL(S) Score:%1 Free:%2").arg(score).arg(free_cells);
    if (mouse_move_sel) qs+=QString(" St");
    setWindowTitle(qs);
    QPixmap pix(16,16);
    pix.fill(Qt::gray);
    QPainter p(&pix);
    for (int i=0;i<3;i++) {
    p.setPen(color[addballs[i]-1]);
    p.setBrush(color[addballs[i]-1]);
    p.drawEllipse(i*5,2,5,12);
    }
    p.end();
    setWindowIcon(QIcon(pix));
    return;
}

void ColorWid::keyPressEvent(QKeyEvent *ev)
{
    switch(ev->key()) {
	case Qt::Key_S:
	    mouse_move_sel = !mouse_move_sel;
	    setMouseTracking(mouse_move_sel);
	    drawTitle();
	    mcount = 0;
	    break;
    }
    return;
}

void ColorWid::paintEvent(QPaintEvent *ev)
{
    QPainter p(this);
    int i,j;
    
    static int selected_animation=0;
    int offset_y;
    
    p.setRenderHint(QPainter::Antialiasing);
    
    selected_animation++;
    selected_animation%=5;
    
    p.save();
    p.setBrush(QColor("white"));
    p.drawRect(0,0,width(),height());
    p.restore();
    
    
    p.save();
    p.setPen(Qt::gray);
    for (i=0;i<field_size;i++)
    for (j=0;j<field_size;j++)
	p.drawRect(j*cell_width,i*cell_width,cell_width,cell_width);
    p.restore();
    
    p.save();
    p.setPen(Qt::NoPen);
    for (i=0;i<cell_number;i++) {
	if (field[i].color>0) {
	if ((selected)&&(select_pos.jj==(i%field_size))&&(select_pos.ii==(i/field_size))) offset_y=selected_animation; else offset_y=0;
	QRadialGradient radialGrad(QPointF(cell_width*(i%field_size)+0.4*cell_width,
					    cell_width*(i/field_size)+0.4*cell_width-offset_y),
					    0.8*cell_width);
    radialGrad.setColorAt(0, Qt::white);
    radialGrad.setColorAt(0.1, color[field[i].color-1]);
    radialGrad.setColorAt(0.9, Qt::black);
	p.setBrush(QBrush(radialGrad));
	p.drawEllipse(cell_width*(i%field_size)+(cell_width-field[i].size)/2,
			cell_width*(i/field_size)+(cell_width-field[i].size)/2-offset_y,
			field[i].size, field[i].size);
	}
    }
    p.restore();
    p.save();
    if (game_over) {
	p.setPen(Qt::black);
	p.setFont(QFont("Arial",48));
	p.drawText(width()/2-QFontMetrics(p.font()).width("Game Over")/2,height()/2,"Game Over");
	p.setPen(Qt::white);
	p.drawText(width()/2-QFontMetrics(p.font()).width("Game Over")/2-4,height()/2-4,"Game Over");
    }
    p.restore();
    p.save();
    p.setPen(Qt::NoPen);
    if (animate_ball>=0) {
		QRadialGradient radialGrad(QPointF(cell_width*(path[animate_ball].jj)+0.4*cell_width,
						    cell_width*(path[animate_ball].ii)+0.4*cell_width),
						    0.8*cell_width);
    radialGrad.setColorAt(0, Qt::white);
    radialGrad.setColorAt(0.1, color[mem.color-1]);
    radialGrad.setColorAt(0.9, Qt::black);
	p.setBrush(QBrush(radialGrad));
	p.drawEllipse(cell_width*(path[animate_ball].jj)+(cell_width-ball_width)/2,
			cell_width*(path[animate_ball].ii)+(cell_width-ball_width)/2,
			ball_width, ball_width);
    }
    p.restore();
    if ((mcount)&&(mcount<=7))
	p.drawRect(cell_width*opos.jj+mcount,cell_width*opos.ii+mcount,cell_width-2*mcount,cell_width-2*mcount);
    return;
}

bool ColorWid::findPath(int si, int sj, int fi, int fj)
{
	bool res;
	TPos pos,pos1;
	int index;
	TPos sdvig[4]={{-1,0},{0,1},{1,0},{0,-1}};
	QVector<TPos> surf,surf1;
	pos.ii = si;
	pos.jj = sj;
	mem = field[si*field_size+sj];
	field[si*field_size+sj].color=-1;
	surf1.append(pos);
	while (1) {
		surf=surf1;
		surf1.resize(0);
		if (surf.count()==0) return false;
		for (int i=0;i<surf.count();i++) {
		for (int j=0;j<sizeof(sdvig)/sizeof(TPos);j++) {
			pos.ii = surf[i].ii+sdvig[j].ii;
			pos.jj = surf[i].jj+sdvig[j].jj;
			index = pos.ii*field_size+pos.jj;
			if (isInside(pos.ii,pos.jj)&&isFree(pos.ii,pos.jj)) {
			    field[index].color=field[surf[i].ii*field_size+surf[i].jj].color-1;
			    surf1.append(pos);
			    if ((pos.ii==fi)&&(pos.jj==fj)) return true;
			}
		    }
		}
	}
    return false;
}

void ColorWid::getPath(int fi, int fj, int si, int sj)
{
    TPos sdvig[4]={{-1,0},{0,1},{1,0},{0,-1}};
    TPos pos,pos1,pos2;
    int index;
    int i;
    pos1.ii = fi;
    pos1.jj = fj;
    path.resize(0);
    path.append(pos1);
    while (1) {
    pos.ii = pos1.ii;
    pos.jj = pos1.jj;
    for (i=0;i<sizeof(sdvig)/sizeof(TPos);i++) {
	pos1.ii = pos.ii + sdvig[i].ii;
	pos1.jj = pos.jj + sdvig[i].jj;
	index = pos1.ii*field_size+pos1.jj;
	if (isInside(pos1.ii,pos1.jj)) {
	    if ((field[index].color-field[pos.ii*field_size+pos.jj].color)==1) {
		break;
	    }
	}
    }
    if (i!=sizeof(sdvig)/sizeof(TPos))  {
	path.append(pos1);
	if ((pos1.ii==si)&&(pos1.jj==sj)) return;
    } else return;
    }
    return;
}

bool ColorWid::deleteCells(int *field_delete, QVector<TPos> &count_vect) {
	if (count_vect.size()>=destr_min) {
		for (int i=0; i<count_vect.size(); ++i) {
			field_delete[count_vect[i].ii*field_size+count_vect[i].jj]=1;
		}
		return true;
	}
	return false;
}

bool ColorWid::destroyLines()
{
    bool res=false;
    QVector<TPos> delete_vect, count_vect;
    TElement mem;
    int scan_dirs[6][3][2]={{{0,0},{1,0},{0,1}},
    				{{0,0},{0,1},{1,0}},
    				{{0,0},{1,-1},{0,1}},
    				{{1,field_size-1},{1,-1},{1,0}},
    				{{field_size-1,0},{1,1},{-1,0}},
    				{{0,1},{1,1},{0,1}}};
    int field_delete[field_size*field_size];
    
    memset(field_delete,0,sizeof(field_delete));
    
    for (int c=0;c<6;++c) {
    int* first = scan_dirs[c][0];
    int* step_j = scan_dirs[c][1];
    int* step_i = scan_dirs[c][2];
    int i=first[1],j=first[0];

    TPos pos0;
    while (isInside(i,j)) {
    pos0.ii = i; pos0.jj = j;
    mem = field[i*field_size+j];
    count_vect.clear();
    while (isInside(i,j)) {
    TPos pos = {i,j};
	if (field[i*field_size+j].color == mem.color) count_vect.push_back(pos);
	else {
		if (mem.color>0) {
			res |= deleteCells(field_delete, count_vect);
		}
		count_vect.clear();
		TPos pos = {i,j};
		mem = field[i*field_size+j];count_vect.push_back(pos);
	    }
	    j += step_j[0];i += step_j[1];
    }
    	if (mem.color>0) {
    		res |= deleteCells(field_delete, count_vect);
    	}
    	i = pos0.ii;j = pos0.jj;
    	j += step_i[0];i += step_i[1];
    }
	}

    int number_of_freed = 0;
	if (res) {
	    for (int i=0;i<cell_number;i++)
		if (field_delete[i]) {
		    field[i].size=ball_width-1;
		    free_cells++;
		    number_of_freed++;
		}
	}
    
    if (number_of_freed>destr_min) {
    	score += 2*number_of_freed;
    } else {
    	score += number_of_freed;
    }
    if (res) game_over = false;
    return res;
}

