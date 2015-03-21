#ifndef GTKMM_EXAMPLEWINDOW_H
#define GTKMM_EXAMPLEWINDOW_H
#include <gtkmm.h>

class ExampleWindow : public Gtk::Window 
{
public:
	ExampleWindow();
	virtual ~ExampleWindow();

protected:
	void on_notebook_switch_page(Gtk::Widget* page, guint page_num);
	void on_textbuffer_changed();
	void on_button_clicked();
    void on_button_clicked_strobe();
    void updateflag_register();
    
	Gtk::Button m_button;
	Gtk::Button m_button_strobe;
  Gtk::Table m_Table;

	Gtk::Frame m_Frame;
	Gtk::Frame m_Frame1;

	Gtk::ScrolledWindow m_ScrolledWindow;
	Gtk::TextView m_TextView;
	Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer;
	
	Gtk::Box m_VBox, m_VBox_run;
	Gtk::Box m_VBox_Register,m_VBox_Flag;
	Gtk::Box m_S,m_Z,m_AC,m_P,m_C;
	Gtk::Box m_A, m_BC, m_DE, m_HL;

  //For 8255 ppi
  Gtk::Frame m_Frame2;
  Gtk::Button strobeA, strobeB;
  Gtk::Box m_ppibox, m_strobeA, m_strobeB;
  Gtk::Label m_labelA, m_labelB, m_labelC;


	Gtk::Notebook m_Notebook;
	Gtk::Label m_Label1, m_Label2, m_Label3, m_Label4, m_Label5;
	Gtk::Label label_A, label_BC, label_DE, label_HL;
	Gtk::Label s_value, z_value, ac_value, p_value, cy_value;
	Gtk::Label a_value, b_value, c_value, d_value, e_value, h_value, l_value;
	Gtk::ButtonBox m_ButtonBox;
	//Tree model columns:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:

    ModelColumns()
    { add(m_col_address_hex); add(m_col_address); add(m_col_data);}

    Gtk::TreeModelColumn<unsigned int> m_col_address_hex;
    Gtk::TreeModelColumn<unsigned int> m_col_address;
    Gtk::TreeModelColumn<short> m_col_data;
  };
  ModelColumns m_Columns;// For memory tab in notebook
  ModelColumns m_Columns1;// For I/O tab in notebook

  //Child widgets:
  Gtk::Box m_VBox1;// For memory tab in notebook
  Gtk::Box m_VBox2;// For I/O tab in notebook

  // For memory tab in notebook
  Gtk::ScrolledWindow m_ScrolledWindow1;
  Gtk::TreeView m_TreeView;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

  //For I/O tab in notebook
  Gtk::ScrolledWindow m_ScrolledWindow2;
  Gtk::TreeView m_TreeView1;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel1;

};
#endif 
