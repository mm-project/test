#include "canvas.hpp"

#include "direct_command_base.hpp"
#include "basic_commands.hpp"
#include "shape_creator_commands.hpp"
#include "command_manager.hpp"
#include "controller.hpp"
#include "shapes.hpp"
#include "working_set.hpp"
#include "runtime_environment.hpp"

#include <QRect>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QMouseEvent>

#include <cassert>
#include <iostream>

canvas::canvas(QWidget* p)
        : QWidget(p), is_runtime_mode(false)
{
        setFocusPolicy(Qt::StrongFocus);
        setMouseTracking(true);
        setObjectName("CANVAS");
        
        m_working_set = new working_set;
        m_runtime_environment = new runtime_environment();
        m_renderer = new renderer(this);
        cm = command_manager::get_instance();
        cm->init2(m_runtime_environment,m_working_set);
        cm->init();

}

void canvas::keyPressEvent(QKeyEvent*) {
    //FIXME some interface needed
    //assert(0);

    if( cm->is_idle() ) 
        return;

    cm->key_pressed();
}

void canvas::mousePressEvent(QMouseEvent* e)
{
    //cm->activate_command(INCMD_CREATE_OBJ(RECT));
    
    //FIXME new? how is deleting
    DirectCommandBase* cmd = new dicmdCanvasAddPoint();
    cmd->add_arg(new PointCommandOption(e->pos()));
    cmd->execute_and_log();
    
    if( cm->is_idle() ) 
        return;

    QPoint p(e->pos());
    cm->mouse_clicked(p.x(),p.y());

}

//FIXME not needed anymore
void canvas::current_type_changed()
{
    //not needed
    //controller* c = controller::get_instance();
    //m_runtime_environment->change_object_type(c->get_object_type());
}

void canvas::mouseMoveEvent(QMouseEvent* e)
{
    if( cm->is_idle() ) 
        return;
    
    cm->mouse_moved(e->pos().x(),e->pos().y());
    update();
}

void canvas::wheelEvent(QWheelEvent*)
{
    m_renderer->incr_zoom_factor();
    update();
}

void canvas::mouseDoubleClickEvent(QMouseEvent* e)
{
    cm->mouse_dbl_clicked(e->pos().x(),e->pos().y());
}

void canvas::on_update()
{
    cm->update();
}

void canvas::paintEvent(QPaintEvent*)
{
    auto painter = m_renderer->get_painter();
    m_renderer->start();
    QRect rect(QPoint(0, 0), QSize(1000,1000));
    QBrush b(Qt::black);
    painter->setBrush(b);
    painter->drawRect(rect);
    std::vector<IBasicShape*> shapes = m_working_set->get_objects();
    for (auto i = shapes.begin(); i != shapes.end(); ++i) {
                    (*i)->draw(painter);
    }
    m_runtime_environment->draw_runtime(painter);
    m_renderer->stop();
}

//FIXME ( may be other more nicer way?)
//FIXME use register_command instead
#define INCMD_CREATE_OBJ(S) new incmdCreateObj<S>(m_runtime_environment,m_working_set)
void canvas::create_line()
{
    //cm->activate_command("cmdCreateNthgon<2>");
    cm->activate_command(INCMD_CREATE_OBJ(LINE));
    //cm->activate_command(new incmdCreateNthgon<2>(m_runtime_environment,m_working_set));
}

void canvas::create_rect()
{
    //cm->activate_command(new incmdCreateNthgon<3>(m_runtime_environment,m_working_set));
    cm->activate_command(INCMD_CREATE_OBJ(RECT));
}

void canvas::create_ellipse()
{
    cm->activate_command(INCMD_CREATE_OBJ(ELLIPSE));
}

void canvas::create_polygon()
{
    cm->activate_command(INCMD_CREATE_OBJ(POLYGON));
}

void canvas::reset()
{
    m_working_set->clear();
    update();
}


