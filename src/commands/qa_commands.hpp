#ifndef qa_commands_hpp
#define qa_commands_hpp

#include "command_manager.hpp"
#include "direct_command_base.hpp"
#include "gui_commands.hpp"

#include "../io/messenger.hpp"
#include "../core/selection.hpp"
#include "../core/postman.hpp"
#include "../core/application.hpp"

#include <QApplication>
#include <QPixmap>
#include <QWidget>
#include <QImage>


#include <sstream>
#include <fstream>
/*
#include <filesystem>
namespace fs = std::filesystem;
*/

enum qaCompType { DESIGN, SELECTION, CANVAS, RUNTIME, SELECTIONCANVAS };

namespace {
       
    bool are_textfiles_different(const QString &file1, const QString &file2) 
    {
            QFile data1(file1);
            if (!data1.open(QIODevice::ReadOnly | QIODevice::Text)){
                return true;
            }

            QFile data2(file2);
            if (!data2.open(QIODevice::ReadOnly | QIODevice::Text)){
                return true;
            }

            QTextStream in1(&data1), in2(&data2);
            while ( !in1.atEnd() && !in2.atEnd() ) {
                QString num1 = in1.readLine();
                QString num2 = in2.readLine();
                if ( num1 != num2  )
                    return true;
            }
            
            return false;
    }

   
    bool are_imagefiles_different(const QString &file1, const QString &file2) {
            QImage img1(file1);
            QImage img2(file2);

            // Check size.
            if (img1.size() != img2.size()) {
                //qDebug("Different size - %dx%d vs %dx%d.", img1.width(), img1.height(), img2.width(), img2.height());
                return true;
            }

            int w = img1.width();
            int h = img1.height();
            for (int ii = 0; ii < w; ii++) {
                for (int jj = 0; jj < h; jj++) {
                    const QRgb px1 = img1.pixel(ii, jj);
                    const QRgb px2 = img2.pixel(ii, jj);
                    if (px1 != px2) {
                        //qDebug("Pixel (%d,%d) differs - (%d,%d,%d,%d) vs (%d,%d,%d,%d).\n",
                         //   ii, jj,
                          //  qAlpha(px1), qRed(px1), qGreen(px1), qBlue(px1),
                          //`  qAlpha(px2), qRed(px2), qGreen(px2), qBlue(px2));
                        return true;
                    }
                }
            }

            return false;
        }
     
    bool are_two_files_different(qaCompType type, const QString &file1, const QString &file2) 
    {
            if ( type == CANVAS ) 
                return are_imagefiles_different(file1,file2);
            return are_textfiles_different(file1,file2);
    }
    
    
}



class dicmdQaReplyStep: public NonTransactionalDirectCommandBase
{
    public:        
        virtual std::string get_name() {
            return "dicmdQaReplyStep";
        }

        virtual void execute() {
            LeCallbackData d;
            NOTIFY(STEP_REPLY,d);
        }
};


class dicmdQaReplyingBreak: public NonTransactionalDirectCommandBase
{
    public:        
        virtual std::string get_name() {
            return "dicmdQaReplyingBreak";
        }

        virtual void execute() {
            LeCallbackData d;
            NOTIFY(STOP_REPLY,d);
        }
};



class dicmdQaReplyingResume: public NonTransactionalDirectCommandBase
{
    public:        
        virtual std::string get_name() {
            return "dicmdQaReplyingResume";
        }

        virtual void execute() {
            LeCallbackData d;
            NOTIFY(RESUME_REPLY,d);
        }
};

class dicmdQaToolExit: public NonTransactionalDirectCommandBase
{
    public:
        
        virtual std::string get_name() {
            return "dicmdQaToolExit";
        }
        
        virtual void execute() {
            //FIXME
            QApplication::quit();
            QApplication::exit();
            exit(0);
        }
    
};

namespace {
    std::string qaCompType2string(qaCompType type) {
    	switch (type) {
                case DESIGN:
                        return("Design");
                        break;
                case SELECTION:
			return("Selection");
			break;
		case CANVAS:
			return("Canvas");
			break;
		case RUNTIME:
			return("Runtime");
			break;            
            }
		return "";
    }
}

template <qaCompType T>
class dicmdQaDump: public NonTransactionalDirectCommandBase
{
    std::string m_fname;
    public:        
        
        dicmdQaDump<T>() {
            add_option("-filename",new StringCommandOptionValue("hopar.png"));
        }
        
        virtual std::string get_name() {
            return "dicmdQaDump"+qaCompType2string(T);
        }

        virtual void execute() {
                m_fname = GET_CMD_ARG(StringCommandOptionValue,"-filename");
                switch (T) {
                    case DESIGN:
                            return dump_design();
                            break;
                    case SELECTION:
                            return dump_selection();
                            break;
                    case CANVAS:
                            return dump_canvas();
                            break;
                    case SELECTIONCANVAS:
                            return dump_canvas_wrapper();
                            break;
                    }            
        }
        
    private:
        void dump_canvas_wrapper() {
            //std::cout << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << std::endl;
            Selection::getInstance().highlight_last_selected_region(true);           
            dump_canvas();
            Selection::getInstance().highlight_last_selected_region(false);
        }

        void dump_canvas() {
            QWidget* w = command_manager::getInstance().get_main_widget()->findChild<QWidget*>("CANVAS");
            //FIXME exception on error or what?
            if ( !w )
                return;
 
            QPixmap pixmap(w->size());
            w->render(&pixmap);
            pixmap.save(m_fname.c_str());

            std::cout << m_fname.c_str() << "\n\n\n\n";

        }
 
        void dump_selection() {
            Selection::getInstance().dumpToFile(m_fname);
        }
        
        void dump_design() {
            //WorkingSet::getInstance().dump_to_file(fname);
        }
    
};


template <qaCompType T>
class dicmdQaCompareInternal: public NonTransactionalDirectCommandBase
{
    public:        
        dicmdQaCompareInternal<T>() {
            add_option("-dumpfile",new StringCommandOptionValue("hopar.png"));
            add_option("-goldenfile",new StringCommandOptionValue("hopar.png.golden"));
        }
        
        virtual std::string get_name() {
            return "dicmdQaCompareInternal"+qaCompType2string(T);
        }

        virtual void execute() {
            std::stringstream z;
            std::string f(GET_CMD_ARG(StringCommandOptionValue,"-dumpfile"));
            std::string g(GET_CMD_ARG(StringCommandOptionValue,"-goldenfile"));
            
            dicmdQaDump<T>().set_arg("-filename",f)->execute();
            
            //std::cout << "regoooooldneeeen" << QString::fromLocal8Bit( qgetenv("ELEN_PAINTER_REGOLDEN").constData() ).toStdString() << std::endl;
            bool regoldenmode = false;
            if ( ! QString::fromLocal8Bit( qgetenv("ELEN_PAINTER_REGOLDEN").constData() ).isEmpty() )
                regoldenmode = true;
            
            if ( regoldenmode ) {
                 //std::cout << "aaaaaaaaar" << std::endl;
                //Messenger::expose_msg(test,"dicmdQaCanvasCompare-compare-regolden: "+f+" "+g);
                //std::cout << "#/t CanvasCompare REGOLDENED: " << f << " " << g << std::endl;
                //FIXME not compatible with other OS
                #ifdef OS_LINUX
                    //std::cout << "hoparrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr" << std::endl;
                    z << "cp " << f << " " << g;
                    system(z.str().c_str());
                    Messenger::expose_msg(test,"comparision->"+qaCompType2string(T)+":PASS "+f+" "+g);
                #else
                    Messenger::expose_msg(err, "Autoregoldening is availble only in linux ( currently )");
                #endif
            } else {
            
                if ( are_two_files_different(T,f.c_str(),g.c_str()) ) {
                    if ( Application::is_debug_mode() ) 
                    {
                        
                        Messenger::expose_msg(err,"comparision->"+qaCompType2string(T)+":MISMATCH "+f+" "+g+". Click <a href=\"file://"+generate_html_view(f,g).toStdString()+"\">here</a> to see the diff.");
                        dicmdQaReplyingBreak().execute_and_log();
                    } else {
                        Messenger::expose_msg(err,"comparision->"+qaCompType2string(T)+":MISMATCH "+f+" "+g);    
                    }
                }
                else 
                    Messenger::expose_msg(test,"comparision->"+qaCompType2string(T)+":PASS "+f+" "+g);
            }
            
        }
        
    private:
            
        //fixme need total refactoring !
        QString generate_html_view(const std::string& f, const std::string& g) 
        {
                QString res(QDir::currentPath()+QString("/"+QString(f.c_str())+".html"));
                #ifdef OS_LINUX
                    //system();
                    //std::string s("touch "+f+".html");
                    std::string s1("cat html_diff.template | sed 's/%fname1%/"+g+"/' | sed 's/%fname2%/"+f+"/' > "+f+".html");
                    //std::string s2("cat "+f+".html | sed 's/%f1%/cat "+g+"/e' > 1"+f+".html");
                    //std::string s3("cat "+f+".html | sed 's/%f2%/cat "+f+"/e' > 2"+f+".html");
                    std::string s2("sed -i 's/%f1%/cat "+g+"/e' "+f+".html");
                    std::string s3("sed -i 's/%f2%/cat "+f+"/e' "+f+".html");
                    
                    system(s1.c_str());
                    system(s2.c_str());
                    system(s3.c_str());
                #endif
                return res;
            }
        
    
};

template <qaCompType T>
class dicmdQaCompare: public NonTransactionalDirectCommandBase 
{
    static int n_index;
        
        std::string get_index_str() {
            std::stringstream z;
            z << qaCompType2string(T) << "_Compare_"<<n_index;
            if ( T == CANVAS ) 
               z <<".png";
            else 
                z << ".txt";
            return z.str();
        }
    
    
    public:        
        virtual std::string get_name() {
            return "dicmdQaCompare"+qaCompType2string(T);
        }

        virtual void execute() {
            //if not a canvas compare, do extra canvas compare in any case
            if ( T != CANVAS ) {
                dicmdQaDump<CANVAS>().set_arg("-filename","CanvasFor_"+get_index_str()+".png")->execute();
                //std::cout << "r1egoooooldneeeen" << QString::fromLocal8Bit( qgetenv("ELEN_PAINTER_REGOLDEN").constData() ).toStdString() << std::endl;
    
                if ( ! QString::fromLocal8Bit( qgetenv("ELEN_PAINTER_REGOLDEN").constData() ).isEmpty() )
                    //std::cout << "r?????" << std::endl;
                    dicmdQaDump<SELECTIONCANVAS>().set_arg("-filename","CanvasFor_"+get_index_str()+".golden.png")->execute();
            }
            
            dicmdQaCompareInternal<T>()
            .set_arg("-dumpfile",get_index_str())
            ->set_arg("-goldenfile",get_index_str()+".golden")
            ->execute();
            
            n_index++;
        }
};    






class dicmdTestCmdListOptions: public NonTransactionalDirectCommandBase
{
    public:        
        dicmdTestCmdListOptions() {
            add_option("-strings",new StringListCommandOptionValue());
            add_option("-points",new PointListCommandOptionValue());
        }
        
        virtual std::string get_name() {
            return "dicmdTestCmdListOptions";
        }

        virtual void execute() {
            //std::string f(GET_CMD_ARG(StringListCommandOptionValue,"-list1"));
            //std::string g(GET_CMD_ARG(StringListCommandOptionValue,"-list2"));
        }
};


#endif
