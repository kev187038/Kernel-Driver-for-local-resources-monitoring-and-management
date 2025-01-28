import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, GLib

def get_process_data():
    ret = {}
    #Creates dictionary { <fieldname> : [values] } to store all values

    with open("/proc/srmc", "r") as f:
        keys = f.readline().strip().split('\t')

        for el in keys:
            ret[el] = []

        line = f.readline().strip().split('\t')

        while(line != [''] and line != '' and line != None):

            for el, k in zip(line, keys):
                
                if k == "PID" or k == "TGID" or k == "Priority" or k == "State":
                    ret[k].append(int(el))
                elif k != "Name":
                    ret[k].append(float(el))
                else:
                    ret[k].append(el)

            line = f.readline().strip().split('\t')


    return ret


def get_data_by_pid(data):
    #Data is dictionary from get_process_data
    ret = {}

    for values in zip(*[data[key] for key in data.keys()]):
        ret[values[0]] = [] #pid is key
        for i in range(1, len(data.keys())):
            ret[values[0]].append(values[i])

    return ret


class ProcessMonitor(Gtk.Window):
    def __init__(self):
        super().__init__(title="Process Monitor")
        self.set_border_width(10)
        self.set_default_size(1200, 600)
        self.processes = get_process_data()

        self.liststore = Gtk.ListStore(int, str, float, float, float, float, float, int, int, int)
        self.populate_liststore()
        self.treeview = Gtk.TreeView(model=self.liststore)
        self.create_columns()

        scrolled_window = Gtk.ScrolledWindow()
        scrolled_window.set_vexpand(True)
        scrolled_window.add(self.treeview)
        self.add(scrolled_window)

        self.timeout_id = GLib.timeout_add(1000, self.update_data)


    #Update values for rows
    def update_data(self):

        try:
            self.processes = get_process_data()
        except OSError as e:
            #print("File currently in use from OS, ignore update_data call")
            return True

        for i in range(len(self.processes["PID"])):

            try:
                iter = self.liststore.get_iter(i)  #Get the iterator for the row at index i
                self.liststore.set(iter, 
                                0, self.processes["PID"][i], 
                                1, self.processes["Name"][i], 
                                2, self.processes["CPU (User) (ms)"][i], 
                                3, self.processes["CPU (Kernel) (ms)"][i], 
                                4, self.processes["MEM(kb)"][i], 
                                5, self.processes["DISK I/O Read(kb)"][i],
                                6, self.processes["DISK I/O Write(kb)"][i],
                                7, self.processes["State"][i],
                                8, self.processes["TGID"][i],
                                9, self.processes["Priority"][i])
            except ValueError:
                #When the row does not exist, add a new row
                self.liststore.append([
                    self.processes["PID"][i], 
                    self.processes["Name"][i], 
                    self.processes["CPU (User) (ms)"][i], 
                    self.processes["CPU (Kernel) (ms)"][i], 
                    self.processes["MEM(kb)"][i], 
                    self.processes["DISK I/O Read(kb)"][i],
                    self.processes["DISK I/O Write(kb)"][i],
                    self.processes["State"][i],
                    self.processes["TGID"][i],
                    self.processes["Priority"][i]
                ])
    
        #Remove all processes not present in the new struct
        proc = get_data_by_pid(self.processes)
        iter = self.liststore.get_iter_first()

        #Use iterator to remove tuples from table if they are not present in the proc dict
        while iter:
            pid = self.liststore.get_value(iter, 0) 
            
            if pid not in proc:
                iter = self.liststore.remove(iter)
            else:
                iter = self.liststore.iter_next(iter)

        return True
    
    def populate_liststore(self):

        for i in range(len(self.processes["PID"])):
            self.liststore.append([
                self.processes["PID"][i], 
                self.processes["Name"][i], 
                self.processes["CPU (User) (ms)"][i], 
                self.processes["CPU (Kernel) (ms)"][i], 
                self.processes["MEM(kb)"][i], 
                self.processes["DISK I/O Read(kb)"][i],
                self.processes["DISK I/O Write(kb)"][i],
                self.processes["State"][i],
                self.processes["TGID"][i],
                self.processes["Priority"][i]
            ])

    #Create columns for treeview
    def create_columns(self):
        renderer = Gtk.CellRendererText()

        column = Gtk.TreeViewColumn("PID", renderer, text=0)
        self.treeview.append_column(column)

        column = Gtk.TreeViewColumn("Name", renderer, text=1)
        self.treeview.append_column(column)

        column = Gtk.TreeViewColumn("CPU (User) (ms)", renderer, text=2)
        self.treeview.append_column(column)

        column = Gtk.TreeViewColumn("CPU (Kernel) (ms)", renderer, text=3)
        self.treeview.append_column(column)

        column = Gtk.TreeViewColumn("MEM(kb)", renderer, text=4)
        self.treeview.append_column(column)

        column = Gtk.TreeViewColumn("DISK I/O Read(kb)", renderer, text=5)
        self.treeview.append_column(column)

        column = Gtk.TreeViewColumn("DISK I/O Write(kb)", renderer, text=6)
        self.treeview.append_column(column)

        column = Gtk.TreeViewColumn("State", renderer, text=7)
        self.treeview.append_column(column)

        column = Gtk.TreeViewColumn("TGID", renderer, text=8)
        self.treeview.append_column(column)

        column = Gtk.TreeViewColumn("Priority", renderer, text=9)
        self.treeview.append_column(column)



win = ProcessMonitor()
win.connect("destroy", Gtk.main_quit)
win.show_all()
Gtk.main()