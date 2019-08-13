#ifndef command_interpreter_hpp
#define command_interpreter_hpp

#include "messenger.hpp"

#include "../core/service.h"
#include "../commands/command_manager.hpp"
#include "../commands/icommand_base.hpp"


#include <QStringList>
#include <QString>

#include <string>
#include <iostream>

class CommandInterp : public Service<CommandInterp>
{
    command_manager* m_cm;
    
    public:
        CommandInterp() {
            m_cm = command_manager::get_instance();
        }
        
        bool interpret_from_string(const std::string& n) {
            execute_cmd(get_cmd_obj(QString(n.c_str())));
        }
        
        
        CommandBase* get_cmd_obj(QString n) {
            QStringList tokens = n.split(" ");  
            //std::cout << "<"<<n.toStdString()<<">" << tokens[0].toStdString() << std::endl;
            
            CommandBase* cmd = command_manager::get_instance()->find_command(tokens[0].toStdString());
            if ( !cmd ) {
                Messenger::expose_msg(err,"Command not found");
                return 0;
            }
                
            //std::cout << cmd << std::endl;
            //std::cout << cmd->get_name() << std::endl;
            //std::cout << cmd->get_type() << std::endl;
            
            //fixme parsing
            for (int i=1; i<tokens.size()-1; i=i+2 ) {
                //std::cout << "  ---<" << tokens[i].toStdString() << "> <=> <"<< tokens[i+1].toStdString() << ">" << std::endl;
                cmd->set_arg(tokens[i].toStdString(),tokens[i+1].toStdString());
            }
            
            return cmd;
                    
        }
        
        void execute_cmd(CommandBase* cmd) {
            if(!cmd)
                return;
            
            if ( cmd->get_type() == Interactive )    
                m_cm->activate_command(dynamic_cast<CommandBase*>(cmd));
            else
                cmd->execute_and_log();
        }
};

#endif