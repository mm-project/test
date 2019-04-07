#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

///////////////////////////////////////////////////////////////////////////////
//
// Includes
//
// Qt
#include <QMainWindow>

class canvas;
class create_shape_gui;
class pen_brush_gui;

///////////////////////////////////////////////////////////////////////////////
//
// @class mainWindow 
//
class main_window : public QMainWindow
{
	Q_OBJECT

public:
	main_window(QWidget* = 0 );
	bool eventFilter(QObject *object, QEvent *event);

private:
	void make_connections();
	void init_menu();

private:
	canvas* m_canvas;
	create_shape_gui* m_shapes;
	pen_brush_gui* m_pen_brush;
};

#endif
