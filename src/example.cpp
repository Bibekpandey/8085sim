#ifndef GTKMM_EXAMPLEWINDOW_H
#define GTKMM_EXAMPLEWINDOW_H
#include <gtkmm.h>

class ExamleWindow : public Gtk::Window 
{
public:
	ExamleWindow();
	virtual ~ExamleWindow();

protected:
	void on_notebook_switch_page(Gtk::Widget* page, guint page_num);

	Gtk::Box m_VBox;
	Gtk::Notebook m_NoteBook;
	Gtk::Label m_Label1, m_Label2;

	Gtk::ButtonBox m_ButtonBox;


};
#endif 