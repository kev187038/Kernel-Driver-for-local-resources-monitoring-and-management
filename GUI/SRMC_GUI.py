import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, GLib

#Opens file and returns the tuples in a dictionary
def get_process_data(by_pid=False):
    ret = {}

    #Returns tuples in format { pid : [other fields' values] }, useful for lookup of processes
    if by_pid:
        with open("/proc/srmc", "r") as f:
            keys = f.readline().strip().split('\t')
            line = f.readline().strip().split('\t')

            while(line != [''] and line != '' and line != None):
                k = int(line[0])
                ret[k] = []
                for i in range(1, len(keys)):
                    val = line[i]
                    if i == 7 or i == 8 or i == 9:
                        val = int(val)
                    elif i == 1:
                        val = str(val)
                    else:
                        val = float(val)
                    
                    ret[k].append(val)

                line = f.readline().strip().split('\t')
    else:
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

#START of Workflow
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
            self.processes = get_process_data(by_pid=True)
        except OSError as e:
            #print("File currently in use from OS, ignore update_data call")
            return True
    
        #Remove all processes not present in the new struct
        iter = self.liststore.get_iter_first()

        #Use iterator to remove tuples from table if they are not present in the proc dict
        while iter: #for some reason iter can return true when it ends
            pid = self.liststore.get_value(iter, 0)
            
            if pid not in self.processes:
                next_iter = self.liststore.iter_next(iter)
                self.liststore.remove(iter)
                iter = next_iter

                if iter == None or iter is bool: #removed tuple could be last
                    break

            else:
                self.liststore.set(iter,
                               1, self.processes[pid][0],  #Name
                               2, self.processes[pid][1],  #CPU (User) (ms)
                               3, self.processes[pid][2],  #CPU (Kernel) (ms)
                               4, self.processes[pid][3],  #MEM (kb)
                               5, self.processes[pid][4],  #DISK I/O Read (kb)
                               6, self.processes[pid][5],  #DISK I/O Write (kb)
                               7, self.processes[pid][6],  #State
                               8, self.processes[pid][7],  #TGID
                               9, self.processes[pid][8])  #Priority
            
            iter = self.liststore.iter_next(iter)

        #Append new processes not present in the table
        for pid in self.processes:
            existing_iter = self.liststore.get_iter_first()
            exists = False
            while existing_iter:
                if self.liststore.get_value(existing_iter, 0) == pid:
                    exists = True
                    break
                existing_iter = self.liststore.iter_next(existing_iter)

            if not exists:
                self.liststore.append([
                    pid,
                    self.processes[pid][0],  #Name
                    self.processes[pid][1],  #CPU (User) (ms)
                    self.processes[pid][2],  #CPU (Kernel) (ms)
                    self.processes[pid][3],  #MEM (kb)
                    self.processes[pid][4],  #DISK I/O Read (kb)
                    self.processes[pid][5],  #DISK I/O Write (kb)
                    self.processes[pid][6],  #State
                    self.processes[pid][7],  #TGID
                    self.processes[pid][8]   #Priority
                ])

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

        for index, title in enumerate(self.processes.keys()):
            column = Gtk.TreeViewColumn(title, renderer, text=index)
            column.set_sort_column_id(index) #enables sorting by column
            self.treeview.append_column(column)



win = ProcessMonitor()
win.connect("destroy", Gtk.main_quit)
win.show_all()
Gtk.main()