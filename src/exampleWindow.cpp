#include <iostream>
#include <exampleWindow.h>
#include <fstream>
#include <string>
#include <cstdlib>
#include <Processor.h>
#include <thread>

extern Processor processor;
extern NewParser parser;

void executeFile(ExampleWindow*, Share_Resource &share_resource);
int dummy = 0;

std::string abc = "0";

ExampleWindow::ExampleWindow()
: m_VBox(Gtk::ORIENTATION_VERTICAL),
singlestep(0),
run(0),
m_VBox1(Gtk::ORIENTATION_VERTICAL),
m_VBox2(Gtk::ORIENTATION_VERTICAL),
m_VBox_Register(Gtk::ORIENTATION_VERTICAL),
m_VBox_Flag(Gtk::ORIENTATION_VERTICAL),
m_Table(7,7,true),
m_VBox_run(Gtk::ORIENTATION_VERTICAL),
m_button("Run"),
m_button_strobe("strobe"),
m_VBox_singlestep(Gtk::ORIENTATION_VERTICAL),
m_button_singlestep("Single Step"),
strobeA("Strobe A"),
strobeB("Strobe B"),
entryA("Entry A"),
entryB("Entry B"),
entryC("Entry C"),
m_ppibox(Gtk::ORIENTATION_VERTICAL),
m_strobeA(Gtk::ORIENTATION_VERTICAL),
m_strobeB(Gtk::ORIENTATION_VERTICAL),
m_labelA("Port A", Gtk::ALIGN_START),
m_labelB("Port B", Gtk::ALIGN_START),
m_labelC("Port C", Gtk::ALIGN_START),
m_valA("00"),
m_valB("00"),
m_valC("00"),


m_adjustment_A(Gtk::Adjustment::create(1,1,31,1,5,0)),
m_adjustment_B(Gtk::Adjustment::create(1,1,31,1,5,0)),
m_adjustment_C(Gtk::Adjustment::create(1,1,31,1,5,0)),

m_SpinButtonA(m_adjustment_A),
m_SpinButtonB(m_adjustment_B),
m_SpinButtonC(m_adjustment_C),

m_Label1("S", Gtk::ALIGN_START),
m_Label2("Z", Gtk::ALIGN_START),
m_Label3("AC", Gtk::ALIGN_START),
m_Label4("P", Gtk::ALIGN_START),
m_Label5("C", Gtk::ALIGN_START),
label_A("A:", Gtk::ALIGN_START),
label_BC("BC:", Gtk::ALIGN_START),
label_DE("DE:", Gtk::ALIGN_START),
label_HL("HL:", Gtk::ALIGN_START),
s_value(abc),
z_value("0"),
ac_value("0"),
p_value("0"),
cy_value("0"),
a_value("00"),
b_value("00"),
c_value("00"),
d_value("00"),
e_value("00"),
h_value("00"),
l_value("00")

{
	set_title("Gtk::Notebook example");
	set_border_width(10);
	set_default_size(900,540);

	//m_button.add_pixlabel("info.xpm", "Cool Button");
	m_VBox_run.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);
		m_ButtonBox.pack_start(m_button, Gtk::PACK_SHRINK);
	m_button.signal_clicked().connect( sigc::mem_fun(*this,
   &ExampleWindow::on_button_clicked) );

    m_VBox_singlestep.pack_start(m_button_box_singlestep, Gtk::PACK_SHRINK);
    m_button_box_singlestep.pack_start(m_button_singlestep, Gtk::PACK_SHRINK);
    m_button_singlestep.signal_clicked().connect(sigc::mem_fun(*this, 
    &ExampleWindow::on_button_clicked_singlestep));

    m_ButtonBox.pack_start(m_button_strobe, Gtk::PACK_SHRINK);
	m_button_strobe.signal_clicked().connect( sigc::mem_fun(*this,&ExampleWindow::on_button_clicked_strobe));
	
   //strobe function
    strobeA.signal_clicked().connect(sigc::mem_fun(*this,
    &ExampleWindow::clickStrobeA));
    
    strobeB.signal_clicked().connect(sigc::mem_fun(*this,
    &ExampleWindow::clickStrobeB));
    
    add(m_Table);
	add(m_VBox);
    add(m_VBox_singlestep);
	add(m_Frame);
	add(m_Frame1);
	/* Set the frames label */
	m_Frame.set_label("Flags");
	m_Frame1.set_label("Registers");

	/* Set the style of the frame */
	m_Frame.set_shadow_type(Gtk::SHADOW_ETCHED_OUT);
	m_Frame1.set_shadow_type(Gtk::SHADOW_ETCHED_OUT);

	/*Set margin for the widgets */
	m_VBox_run.set_border_width(0);
	m_Notebook.set_border_width(10);
	m_Frame1.set_border_width(10);
	m_Frame.set_border_width(10);
	m_Frame2.set_border_width(10);

	/*----- 8255 PPI part -------*/
	add(m_Frame2);
	m_Frame2.set_label("8255 PPI");
	m_Frame2.set_shadow_type(Gtk::SHADOW_ETCHED_OUT);
	m_Frame2.add(m_ppibox);
	m_ppibox.set_border_width(10);
	m_ppibox.pack_start(m_strobeA, Gtk::PACK_EXPAND_WIDGET, 5);
	m_strobeA.pack_start(strobeA,Gtk::PACK_SHRINK);	

	m_ppibox.pack_start(m_strobeB, Gtk::PACK_EXPAND_WIDGET, 5);
	m_strobeB.pack_start(strobeB, Gtk::PACK_SHRINK);

	m_ppibox.pack_start(m_PortA, Gtk::PACK_EXPAND_WIDGET, 5);	
	m_PortA.pack_start(m_labelA);
	m_PortA.pack_start(m_valA);
	m_SpinButtonA.set_wrap();
	m_PortA.pack_start(m_SpinButtonA);
	

	//button_boxA.pack_start(entryA, Gtk::PACK_SHRINK);
	entryA.signal_clicked().connect( sigc::mem_fun(*this,
	&ExampleWindow::on_entryA_click));
	
	m_PortA.pack_start(entryA, Gtk::PACK_SHRINK);
	
	
	m_ppibox.pack_start(m_PortB, Gtk::PACK_EXPAND_WIDGET, 5);
	m_PortB.pack_start(m_labelB);
	m_PortB.pack_start(m_valB);
	m_SpinButtonB.set_wrap();
	m_PortB.pack_start(m_SpinButtonB);
	m_PortB.pack_start(entryB, Gtk::PACK_SHRINK);
	entryB.signal_clicked().connect( sigc::mem_fun(*this,
	&ExampleWindow::on_entryB_click));	
	
	m_ppibox.pack_start(m_PortC, Gtk::PACK_EXPAND_WIDGET, 5);	
	m_PortC.pack_start(m_labelC);
	m_PortC.pack_start(m_valC);
	m_SpinButtonC.set_wrap();
	m_PortC.pack_start(m_SpinButtonC);
	m_PortC.pack_start(entryC, Gtk::PACK_SHRINK);
	entryC.signal_clicked().connect( sigc::mem_fun(*this,
	&ExampleWindow::on_entryC_click));
	

	/* Set table position */
	m_Table.attach(m_VBox_run, 0,1,0,1);
	m_Table.attach(m_VBox_singlestep, 0,3,0,3);
	m_Table.attach(m_Notebook, 5,7,1,4);
	m_Table.attach(m_Frame, 4,5,1,4);
	m_Table.attach(m_Frame1, 3,4,1,4);
	m_Table.attach(m_Frame2, 3,6,4,7);
	m_Table.attach(m_ScrolledWindow, 0,3,1,6);

	updateflag_register();
	m_VBox.pack_start(m_Notebook);
	//m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

	// Create the buffer and set it for the TextView:
	m_refTextBuffer = Gtk::TextBuffer::create();
	m_TextView.set_buffer(m_refTextBuffer);
	// Add inside a ScrolledWindow:
	m_ScrolledWindow.add(m_TextView);
	// Show the scrollbars only when they are necessary:
	m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_VBox.pack_start(m_ScrolledWindow);

	m_refTextBuffer->signal_changed().connect(sigc::mem_fun(*this,
	&ExampleWindow::on_textbuffer_changed) );


  //Add the TreeView, inside a ScrolledWindow:
  m_ScrolledWindow1.add(m_TreeView);
  m_ScrolledWindow2.add(m_TreeView1);

  //Only show the scrollbars when they are necessary:
  m_ScrolledWindow1.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	 m_VBox1.pack_start(m_ScrolledWindow1);
	 m_ScrolledWindow2.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	 m_VBox2.pack_start(m_ScrolledWindow2);

	 //Create the Tree model:
  m_refTreeModel = Gtk::ListStore::create(m_Columns);
  m_TreeView.set_model(m_refTreeModel);
   m_refTreeModel1 = Gtk::ListStore::create(m_Columns1);
  m_TreeView1.set_model(m_refTreeModel1);

/* --------- For Memory tab ----------*/

  int start = Helper::ToDec("9000");
  for(int i=start; i<start+100; i++)
  {
	  //Fill the TreeView's model
	  Gtk::TreeModel::Row row = *(m_refTreeModel->append());
	  row[m_Columns.m_col_address_hex] = 0;
	  row[m_Columns.m_col_address] = i;
	  row[m_Columns.m_col_data] = processor.m_memory[i];

 }

	  //Add the TreeView's view columns:
	  //This number will be shown with the default numeric formatting.
	  m_TreeView.append_column_numeric("Address(Hex)", m_Columns.m_col_address_hex, "%04d" /* 10 digits, using leading zeroes. */);
	  m_TreeView.append_column("Address", m_Columns.m_col_address);
		m_TreeView.append_column("Data", m_Columns.m_col_data); 


for(guint i = 0; i < 2; i++)
  {
    Gtk::TreeView::Column* pColumn = m_TreeView.get_column(i);
    pColumn->set_reorderable();
  }

/*------For I/O tab ------- */

   //Fill the TreeView's model
  for(int i=0; i<30; i++)
  {
  	Gtk::TreeModel::Row row1 = *(m_refTreeModel1->append());
  	row1[m_Columns1.m_col_address_hex] = 1;
  	row1[m_Columns1.m_col_address] = i;
  	row1[m_Columns1.m_col_data] = 10;
  }

  //Add the TreeView's view columns:
  //This number will be shown with the default numeric formatting.
  m_TreeView1.append_column("Address(Hex)", m_Columns1.m_col_address_hex);
  m_TreeView1.append_column("Address", m_Columns1.m_col_address);
	m_TreeView1.append_column_numeric("Data", m_Columns1.m_col_data,
          "%04d" /* 10 digits, using leading zeroes. */); 
	
for(guint i = 0; i < 2; i++)
  {
    Gtk::TreeView::Column* pColumn = m_TreeView1.get_column(i);
    pColumn->set_reorderable();
  }

	//Add the Notebook pages:
	m_Notebook.append_page(m_VBox1, "Memory");
	m_Notebook.append_page(m_VBox2, "I/O");
	m_Notebook.signal_switch_page().connect(sigc::mem_fun(*this,
	&ExampleWindow::on_notebook_switch_page) );

	show_all_children();
    
    // thread creation
    Glib::Thread::create(sigc::mem_fun(*this, &ExampleWindow::threadtest), true);

}

void ExampleWindow::threadtest()
{
    while(run!=1);
    //executeFile(this, share_resource);
    processor.Run();
    run=0;
}
void ExampleWindow::on_button_clicked_singlestep()
{
    static int parsed = 0;
    run=0;
    singlestep=1;
    if(!parsed)
    {
        std::ofstream outfile("program.txt");
        outfile << m_TextView.get_buffer()->get_text();
        outfile.close();
        parser.LoadMnemonics("program.txt", processor.m_memory);
        processor.Initialize(&parser, this);
        parsed =1;
    }

  //  share_resource.ioMemory[64] = 10;


    processor.Run();
}
void ExampleWindow::clickStrobeA()
{

        share_resource.strobeA = true;

}

void ExampleWindow::clickStrobeB()
{

        share_resource.strobeB = true;
}
 
ExampleWindow::~ExampleWindow()
{

}

void ExampleWindow::updateflag_register()
{
	/*------- FIll Flag-------*/
	m_VBox_Flag.set_border_width(20);
	m_Frame.add(m_VBox_Flag);
	m_VBox_Flag.pack_start(m_S, Gtk::PACK_EXPAND_WIDGET, 5);
	m_S.pack_start(m_Label1);
	m_S.pack_start(s_value);

	m_VBox_Flag.pack_start(m_Z, Gtk::PACK_EXPAND_WIDGET, 5);
	m_Z.pack_start(m_Label2);
	m_Z.pack_start(z_value);

	m_VBox_Flag.pack_start(m_AC, Gtk::PACK_EXPAND_WIDGET, 5);
	m_AC.pack_start(m_Label3);
	m_AC.pack_start(ac_value);	

	m_VBox_Flag.pack_start(m_P, Gtk::PACK_EXPAND_WIDGET, 5);
	m_P.pack_start(m_Label4);
	m_P.pack_start(p_value);

	m_VBox_Flag.pack_start(m_C, Gtk::PACK_EXPAND_WIDGET, 5);
	m_C.pack_start(m_Label5);
	m_C.pack_start(cy_value);

	/*------ Fill Register ------*/
	m_VBox_Register.set_border_width(20);
	m_Frame1.add(m_VBox_Register);

	m_VBox_Register.pack_start(m_A, Gtk::PACK_EXPAND_WIDGET, 5);
	m_A.pack_start(label_A);
	m_A.pack_start(a_value);
   

    m_VBox_Register.pack_start(m_BC, Gtk::PACK_EXPAND_WIDGET, 5);
	m_BC.pack_start(label_BC);
	m_BC.pack_start(b_value);
	m_BC.pack_start(c_value);

	m_VBox_Register.pack_start(m_DE, Gtk::PACK_EXPAND_WIDGET, 5);
	m_DE.pack_start(label_DE);
	m_DE.pack_start(d_value);
	m_DE.pack_start(e_value);

	m_VBox_Register.pack_start(m_HL, Gtk::PACK_EXPAND_WIDGET, 5);
	m_HL.pack_start(label_HL);
	m_HL.pack_start(h_value);
	m_HL.pack_start(l_value);

}
void ExampleWindow::on_button_clicked()
{

//std::cout<<m_TextView.get_buffer()->get_text();

//file is sucessfully writtn
std::ofstream outfile("program.txt");
outfile << m_TextView.get_buffer()->get_text();
outfile.close();

//share_resource.ioMemory[64] = 10;

parser.LoadMnemonics("program.txt", processor.m_memory);
processor.Initialize(&parser, this);

run = 1;

//executeFile(this, share_resource);

}

void ExampleWindow::on_notebook_switch_page(Gtk::Widget* /* page */, guint page_num)
{
	std::cout << "Switched to tab with index " << page_num << std::endl;
	//You can also use m_Notebook.get_current_page() to get this index.
}
void ExampleWindow::on_entryA_click()
{
	//std::cout<<"MAnil";	
 	int int_valueA=m_SpinButtonA.get_value() ;
 
    share_resource.ioMemory[64] = int_valueA;

        std::string str_vala = std::to_string(int_valueA);
	
//	std::cout<<str_vala<<std::endl;
	//valA= static_cast<char>(int_valueA);
	m_valA.set_text(str_vala);
	
}

void ExampleWindow::on_entryB_click()
{
 	//here is the B port value
    
    int int_valueB=m_SpinButtonB.get_value() ;

    share_resource.ioMemory[65] = int_valueB;
    
    std::string str_valb = std::to_string(int_valueB);
	
//	std::cout<<str_valb<<std::endl;
	//valA= static_cast<char>(int_valueA);
	m_valB.set_text(str_valb);
	
}
void ExampleWindow::on_entryC_click()
{
	//std::cout<<"MAnil";	
 	int int_valueC=m_SpinButtonC.get_value() ;
	

    share_resource.ioMemory[66] = int_valueC;
    
    std::string str_valc = std::to_string(int_valueC);
	
//	std::cout<<str_valc<<std::endl;
	//valA= static_cast<char>(int_valueA);
	m_valC.set_text(str_valc);
	
}

void ExampleWindow::on_textbuffer_changed()
{
 
 //std::cout<<static_cast<char*>(m_refTextBuffer);

}

void ExampleWindow::on_button_clicked_strobe()
{
  
        std::cout<<"button is clicked"<<std::endl;
}

void ExampleWindow::on_spinbutton_digits_changed()
{
  // std::cout<<m_SpinButtonA.get_value() ;
}

