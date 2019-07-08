#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "../core/basic_shape.hpp"

class controller
{
public:
        static controller* get_instance();

        void change_object_type(ObjectType);
        void change_pen_width(int);
        void change_pen_color(QColor);
        void change_brush_color(QColor);
        void change_pen_style(Qt::PenStyle);
        void change_pen_cap_style(Qt::PenCapStyle);
        void change_pen_join_style(Qt::PenJoinStyle);
        void change_brush_style(Qt::BrushStyle);

        ShapeProperties get_shape_properties() const {return properties;}
        ObjectType get_object_type() const;
        int get_pen_width() const;
        QColor get_pen_color() const;
        QColor get_brush_color() const;
        Qt::PenStyle get_pen_style() const;
        Qt::PenCapStyle get_pen_cap_style() const;
        Qt::PenJoinStyle get_pen_join_style() const;
        Qt::BrushStyle get_brush_style() const;

private:
        controller();
        controller(const controller&) {}

private:
        static controller* m_instance;

        ObjectType type	= LINE;
        ShapeProperties properties;
};

#endif
