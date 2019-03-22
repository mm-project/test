#include <QApplication>

#include "main_window.hpp"
#include "log_reader.hpp"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	main_window window;
	window.show();
        //FIXME add argument check and replay if needed
        //LogReader r;
        //r.replay_log("replay.log");
        
	return app.exec();
}
