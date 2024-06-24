#include <QWidget>
#include <QMouseEvent>
#include <QTimer>
#include <time.h>
#include <iostream>
#ifndef __COLOR_WID_H__
#define __COLOR_WID_H__

typedef struct {
    int ii;
    int jj;
} TPos;

typedef struct {
    int color;
    int size;
} TElement;

class ColorWid : public QWidget
{
    Q_OBJECT
    public:
	ColorWid(QWidget* parent=0) : QWidget(parent)
	{
	    field_width = 360;
	    mouse_move_sel = false;
	    setMouseTracking(mouse_move_sel);
	    srand(time(NULL));
	    setFixedSize(field_width,field_width);
	    tmr = new QTimer;
	    connect(tmr, SIGNAL(timeout()), this, SLOT(timeOut()));
	    tmr->start(100);
	    initParams();
	    newGame();
	}
	
	void newGame()
	{
	    free_cells = cell_number;
	    score = 0;
	    game_over=false;
	    selected = false;
	    mcount = 0;
	    for (int i=0;i<cell_number;i++) {
		field[i].color=0;
		field[i].size =0;
	    }
	    memset(addballs,0,sizeof(addballs));
	    mpos.ii = mpos.jj = -1;
	    memset(&opos,0,sizeof(opos));
	    randBalls();
	    addBalls();
	    randBalls();
	    drawTitle();
	    repaint();
	    return;
	}
	
	void initParams()
	{
	    field_size = 9;
	    diff_balls = 7;
	    balls_appear = 3;
	    destr_min = 5;
	    animate_ball = -1;
	    cell_width = field_width/field_size;
	    cell_number = field_size*field_size;
	    ball_width = cell_width-8;
	    field = new TElement [cell_number];
	    return;
	}
	
	void drawTitle();
	void showHighScores();
	
	void clickAction(TPos pos);
	void randBalls() {
	    for (int c=0;c<3;c++) {
	    addballs[c]=rand()%diff_balls+1;
	    }
	    return;
	}
	
	void addBalls() {
	    int pos,i,j,c;
	    for (c=0;c<3;c++) {
		if (!free_cells) {
		    game_over=true;
		    return;
		}
		pos = rand()%free_cells;
		j=0;
		for (i=0;i<cell_number;i++)
		    if ((field[i].color==0)&&(j==pos)) {
			field[i].color=addballs[c];
			field[i].size=ball_width;
			free_cells--;
			break;
		    } else {
			if (field[i].color==0) j++;
		    }
	    }
	    if (!free_cells)
		game_over=true;
	    return;
	}
	
	void cleanMinus()
	{
	    for (int i=0;i<cell_number;i++)
		if (field[i].color<0) field[i].color=0;
	    return;
	}
	
	bool isInside(int cur_i, int cur_j)
	{
	    if ((cur_i>=0)&&(cur_i<field_size)&&(cur_j>=0)&&(cur_j<field_size)) return true;
	    return false;
	}
	
	bool isFree(int cur_i, int cur_j)
	{
	    if (field[cur_i*field_size+cur_j].color==0) return true;
	    return false;
	}
	
	bool findPath(int si, int sj, int fi, int fj);
	void getPath(int fi,int fj,int si,int sj);
	bool destroyLines();
    protected:
	TPos mpos,opos;
	int mcount;
	int field_width,cell_width;
	int diff_balls;
	int balls_appear;
	int ball_width;
	int field_size;
	int cell_number;
	int free_cells;
	int destr_min;
	int animate_ball;
	TElement mem;
	int score;
	int addballs[3];
	bool game_over;
	TPos select_pos;
	bool mouse_move_sel;
	QVector<TPos> path;
	QTimer *tmr;
	bool selected;
	TElement *field;
	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void wheelEvent(QWheelEvent *ev);
	void keyPressEvent(QKeyEvent *ev);
	void paintEvent(QPaintEvent *ev);
    protected slots:
	void timeOut();
	private:
	bool deleteCells(int *field_delete, QVector<TPos> &count_vect);
};
#endif
