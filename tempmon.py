import serial
import time
import curses
import curses.wrapper
from binascii import hexlify


s = serial.Serial('/dev/ttyACM0',9600,timeout=0)
time.sleep(1)
d = s.read(1000)
while d:
    print d
    d = s.read(1000)
    time.sleep(1)

ordered_list = []

def getreadings():
    s.readlines()
    s.write('b')
    time.sleep(1)
    data = s.read(1000)
    names = []
    rawdata = []
    temps = []
    while (len(data) >= 17):
        d = map(ord, data[0:17])
        family = data[0]
        serial = data[7:0:-1]
        templ = d[8]
        temph = d[9]
        alarmh = d[10]
        alarml = d[11]
        config = data[12]
        
        names.append(hexlify(family) + '-' + hexlify(serial))
        temp = templ + 256*temph
        if (temp > 0x7fff):
            temp = temp - 0x10000
        temps.append(temp / 16.)
        rawdata.append(data[0:17])
        data = data[17:]
    return names, temps, rawdata, data
        
        
def getdevicenames():
    names = os.listdir(devicerootpath)
    names.remove('w1_bus_master1')
    return names

def appendnewnames(new, ordered_list):
    for name in new:
        if name not in ordered_list:
            ordered_list.append(name)

def getColor(t):
    splits = [15,30,45,60,75,90]
    n = 1;
    for s in splits:
        if (t < s): return n
        n+=1
    return 7

def c(n):
    r = n*1000./255.
    r = int(r)
    if (r > 1000): r = 1000
    if (r < 0): r = 0
    return r

def main(screen):
    curses.start_color()
    curses.use_default_colors()
    curses.init_color(1,c(255),c(0),c(0))
    curses.init_color(2,c(255),c(197),c(0))
    curses.init_color(3,c(255),c(240),c(0))
    curses.init_color(4,c(126),c(255),c(0))
    curses.init_color(5,c(0),c(255),c(255))
    curses.init_color(6,c(0),c(160),c(255))
    curses.init_color(7,c(100),c(0),c(255))
    
    curses.init_pair(0,-1,-1)
    curses.init_pair(1,7,-1)
    curses.init_pair(2,6,-1)
    curses.init_pair(3,5,-1)
    curses.init_pair(4,4,-1)
    curses.init_pair(5,3,-1)
    curses.init_pair(6,2,-1)
    curses.init_pair(7,1,-1)
    INFO = curses.color_pair(0)
    
    count = 0
    while 1:
        count += 1
        names, temps, rawdata, leftoverdata = getreadings()
        appendnewnames(names, ordered_list)
        screen.addstr(0,0,"uptime: " + str(count), INFO)
        screen.refresh()
        n = 0;
        for name in ordered_list:
            n += 1

            try:
                index = names.index(name)
                temp = temps[index]-40+count*5+n
                COLOR = curses.color_pair(getColor(temp))
                lsb = ord(rawdata[index][1])

                if (n < 14):
                    screen.addstr(n,0," "*25)
                    screen.addstr(n,0,name,COLOR)
                    screen.addstr(n,18,str(temp),COLOR)
                    screen.addstr(n,26,'{:08b}'.format(lsb),COLOR)
                else:
                    screen.addstr(14,0,"MORE SENSORS!", INFO)

            except ValueError:
                if (n < 14):
                    screen.addstr(n,0,name)
                    screen.addstr(n,24, "!")
                else:
                    screen.addstr(14,0,"MORE SENSORS!", INFO)

            finally:
                screen.refresh()
        time.sleep(1)

if __name__ == '__main__':
    try:
        stdscr = curses.initscr()
        curses.noecho()
        curses.cbreak()
        stdscr.keypad(1)
        main(stdscr)

    finally:
        stdscr.erase()
        stdscr.refresh()
        stdscr.keypad(0)
        curses.echo();
        curses.nocbreak()
        curses.endwin()
