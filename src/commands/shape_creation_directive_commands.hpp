#ifndef shapecreationdirective_commands_hpp
#define shapecreationdirective_commands_hpp

#include "direct_command_base.hpp"
#include "undo_manager.hpp"

#include "../core/shape_creator.hpp"
#include "../core/runtime_environment.hpp"

#include "../io/messenger.hpp"
#include "../core/rq/RegionQueryService.hpp"

#include <QFile>
#include <QTextStream>
#include <QString>

#include <string>



template <ObjectType T>
class dicmdCreateObj : public TransactionalDirectCommandBase
{

        IShape* m_shape;    
		IShape* m_executed_object;
        IObjectPoolPtr m_ws;
public:
        dicmdCreateObj<T>(IObjectPoolPtr s): m_ws(s) { //rq(RegionQuery::getInstance()) {
                add_option("-points",new PointListCommandOptionValue());
                add_option("-color",new StringCommandOptionValue("#000000"));
                add_option("-brush",new IntCommandOptionValue(0));
                add_option("-fill",new IntCommandOptionValue(0));
        }

       	dicmdCreateObj<T>(const std::vector<PointCommandOptionValue>& pl, const ShapeProperties& pr, IObjectPoolPtr s): m_ws(s) {
                //m_pr = pr;
                //std::to_string(pr.toStringsMap()["color"])
                add_option("-points",new PointListCommandOptionValue(pl));
                add_option("-color",new StringCommandOptionValue(pr.brush_color.name().toStdString()));
                add_option("-brush",new IntCommandOptionValue(pr.toStringsMap()["brush"]));
                add_option("-fill",new IntCommandOptionValue(pr.toStringsMap()["fill"]));
        }
        
	    void dump(const std::string& f) {
                QFile* m_cmdfile = new QFile(f.c_str());
                m_cmdfile->open( QIODevice::WriteOnly | QIODevice::Append ); 
                QTextStream* cmd_stream = new QTextStream(m_cmdfile);
                //(*cmd_stream) << (GET_CMD_ARG(PointListCommandOptionValue,"-points").to_string()+"\n").c_str();
                (*cmd_stream) << (get_cmdname_and_stringified_opts()+"\n").c_str();
                m_cmdfile->flush();
                m_cmdfile->close();
        }
        
	   virtual void execute() {
                RegionQuery& rq = RegionQuery::getInstance();
                //* //std::vector<QPoint> v(GET_CMD_ARG(PointListCommandOptionValue,"-points"));
                m_shape = ShapeCreator::getInstance().create(T);
                for( auto it: PL_ARG("-points") )
                    m_shape->addPoint(it.get());

                ShapeProperties pr;
                pr.fromString(S_ARG("-color"),I_ARG("-brush"),I_ARG("-fill"));
                m_shape->updateProperties(pr);
                m_executed_object = m_ws->addObject(m_shape);
                m_executed_object->updateProperties(pr);
                rq.insertObject(m_executed_object);
                /**/
        }
        virtual std::string get_name() {
                return "dicmdCreateObj"+ObjType2String(T);
        }
       
        void undo() override
        {
                // temp
                m_ws->removeObject(m_executed_object);
        }

        void redo() override
        {
                execute();
        }
};


#endif
